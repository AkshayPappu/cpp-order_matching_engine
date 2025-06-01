#include <random>
#include <cstdint>
#include <mutex>

class RandomIdGenerator {
private:
    std::mt19937_64 gen_;
    std::uniform_int_distribution<uint64_t> dis_;
    std::mutex id_gen_mutex_;

public:
    RandomIdGenerator()
        : gen_(std::random_device{}()), dis_(0, UINT64_MAX) {}

    uint64_t generate() {
        std::lock_guard<std::mutex> lock(id_gen_mutex_);
        return dis_(gen_);
    }
};