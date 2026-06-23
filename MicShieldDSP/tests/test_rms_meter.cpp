#include "micshield/dsp/RmsMeter.hpp"
#include <array>
#include <cassert>

void test_rms_meter() {
    micshield::dsp::RmsMeter meter;
    std::array<float, 4> x{1.0f, -1.0f, 1.0f, -1.0f};
    const float db = meter.process(x.data(), x.size());
    assert(db > -0.1f && db < 0.1f);
}

int main();
