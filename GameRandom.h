#ifndef GAME_RANDOM_H
#define GAME_RANDOM_H

#include <chrono>
#include <cstdint>
#include <random>

namespace GameRandom {

using Engine = std::mt19937;

inline Engine &engine() {
    static Engine generator;
    return generator;
}

inline void seed(std::uint32_t value) {
    engine().seed(value);
}

inline void seedFromClock() {
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    seed(static_cast<std::uint32_t>(now));
}

inline int integer(int minimum, int maximum) {
    return std::uniform_int_distribution<int>(minimum, maximum)(engine());
}

inline float real(float minimum, float maximum) {
    return std::uniform_real_distribution<float>(minimum, maximum)(engine());
}

inline bool boolean() {
    return std::bernoulli_distribution(0.5)(engine());
}

} // namespace GameRandom

#endif
