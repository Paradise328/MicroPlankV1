#ifndef MANUALGRIPCOMMAND_H
#define MANUALGRIPCOMMAND_H

#include <algorithm>
#include <cmath>

// Existing test-command envelope, NOT a certified mechanical travel limit.
// These angles retain targetPose's historical meaning, not a measured jaw angle.
struct ManualGripCommand {
    static constexpr double NoRequest = 1000.0;
    static bool accepts(int axes, double degrees) {
        return (axes == 4 || axes == 6) && std::isfinite(degrees)
            && degrees >= (axes == 4 ? -10.0 : -20.0) && degrees <= 30.0;
    }
    static double advance(double current, double target, double dt) {
        // 5 deg/s, with no catch-up jump after a stalled control loop.
        const double step = 5.0 * std::max(0.0, std::min(dt, 0.02));
        return current + std::max(-step, std::min(target - current, step));
    }
};

#endif
