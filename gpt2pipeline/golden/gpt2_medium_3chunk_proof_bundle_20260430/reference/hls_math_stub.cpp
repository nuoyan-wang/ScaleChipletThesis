// Software stubs for hls:: math functions declared in hls_math.h but not
// defined in the HLS include-only headers.  During CSIM Vitis HLS links
// libhlsmathsim.so; here we redirect to <cmath> equivalents.
// Both monolithic and 3-chunk use the same stubs, so comparisons remain valid.

#include <cmath>

namespace hls {
    float  tanh(float  x) { return std::tanh(x); }
    double tanh(double x) { return std::tanh(x); }
    float  tanhf(float x) { return std::tanh(x); }

    float  exp(float  x) { return std::exp(x); }
    double exp(double x) { return std::exp(x); }

    float  log(float  x) { return std::log(x); }
    double log(double x) { return std::log(x); }

    float  sqrt(float  x) { return std::sqrt(x); }
    double sqrt(double x) { return std::sqrt(x); }

    float  rsqrt(float  x) { return 1.0f / std::sqrt(x); }
    double rsqrt(double x) { return 1.0  / std::sqrt(x); }

    float  pow(float  x, float  y) { return std::pow(x, y); }
    double pow(double x, double y) { return std::pow(x, y); }

    float  divide(float  a, float  b) { return a / b; }
    double divide(double a, double b) { return a / b; }
}
