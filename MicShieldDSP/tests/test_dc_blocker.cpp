#include "micshield/dsp/DCBlocker.hpp"
#include <array>
#include <cassert>
#include <cmath>

void test_dc_blocker() {
    micshield::dsp::DCBlocker dc;
    dc.prepare(48000.0);
    std::array<float, 512> x{};
    x.fill(0.25f);
    dc.process(x.data(), x.size());
    assert(std::isfinite(x.back()));
    assert(std::abs(x.back()) < 0.25f);
}

int main();
