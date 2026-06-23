#pragma once

#include <atomic>
#include <cstddef>
#include <vector>

namespace micshield::buffer {

template <typename T>
class SpscRingBuffer {
public:
    explicit SpscRingBuffer(size_t capacity = 0) { resize(capacity); }

    void resize(size_t capacity) {
        capacity_ = capacity + 1;
        storage_.assign(capacity_, T{});
        reset();
    }

    bool push(const T& item) noexcept {
        const auto head = head_.load(std::memory_order_relaxed);
        const auto next = increment(head);
        if (next == tail_.load(std::memory_order_acquire)) {
            overflow_.fetch_add(1, std::memory_order_relaxed);
            return false;
        }
        storage_[head] = item;
        head_.store(next, std::memory_order_release);
        return true;
    }

    bool pop(T& item) noexcept {
        const auto tail = tail_.load(std::memory_order_relaxed);
        if (tail == head_.load(std::memory_order_acquire)) {
            underflow_.fetch_add(1, std::memory_order_relaxed);
            return false;
        }
        item = storage_[tail];
        tail_.store(increment(tail), std::memory_order_release);
        return true;
    }

    size_t availableRead() const noexcept {
        const auto head = head_.load(std::memory_order_acquire);
        const auto tail = tail_.load(std::memory_order_acquire);
        return head >= tail ? head - tail : capacity_ - tail + head;
    }

    size_t availableWrite() const noexcept {
        return capacity_ > 0 ? capacity_ - 1 - availableRead() : 0;
    }

    void reset() noexcept {
        head_.store(0, std::memory_order_relaxed);
        tail_.store(0, std::memory_order_relaxed);
        overflow_.store(0, std::memory_order_relaxed);
        underflow_.store(0, std::memory_order_relaxed);
    }

    uint64_t overflowCount() const noexcept { return overflow_.load(std::memory_order_relaxed); }
    uint64_t underflowCount() const noexcept { return underflow_.load(std::memory_order_relaxed); }

private:
    size_t increment(size_t index) const noexcept { return (index + 1) % capacity_; }

    std::vector<T> storage_;
    size_t capacity_ = 1;
    std::atomic<size_t> head_{0};
    std::atomic<size_t> tail_{0};
    std::atomic<uint64_t> overflow_{0};
    std::atomic<uint64_t> underflow_{0};
};

} // namespace micshield::buffer
