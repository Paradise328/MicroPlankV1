// g++ -std=c++11 -O2 tests/force_history_test.cpp -o /tmp/force_history_test && /tmp/force_history_test
#include "../Modules/UIModule/ForceHistory.h"
#include <cassert>
#include <limits>
#include <iostream>

int main() {
    ForceHistory history;
    history.reset(false);
    for (int i = 0; i <= 4200; ++i) { history.append({i / 10.0, 1, 2, true}); }
    assert(history.size() == 3001);
    const auto rolling = history.plot(0, 0, 420, 400);
    assert(rolling.front().time == 120 && rolling.back().time == 420);
    assert(rolling.front().value == 3);
    history.reset(true);
    for (int i = 0; i <= 720000; ++i) {
        history.append({i / 10.0, i == 350001 ? 100.0 : 1.0, 2, true});
    }
    assert(history.size() == 720001);
    const auto all = history.plot(0, 0, 72000, 800);
    assert(all.size() <= 3200);
    assert(all.front().time == 0 && all.back().time == 72000);
    bool peak = false;
    for (const auto& p : all) { peak = peak || p.value == 102; }
    assert(peak); // Full-range decimation must retain single-sample peaks.
    const auto zoom = history.plot(1, 35000, 35001, 800);
    assert(zoom.front().time >= 35000 && zoom.back().time <= 35001);
    history.append({72000.1, 0, 0, false});
    history.append({72000.2, 1, 2, true});
    const auto gap = history.plot(0, 71999, 72001, 100);
    bool missing = false;
    for (const auto& p : gap) { missing = missing || !p.valid; }
    assert(missing);
    const auto count = history.size();
    history.append({72000.2, 99, 99, true});
    history.append({-1, 99, 99, true});
    assert(history.size() == count);
    history.append({72000.3, std::numeric_limits<double>::quiet_NaN(), 2, true});
    assert(!history.plot(0, 72000.3, 72001, 100).front().valid);
    history.reset(false);
    assert(history.size() == 0 && history.endTime() == 0);
    assert(history.plot(0, 0, 300, 800).empty());
    std::cout << "PASS: 5-minute retention, 20-hour history, peaks, zoom, gaps and resets\n";
}
