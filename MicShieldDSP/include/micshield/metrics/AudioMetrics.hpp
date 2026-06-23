#pragma once

#include <cstdint>

namespace micshield::metrics {

struct AudioMetrics {
    float inputRmsDb = -120.0f;
    float outputRmsDb = -120.0f;
    float peakDb = -120.0f;
    float noiseFloorDb = -80.0f;
    bool vadActive = false;
    float gainReductionDb = 0.0f;
    uint64_t clippedSampleCount = 0;
    uint64_t ringBufferOverflowCount = 0;
    uint64_t ringBufferUnderflowCount = 0;
};

} // namespace micshield::metrics
