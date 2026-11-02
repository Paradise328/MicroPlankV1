#ifndef MANUALGRIPCOMMAND_H
#define MANUALGRIPCOMMAND_H

#include <algorithm>
#include <array>
#include <cmath>

// User-requested command envelope, NOT certified mechanical/combined travel limits.
// Degrees: single-jaw opening (negative = clamping overtravel), yaw, pitch, roll.
struct ManualGripCommand {
    using Pose = std::array<double, 4>;
    static constexpr double NoRequest = 1000.0;
    static Pose noRequest() { return {{NoRequest, 0.0, 0.0, 0.0}}; }
    static bool accepts(int axes, const Pose& pose) {
        return (axes == 4 || axes == 6)
            && std::all_of(pose.begin(), pose.end(), [](double v) { return std::isfinite(v); })
            && pose[0] >= (axes == 4 ? -10.0 : -20.0) && pose[0] <= 20.0
            && std::abs(pose[1]) <= 80.0 && std::abs(pose[2]) <= 50.0
            && std::abs(pose[3]) <= 60.0;
    }
    static bool accepts(int axes, double degrees) {
        return accepts(axes, Pose{{degrees, 0.0, 0.0, 0.0}});
    }
    static double advance(double current, double target, double dt) {
        // 5 deg/s, with no catch-up jump after a stalled control loop.
        const double step = 5.0 * std::max(0.0, std::min(dt, 0.02));
        return current + std::max(-step, std::min(target - current, step));
    }
    static Pose advance(const Pose& current, const Pose& target, double dt) {
        Pose next{};
        for (std::size_t axis = 0; axis < next.size(); ++axis) {
            next[axis] = advance(current[axis], target[axis], dt);
        }
        return next;
    }
};

#endif
