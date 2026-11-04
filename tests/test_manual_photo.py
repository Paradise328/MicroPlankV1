"""Compile the real photo methods against in-memory camera, disk and Qt fakes.

No camera is opened and no production photo directory is accessed. Run with a
C++17 compiler on PATH; --msvc-sdk supplies the optional Windows SDK root.
"""
import argparse
from pathlib import Path
import re
import shutil
import subprocess
import tempfile

parser = argparse.ArgumentParser()
parser.add_argument("--msvc-sdk", type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
source = (root / "Modules/RobotControlModule/RobotControl.cpp").read_text(encoding="utf-8")
header = (root / "Modules/RobotControlModule/RobotControl.h").read_text(encoding="utf-8")


def method(signature):
    """Extract a production definition, without substituting its implementation."""
    begin = source.index(signature)
    opening = source.index("{", begin)
    depth = 1
    end = opening + 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return source[begin:end] + "\n"


# Retain the production atomic initializer too; the fake constructor must not
# accidentally mask an uninitialized busy flag in the real class.
busy_member = re.search(r"std::atomic<bool>\s+m_isTakingPhoto\b[^;\n]*;", header)
assert busy_member, "Missing production photo busy member"
methods = "\n".join(method(signature) for signature in (
    "RobotControl::~RobotControl()",
    "void RobotControl::takePhotoTask(std::string stepName, double angle)",
    "void RobotControl::triggerPhoto(std::string stepName, double angle)",
))

prefix = r'''
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdlib>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
using namespace std::chrono_literals;
void require(bool ok, const char* description) {
    if (!ok) { std::cerr << "FAIL: " << description << '\n'; std::abort(); }
}
enum class Fault { None, CvRead, StdRead, UnknownRead, CvWrite, StdWrite, UnknownWrite };
struct FakeIO {
    std::mutex mutex;
    std::condition_variable changed;
    bool opened=true, mkpathResult=true, writeResult=true, emptyFrame=false;
    bool blockRead=false, waiting=false, release=false;
    bool readResults[3]={true,true,true};
    int reads=0, uuid=0;
    Fault fault=Fault::None;
    std::vector<std::string> directories, writes, formats, results;
    void reset() {
        std::lock_guard<std::mutex> lock(mutex);
        opened=mkpathResult=writeResult=true; emptyFrame=blockRead=waiting=release=false;
        for (auto& result : readResults) result=true;
        reads=uuid=0; fault=Fault::None;
        directories.clear(); writes.clear(); formats.clear(); results.clear();
    }
    void awaitCapture() {
        std::unique_lock<std::mutex> lock(mutex);
        require(changed.wait_for(lock,5s,[&]{return waiting;}), "worker reaches camera gate");
    }
    void releaseCapture() {
        std::lock_guard<std::mutex> lock(mutex);
        release=true; changed.notify_all();
    }
    void awaitResults(std::size_t count) {
        std::unique_lock<std::mutex> lock(mutex);
        require(changed.wait_for(lock,5s,[&]{return results.size()>=count;}), "worker reports completion");
    }
} io;
class QString {
    std::string value;
public:
    QString()=default;
    QString(const char* text):value(text){}
    QString(std::string text):value(std::move(text)){}
    static QString fromStdString(const std::string& text) { return text; }
    static QString fromUtf8(const char* text) { return text; }
    std::string toStdString() const { return value; }
    friend QString operator+(const QString& left,const QString& right) { return left.value+right.value; }
};
#define QStringLiteral(text) QString(text)
class QDir {
public:
    bool mkpath(const QString& path) {
        std::lock_guard<std::mutex> lock(io.mutex);
        io.directories.push_back(path.toStdString());
        return io.mkpathResult;
    }
};
class QDateTime {
public:
    static QDateTime currentDateTime() { return {}; }
    QString toString(const char* format) const {
        std::lock_guard<std::mutex> lock(io.mutex);
        io.formats.emplace_back(format);
        // Freeze time so two captures test UUID collision protection.
        return "20300102_030405_006";
    }
};
class QUuid {
    int number;
public:
    enum StringFormat { WithoutBraces };
    explicit QUuid(int value):number(value){}
    static QUuid createUuid() {
        std::lock_guard<std::mutex> lock(io.mutex);
        return QUuid(++io.uuid);
    }
    QString toString(StringFormat) const { return "mock-uuid-"+std::to_string(number); }
};
namespace cv {
class Exception:public std::runtime_error { public: using std::runtime_error::runtime_error; };
class Mat { public: bool isEmpty=true; bool empty() const { return isEmpty; } };
class VideoCapture {
public:
    bool isOpened() {
        std::lock_guard<std::mutex> lock(io.mutex);
        return io.opened;
    }
    bool read(Mat& frame) {
        std::unique_lock<std::mutex> lock(io.mutex);
        int index=io.reads++;
        if (index==2) {
            if (io.blockRead) {
                io.waiting=true; io.changed.notify_all();
                require(io.changed.wait_for(lock,5s,[&]{return io.release;}), "camera gate is released");
            }
            if (io.fault==Fault::CvRead) throw Exception("mock cv read error");
            if (io.fault==Fault::StdRead) throw std::runtime_error("mock standard read error");
            if (io.fault==Fault::UnknownRead) throw 17;
        }
        bool success=io.readResults[index%3];
        frame.isEmpty=!success || io.emptyFrame;
        return success;
    }
};
bool imwrite(const std::string& path,const Mat& frame) {
    std::lock_guard<std::mutex> lock(io.mutex);
    require(!frame.empty(), "empty image never reaches imwrite");
    io.writes.push_back(path);
    if (io.fault==Fault::CvWrite) throw Exception("mock cv write error");
    if (io.fault==Fault::StdWrite) throw std::runtime_error("mock standard write error");
    if (io.fault==Fault::UnknownWrite) throw 18;
    return io.writeResult;
}
}
'''
declaration = r'''
struct NullLog { template<class T> NullLog& operator<<(const T&) { return *this; } };
#define LOG(level) NullLog()
enum class Module_Inner_E { Uiinterface };
enum class UIAction_E { ManualGripPhotoResult=0x10d };
class RobotControl {
public:
    std::atomic<bool> m_isSystemTerminated{false};
    cv::VideoCapture m_camera;
    BUSY_MEMBER
    std::thread m_photoThread;
    ~RobotControl();
    void takePhotoTask(std::string,double);
    void triggerPhoto(std::string,double);
    void SendInnerMsg(Module_Inner_E module,int action,const QString& text) {
        require(module==Module_Inner_E::Uiinterface, "result goes to UI module");
        require(action==0x10d, "result uses photo result action");
        std::lock_guard<std::mutex> lock(io.mutex);
        io.results.push_back(text.toStdString()); io.changed.notify_all();
    }
};
'''.replace("BUSY_MEMBER", busy_member.group())

checks = r'''
void join(RobotControl& robot) {
    if (robot.m_photoThread.joinable()) robot.m_photoThread.join();
    require(!robot.m_isTakingPhoto.load(), "completion clears busy flag");
}
void reset() { io.reset(); }
void expectError(const char* reason=nullptr) {
    require(io.results.size()==1, "failure sends exactly one result");
    require(io.results[0].find("error|")==0 && io.results[0].size()>6, "error includes a reason");
    if (reason) require(io.results[0]==std::string("error|")+reason, "failure reports the correct reason");
}
int main() {
    const std::string under="/home/a/Desktop/TestPhotos/Angle_Under_20/";
    const std::string over="/home/a/Desktop/TestPhotos/Angle_Over_20/";
    const std::string name="ManualGrip_CmdG_10.000_Y_-2.000_P_3.000_R_4.000";
    struct AngleCase { double angle; bool upper; };
    for (auto sample : {AngleCase{0,false},{9.999,false},{-9.999,false},
                        {10,true},{-10,true},{20,true},{-20,true}}) {
        reset(); RobotControl robot;
        require(!robot.m_isTakingPhoto.load(), "new robot starts with camera idle");
        robot.triggerPhoto(name,sample.angle); join(robot);
        auto directory=sample.upper?over:under;
        auto expected=directory+name+"_20300102_030405_006_mock-uuid-1.jpg";
        require(io.reads==3, "capture flushes two frames and reads the third");
        require(io.directories==std::vector<std::string>{directory}, "existing base path and angle split are preserved");
        require(io.writes==std::vector<std::string>{expected}, "filename retains command angles, milliseconds and UUID");
        require(io.formats==std::vector<std::string>{"yyyyMMdd_HHmmss_zzz"}, "timestamp includes milliseconds");
        require(io.results==std::vector<std::string>{"saved|"+expected}, "success returns the actual saved path");
    }
    {
        reset(); RobotControl robot;
        robot.triggerPhoto(name,10); io.awaitResults(1);
        // Leave the finished std::thread joinable; the next trigger must join it.
        robot.triggerPhoto(name,10); join(robot);
        require(io.reads==6 && io.writes.size()==2, "completed worker can be replaced by another capture");
        require(io.writes[0]!=io.writes[1], "same-millisecond captures cannot overwrite one another");
        require(io.results.size()==2 && io.results[1]=="saved|"+io.writes[1], "second capture reports its own path");
    }
    {
        reset(); io.readResults[0]=io.readResults[1]=false; RobotControl robot;
        robot.triggerPhoto(name,0); join(robot);
        require(io.reads==3 && io.writes.size()==1, "third valid frame succeeds despite stale flush failures");
    }
    {
        reset(); io.opened=false; RobotControl robot;
        robot.triggerPhoto(name,0); join(robot);
        expectError("相机未连接或未成功打开。");
        require(io.reads==0 && io.directories.empty() && io.writes.empty(), "disconnected camera never reads or writes");
    }
    for (bool empty : {false,true}) {
        reset(); io.emptyFrame=empty; io.readResults[2]=empty; RobotControl robot;
        robot.triggerPhoto(name,0); join(robot);
        expectError("无法读取相机画面，请检查相机连接。");
        require(io.reads==3 && io.directories.empty() && io.writes.empty(), "failed or empty final frame never reaches disk");
    }
    {
        reset(); io.mkpathResult=false; RobotControl robot;
        robot.triggerPhoto(name,0); join(robot);
        expectError("无法创建照片目录，请检查目录权限。");
        require(io.directories.size()==1 && io.writes.empty(), "directory creation failure prevents image write");
    }
    {
        reset(); io.writeResult=false; RobotControl robot;
        robot.triggerPhoto(name,0); join(robot);
        expectError("照片写入失败，请检查目录权限与磁盘空间。");
        require(io.writes.size()==1, "false imwrite result is reported as failure");
    }
    for (Fault fault : {Fault::CvRead,Fault::StdRead,Fault::UnknownRead,
                        Fault::CvWrite,Fault::StdWrite,Fault::UnknownWrite}) {
        reset(); io.fault=fault; RobotControl robot;
        robot.triggerPhoto(name,0); join(robot); expectError();
        io.fault=Fault::None;
        robot.triggerPhoto(name,0); join(robot);
        require(io.results.size()==2 && io.results[1].find("saved|")==0, "capture recovers after any worker exception");
    }
    {
        reset(); io.blockRead=true; RobotControl robot;
        robot.triggerPhoto(name,10); io.awaitCapture();
        require(robot.m_isTakingPhoto.load(), "blocked worker keeps camera busy");
        robot.triggerPhoto("duplicate",0);
        expectError("相机正在拍照，请等待本次拍摄完成。");
        require(io.reads==3 && io.writes.empty() && robot.m_isTakingPhoto.load(), "duplicate does not start work or clear active busy flag");
        io.releaseCapture(); join(robot);
        require(io.reads==3 && io.writes.size()==1 && io.results.size()==2, "original capture completes after rejecting duplicate");
    }
    {
        reset(); RobotControl robot; robot.m_isSystemTerminated.store(true);
        robot.triggerPhoto(name,10);
        expectError("程序正在关闭，无法拍照。");
        require(!robot.m_photoThread.joinable() && !robot.m_isTakingPhoto.load() && io.reads==0, "shutdown rejects capture without starting a worker");
    }
    {
        reset(); io.blockRead=true;
        auto robot=std::make_unique<RobotControl>();
        robot->triggerPhoto(name,10); io.awaitCapture();
        std::promise<void> started; auto began=started.get_future();
        auto destroyed=std::async(std::launch::async,[&started,robot=std::move(robot)]() mutable {
            started.set_value(); robot.reset();
        });
        began.wait();
        require(destroyed.wait_for(50ms)==std::future_status::timeout, "destructor waits for active capture");
        io.releaseCapture();
        require(destroyed.wait_for(5s)==std::future_status::ready, "destructor completes after worker finishes");
        destroyed.get();
        require(io.results.size()==1 && io.results[0].find("saved|")==0, "worker can report completion before owner destruction");
    }
    std::cout << "PASS: real photo methods; paths/filenames, camera and disk failures, "
                 "exception recovery, busy rejection, repeated capture, shutdown and destructor join\n";
}
'''

compiler = shutil.which("cl") or shutil.which("g++") or shutil.which("clang++")
if not compiler:
    raise SystemExit("No C++ compiler on PATH; run from its developer environment")
with tempfile.TemporaryDirectory(prefix="manual-photo-") as folder:
    folder = Path(folder)
    cpp = folder / "photo.cpp"
    exe = folder / ("photo.exe" if Path(compiler).stem.lower() == "cl" else "photo")
    cpp.write_text(prefix + declaration + methods + checks, encoding="utf-8")
    if Path(compiler).stem.lower() == "cl":
        command = [compiler, "/nologo", "/std:c++17", "/EHsc", "/utf-8", str(cpp), "/Fe:" + str(exe)]
        if args.msvc_sdk:
            for part in ("ucrt", "um", "shared"):
                command.append("/I" + str(args.msvc_sdk / "Include/10.0.26100.0" / part))
            command += ["/link"] + ["/LIBPATH:" + str(args.msvc_sdk / "Lib/10.0.26100.0" / p / "x64")
                                   for p in ("ucrt", "um")]
    else:
        command = [compiler, "-std=c++17", "-pthread", str(cpp), "-o", str(exe)]
    subprocess.run(command, cwd=folder, check=True, timeout=120)
    subprocess.run([str(exe)], check=True, timeout=30)
