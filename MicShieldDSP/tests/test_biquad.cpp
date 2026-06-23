#include "micshield/dsp/BiquadFilter.hpp"
#include <array>
#include <cassert>
#include <cmath>

void test_biquad() {
    micshield::dsp::BiquadFilter f;
    f.setHighPass(48000.0, 80.0, 0.707);
    std::array<float, 128> x{};
    x[0] = 1.0f;
    f.process(x.data(), x.size());
    for (float v : x) assert(std::isfinite(v));
}

int main();
