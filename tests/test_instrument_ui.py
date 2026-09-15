"""Offscreen QML smoke test. Uses a fake UIinterface; never loads robot code.

Run: python tests/test_instrument_ui.py --runtime output/test-ui-runtime
Requires PySide6-Essentials (or a normal PySide6 installation).
"""
import argparse
import os
import sys
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument("--runtime", type=Path)
args = parser.parse_args()
if args.runtime:
    sys.path.insert(0, str(args.runtime.resolve()))
os.environ["QT_QPA_PLATFORM"] = "offscreen"
os.environ["QT_QUICK_BACKEND"] = "software"
os.environ["QT_QUICK_CONTROLS_STYLE"] = "Basic"

from PySide6.QtCore import QObject, Property, Signal, Slot, QUrl, QPointF, Qt
from PySide6.QtGui import QGuiApplication, QFont, QFontDatabase
from PySide6.QtQml import QQmlApplicationEngine
from PySide6.QtQuick import QQuickItem, QQuickWindow
from PySide6.QtTest import QTest


class FakeInterface(QObject):
    changed = Signal()
    elapsedChanged = Signal()

    def __init__(self):
        super().__init__()
        self.state = 0
        self.status = "正在初始化设备，请稍候…"
        self.home_requests = 0
        self.test_requests = []
        self.stop_requests = 0
        self.shutdown_requests = 0
        self.axes_requests = []
        self.axes = 0
        self.elapsed = "00:00:00"

    testState = Property(int, lambda self: self.state, notify=changed)
    testStatus = Property(str, lambda self: self.status, notify=changed)
    instrumentAxes = Property(int, lambda self: self.axes, notify=changed)
    testElapsed = Property(str, lambda self: self.elapsed, notify=elapsedChanged)

    @Slot(int)
    def selectInstrumentAxes(self, axes):
        if axes == self.axes:
            return
        self.axes_requests.append(axes)
        self.axes = 0
        self.set_state(7, "正在配置器械，请稍候…")

    def acknowledge_axes(self, axes):
        self.axes = axes
        self.set_state(1, f"已选择{axes}轴器械，请先将器械归零。" if axes else "器械参数加载失败，请重新选择轴数。")

    def set_elapsed(self, text):
        self.elapsed = text
        self.elapsedChanged.emit()
        QTest.qWait(50)

    def set_state(self, state, text):
        self.state, self.status = state, text
        self.changed.emit()
        QTest.qWait(50)

    @Slot()
    def homeRightInstrument(self):
        self.home_requests += 1
        self.set_state(2, "器械归零中，请勿触碰器械…")

    @Slot(int)
    def enterInstrumentTest(self, mode):
        self.test_requests.append(mode)
        self.set_state(4, "测试进行中，归零和模式切换已锁定。")
        self.set_elapsed("00:00:00")

    @Slot()
    def stopInstrumentTest(self):
        self.stop_requests += 1
        self.set_state(6, "正在停止测试，请稍候…")

    @Slot()
    def onButton_PowerOff(self):
        self.shutdown_requests += 1


app = QGuiApplication(sys.argv[:1])
# Windows' offscreen platform does not enumerate system fonts automatically.
font_file = Path(os.environ.get("WINDIR", "C:/Windows")) / "Fonts/msyh.ttc"
if font_file.exists():
    font_id = QFontDatabase.addApplicationFont(str(font_file))
    assert font_id >= 0, "Failed to load preview font"
    app.setFont(QFont(QFontDatabase.applicationFontFamilies(font_id)[0]))
backend = FakeInterface()
engine = QQmlApplicationEngine()
warnings = []
engine.warnings.connect(lambda errors: warnings.extend(e.toString() for e in errors))
engine.rootContext().setContextProperty("UIinterface", backend)
root = Path(__file__).resolve().parents[1]
engine.load(QUrl.fromLocalFile(str(root / "Modules/UIModule/main.qml")))
assert engine.rootObjects(), "QML failed to load"
window = engine.rootObjects()[0]
window.showNormal()
window.resize(1280, 800)
QTest.qWait(200)


def item(name):
    pending = [window.contentItem()]
    while pending:
        found = pending.pop()
        if found.objectName() == name:
            return found
        pending.extend(found.childItems())
    raise AssertionError(name)


def click(control):
    pos = control.mapToScene(QPointF(control.width() / 2, control.height() / 2))
    QTest.mouseClick(window, Qt.LeftButton, Qt.NoModifier, pos.toPoint())
    QTest.qWait(50)


home = item("testHomeRightButton")
enter = item("testEnterButton")
stop = item("testStopButton")
shutdown = item("testShutdownButton")
axes_buttons = [item("testAxesButton" + str(n)) for n in (4, 6)]
elapsed_label = item("testElapsedLabel")
modes = [item("testModeButton" + str(i)) for i in range(3)]
assert not home.property("enabled") and not enter.property("enabled")
assert not stop.property("enabled")
assert not shutdown.property("enabled")
assert all(not b.property("enabled") for b in axes_buttons)
assert all(not m.property("enabled") for m in modes)
backend.set_state(1, "控制程序已就绪，请确认设备连接并将器械归零。")
assert not home.property("enabled") and not enter.property("enabled")
assert elapsed_label.property("text") == "已测试 00:00:00"
click(axes_buttons[0])
assert backend.axes_requests == [4] and backend.state == 7
assert all(not b.property("enabled") for b in [home, enter, shutdown] + axes_buttons + modes)
backend.acknowledge_axes(0)
assert not home.property("enabled"), "Bad profile must not allow homing"
click(axes_buttons[0])
backend.acknowledge_axes(4)
assert axes_buttons[0].property("checked") and not axes_buttons[1].property("checked")
assert home.property("enabled")
assert home.property("text") == "器械归零"
assert all(not m.property("enabled") for m in modes)
click(home)
assert backend.home_requests == 1 and not home.property("enabled")
assert not shutdown.property("enabled")
assert all(not b.property("enabled") for b in axes_buttons)
assert all(not m.property("enabled") for m in modes)
backend.set_state(3, "器械已归零，选择模式后即可进入测试。")
assert not enter.property("enabled"), "Must select a mode first"

for index in range(6):
    mode = index % 3
    if index:
        assert all(not m.property("enabled") for m in modes)
        if index == 3:
            click(axes_buttons[1])
            assert backend.axes_requests[-1] == 6
            backend.acknowledge_axes(6)
        click(home)
        backend.set_state(3, "器械已归零，选择模式后即可进入测试。")
        assert not enter.property("enabled"), "Rehoming must require a fresh mode selection"
    click(modes[mode])
    assert window.property("selectedMode") == mode
    assert [m.property("checked") for m in modes] == [i == mode for i in range(3)]
    # Clicking the same card must not deselect it.
    click(modes[mode])
    assert modes[mode].property("checked") and enter.property("enabled")
    if index == 0:
        output = root / "output/test-ui-preview.png"
        assert window.grabWindow().save(str(output))
    click(enter)
    assert backend.test_requests[-1] == mode
    assert elapsed_label.property("text") == "已测试 00:00:00"
    backend.set_elapsed("25:01:01")
    assert elapsed_label.property("text") == "已测试 25:01:01"
    assert all(not b.property("enabled") for b in axes_buttons)
    selection_count = len(backend.axes_requests)
    click(axes_buttons[0])
    click(axes_buttons[1])
    assert len(backend.axes_requests) == selection_count
    assert not enter.property("enabled") and not home.property("enabled")
    assert all(not m.property("enabled") for m in modes)
    click(enter)
    assert len(backend.test_requests) == index + 1, "Duplicate start was not blocked"
    assert stop.property("enabled")
    assert not shutdown.property("enabled")
    assert stop.mapToScene(QPointF(0, 0)).y() >= enter.mapToScene(QPointF(0, enter.height())).y()
    click(stop)
    assert backend.stop_requests == index + 1
    assert backend.state == 6 and window.property("selectedMode") == -1
    assert not shutdown.property("enabled")
    assert all(not control.property("enabled") for control in [home, enter, stop] + modes)
    click(stop)
    click(home)
    click(modes[mode])
    click(enter)
    assert backend.stop_requests == index + 1, "Duplicate stop was not blocked"
    assert backend.home_requests == index + 1, "Homing must wait for the stop acknowledgement"
    assert len(backend.test_requests) == index + 1
    backend.set_state(1, "测试已停止；请先将器械归零，再重新选择测试模式。")
    assert not enter.property("enabled") and home.property("enabled")
    assert all(not m.property("enabled") for m in modes)
    click(modes[mode])
    click(enter)
    assert window.property("selectedMode") == -1
    assert len(backend.test_requests) == index + 1, "Restart before rehoming was not blocked"
    assert elapsed_label.property("text") == "已测试 25:01:01", "Stopped duration must be retained"

click(home)
backend.set_state(3, "器械已归零，选择模式后即可进入测试。")
click(modes[0])
click(axes_buttons[0])
assert backend.state == 7 and window.property("selectedMode") == -1
assert not enter.property("enabled") and not home.property("enabled")
backend.acknowledge_axes(4)
assert not enter.property("enabled"), "Axis switch invalidates homing"
click(home)
backend.set_state(3, "器械已归零，选择模式后即可进入测试。")
assert not enter.property("enabled")
click(modes[0])
click(enter)
assert len(backend.test_requests) == 7
backend.set_elapsed("02:00:07")
backend.set_state(1, "测试已完成；再次测试前，请重新归零。")
assert home.property("enabled") and window.property("selectedMode") == -1
assert all(not control.property("enabled") for control in [enter, stop] + modes)
assert elapsed_label.property("text") == "已测试 02:00:07"

backend.set_state(5, "收到异常信号，请使用硬件急停并检查设备。")
assert not home.property("enabled") and not enter.property("enabled")
assert all(not m.property("enabled") for m in modes)
assert not stop.property("enabled")
backend.set_state(3, "器械已归零，选择模式后即可进入测试。")
window.resize(800, 640)
QTest.qWait(100)
for control in [home, enter, stop, shutdown, elapsed_label] + axes_buttons + modes:
    top = control.mapToScene(QPointF(0, 0))
    bottom = control.mapToScene(QPointF(control.width(), control.height()))
    assert top.x() >= 0 and top.y() >= 0 and bottom.x() <= 800 and bottom.y() <= 640, control.objectName()
assert window.grabWindow().save(str(root / "output/test-ui-preview-small.png"))

assert shutdown.property("enabled")
click(shutdown)
assert backend.shutdown_requests == 0, "Shutdown must wait for confirmation"
click(item("testCancelShutdownButton"))
assert backend.shutdown_requests == 0 and shutdown.property("enabled")
click(shutdown)
click(item("testConfirmShutdownButton"))
assert backend.shutdown_requests == 1 and window.property("shutdownRequested")
assert all(not control.property("enabled") for control in [home, enter, stop, shutdown] + modes)
click(shutdown)
assert backend.shutdown_requests == 1, "Duplicate shutdown was not blocked"
assert not warnings, "\n".join(warnings)
print("PASS: QML, 4/6-axis selection and rehoming, all 6 axis/mode combinations, elapsed display/reset/retention, stop/shutdown locks, 2 sizes")
window.hide()
engine.deleteLater()
app.processEvents()
