#pragma once

#include "../stream_abi_common.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <pthread.h>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace fs = std::filesystem;

using packed_fxp64_t = ap_uint<512>;

constexpr int SH_NUM_PROMPTS = 3;
constexpr int SH_HIDDEN = 1024;
constexpr int SH_FC1 = 4096;
constexpr int SH_HIDDEN_BEATS = SH_HIDDEN / 32;
constexpr int SH_FC1_BEATS = SH_FC1 / 32;
constexpr size_t SH_WORD_BYTES_256 = 32;
constexpr size_t SH_WORD_BYTES_512 = 64;
constexpr size_t SH_C0_KV_HALF_SLICE_WORDS = 8 * 128 * 2;
constexpr size_t SH_C0_KVT_SLICE_WORDS = 16 * 64 * 4;
constexpr size_t SH_C0_KV_SLICES = 24 * 3;

inline std::vector<uint8_t> read_file(const fs::path& path) {
  std::ifstream file(path, std::ios::binary);
  if (!file)
    throw std::runtime_error("failed to open " + path.string());
  return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
}

inline void write_file(const fs::path& path, const std::vector<uint8_t>& bytes) {
  std::ofstream file(path, std::ios::binary);
  if (!file)
    throw std::runtime_error("failed to write " + path.string());
  file.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
}

template <typename T>
constexpr size_t packed_word_bytes() {
  if constexpr (std::is_same_v<T, packed_fxp32_t>)
    return SH_WORD_BYTES_256;
  else if constexpr (std::is_same_v<T, packed_fxp64_t>)
    return SH_WORD_BYTES_512;
  else
    return 0;
}

template <typename T>
inline std::vector<T> bytes_to_packed(const std::vector<uint8_t>& bytes) {
  constexpr size_t kWordBytes = packed_word_bytes<T>();
  static_assert(kWordBytes != 0, "unsupported packed word type");
  if ((bytes.size() % kWordBytes) != 0)
    throw std::runtime_error("packed bytes not word aligned");
  std::vector<T> out(bytes.size() / kWordBytes);
  for (size_t w = 0; w < out.size(); ++w) {
    T word = 0;
    for (size_t b = 0; b < kWordBytes; ++b)
      word.range(static_cast<int>(b * 8 + 7), static_cast<int>(b * 8)) = bytes[w * kWordBytes + b];
    out[w] = word;
  }
  return out;
}

template <typename T>
inline std::vector<uint8_t> packed_to_bytes(const std::vector<T>& words) {
  constexpr size_t kWordBytes = packed_word_bytes<T>();
  static_assert(kWordBytes != 0, "unsupported packed word type");
  std::vector<uint8_t> out(words.size() * kWordBytes);
  for (size_t w = 0; w < words.size(); ++w) {
    for (size_t b = 0; b < kWordBytes; ++b)
      out[w * kWordBytes + b] =
          static_cast<uint8_t>(words[w].range(static_cast<int>(b * 8 + 7), static_cast<int>(b * 8)).to_uint());
  }
  return out;
}

inline void bytes_to_stream(const std::vector<uint8_t>& bytes, hls::stream<axis256_t>& out) {
  if ((bytes.size() % SH_WORD_BYTES_256) != 0)
    throw std::runtime_error("stream bytes not 256b aligned");
  for (size_t off = 0; off < bytes.size(); off += SH_WORD_BYTES_256) {
    axis256_t word = 0;
    for (size_t b = 0; b < SH_WORD_BYTES_256; ++b)
      word.range(static_cast<int>(b * 8 + 7), static_cast<int>(b * 8)) = bytes[off + b];
    out.write(word);
  }
}

inline std::vector<uint8_t> stream_to_bytes(hls::stream<axis256_t>& in, size_t beat_count) {
  std::vector<uint8_t> out(beat_count * SH_WORD_BYTES_256);
  for (size_t beat = 0; beat < beat_count; ++beat) {
    axis256_t word = in.read();
    for (size_t b = 0; b < SH_WORD_BYTES_256; ++b)
      out[beat * SH_WORD_BYTES_256 + b] =
          static_cast<uint8_t>(word.range(static_cast<int>(b * 8 + 7), static_cast<int>(b * 8)).to_uint());
  }
  return out;
}

inline std::vector<uint8_t> read_exact_bytes(const fs::path& path, size_t expected) {
  auto bytes = read_file(path);
  if (bytes.size() != expected)
    throw std::runtime_error("unexpected byte count in " + path.string());
  return bytes;
}

inline int8_t float_to_fxp8_scaled(float value) {
  int scaled = static_cast<int>(std::lrint(value * 16.0f));
  scaled = std::max(-128, std::min(127, scaled));
  return static_cast<int8_t>(scaled);
}

inline float fxp8_scaled_to_float(uint8_t raw) {
  return static_cast<float>(static_cast<int8_t>(raw)) / 16.0f;
}

inline uint64_t fnv1a_u8(const uint8_t* data, size_t size) {
  uint64_t acc = 1469598103934665603ull;
  for (size_t i = 0; i < size; ++i) {
    acc ^= static_cast<uint64_t>(data[i]);
    acc *= 1099511628211ull;
  }
  return acc;
}

inline std::vector<uint8_t> make_zero_bytes(size_t n) {
  return std::vector<uint8_t>(n, 0);
}

struct ThreadTask {
  std::function<void()> fn;
};

inline void* stage_thread_entry(void* arg) {
  auto* task = static_cast<ThreadTask*>(arg);
  task->fn();
  return nullptr;
}

inline void run_with_large_stack(std::function<void()> fn, size_t stack_bytes = 512ull * 1024ull * 1024ull) {
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, std::max(stack_bytes, static_cast<size_t>(PTHREAD_STACK_MIN)));
  auto* task = new ThreadTask{std::move(fn)};
  pthread_t tid{};
  if (pthread_create(&tid, &attr, stage_thread_entry, task) != 0) {
    pthread_attr_destroy(&attr);
    delete task;
    throw std::runtime_error("pthread_create failed");
  }
  pthread_attr_destroy(&attr);
  pthread_join(tid, nullptr);
  delete task;
}
