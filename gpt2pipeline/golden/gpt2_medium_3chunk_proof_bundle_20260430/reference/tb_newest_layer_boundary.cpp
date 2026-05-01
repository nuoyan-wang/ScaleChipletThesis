#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <pthread.h>
#include <stdexcept>
#include <string>
#include <vector>

#include <ap_fixed.h>
#include <ap_int.h>

namespace fs = std::filesystem;

void kernel_0(
    ap_uint<512> arg0000[1][1][1024][2], ap_fixed<8, 4> arg0001[1],
    ap_int<1> arg0002[1][1][1024][1024], ap_fixed<8, 4> arg0003[1],
    ap_int<1> arg0004[1][1][1024][1024], ap_fixed<8, 4> arg0005[1],
    ap_int<1> arg0006[1][1][1024][1024], ap_fixed<8, 4> arg0007[1],
    ap_int<1> arg0008[1][1][1024][1024], ap_fixed<8, 4> arg0009[1],
    ap_int<1> arg0010[1][1][1024][1024], ap_fixed<8, 4> arg0011[1],
    ap_int<1> arg0012[1][1][1024][1024], ap_fixed<8, 4> arg0013[1],
    ap_int<1> arg0014[1][1][1024][1024], ap_fixed<8, 4> arg0015[1],
    ap_int<1> arg0016[1][1][1024][1024], ap_fixed<8, 4> arg0017[1],
    ap_int<1> arg0018[1][1][1024][1024], ap_fixed<8, 4> arg0019[1],
    ap_int<1> arg0020[1][1][1024][1024], ap_fixed<8, 4> arg0021[1],
    ap_int<1> arg0022[1][1][1024][1024], ap_fixed<8, 4> arg0023[1],
    ap_int<1> arg0024[1][1][1024][1024], ap_fixed<8, 4> arg0025[1],
    ap_int<1> arg0026[1][1][1024][1024], ap_fixed<8, 4> arg0027[1],
    ap_int<1> arg0028[1][1][1024][1024], ap_fixed<8, 4> arg0029[1],
    ap_int<1> arg0030[1][1][1024][1024], ap_fixed<8, 4> arg0031[1],
    ap_int<1> arg0032[1][1][1024][1024], ap_fixed<8, 4> arg0033[1],
    ap_int<1> arg0034[1][1][1024][1024], ap_fixed<8, 4> arg0035[1],
    ap_int<1> arg0036[1][1][1024][1024], ap_fixed<8, 4> arg0037[1],
    ap_int<1> arg0038[1][1][1024][1024], ap_fixed<8, 4> arg0039[1],
    ap_int<1> arg0040[1][1][1024][1024], ap_fixed<8, 4> arg0041[1],
    ap_int<1> arg0042[1][1][1024][1024], ap_fixed<8, 4> arg0043[1],
    ap_int<1> arg0044[1][1][1024][1024], ap_fixed<8, 4> arg0045[1],
    ap_int<1> arg0046[1][1][1024][1024], ap_fixed<8, 4> arg0047[1],
    ap_uint<512> arg0048[1][1][16], ap_int<32> arg0049[1],
    ap_fixed<8, 4> arg0050[1][1][1][128], ap_uint<512> arg0051[1][16][128][1],
    ap_uint<512> arg0052[1][16][128][1], ap_uint<512> arg0053[1][1][16],
    ap_uint<512> arg0054[1][16][128][1], ap_uint<512> arg0055[1][16][128][1],
    ap_uint<512> arg0056[16], ap_uint<512> arg0057[4096][16], ap_uint<512> arg0058[64],
    ap_uint<512> arg0059[1024][64], ap_uint<512> arg0060[16], ap_uint<512> arg0061[16],
    ap_uint<512> arg0062[16], ap_uint<512> arg0063[1024][16], ap_uint<512> arg0064[48],
    ap_uint<512> arg0065[1024][48], ap_uint<512> arg0066[16], ap_uint<512> arg0067[16],
    ap_uint<512> arg0068[16][128][1], ap_uint<512> arg0069[16][128][1],
    ap_uint<512> arg0070[1][16][64][2]);

static ap_uint<512> arg0000[1][1][1024][2];
static ap_fixed<8, 4> arg0001[1];
static ap_int<1> arg0002[1][1][1024][1024];
static ap_fixed<8, 4> arg0003[1];
static ap_int<1> arg0004[1][1][1024][1024];
static ap_fixed<8, 4> arg0005[1];
static ap_int<1> arg0006[1][1][1024][1024];
static ap_fixed<8, 4> arg0007[1];
static ap_int<1> arg0008[1][1][1024][1024];
static ap_fixed<8, 4> arg0009[1];
static ap_int<1> arg0010[1][1][1024][1024];
static ap_fixed<8, 4> arg0011[1];
static ap_int<1> arg0012[1][1][1024][1024];
static ap_fixed<8, 4> arg0013[1];
static ap_int<1> arg0014[1][1][1024][1024];
static ap_fixed<8, 4> arg0015[1];
static ap_int<1> arg0016[1][1][1024][1024];
static ap_fixed<8, 4> arg0017[1];
static ap_int<1> arg0018[1][1][1024][1024];
static ap_fixed<8, 4> arg0019[1];
static ap_int<1> arg0020[1][1][1024][1024];
static ap_fixed<8, 4> arg0021[1];
static ap_int<1> arg0022[1][1][1024][1024];
static ap_fixed<8, 4> arg0023[1];
static ap_int<1> arg0024[1][1][1024][1024];
static ap_fixed<8, 4> arg0025[1];
static ap_int<1> arg0026[1][1][1024][1024];
static ap_fixed<8, 4> arg0027[1];
static ap_int<1> arg0028[1][1][1024][1024];
static ap_fixed<8, 4> arg0029[1];
static ap_int<1> arg0030[1][1][1024][1024];
static ap_fixed<8, 4> arg0031[1];
static ap_int<1> arg0032[1][1][1024][1024];
static ap_fixed<8, 4> arg0033[1];
static ap_int<1> arg0034[1][1][1024][1024];
static ap_fixed<8, 4> arg0035[1];
static ap_int<1> arg0036[1][1][1024][1024];
static ap_fixed<8, 4> arg0037[1];
static ap_int<1> arg0038[1][1][1024][1024];
static ap_fixed<8, 4> arg0039[1];
static ap_int<1> arg0040[1][1][1024][1024];
static ap_fixed<8, 4> arg0041[1];
static ap_int<1> arg0042[1][1][1024][1024];
static ap_fixed<8, 4> arg0043[1];
static ap_int<1> arg0044[1][1][1024][1024];
static ap_fixed<8, 4> arg0045[1];
static ap_int<1> arg0046[1][1][1024][1024];
static ap_fixed<8, 4> arg0047[1];
static ap_uint<512> arg0048[1][1][16];
static ap_int<32> arg0049[1];
static ap_fixed<8, 4> arg0050[1][1][1][128];
static ap_uint<512> arg0051[1][16][128][1];
static ap_uint<512> arg0052[1][16][128][1];
static ap_uint<512> arg0053[1][1][16];
static ap_uint<512> arg0054[1][16][128][1];
static ap_uint<512> arg0055[1][16][128][1];
static ap_uint<512> arg0056[16];
static ap_uint<512> arg0057[4096][16];
static ap_uint<512> arg0058[64];
static ap_uint<512> arg0059[1024][64];
static ap_uint<512> arg0060[16];
static ap_uint<512> arg0061[16];
static ap_uint<512> arg0062[16];
static ap_uint<512> arg0063[1024][16];
static ap_uint<512> arg0064[48];
static ap_uint<512> arg0065[1024][48];
static ap_uint<512> arg0066[16];
static ap_uint<512> arg0067[16];
static ap_uint<512> arg0068[16][128][1];
static ap_uint<512> arg0069[16][128][1];
static ap_uint<512> arg0070[1][16][64][2];

static std::vector<uint8_t> read_bytes(const fs::path &path) {
  std::ifstream f(path, std::ios::binary);
  if (!f) throw std::runtime_error("failed to open " + path.string());
  return std::vector<uint8_t>(std::istreambuf_iterator<char>(f), {});
}

template <typename T>
static std::vector<T> read_pod(const fs::path &path) {
  std::ifstream f(path, std::ios::binary);
  if (!f) throw std::runtime_error("failed to open " + path.string());
  std::vector<T> out;
  T value;
  while (f.read(reinterpret_cast<char *>(&value), sizeof(T))) out.push_back(value);
  return out;
}

static ap_fixed<8, 4> float_to_fxp8(float x) {
  int scaled = static_cast<int>(std::lrint(x * 16.0f));
  scaled = std::max(-128, std::min(127, scaled));
  ap_fixed<8, 4> v;
  v.range(7, 0) = static_cast<uint8_t>(static_cast<int8_t>(scaled));
  return v;
}

static void pack_word_from_bytes(const uint8_t *src, ap_uint<512> &dst) {
  dst = 0;
  for (int b = 0; b < 64; ++b)
    dst.range(b * 8 + 7, b * 8) = src[b];
}

static void unpack_word_to_bytes(const ap_uint<512> &src, uint8_t *dst) {
  for (int b = 0; b < 64; ++b)
    dst[b] = static_cast<uint8_t>(src.range(b * 8 + 7, b * 8).to_uint());
}

static void pack_linear_words(const std::vector<uint8_t> &raw, ap_uint<512> *dst,
                              size_t word_count) {
  if (raw.size() != word_count * 64)
    throw std::runtime_error("unexpected packed byte count");
  for (size_t i = 0; i < word_count; ++i)
    pack_word_from_bytes(raw.data() + i * 64, dst[i]);
}

static std::vector<uint8_t> unpack_linear_words(const ap_uint<512> *src, size_t word_count) {
  std::vector<uint8_t> raw(word_count * 64);
  for (size_t i = 0; i < word_count; ++i)
    unpack_word_to_bytes(src[i], raw.data() + i * 64);
  return raw;
}

static void build_causal_masks(int position) {
  std::memset(arg0000, 0, sizeof(arg0000));
  for (int i = 0; i <= position; ++i) {
    const int word = i / 512;
    const int bit = i % 512;
    arg0000[0][0][127][word][bit] = 1;
  }
  arg0001[0] = float_to_fxp8(-8.0f);

  ap_int<1> (*masks[23])[1][1024][1024] = {
      arg0002, arg0004, arg0006, arg0008, arg0010, arg0012, arg0014, arg0016,
      arg0018, arg0020, arg0022, arg0024, arg0026, arg0028, arg0030, arg0032,
      arg0034, arg0036, arg0038, arg0040, arg0042, arg0044, arg0046};
  ap_fixed<8, 4> *fills[23] = {
      arg0003, arg0005, arg0007, arg0009, arg0011, arg0013, arg0015, arg0017,
      arg0019, arg0021, arg0023, arg0025, arg0027, arg0029, arg0031, arg0033,
      arg0035, arg0037, arg0039, arg0041, arg0043, arg0045, arg0047};
  for (int p = 0; p < 23; ++p) {
    std::memset(masks[p], 0, sizeof(arg0002));
    for (int i = 0; i < 128; ++i) (*masks[p])[0][0][127][i] = (i <= (p + 1)) ? 1 : 0;
    fills[p][0] = float_to_fxp8(-8.0f);
  }
}

static void load_seed_hidden(const fs::path &io_dir, int prompt_idx, int position) {
  int token_id = -1;
  if (const char *token_override = std::getenv("MODEL_TOKEN_ID")) {
    if (token_override[0]) token_id = std::stoi(token_override);
  }
  if (token_id < 0) {
    auto prompt_ids =
        read_pod<int32_t>(io_dir / ("prompt_" + std::to_string(prompt_idx) + "_token_ids.bin"));
    token_id = prompt_ids.at(position);
  }
  auto wte = read_pod<float>(io_dir / "wte_f32.bin");
  auto wpe = read_pod<float>(io_dir / "wpe_f32.bin");
  const char *quiet = std::getenv("MODEL_QUIET");
  if (!(quiet && quiet[0])) {
    std::printf("tb_newest_layer_boundary prompt=%d position=%d token_id=%d\n",
                prompt_idx, position, token_id);
  }
  std::vector<uint8_t> seed_raw(1024);
  for (int i = 0; i < 1024; ++i) {
    ap_fixed<8, 4> fx = float_to_fxp8(wte[token_id * 1024 + i] + wpe[position * 1024 + i]);
    seed_raw[i] = static_cast<uint8_t>(fx.range(7, 0).to_uint());
  }
  for (int w = 0; w < 16; ++w)
    pack_word_from_bytes(seed_raw.data() + w * 64, arg0048[0][0][w]);
}

static void load_hidden_override(const fs::path &path) {
  auto raw = read_bytes(path);
  if (raw.size() != 1024)
    throw std::runtime_error("hidden override must be exactly 1024 bytes");
  for (int w = 0; w < 16; ++w)
    pack_word_from_bytes(raw.data() + w * 64, arg0048[0][0][w]);
}

static void load_kv_cache(const fs::path &k_path, const fs::path &v_path) {
  pack_linear_words(read_bytes(k_path), &arg0051[0][0][0][0], 16 * 128);
  pack_linear_words(read_bytes(v_path), &arg0052[0][0][0][0], 16 * 128);
}

static void save_kv_cache(const fs::path &k_path, const fs::path &v_path) {
  auto k_raw = unpack_linear_words(&arg0054[0][0][0][0], 16 * 128);
  auto v_raw = unpack_linear_words(&arg0055[0][0][0][0], 16 * 128);
  std::ofstream fk(k_path, std::ios::binary);
  if (!fk) throw std::runtime_error("failed to write " + k_path.string());
  fk.write(reinterpret_cast<const char *>(k_raw.data()),
           static_cast<std::streamsize>(k_raw.size()));
  std::ofstream fv(v_path, std::ios::binary);
  if (!fv) throw std::runtime_error("failed to write " + v_path.string());
  fv.write(reinterpret_cast<const char *>(v_raw.data()),
           static_cast<std::streamsize>(v_raw.size()));
}

static void load_layer_weights(const fs::path &io_dir, int layer) {
  fs::path layer_dir =
      io_dir / ("layer_" + std::string(layer < 10 ? "0" : "") + std::to_string(layer));

  for (int bank = 0; bank < 8; ++bank) {
    auto raw = read_bytes(layer_dir / ("c0_b" + std::to_string(bank) + ".bin"));
    size_t off = 0;
    for (int r = 0; r < 128; ++r) {
      int row = bank + 8 * r;
      for (int w = 0; w < 48; ++w, off += 64)
        pack_word_from_bytes(raw.data() + off, arg0065[row][w]);
    }
    for (int r = 0; r < 128; ++r) {
      int row = bank + 8 * r;
      for (int w = 0; w < 16; ++w, off += 64)
        pack_word_from_bytes(raw.data() + off, arg0063[row][w]);
    }
    if (bank == 0) {
      for (int w = 0; w < 48; ++w, off += 64)
        pack_word_from_bytes(raw.data() + off, arg0064[w]);
      for (int w = 0; w < 16; ++w, off += 64)
        pack_word_from_bytes(raw.data() + off, arg0062[w]);
      for (int w = 0; w < 16; ++w, off += 64)
        pack_word_from_bytes(raw.data() + off, arg0066[w]);
      for (int w = 0; w < 16; ++w, off += 64)
        pack_word_from_bytes(raw.data() + off, arg0067[w]);
    }
  }

  for (int bank = 0; bank < 8; ++bank) {
    auto raw = read_bytes(layer_dir / ("c1_fc1_b" + std::to_string(bank) + ".bin"));
    size_t off = 0;
    for (int r = 0; r < 128; ++r) {
      int row = bank + 8 * r;
      for (int w = 0; w < 64; ++w, off += 64)
        pack_word_from_bytes(raw.data() + off, arg0059[row][w]);
    }
  }
  pack_linear_words(read_bytes(layer_dir / "c1_fc1_bias.bin"), arg0058, 64);
  pack_linear_words(read_bytes(layer_dir / "c1_ln2_beta.bin"), arg0060, 16);
  pack_linear_words(read_bytes(layer_dir / "c1_ln2_gamma.bin"), arg0061, 16);

  for (int bank = 0; bank < 8; ++bank) {
    auto raw = read_bytes(layer_dir / ("c2_fc2_b" + std::to_string(bank) + ".bin"));
    size_t off = 0;
    for (int r = 0; r < 512; ++r) {
      int row = bank + 8 * r;
      for (int w = 0; w < 16; ++w, off += 64)
        pack_word_from_bytes(raw.data() + off, arg0057[row][w]);
    }
  }
  pack_linear_words(read_bytes(layer_dir / "c2_fc2_bias.bin"), arg0056, 16);
}

struct ThreadTask {
  std::function<void()> fn;
};

static void *thread_entry(void *arg) {
  auto *task = static_cast<ThreadTask *>(arg);
  task->fn();
  return nullptr;
}

static void run_with_large_stack(std::function<void()> fn,
                                 size_t stack_bytes = 512ull * 1024ull * 1024ull) {
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr,
                            std::max(stack_bytes, static_cast<size_t>(PTHREAD_STACK_MIN)));
  auto *task = new ThreadTask{std::move(fn)};
  pthread_t tid{};
  if (pthread_create(&tid, &attr, thread_entry, task) != 0)
    throw std::runtime_error("pthread_create failed");
  pthread_attr_destroy(&attr);
  pthread_join(tid, nullptr);
  delete task;
}

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: tb_newest_layer_boundary <io_dir> <layer> <prompt> <position>\n";
    return 1;
  }
  fs::path io_dir = fs::absolute(argv[1]);
  int layer = std::stoi(argv[2]);
  int prompt = std::stoi(argv[3]);
  int position = std::stoi(argv[4]);
  if (position >= 24) {
    std::cerr << "position must be < 24 for current mask ABI\n";
    return 1;
  }

  std::memset(arg0050, 0, sizeof(arg0050));
  std::memset(arg0051, 0, sizeof(arg0051));
  std::memset(arg0052, 0, sizeof(arg0052));
  std::memset(arg0053, 0, sizeof(arg0053));
  std::memset(arg0054, 0, sizeof(arg0054));
  std::memset(arg0055, 0, sizeof(arg0055));
  std::memset(arg0068, 0, sizeof(arg0068));
  std::memset(arg0069, 0, sizeof(arg0069));
  std::memset(arg0070, 0, sizeof(arg0070));
  build_causal_masks(position);
  if (const char *hidden_override = std::getenv("MODEL_HIDDEN_IN_FILE")) {
    if (hidden_override[0]) {
      load_hidden_override(fs::absolute(hidden_override));
    } else {
      load_seed_hidden(io_dir, prompt, position);
    }
  } else {
    load_seed_hidden(io_dir, prompt, position);
  }
  load_layer_weights(io_dir, layer);
  if (const char *kvk = std::getenv("MODEL_KV_K_IN_FILE")) {
    const char *kvv = std::getenv("MODEL_KV_V_IN_FILE");
    if (!kvv || !kvv[0])
      throw std::runtime_error("MODEL_KV_V_IN_FILE must be set with MODEL_KV_K_IN_FILE");
    if (kvk[0]) load_kv_cache(fs::absolute(kvk), fs::absolute(kvv));
  }
  arg0049[0] = position;

  run_with_large_stack([]() {
    kernel_0(arg0000, arg0001, arg0002, arg0003, arg0004, arg0005, arg0006,
             arg0007, arg0008, arg0009, arg0010, arg0011, arg0012, arg0013,
             arg0014, arg0015, arg0016, arg0017, arg0018, arg0019, arg0020,
             arg0021, arg0022, arg0023, arg0024, arg0025, arg0026, arg0027,
             arg0028, arg0029, arg0030, arg0031, arg0032, arg0033, arg0034,
             arg0035, arg0036, arg0037, arg0038, arg0039, arg0040, arg0041,
             arg0042, arg0043, arg0044, arg0045, arg0046, arg0047, arg0048,
             arg0049, arg0050, arg0051, arg0052, arg0053, arg0054, arg0055,
             arg0056, arg0057, arg0058, arg0059, arg0060, arg0061, arg0062,
             arg0063, arg0064, arg0065, arg0066, arg0067, arg0068, arg0069,
             arg0070);
  });

  const char *quiet = std::getenv("MODEL_QUIET");
  if (!(quiet && quiet[0])) {
    std::printf("tb_newest_layer_boundary output_head=[");
    for (int i = 0; i < 8; ++i) {
      if (i) std::printf(", ");
      int word = i / 64;
      int byte = i % 64;
      ap_fixed<8, 4> v;
      v.range(7, 0) = arg0053[0][0][word].range(byte * 8 + 7, byte * 8);
      std::printf("%.4f", (float)v);
    }
    std::printf("]\n");
  }
  if (const char *kvk_out = std::getenv("MODEL_KV_K_OUT_FILE")) {
    const char *kvv_out = std::getenv("MODEL_KV_V_OUT_FILE");
    if (!kvv_out || !kvv_out[0])
      throw std::runtime_error("MODEL_KV_V_OUT_FILE must be set with MODEL_KV_K_OUT_FILE");
    if (kvk_out[0]) save_kv_cache(fs::absolute(kvk_out), fs::absolute(kvv_out));
  }
  return 0;
}
