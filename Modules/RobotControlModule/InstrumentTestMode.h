#ifndef INSTRUMENTTESTMODE_H
#define INSTRUMENTTESTMODE_H

// Values are the mode IDs sent by the test UI. No hardware dependencies here.
enum class InstrumentTestMode { PreRun = 0, GripForce = 1, Endurance = 2 };

constexpr bool isInstrumentTestMode(int mode) { return mode >= 0 && mode <= 2; }

struct InstrumentTestSettings {
    double durationSeconds;
    int stepAfterZero;
    int stepAfterSweep;
    double sweepYaw;
    double gripYaw;
};

constexpr InstrumentTestSettings instrumentTestSettings(InstrumentTestMode mode, bool fourMaxons)
{
    return {
        mode == InstrumentTestMode::PreRun ? 7200.0 :
            (mode == InstrumentTestMode::Endurance ? 72000.0 : 61200.0),
        mode == InstrumentTestMode::GripForce ? 5 : 1,
        mode == InstrumentTestMode::PreRun ? 8 : 5,
        fourMaxons ? -14.0 : -20.0,
        fourMaxons ? -10.0 : -20.0
    };
}

#endif
