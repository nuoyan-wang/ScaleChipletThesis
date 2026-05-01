#include <cmath>

namespace hls {
float exp(float x) { return std::exp(x); }
float rsqrt(float x) { return 1.0f / std::sqrt(x); }
float divide(float a, float b) { return a / b; }
float tanh(float x) { return std::tanh(x); }
}
