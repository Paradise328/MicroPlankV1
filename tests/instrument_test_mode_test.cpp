#include "../Modules/RobotControlModule/InstrumentTestMode.h"
#include <initializer_list>

// Compile on the target/toolchain without Qt or motor libraries:
// c++ -std=c++17 tests/instrument_test_mode_test.cpp -o /tmp/instrument-test-mode
constexpr bool settingsAreCorrect()
{
    for (const bool four : {false, true}) {
        const auto pre = instrumentTestSettings(InstrumentTestMode::PreRun, four);
        const auto grip = instrumentTestSettings(InstrumentTestMode::GripForce, four);
        const auto endurance = instrumentTestSettings(InstrumentTestMode::Endurance, four);
        if (pre.durationSeconds != 7200 || pre.stepAfterZero != 1 || pre.stepAfterSweep != 8
            || grip.durationSeconds != 61200 || grip.stepAfterZero != 5 || grip.stepAfterSweep != 5
            || endurance.durationSeconds != 72000 || endurance.stepAfterZero != 1
            || endurance.stepAfterSweep != 5) { return false; }
        for (const auto settings : {pre, grip, endurance}) {
            if (settings.sweepYaw != (four ? -14 : -20)
                || settings.gripYaw != (four ? -10 : -20)) { return false; }
        }
    }
    return !isInstrumentTestMode(-1) && isInstrumentTestMode(0)
        && isInstrumentTestMode(1) && isInstrumentTestMode(2) && !isInstrumentTestMode(3);
}
static_assert(settingsAreCorrect(), "Instrument test mode mapping mismatch");
static_assert(instrumentFirstMaxon(4) == 2 && instrumentFirstMaxon(6) == 0,
              "Preserve four-axis Maxon 2..5 and six-axis Maxon 0..5 wiring");
static_assert(instrumentFirstMaxon(0) == 6 && instrumentFirstMaxon(-1) == 6
              && instrumentFirstMaxon(5) == 6, "Unselected/invalid axes must not access Maxons");
int main() { return 0; }
