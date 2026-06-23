add_executable(MicShieldDSPTests
    test_biquad.cpp
    test_dc_blocker.cpp
    test_ring_buffer.cpp
    test_rms_meter.cpp
    test_limiter.cpp)
target_link_libraries(MicShieldDSPTests PRIVATE micshield_core)
add_test(NAME MicShieldDSPTests COMMAND MicShieldDSPTests)
