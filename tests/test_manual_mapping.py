"""Compile the real mapping functions in an isolated, hardware-free C++ harness.

Run with a C++17 compiler on PATH. --msvc-sdk optionally supplies a Windows SDK
include/lib root. This checks geometry and units, not motor polarity on hardware.
Backlash compensation is deliberately stubbed to isolate the kinematic mapping.
"""
import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

parser = argparse.ArgumentParser()
parser.add_argument("--msvc-sdk", type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
source = (root / "Modules/RobotControlModule/RobotControl.cpp").read_text(encoding="utf-8")


def section(start, end):
    return source[source.index(start):source.index(end, source.index(start))]


mapping = section("std::array<double, ControlValueNum> RobotControl::motionMapping_R(",
                  "double RobotControl::calculateNewOpenangle(")
opening = section("double RobotControl::calculateNewOpenangle(", "void RobotControl::storeCurAsPrev(")
cables = section("double RobotControl::cableLengths_2(",
                 "std::array<int, MotorNumPerSide> RobotControl::calculateTargetEncoder(")
# Compile the previous implementation too: explicit legacy mode must not alter
# either automatic mode's old results, including its historical angle convention.
# Baseline comes from the original, committed implementation (not a rewritten formula).
baseline = subprocess.run(["git", "show", "68fe241:Modules/RobotControlModule/RobotControl.cpp"],
                          cwd=root, check=True, capture_output=True, encoding="utf-8").stdout
begin = baseline.index("std::array<double, ControlValueNum> RobotControl::motionMapping_R(")
end = baseline.index("double RobotControl::calculateNewOpenangle(", begin)
legacy = baseline[begin:end].replace("RobotControl::motionMapping_R(", "RobotControl::legacyMapping(", 1)

prefix = r'''
#include <array>
#include <cmath>
#include <cassert>
#include <iostream>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
constexpr int MotorNumPerSide = 12, ControlValueNum = 12;
struct HandlePose {
    double handlePoseR_Elevation=0, handlePoseR_Arzimuth=0;
    double handlePoseR_Roll=0, handlePoseR_OpenAngle=0, graspIndex_R=0;
};
enum class EndeffectorConfiguration { fourMaxons, sixMaxons };
class RobotControl {
public:
    int m_alignmentNumber_R = 200;
    HandlePose m_handlePoseOrg_R{}, m_handlePoseInit_R{}, m_handlePoseLastLoop_R{};
    double m_delt_alpha_R=0, m_delt_beta_R=0, m_delt_gamma_R=0;
    double m_compRatio_R=0.25, m_moonsTargetDisp=0;
    EndeffectorConfiguration m_endeffectorConfiguration_R=EndeffectorConfiguration::fourMaxons;
    std::array<double, ControlValueNum> motionMapping_R(const HandlePose&, const std::array<int, MotorNumPerSide>&, bool=false);
    std::array<double, ControlValueNum> legacyMapping(const HandlePose&, const std::array<int, MotorNumPerSide>&);
    double calculateNewOpenangle(double);
    double cableLengths_2(double) const;
    double cableLengths_3(double, double, double) const;
    double backlashCompensate(double v, const char&) { return v; }
};
bool near(double a, double b) { return std::abs(a-b) < 1e-8; }
HandlePose pose(double grip, double yaw=0, double pitch=0, double roll=0) {
    HandlePose p;
    p.handlePoseR_OpenAngle=grip*M_PI/180; p.handlePoseR_Arzimuth=yaw*M_PI/180;
    p.handlePoseR_Elevation=pitch*M_PI/180; p.handlePoseR_Roll=roll*M_PI/180;
    return p;
}
'''
checks = r'''
int main() {
    RobotControl r;
    std::array<int, MotorNumPerSide> enc{};
    auto open=r.motionMapping_R(pose(20),enc,true);
    assert(near(open[6],20) && near(open[7],20));
    assert(near(r.m_delt_beta_R,0)); // pure opening must not inject yaw
    auto yaw=r.motionMapping_R(pose(0,30),enc,true);
    assert(near(yaw[6],-30) && near(yaw[7],30)); // retain existing DRIVE polarity
    auto both=r.motionMapping_R(pose(20,30),enc,true);
    assert(near(both[6],-10) && near(both[7],50));
    assert(near((both[7]-both[6])/2,30) && near((both[7]+both[6])/2,20));
    auto mixed=r.motionMapping_R(pose(-10,-80,50,-60),enc,true);
    assert(near(mixed[6],82.5) && near(mixed[7],-102.5));
    assert(near(mixed[8],50) && near(mixed[9],60));
    r.m_endeffectorConfiguration_R=EndeffectorConfiguration::sixMaxons;
    for (double grip : {-20.,0.,20.}) for (double y : {-80.,0.,80.})
    for (double pitch : {-50.,0.,50.}) for (double roll : {-60.,0.,60.}) {
        auto v=r.motionMapping_R(pose(grip,y,pitch,roll),enc,true);
        for(double a : v) assert(std::isfinite(a));
        assert(near(r.m_delt_beta_R,y*M_PI/180));
        assert(near(v[6],-r.cableLengths_3(pitch,y,-grip)));
        assert(near(v[7],-r.cableLengths_3(pitch,-y,grip)));
        assert(near(v[5],-r.cableLengths_3(-pitch,y,grip)));
        assert(near(v[4],-r.cableLengths_3(-pitch,-y,-grip)));
        assert(near(v[8],r.cableLengths_2(pitch)) && near(v[9],-roll));
    }
    // Exhaustively compare representative automatic inputs to the committed
    // baseline, including states used for alignment/relative input handling.
    for(auto config : {EndeffectorConfiguration::fourMaxons,EndeffectorConfiguration::sixMaxons})
    for(double grip : {-20.,-10.,0.,20.,30.,45.}) for(double pitch : {-50.,0.,50.}) {
        r.m_endeffectorConfiguration_R=config;
        for(int aligned : {0,100,200}) {
            r.m_handlePoseInit_R=pose(2,3,4,5);
            r.m_handlePoseOrg_R=pose(1,2,1,2);
            r.m_handlePoseLastLoop_R=pose(3,4,2,3);
            r.m_alignmentNumber_R=aligned;
            auto actual=r.motionMapping_R(pose(grip,77,pitch,60),enc);
            r.m_alignmentNumber_R=aligned;
            auto expected=r.legacyMapping(pose(grip,77,pitch,60),enc);
            for(std::size_t i=0;i<actual.size();++i) assert(near(actual[i],expected[i]));
        }
    }
    std::cout << "PASS: real mapping functions; independent opening/yaw, superposition, "
                 "pitch/roll, 4/6-axis limits finite; automatic mapping matches baseline\n";
}
'''
compiler = shutil.which("cl") or shutil.which("g++") or shutil.which("clang++")
if not compiler:
    raise SystemExit("No C++ compiler on PATH; run from its developer environment")
with tempfile.TemporaryDirectory(prefix="manual-mapping-") as folder:
    folder = Path(folder)
    cpp = folder / "mapping.cpp"
    exe = folder / ("mapping.exe" if Path(compiler).stem.lower() == "cl" else "mapping")
    cpp.write_text(prefix + mapping + legacy + opening + cables + checks, encoding="utf-8")
    if Path(compiler).stem.lower() == "cl":
        command = [compiler, "/nologo", "/std:c++17", "/EHsc", "/utf-8", str(cpp), "/Fe:" + str(exe)]
        if args.msvc_sdk:
            for part in ("ucrt", "um", "shared"):
                command.append("/I" + str(args.msvc_sdk / "Include/10.0.26100.0" / part))
            command += ["/link"] + ["/LIBPATH:" + str(args.msvc_sdk / "Lib/10.0.26100.0" / p / "x64")
                                   for p in ("ucrt", "um")]
    else:
        command = [compiler, "-std=c++17", str(cpp), "-o", str(exe)]
    subprocess.run(command, cwd=folder, check=True)
    subprocess.run([str(exe)], check=True)
