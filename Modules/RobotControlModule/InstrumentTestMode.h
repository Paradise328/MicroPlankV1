#ifndef INSTRUMENTTESTMODE_H
#define INSTRUMENTTESTMODE_H

// Values are the mode IDs sent by the test UI. No hardware dependencies here.
enum class InstrumentTestMode { PreRun = 0, GripForce = 1, Endurance = 2 };

constexpr bool isInstrumentTestMode(int mode) { return mode >= 0 && mode <= 2; }

// Preserve the existing wiring: four-axis targets occupy encoder slots 6..9
// (Maxon 2..5); six-axis targets occupy slots 4..9 (Maxon 0..5).
constexpr int instrumentFirstMaxon(int axes) { return axes == 4 ? 2 : (axes == 6 ? 0 : 6); }

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
