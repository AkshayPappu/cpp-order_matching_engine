#include <random>
#include <cstdint>

class RandomIdGenerator {
private:
    std::mt19937_64 gen_;
    std::uniform_int_distribution<uint64_t> dis_;

public:
    RandomIdGenerator()
        : gen_(std::random_device{}()), dis_(0, UINT64_MAX) {}

    uint64_t generate() {
        return dis_(gen_);
    }
};