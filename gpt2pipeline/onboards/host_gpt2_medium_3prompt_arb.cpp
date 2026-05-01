#include <xrt/xrt_bo.h>
#include <xrt/xrt_device.h>
#include <xrt/xrt_kernel.h>
#include <xrt/experimental/xrt_profile.h>
#include <xrt/xrt_uuid.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

// Synthetic host for the current 5-kernel 3-chunk design.
//
// This version is intentionally not tied to real GPT-2 weights or prompts.
// It exists only to measure performance of the currently linked hardware path:
//
//   c0_router -> chunk0_stream_ip -> chunk1_stream_ip -> chunk2_stream_ip -> c2_router
//
// The host still drives:
//   - token-round seed generation
//   - one prompt-block launch per generated token
//
// The data written into HBM is arbitrary but deterministic:
//   - all chunk weight BOs are pattern-filled once at startup
//   - each prompt seed vector is pattern-filled per generation round
//
// This keeps the memory traffic and kernel launches real, while discarding any
// requirement that the outputs correspond to GPT-2 semantics.

constexpr int kNumLayers = 24;
constexpr int kNumPrompts = 3;
constexpr int kHidden = 1024;
constexpr int kCacheSeq = 128;
constexpr int kWordBytes = 32;
constexpr int kHiddenBeats = kHidden / 32;
constexpr int kMaxNewTokensDefault = 128;

constexpr size_t kSeedBoBytes = kNumPrompts * kHiddenBeats * kWordBytes;
constexpr size_t kFinalBoBytes = kNumPrompts * kHiddenBeats * kWordBytes;

constexpr size_t kC0B0Words = 16576;
constexpr size_t kC0BankWords = 16384;
constexpr size_t kC1Fc1Words = 16384;
constexpr size_t kC1Fc1BiasWords = 128;
constexpr size_t kC1LnWords = 32;
constexpr size_t kC2Fc2Words = 16384;
constexpr size_t kC2Fc2BiasWords = 32;
constexpr size_t kC0KvHalfSliceWords = 8 * 128 * 2;  // 8 heads per lo/hi half
constexpr size_t kC0KvSlices = kNumLayers * kNumPrompts;
constexpr size_t kC0KvHalfBytes = kC0KvHalfSliceWords * kC0KvSlices * kWordBytes;
constexpr size_t kC0KvTSliceWords = 16 * 64 * 4;  // 16 heads × 64 depth × 4 words (128-token K^T)
constexpr size_t kC0KvTBytes = kC0KvTSliceWords * kC0KvSlices * kWordBytes;
constexpr size_t kC0MetaWords = 192;  // QKV bias + Wout bias + LN1 beta/gamma per layer
constexpr size_t kC0MetaBytesPerLayer = kC0MetaWords * kWordBytes;

struct NamedBo {
  std::string name;
  size_t bytes = 0;
  xrt::bo bo;
};

struct PackedC0 {
  std::array<NamedBo, 8> banks;
};

struct PackedC1 {
  NamedBo fc1_b0;
  NamedBo fc1_bias;
  NamedBo ln2_beta;
  NamedBo ln2_gamma;
  std::array<NamedBo, 7> fc1_tail;
};

struct PackedC2 {
  NamedBo fc2_b0;
  NamedBo fc2_bias;
  std::array<NamedBo, 7> fc2_tail;
};

struct DeviceBundle {
  xrt::device device;
  xrt::uuid uuid;
  xrt::kernel k_router0;
  xrt::kernel k_c0;
  xrt::kernel k_c1;
  xrt::kernel k_c2;
  xrt::kernel k_router2;
  NamedBo seed_hidden_in;
  NamedBo final_hidden_out;
  NamedBo c0_meta_hbm;
  NamedBo kv_k_hbm_lo;
  NamedBo kv_v_hbm_lo;
  NamedBo kv_k_hbm_hi;
  NamedBo kv_v_hbm_hi;
  NamedBo kv_k_t_hbm;
  PackedC0 c0;
  PackedC1 c1;
  PackedC2 c2;
};

struct StepRuns {
  xrt::run rr2;
  xrt::run r2;
  xrt::run r1;
  xrt::run r0;
  xrt::run rr0;
};

struct StageTimingAccums {
  double us_router0 = 0.0;
  double us_chunk0 = 0.0;
  double us_chunk1 = 0.0;
  double us_chunk2 = 0.0;
  double us_router2 = 0.0;
  uint64_t rounds = 0;
};

xrt::kernel open_kernel(xrt::device& device, const xrt::uuid& uuid,
                        const std::vector<std::string>& names) {
  std::exception_ptr last_error;
  for (const auto& name : names) {
    try {
      return xrt::kernel(device, uuid, name);
    } catch (...) {
      last_error = std::current_exception();
    }
  }
  if (last_error)
    std::rethrow_exception(last_error);
  throw std::runtime_error("failed to open any requested kernel");
}

NamedBo make_bo(xrt::device& device, xrt::kernel& kernel, int arg_index,
                size_t bytes, const std::string& name) {
  return NamedBo{name, bytes, xrt::bo(device, bytes, kernel.group_id(arg_index))};
}

uint32_t mix32(uint32_t x) {
  x ^= x >> 16;
  x *= 0x7feb352dU;
  x ^= x >> 15;
  x *= 0x846ca68bU;
  x ^= x >> 16;
  return x;
}

void fill_bo_pattern(NamedBo& buffer, uint32_t seed) {
  auto* ptr = buffer.bo.map<uint8_t*>();
  uint32_t state = seed;
  for (size_t i = 0; i < buffer.bytes; ++i) {
    state = state * 1664525U + 1013904223U;
    ptr[i] = static_cast<uint8_t>(state >> 24);
  }
  buffer.bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);
}

void clear_bo(NamedBo& buffer) {
  auto* ptr = buffer.bo.map<uint8_t*>();
  std::memset(ptr, 0, buffer.bytes);
  buffer.bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);
}

void write_seed_slot_pattern(NamedBo& seed_bo, int prompt_slot, int token_round,
                             int cache_position) {
  if (prompt_slot < 0 || prompt_slot >= kNumPrompts)
    throw std::runtime_error("prompt slot out of range");
  if (cache_position < 0 || cache_position >= kCacheSeq)
    throw std::runtime_error("cache position out of range");

  auto* ptr = seed_bo.bo.map<uint8_t*>();
  const size_t slot_offset = static_cast<size_t>(prompt_slot) * kHiddenBeats * kWordBytes;
  uint32_t state = mix32(static_cast<uint32_t>(prompt_slot) * 0x10001U ^
                         static_cast<uint32_t>(token_round) * 0x9e3779b9U ^
                         static_cast<uint32_t>(cache_position));
  for (int i = 0; i < kHidden; ++i) {
    state = state * 1103515245U + 12345U;
    ptr[slot_offset + static_cast<size_t>(i)] = static_cast<uint8_t>(state >> 24);
  }
}

uint64_t checksum_final_slot(NamedBo& final_bo, int prompt_slot) {
  if (prompt_slot < 0 || prompt_slot >= kNumPrompts)
    throw std::runtime_error("prompt slot out of range");

  auto* ptr = final_bo.bo.map<const uint8_t*>();
  const size_t slot_offset = static_cast<size_t>(prompt_slot) * kHiddenBeats * kWordBytes;
  uint64_t acc = 1469598103934665603ULL;
  for (int i = 0; i < kHidden; ++i) {
    acc ^= static_cast<uint64_t>(ptr[slot_offset + static_cast<size_t>(i)]);
    acc *= 1099511628211ULL;
  }
  return acc;
}

DeviceBundle make_device_bundle(const std::string& xclbin_path, unsigned device_index) {
  DeviceBundle d{
      xrt::device(device_index),
      {},
      {},
      {},
      {},
      {},
      {},
      {},
      {},
      {},
      {},
      {},
      {},
      {},
      {},  // kv_k_t_hbm
      {},
      {},
      {},
  };
  d.uuid = d.device.load_xclbin(xclbin_path);
  d.k_router0 = open_kernel(d.device, d.uuid, {"c0_router"});
  d.k_c0 = open_kernel(d.device, d.uuid, {"chunk0_stream_ip"});
  d.k_c1 = open_kernel(d.device, d.uuid, {"chunk1_stream_ip"});
  d.k_c2 = open_kernel(d.device, d.uuid, {"chunk2_stream_ip"});
  d.k_router2 = open_kernel(d.device, d.uuid, {"c2_router"});

  // Router-visible hidden-state buffers.
  d.seed_hidden_in = make_bo(d.device, d.k_router0, 0, kSeedBoBytes, "seed_hidden_in");
  d.final_hidden_out = make_bo(d.device, d.k_router2, 4, kFinalBoBytes, "final_hidden_out");
  d.c0_meta_hbm = make_bo(d.device, d.k_c0, 17, kNumLayers * kC0MetaBytesPerLayer, "c0_meta_hbm");
  d.kv_k_hbm_lo = make_bo(d.device, d.k_c0, 18, kC0KvHalfBytes, "c0_kv_k_hbm_lo");
  d.kv_v_hbm_lo = make_bo(d.device, d.k_c0, 19, kC0KvHalfBytes, "c0_kv_v_hbm_lo");
  d.kv_k_hbm_hi = make_bo(d.device, d.k_c0, 20, kC0KvHalfBytes, "c0_kv_k_hbm_hi");
  d.kv_v_hbm_hi = make_bo(d.device, d.k_c0, 21, kC0KvHalfBytes, "c0_kv_v_hbm_hi");
  d.kv_k_t_hbm  = make_bo(d.device, d.k_c0, 22, kC0KvTBytes,     "c0_kv_k_t_hbm");
  clear_bo(d.seed_hidden_in);
  clear_bo(d.final_hidden_out);
  clear_bo(d.c0_meta_hbm);
  clear_bo(d.kv_k_hbm_lo);
  clear_bo(d.kv_v_hbm_lo);
  clear_bo(d.kv_k_hbm_hi);
  clear_bo(d.kv_v_hbm_hi);
  clear_bo(d.kv_k_t_hbm);

  d.c0.banks[0] = make_bo(d.device, d.k_c0, 9, kNumLayers * kC0B0Words * kWordBytes,
                          "c0_b0_all");
  for (int i = 1; i < 8; ++i) {
    d.c0.banks[static_cast<size_t>(i)] =
        make_bo(d.device, d.k_c0, 9 + i, kNumLayers * kC0BankWords * kWordBytes,
                "c0_b" + std::to_string(i) + "_all");
  }
  for (int layer = 0; layer < kNumLayers; ++layer) {
    for (int i = 0; i < 8; ++i) {
      auto* ptr = d.c0.banks[static_cast<size_t>(i)].bo.map<uint8_t*>();
      const size_t layer_bytes = ((i == 0) ? kC0B0Words : kC0BankWords) * kWordBytes;
      uint32_t state = mix32(0xC0000000U ^ static_cast<uint32_t>(layer << 8) ^
                             static_cast<uint32_t>(i));
      for (size_t j = 0; j < layer_bytes; ++j) {
        state = state * 1664525U + 1013904223U;
        ptr[layer * layer_bytes + j] = static_cast<uint8_t>(state >> 24);
      }
    }
  }
  for (int i = 0; i < 8; ++i)
    d.c0.banks[static_cast<size_t>(i)].bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);

  d.c1.fc1_b0 = make_bo(d.device, d.k_c1, 6, kNumLayers * kC1Fc1Words * kWordBytes, "c1_fc1_b0_all");
  d.c1.fc1_bias = make_bo(d.device, d.k_c1, 7, kNumLayers * kC1Fc1BiasWords * kWordBytes, "c1_fc1_bias_all");
  d.c1.ln2_beta = make_bo(d.device, d.k_c1, 8, kNumLayers * kC1LnWords * kWordBytes, "c1_ln2_beta_all");
  d.c1.ln2_gamma = make_bo(d.device, d.k_c1, 9, kNumLayers * kC1LnWords * kWordBytes, "c1_ln2_gamma_all");
  for (int i = 0; i < 7; ++i) {
    d.c1.fc1_tail[static_cast<size_t>(i)] =
        make_bo(d.device, d.k_c1, 10 + i, kNumLayers * kC1Fc1Words * kWordBytes,
                "c1_fc1_b" + std::to_string(i + 1) + "_all");
  }
  fill_bo_pattern(d.c1.fc1_b0, mix32(0xC1000000U));
  fill_bo_pattern(d.c1.fc1_bias, mix32(0xC1010000U));
  fill_bo_pattern(d.c1.ln2_beta, mix32(0xC1020000U));
  fill_bo_pattern(d.c1.ln2_gamma, mix32(0xC1030000U));
  for (int i = 0; i < 7; ++i)
    fill_bo_pattern(d.c1.fc1_tail[static_cast<size_t>(i)],
                    mix32(0xC1100000U ^ static_cast<uint32_t>(i)));

  d.c2.fc2_b0 = make_bo(d.device, d.k_c2, 5, kNumLayers * kC2Fc2Words * kWordBytes, "c2_fc2_b0_all");
  d.c2.fc2_bias = make_bo(d.device, d.k_c2, 6, kNumLayers * kC2Fc2BiasWords * kWordBytes, "c2_fc2_bias_all");
  for (int i = 0; i < 7; ++i) {
    d.c2.fc2_tail[static_cast<size_t>(i)] =
        make_bo(d.device, d.k_c2, 7 + i, kNumLayers * kC2Fc2Words * kWordBytes,
                "c2_fc2_b" + std::to_string(i + 1) + "_all");
  }
  fill_bo_pattern(d.c2.fc2_b0, mix32(0xC2000000U));
  fill_bo_pattern(d.c2.fc2_bias, mix32(0xC2010000U));
  for (int i = 0; i < 7; ++i)
    fill_bo_pattern(d.c2.fc2_tail[static_cast<size_t>(i)],
                    mix32(0xC2100000U ^ static_cast<uint32_t>(i)));

  return d;
}

StepRuns make_step_runs(DeviceBundle& d) {
  return StepRuns{
      xrt::run(d.k_router2),
      xrt::run(d.k_c2),
      xrt::run(d.k_c1),
      xrt::run(d.k_c0),
      xrt::run(d.k_router0),
  };
}

double dur_us(const std::chrono::steady_clock::time_point& t0,
              const std::chrono::steady_clock::time_point& t1) {
  return std::chrono::duration<double, std::micro>(t1 - t0).count();
}

void run_token_round(DeviceBundle& d, StepRuns& runs, int prompt_base,
                     int num_prompts,
                     const std::array<int32_t, kNumPrompts>& cache_positions,
                     StageTimingAccums& timing_accums) {
  runs.rr2.set_arg(4, d.final_hidden_out.bo);
  runs.rr2.set_arg(5, static_cast<uint32_t>(prompt_base));
  runs.rr2.set_arg(6, static_cast<uint32_t>(num_prompts));
  runs.rr2.set_arg(7, static_cast<uint32_t>(kNumLayers));

  runs.r2.set_arg(3, static_cast<uint32_t>(num_prompts));
  runs.r2.set_arg(4, static_cast<uint32_t>(kNumLayers));
  runs.r2.set_arg(5, d.c2.fc2_b0.bo);
  runs.r2.set_arg(6, d.c2.fc2_bias.bo);
  for (int i = 0; i < 7; ++i)
    runs.r2.set_arg(7 + i, d.c2.fc2_tail[static_cast<size_t>(i)].bo);

  runs.r1.set_arg(4, static_cast<uint32_t>(num_prompts));
  runs.r1.set_arg(5, static_cast<uint32_t>(kNumLayers));
  runs.r1.set_arg(6, d.c1.fc1_b0.bo);
  runs.r1.set_arg(7, d.c1.fc1_bias.bo);
  runs.r1.set_arg(8, d.c1.ln2_beta.bo);
  runs.r1.set_arg(9, d.c1.ln2_gamma.bo);
  for (int i = 0; i < 7; ++i)
    runs.r1.set_arg(10 + i, d.c1.fc1_tail[static_cast<size_t>(i)].bo);

  runs.r0.set_arg(3, static_cast<uint32_t>(prompt_base));
  runs.r0.set_arg(4, static_cast<uint32_t>(num_prompts));
  runs.r0.set_arg(5, static_cast<uint32_t>(kNumLayers));
  runs.r0.set_arg(6, static_cast<uint32_t>(cache_positions[0]));
  runs.r0.set_arg(7, static_cast<uint32_t>(cache_positions[1]));
  runs.r0.set_arg(8, static_cast<uint32_t>(cache_positions[2]));
  for (int i = 0; i < 8; ++i)
    runs.r0.set_arg(9 + i, d.c0.banks[static_cast<size_t>(i)].bo);
  runs.r0.set_arg(17, d.c0_meta_hbm.bo);
  runs.r0.set_arg(18, d.kv_k_hbm_lo.bo);
  runs.r0.set_arg(19, d.kv_v_hbm_lo.bo);
  runs.r0.set_arg(20, d.kv_k_hbm_hi.bo);
  runs.r0.set_arg(21, d.kv_v_hbm_hi.bo);
  runs.r0.set_arg(22, d.kv_k_t_hbm.bo);

  runs.rr0.set_arg(0, d.seed_hidden_in.bo);
  runs.rr0.set_arg(5, static_cast<uint32_t>(prompt_base));
  runs.rr0.set_arg(6, static_cast<uint32_t>(num_prompts));
  runs.rr0.set_arg(7, static_cast<uint32_t>(kNumLayers));

  // Start downstream first so stream consumers are ready before the source.
  runs.rr2.start();
  runs.r2.start();
  runs.r1.start();
  runs.r0.start();
  runs.rr0.start();

  const auto t0 = std::chrono::steady_clock::now();
  runs.rr0.wait();
  const auto t1 = std::chrono::steady_clock::now();
  runs.r0.wait();
  const auto t2 = std::chrono::steady_clock::now();
  runs.r1.wait();
  const auto t3 = std::chrono::steady_clock::now();
  runs.r2.wait();
  const auto t4 = std::chrono::steady_clock::now();
  runs.rr2.wait();
  const auto t5 = std::chrono::steady_clock::now();

  timing_accums.us_router0 += dur_us(t0, t1);
  timing_accums.us_chunk0 += dur_us(t1, t2);
  timing_accums.us_chunk1 += dur_us(t2, t3);
  timing_accums.us_chunk2 += dur_us(t3, t4);
  timing_accums.us_router2 += dur_us(t4, t5);
  ++timing_accums.rounds;
}

int run_main(int argc, char** argv) {
  try {
    if (argc < 2 || argc > 4) {
      std::cerr << "Usage: " << argv[0]
                << " <xclbin> [max_new_tokens=128] [device_index=0]\n";
      return 1;
    }

    const std::string xclbin = argv[1];
    const int max_new_tokens =
        (argc >= 3) ? std::stoi(argv[2]) : kMaxNewTokensDefault;
    const unsigned device_index =
        (argc >= 4) ? static_cast<unsigned>(std::stoul(argv[3])) : 0u;
    if (max_new_tokens <= 0)
      throw std::runtime_error("max_new_tokens must be positive");
    if (max_new_tokens > kCacheSeq)
      throw std::runtime_error("max_new_tokens must be <= cache length 128");

    auto device = make_device_bundle(xclbin, device_index);
    auto runs = make_step_runs(device);

    double device_only_ms = 0.0;
    double round_wall_ms = 0.0;
    StageTimingAccums timing_accums;

    // Synthetic steady-state generation loop:
    //   - write 3 fresh seed vectors into HBM[31]
    //   - launch one on-device 24-layer x 3-prompt round
    //   - read 3 final vectors from HBM[0] and accumulate checksums
    std::array<uint64_t, kNumPrompts> final_checksums{};
    for (int gen_round = 0; gen_round < max_new_tokens; ++gen_round) {
      const std::string round_tag = std::to_string(gen_round);
      for (int p = 0; p < kNumPrompts; ++p) {
        write_seed_slot_pattern(device.seed_hidden_in, p, gen_round, gen_round);
      }
      {
        xrt::profile::user_range r_dma_in("seed_dma", round_tag.c_str());
        device.seed_hidden_in.bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);
      }

      const auto round_wall_t0 = std::chrono::steady_clock::now();
      const auto device_t0 = std::chrono::steady_clock::now();
      std::array<int32_t, kNumPrompts> cache_positions{
          gen_round, gen_round, gen_round};
      {
        xrt::profile::user_range r_kern("token_round", round_tag.c_str());
        run_token_round(device, runs, 0, kNumPrompts, cache_positions, timing_accums);
      }
      const auto device_t1 = std::chrono::steady_clock::now();
      device_only_ms +=
          std::chrono::duration<double, std::milli>(device_t1 - device_t0).count();

      {
        xrt::profile::user_range r_dma_out("final_dma", round_tag.c_str());
        device.final_hidden_out.bo.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
      }
      {
        xrt::profile::user_range r_cpu("cpu_decode", round_tag.c_str());
        for (int p = 0; p < kNumPrompts; ++p) {
          final_checksums[static_cast<size_t>(p)] ^= checksum_final_slot(device.final_hidden_out, p);
        }
      }
      const auto round_wall_t1 = std::chrono::steady_clock::now();
      round_wall_ms +=
          std::chrono::duration<double, std::milli>(round_wall_t1 - round_wall_t0).count();
    }

    const double timed_rounds = static_cast<double>(max_new_tokens);
    const double timed_tokens_3prompt =
        static_cast<double>(kNumPrompts) * timed_rounds;
    const double device_only_tok_s_3prompt =
        (device_only_ms > 0.0) ? (timed_tokens_3prompt * 1000.0 / device_only_ms) : 0.0;
    (void)round_wall_ms;
    (void)timing_accums;
    (void)final_checksums;
    std::cout << std::fixed << std::setprecision(3)
              << device_only_tok_s_3prompt << " tok/s\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << "\n";
    return 1;
  }
}

}  // namespace

int main(int argc, char** argv) {
  return run_main(argc, argv);
}
