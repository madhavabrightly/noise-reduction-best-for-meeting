#include "micshield/dsp/Limiter.hpp"
#include <array>
#include <cassert>
#include <cmath>

void test_limiter() {
    micshield::dsp::Limiter limiter;
    limiter.prepare(-1.0f);
    std::array<float, 4> x{2.0f, -2.0f, 0.5f, -0.5f};
    limiter.process(x.data(), x.size());
    for (float v : x) assert(std::abs(v) <= 0.892f);
}

extern void test_ring_buffer();
extern void test_dc_blocker();
extern void test_biquad();
extern void test_rms_meter();

int main() {
    test_ring_buffer();
    test_dc_blocker();
    test_biquad();
    test_rms_meter();
    test_limiter();
    return 0;
}
