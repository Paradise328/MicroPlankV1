// c++ -std=c++17 -O2 tests/manual_grip_test.cpp -o /tmp/manual-grip-test
// Pure control-envelope/history checks: no Qt, serial port or motors are loaded.
#include "../Modules/RobotControlModule/ManualGripCommand.h"
#include "../Modules/UIModule/ManualGripHistory.h"
#include <cassert>
#include <iostream>
#include <limits>

namespace {
bool near(double first, double second) { return std::abs(first - second) < 1e-9; }
ManualGripHistory::Sample sample(double time, double value = 1.0) {
    ManualGripHistory::Sample result;
    result.time = time;
    result.values.fill(value);
    result.validMask = 0x0FFF;
    return result;
}
}

int main() {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();
    assert(ManualGripCommand::accepts(4, -10) && ManualGripCommand::accepts(4, 30));
    assert(ManualGripCommand::accepts(6, -20) && ManualGripCommand::accepts(6, 30));
    assert(!ManualGripCommand::accepts(4, -10.001));
    assert(!ManualGripCommand::accepts(6, -20.001));
    for (const int axes : {4, 6}) {
        assert(!ManualGripCommand::accepts(axes, 30.001));
        assert(!ManualGripCommand::accepts(axes, nan));
        assert(!ManualGripCommand::accepts(axes, inf));
        assert(!ManualGripCommand::accepts(axes, -inf));
        assert(!ManualGripCommand::accepts(axes, ManualGripCommand::NoRequest));
    }
    for (const int axes : {-1, 0, 3, 5, 7}) {
        assert(!ManualGripCommand::accepts(axes, 0));
    }
    assert(near(ManualGripCommand::advance(0, 20, .01), .05));
    assert(near(ManualGripCommand::advance(0, -20, .01), -.05));
    assert(near(ManualGripCommand::advance(0, 20, 10), .1));
    assert(near(ManualGripCommand::advance(0, -20, 10), -.1));
    assert(near(ManualGripCommand::advance(0, 20, 0), 0));
    assert(near(ManualGripCommand::advance(0, 20, -1), 0));
    assert(near(ManualGripCommand::advance(1, 1.01, .02), 1.01));
    assert(near(ManualGripCommand::advance(1, .99, .02), .99));
    double angle = 0;
    for (int i = 0; i < 1000; ++i) {
        const double next = ManualGripCommand::advance(angle, -20, .02);
        assert(next <= angle && next >= -20 && angle - next <= .100000001);
        angle = next;
    }
    assert(near(angle, -20));

    ManualGripHistory history;
    assert(history.size() == 0 && history.endTime() == 0);
    for (int i = 0; i <= 4200; ++i) {
        auto point = sample(i / 10.0);
        point.values[2] = i == 3501 ? 100.0 : -0.23;
        point.values[11] = i;
        assert(history.append(point));
    }
    assert(history.size() == 3001 && near(history.endTime(), 420));
    const auto rolling = history.plot(2, 0, 420, 400);
    assert(near(rolling.front().time, 120) && near(rolling.back().time, 420));
    assert(rolling.size() <= 1600);
    bool peak = false;
    for (const auto& point : rolling) { peak = peak || point.value == 100; }
    assert(peak);
    const auto encoders = history.plot(11, 419, 420, 100);
    assert(encoders.back().value == 4200 && encoders.back().valid);

    const auto previousSize = history.size();
    for (const double time : {-1.0, 420.0, 419.0, nan, inf}) {
        assert(!history.append(sample(time)));
    }
    assert(history.size() == previousSize);
    assert(history.plot(-1, 0, 420, 400).empty());
    assert(history.plot(12, 0, 420, 400).empty());
    assert(history.plot(0, 0, 0, 400).empty());
    assert(history.plot(0, nan, 420, 400).empty());
    assert(history.plot(0, 0, inf, 400).empty());

    auto invalid = sample(420.1);
    invalid.values[2] = nan;
    invalid.values[3] = inf;
    invalid.validMask &= ~(1U << 4);
    assert(history.append(invalid));
    assert(!history.plot(2, 420.1, 420.2, 10).front().valid);
    assert(!history.plot(3, 420.1, 420.2, 10).front().valid);
    assert(!history.plot(4, 420.1, 420.2, 10).front().valid);
    assert(history.plot(5, 420.1, 420.2, 10).front().valid);
    assert(history.append(sample(420.2)));
    const auto gap = history.plot(2, 420, 420.2, 1);
    assert(gap.size() >= 3 && gap.front().valid && gap.back().valid);
    bool invalidInside = false;
    for (const auto& point : gap) { invalidInside = invalidInside || !point.valid; }
    assert(invalidInside); // Even at one pixel, missing data must not be bridged.

    assert(history.append(sample(421))); // No telemetry for 0.8 seconds.
    const auto pause = history.plot(0, 420.2, 421, 1);
    assert(pause.size() == 3 && pause.front().valid && !pause[1].valid && pause.back().valid);
    history.reset();
    assert(history.size() == 0 && history.endTime() == 0);
    assert(history.plot(0, 0, 300, 100).empty());
    // A misconfigured high-rate producer still has bounded memory.
    for (int i = 0; i < 5000; ++i) { assert(history.append(sample(i / 1000.0))); }
    assert(history.size() == 3001);
    std::cout << "PASS: angle limits, invalid commands, slew/no overshoot; 12-series history, "
                 "5-minute/time+count bounds, peaks, invalid samples and dropped-frame gaps\n";
}
