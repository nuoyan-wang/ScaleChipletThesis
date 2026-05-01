import os
from pathlib import Path
import sys


THIS_DIR = Path(__file__).resolve().parent
REPO_ROOT = THIS_DIR.parents[3]
DEFAULT_FRESH_PYTHON = Path(
    os.environ.get(
        "SCALECHIPLET_TORCH_MLIR_PYTHON",
        REPO_ROOT / "torch-mlir-fresh" / ".venv" / "bin" / "python",
    )
)
DEFAULT_EXTRA_SITE = Path(
    os.environ.get(
        "SCALECHIPLET_EXTRA_SITE_PACKAGES",
        REPO_ROOT
        / "mlir_venv"
        / "lib"
        / f"python{sys.version_info.major}.{sys.version_info.minor}"
        / "site-packages",
    )
)


def _append_extra_site_packages():
    if DEFAULT_EXTRA_SITE.exists() and str(DEFAULT_EXTRA_SITE) not in sys.path:
        sys.path.append(str(DEFAULT_EXTRA_SITE))


def _ensure_torch_mlir_fx():
    try:
        import torch  # noqa: F401
        import torch_mlir.ir  # noqa: F401

        _append_extra_site_packages()
        from torch_mlir import fx

        return fx
    except ImportError as exc:
        if DEFAULT_FRESH_PYTHON.exists() and Path(sys.executable) != DEFAULT_FRESH_PYTHON:
            os.execv(str(DEFAULT_FRESH_PYTHON), [str(DEFAULT_FRESH_PYTHON), __file__, *sys.argv[1:]])
        raise RuntimeError(
            "Could not import torch_mlir.fx. Run this script with the fresh Torch-MLIR "
            f"venv at {DEFAULT_FRESH_PYTHON} or install a torch_mlir build that includes fx.export_and_import."
        ) from exc


fx = _ensure_torch_mlir_fx()

_append_extra_site_packages()

import torch
import torch_mlir.ir

from gpt2_block import GPT2BlockWrapper
from utils import create_causal_mask

# set parameters
model_name = "gpt2-medium"
data_type = "bf16"
if data_type == "fp32":
    torch_dtype = torch.float32
elif data_type == "bf16":
    torch_dtype = torch.bfloat16
batch_size = 1
max_seq_length = 128
target = {"type": "torch"}

# define model
model = GPT2BlockWrapper(
    batch_size=batch_size,
    max_length=max_seq_length,
    data_type=data_type,
    target=target,
    model_name=model_name,
)

# model parameters
num_heads = model.config.n_head
num_layers = model.config.n_layer
hidden_size = model.config.hidden_size
head_dim = hidden_size // num_heads

# Generate dummy inputs
hidden_states = torch.zeros((batch_size, 1, hidden_size), dtype=torch_dtype)
cache_position = torch.tensor([0], dtype=torch.int32)
attention_mask = torch.zeros((batch_size, max_seq_length), dtype=torch.int32)
causal_mask = create_causal_mask(attention_mask, hidden_states.shape, max_seq_length, dtype=torch_dtype)
key_cache = torch.zeros(batch_size, num_heads, max_seq_length, head_dim, dtype=torch_dtype)
value_cache = torch.zeros(batch_size, num_heads, max_seq_length, head_dim, dtype=torch_dtype)

# generate torch-mlir tosa output
for layer_idx in range(num_layers):
    print(f"Exporting layer {layer_idx}...")
    model.set_index(layer_idx)
    with torch.autocast(device_type="cpu", dtype=torch_dtype), torch.no_grad():
        module = fx.export_and_import(
            model,
            hidden_states, cache_position, causal_mask, key_cache, value_cache,
            output_type="tosa",
            func_name="kernel_" + str(layer_idx),
        )

    if layer_idx == 0:
        main_module = module
    else:
        bytecode = module.operation.get_asm(binary=True)
        with main_module.context:
            sub_module = torch_mlir.ir.Module.parse(bytecode)
        for op in sub_module.body.operations:
            main_module.body.append(op)

output_name = model_name.replace("-", "_") + "_block_" + data_type + ".mlir"
with open(Path(output_name), "w") as f:
    f.write(str(main_module))
