import math
from pathlib import Path
import torch
from torch import nn
from typing import Optional, Tuple, Callable
import torch_mlir
import numpy as np
torch.manual_seed(0)
np.random.seed(0)


class GPT2Config:
    def __init__(
        self,
        vocab_size,
        n_positions,
        n_ctx,
        n_embd,
        n_layer,
        n_head,
        n_inner,
        resid_pdrop,
        embd_pdrop,
        attn_pdrop,
    ):
        self.vocab_size = vocab_size
        self.n_positions = n_positions
        self.n_ctx = n_ctx
        self.hidden_size = n_embd
        self.n_layer = n_layer
        self.num_attention_heads = n_head
        self.n_inner = n_inner
        self.resid_pdrop = resid_pdrop
        self.embd_pdrop = embd_pdrop
        self.attn_pdrop = attn_pdrop
        self.scale_attn_weights = True
        self.scale_attn_by_inverse_layer_idx = False
        self.reorder_and_upcast_attn = False
        self.add_cross_attention = False
        self.layer_norm_epsilon = 1e-5
        self._attn_implementation = "eager"


class Conv1D(nn.Module):
    def __init__(self, nf, nx):
        super().__init__()
        self.weight = nn.Parameter(torch.empty(nx, nf))
        self.bias = nn.Parameter(torch.zeros(nf))
        nn.init.normal_(self.weight, std=0.02)

    def forward(self, x):
        size_out = x.shape[:-1] + (self.bias.shape[0],)
        x = torch.matmul(x, self.weight) + self.bias
        return x.view(size_out)


class GPT2MLP(nn.Module):
    def __init__(self, intermediate_size, config):
        super().__init__()
        self.c_fc = Conv1D(intermediate_size, config.hidden_size)
        self.c_proj = Conv1D(config.hidden_size, intermediate_size)
        self.dropout = nn.Dropout(config.resid_pdrop)

    def forward(self, hidden_states):
        hidden_states = self.c_fc(hidden_states)
        hidden_states = nn.functional.gelu(hidden_states)
        hidden_states = self.c_proj(hidden_states)
        return self.dropout(hidden_states)


class GPT2Attention(nn.Module):
    def __init__(self, config, is_cross_attention=False, layer_idx=None):
        super().__init__()
        self.embed_dim = config.hidden_size
        self.num_heads = config.num_attention_heads
        self.head_dim = self.embed_dim // self.num_heads
        self.split_size = self.embed_dim
        if self.head_dim * self.num_heads != self.embed_dim:
            raise ValueError(
                f"`embed_dim` must be divisible by num_heads (got `embed_dim`: {self.embed_dim} and `num_heads`:"
                f" {self.num_heads})."
            )

        self.scale_attn_weights = config.scale_attn_weights
        self.is_cross_attention = is_cross_attention

        # Layer-wise attention scaling, reordering, and upcasting
        self.scale_attn_by_inverse_layer_idx = config.scale_attn_by_inverse_layer_idx
        self.layer_idx = layer_idx
        self.reorder_and_upcast_attn = config.reorder_and_upcast_attn

        self.q_proj = Conv1D(self.embed_dim, self.embed_dim)
        self.k_proj = Conv1D(self.embed_dim, self.embed_dim)
        self.v_proj = Conv1D(self.embed_dim, self.embed_dim)
        self.c_proj = Conv1D(self.embed_dim, self.embed_dim)

        self.attn_dropout = nn.Dropout(config.attn_pdrop)
        self.resid_dropout = nn.Dropout(config.resid_pdrop)
        self.is_causal = True

        self.pruned_heads = set()

    def forward(
        self,
        hidden_states: torch.Tensor,
        attention_mask: Optional[torch.FloatTensor] = None,
        head_mask: Optional[torch.FloatTensor] = None,
        key_cache: Optional[torch.Tensor] = None,
        value_cache: Optional[torch.Tensor] = None,
        cache_position: Optional[int] = None,
        output_attentions: bool = False,
    ) -> Tuple[torch.Tensor, Optional[torch.Tensor], torch.Tensor, torch.Tensor]:
        query_states = self.q_proj(hidden_states)
        key_states = self.k_proj(hidden_states)
        value_states = self.v_proj(hidden_states)

        batch_size = hidden_states.size(0)
        seq_len = hidden_states.size(1)

        query_states = query_states.view(batch_size, seq_len, self.num_heads, self.head_dim).transpose(1, 2)
        key_states = key_states.view(batch_size, seq_len, self.num_heads, self.head_dim).transpose(1, 2)
        value_states = value_states.view(batch_size, seq_len, self.num_heads, self.head_dim).transpose(1, 2)

        # Handle caching for decoding (input length = 1)
        if key_cache is not None and value_cache is not None and cache_position is not None:
            if hidden_states.shape[1] != 1:
                raise ValueError("When using cache, input length must be 1 (decoding mode)")
            new_key_cache = key_cache.clone()
            new_value_cache = value_cache.clone()
            new_key_cache[:, :, cache_position] = key_states[:, :, 0]
            new_value_cache[:, :, cache_position] = value_states[:, :, 0]
            key_states = new_key_cache
            value_states = new_value_cache

        is_causal = attention_mask is None and query_states.shape[-2] > 1

        attn_weights = torch.matmul(query_states, key_states.transpose(-1, -2))
        if self.scale_attn_weights:
            attn_weights = attn_weights / torch.full(
                [], value_states.size(-1) ** 0.5, dtype=attn_weights.dtype, device=attn_weights.device
            )
        if self.scale_attn_by_inverse_layer_idx:
            attn_weights = attn_weights / float(self.layer_idx + 1)
        if attention_mask is None and not self.is_cross_attention:
            query_length, key_length = query_states.size(-2), key_states.size(-2)
            causal_mask = torch.tril(torch.ones((query_length, key_length), dtype=torch.bool, device=query_states.device))
            causal_mask = causal_mask.view(1, 1, query_length, key_length)
            mask_value = torch.full([], -1.0e4, dtype=attn_weights.dtype, device=attn_weights.device)
            attn_weights = torch.where(causal_mask, attn_weights.to(attn_weights.dtype), mask_value)
        if attention_mask is not None:
            attn_weights = attn_weights + attention_mask
        attn_weights = nn.functional.softmax(attn_weights, dim=-1)
        attn_weights = attn_weights.type(value_states.dtype)
        attn_weights = self.attn_dropout(attn_weights)
        if head_mask is not None:
            attn_weights = attn_weights * head_mask
        attn_output = torch.matmul(attn_weights, value_states)
        attn_output = attn_output.transpose(1, 2)

        attn_output = attn_output.reshape(batch_size, seq_len, -1).contiguous()
        attn_output = self.c_proj(attn_output)
        attn_output = self.resid_dropout(attn_output)

        return attn_output, attn_weights, key_states, value_states


class GPT2Block(nn.Module):
    def __init__(self, config, layer_idx=None):
        super().__init__()
        hidden_size = config.hidden_size
        inner_dim = config.n_inner if config.n_inner is not None else 4 * hidden_size

        self.ln_1 = nn.LayerNorm(hidden_size, eps=config.layer_norm_epsilon)
        self.attn = GPT2Attention(config=config, layer_idx=layer_idx)
        self.ln_2 = nn.LayerNorm(hidden_size, eps=config.layer_norm_epsilon)

        if config.add_cross_attention:
            self.crossattention = GPT2Attention(config=config, is_cross_attention=True, layer_idx=layer_idx)
            self.ln_cross_attn = nn.LayerNorm(hidden_size, eps=config.layer_norm_epsilon)

        self.mlp = GPT2MLP(inner_dim, config)

    def forward(
        self,
        hidden_states: torch.Tensor,
        attention_mask: Optional[torch.FloatTensor] = None,
        head_mask: Optional[torch.FloatTensor] = None,
        key_cache: Optional[torch.Tensor] = None,
        value_cache: Optional[torch.Tensor] = None,
        cache_position: Optional[int] = None,
        output_attentions: bool = False,
    ) -> torch.Tensor:
        residual = hidden_states
        # hidden_states = self.ln_1(hidden_states)

        attn_output, attn_weights, key_states, value_states = self.attn(
            hidden_states,
            attention_mask=attention_mask,
            head_mask=head_mask,
            key_cache=key_cache,
            value_cache=value_cache,
            cache_position=cache_position,
            output_attentions=output_attentions,
        )

        # residual connection
        hidden_states = attn_output + residual

        residual = hidden_states
        # hidden_states = self.ln_2(hidden_states)
        feed_forward_hidden_states = self.mlp(hidden_states)
        # residual connection
        hidden_states = residual + feed_forward_hidden_states
        return hidden_states


class GPT2Sample(nn.Module):
    def __init__(self):
        super().__init__()
        config = GPT2Config(
            vocab_size=128,
            n_positions=16,
            n_ctx=16,
            n_embd=32,
            n_layer=1,
            n_head=4,
            n_inner=64,
            resid_pdrop=0.0,
            embd_pdrop=0.0,
            attn_pdrop=0.0,
        )
        config._attn_implementation = "eager"
        self.block = GPT2Block(config, layer_idx=0)

    def forward(self, hidden_states, attention_mask):
        return self.block(hidden_states, attention_mask=attention_mask)


model = GPT2Sample()
model.eval()

example_hidden_states = torch.ones(1, 8, 32)
example_attention_mask = torch.zeros(1, 1, 8, 8)
mlir_module = torch_mlir.compile(
    model, (example_hidden_states, example_attention_mask), output_type="tosa"
)

output_path = Path(__file__).with_name("gpt2_small_bf16.mlir")
with open(output_path, "w") as f:
    f.write(str(mlir_module))

print(f"MLIR written to {output_path.name}")
