//
// Created by ZQD on 2026/6/13.
//

#pragma once

#include <chrono>
#include <cstdint>

class Timer {
public:
    using Clock = std::chrono::steady_clock;

    Timer() : m_Start(Clock::now()) {
    }

    void Restart() {
        m_Start = Clock::now();
    }

    [[nodiscard]] double ElapsedMilliseconds() const {
        return std::chrono::duration<double, std::milli>(Clock::now() - m_Start).count();
    }

    [[nodiscard]] int64_t ElapsedMicroseconds() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - m_Start).count();
    }

    double Stop(bool reset = false) {
        const auto now = Clock::now();
        const auto elapsed = std::chrono::duration<double, std::milli>(now - m_Start).count();
        if (reset) {
            m_Start = now;
        }
        return elapsed;
    }

    int64_t StopMicroseconds(bool reset = false) {
        const auto now = Clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - m_Start).count();
        if (reset) {
            m_Start = now;
        }
        return elapsed;
    }

private:
    Clock::time_point m_Start;
};
