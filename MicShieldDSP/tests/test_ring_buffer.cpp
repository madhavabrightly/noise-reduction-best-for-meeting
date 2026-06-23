#include "micshield/buffer/SpscRingBuffer.hpp"
#include <cassert>

void test_ring_buffer() {
    micshield::buffer::SpscRingBuffer<int> rb(2);
    assert(rb.push(1));
    assert(rb.push(2));
    assert(!rb.push(3));
    int v = 0;
    assert(rb.pop(v) && v == 1);
    assert(rb.pop(v) && v == 2);
    assert(!rb.pop(v));
}

int main();
