#include "micshield/dsp/RmsMeter.hpp"
#include "micshield/utils/MathUtils.hpp"

#include <cmath>

namespace micshield::dsp {

float RmsMeter::process(const float* samples, size_t count) noexcept {
    double sum = 0.0;
    for (size_t i = 0; i < count; ++i) {
        const float x = utils::sanitizeFloat(samples[i]);
        sum += static_cast<double>(x) * x;
    }
    rms_ = count == 0 ? 0.0f : static_cast<float>(std::sqrt(sum / static_cast<double>(count)));
    dbFs_ = utils::linearToDb(rms_);
    return dbFs_;
}

} // namespace micshield::dsp
