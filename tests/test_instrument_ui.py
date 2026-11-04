"""Offscreen QML smoke test. Uses a fake UIinterface; never loads robot code.

Run: python tests/test_instrument_ui.py --runtime output/test-ui-runtime
Requires PySide6-Essentials (or a normal PySide6 installation).
"""
import argparse
import os
import sys
import math
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
    forceReset = Signal()
    forceDataChanged = Signal()
    manualGripReset = Signal()
    manualGripDataChanged = Signal()
    manualPhotoChanged = Signal()

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
        self.force_mode = -1
        self.force_time = 0.0
        self.force_values = []
        self.force_samples = []
        self.manual_active = False
        self.manual_time = 0.0
        self.manual_values = []
        self.manual_samples = []
        self.manual_stale = False
        self.manual_requests = []
        self.manual_photo_busy = False
        self.manual_photo_status = ""
        self.manual_photo_requests = []

    testState = Property(int, lambda self: self.state, notify=changed)
    testStatus = Property(str, lambda self: self.status, notify=changed)
    instrumentAxes = Property(int, lambda self: self.axes, notify=changed)
    testElapsed = Property(str, lambda self: self.elapsed, notify=elapsedChanged)
    forceMode = Property(int, lambda self: self.force_mode, notify=forceReset)
    forceTime = Property(float, lambda self: self.force_time, notify=forceDataChanged)
    forceValues = Property("QVariantList", lambda self: self.force_values, notify=forceDataChanged)
    forceRecordStatus = Property(str, lambda self: "10 Hz记录：模拟数据，不连接硬件", notify=forceDataChanged)
    manualGripActive = Property(bool, lambda self: self.manual_active, notify=manualGripDataChanged)
    manualGripHasData = Property(bool, lambda self: bool(self.manual_samples), notify=manualGripDataChanged)
    manualGripStale = Property(bool, lambda self: self.manual_stale, notify=manualGripDataChanged)
    manualGripAxes = Property(int, lambda self: self.axes, notify=manualGripDataChanged)
    manualGripTime = Property(float, lambda self: self.manual_time, notify=manualGripDataChanged)
    manualGripValues = Property("QVariantList", lambda self: self.manual_values, notify=manualGripDataChanged)
    manualPhotoBusy = Property(bool, lambda self: self.manual_photo_busy, notify=manualPhotoChanged)
    manualPhotoStatus = Property(str, lambda self: self.manual_photo_status, notify=manualPhotoChanged)

    @Slot()
    def captureManualGripPhoto(self):
        if self.manual_photo_busy or self.axes not in (4, 6):
            return
        if self.state == 3:
            commanded = (0, 0, 0, 0)
        elif self.state == 4 and self.manual_active and self.manual_samples and not self.manual_stale:
            commanded = tuple(self.manual_values[index] for index in (1, 13, 15, 17))
        else:
            return
        self.manual_photo_requests.append(commanded)
        self.manual_photo_busy = True
        self.manual_photo_status = "正在拍照…"
        self.manualPhotoChanged.emit()

    def finish_manual_photo(self, saved_path=None, error=None):
        assert self.manual_photo_busy
        assert bool(saved_path) != bool(error)
        self.manual_photo_busy = False
        self.manual_photo_status = "照片已保存：" + saved_path if saved_path else "拍照失败：" + error
        self.manualPhotoChanged.emit()
        QTest.qWait(50)

    @Slot(float, float, float, float)
    def setManualGripAngle(self, angle, yaw, pitch, roll):
        if self.state != 3 and not (self.state == 4 and self.manual_active):
            return
        pose = (angle, yaw, pitch, roll)
        if self.axes not in (4, 6) or not all(math.isfinite(value) for value in pose):
            return
        if not ((-10 if self.axes == 4 else -20) <= angle <= 20
                and abs(yaw) <= 80 and abs(pitch) <= 50 and abs(roll) <= 60):
            return
        self.manual_requests.append(pose)
        if not self.manual_active:
            self.manual_active = True
            self.manualGripReset.emit()
        self.set_state(4, "手动夹持进行中")
        self.manualGripDataChanged.emit()

    @Slot(int, float, float, int, result="QVariantList")
    def manualGripPlot(self, series, start, end, columns):
        return [[t, values[series], valid and not (self.axes == 4 and series in (6, 7))]
                for t, values, valid in self.manual_samples if start <= t <= end]

    def feed_manual(self, seconds, valid=True):
        self.manual_time = seconds
        self.manual_samples = [(max(0, seconds - 300) + 300 * i / 1000,
                                [5, 4.5] + [-.23 + .01 * math.sin(i / 20 + ch) for ch in range(4)]
                                + [1000 * axis + i for axis in range(6)]
                                + [15, 14.125, -10, -9.375, 30, 29.625], valid and not 480 < i < 500)
                               for i in range(1001)]
        self.manual_values = self.manual_samples[-1][1] if valid else [None] * 18
        self.manual_stale = not valid
        self.manualGripDataChanged.emit()
        QTest.qWait(100)

    @Slot(int, float, float, int, result="QVariantList")
    def forcePlot(self, channel, start, end, columns):
        samples = [p for p in self.force_samples if start <= p[0] <= end]
        return [[t, f1 + f2 if channel == 0 else f1 if channel == 1 else f2, valid]
                for t, f1, f2, valid in samples]

    def begin_force(self, mode):
        self.force_mode = mode
        self.force_time = 0.0
        self.force_values = []
        self.force_samples = []
        self.forceReset.emit()
        self.forceDataChanged.emit()
        QTest.qWait(50)

    def feed_force(self, seconds, valid=True):
        start = max(0, seconds - 300) if self.force_mode == 1 else 0
        self.force_samples = [(start + (seconds - start) * i / 1000,
                              2 + math.sin(i / 23), 1 + 0.7 * math.cos(i / 31),
                              valid and not 480 < i < 500) for i in range(1001)]
        self.force_time = seconds
        self.force_values = [sum(self.force_samples[-1][1:3]), *self.force_samples[-1][1:3]] if valid else []
        self.forceDataChanged.emit()
        QTest.qWait(100)

    @Slot(int)
    def selectInstrumentAxes(self, axes):
        if axes == self.axes:
            return
        self.axes_requests.append(axes)
        self.axes = 0
        self.set_state(7, "正在配置器械，请稍候…")

    def acknowledge_axes(self, axes):
        self.axes = axes
        name = "4.5MM" if axes == 4 else "3.5MM"
        self.set_state(1, f"已选择{name}器械，请先将器械归零。" if axes else "器械参数加载失败，请重新选择轴数。")

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

# Exercise the actual QML chart layout and its history controls with synthetic input.
backend.set_state(4, "测试进行中，归零和模式切换已锁定。")
backend.begin_force(1)
backend.feed_force(420)
panel = item("forcePanel")
assert window.property("showForcePlots") and panel.property("visible")
assert panel.property("startTime") == 120 and panel.property("endTime") == 420
assert not item("forceViewToggle").property("enabled")
assert item("forceTotalChart").property("points") is not None
for size in [(1280, 800), (800, 640)]:
    window.resize(*size)
    QTest.qWait(100)
    for name in ["forceTotalChart", "forceSensor1Chart", "forceSensor2Chart", "testStopButton"]:
        control = item(name)
        top = control.mapToScene(QPointF(0, 0))
        bottom = control.mapToScene(QPointF(control.width(), control.height()))
        assert top.x() >= 0 and top.y() >= 0 and bottom.x() <= size[0] and bottom.y() <= size[1], name
    assert window.grabWindow().save(str(root / f"output/force-grip-{size[0]}.png"))
backend.feed_force(421, False)
assert backend.force_values == []
backend.begin_force(2)
assert backend.force_time == 0 and backend.force_samples == []
backend.feed_force(72000)
assert panel.property("startTime") == 0 and panel.property("endTime") == 72000
zoom = item("forceZoom")
zoom.setProperty("currentIndex", 2)
QTest.qWait(50)
assert panel.property("startTime") == 71700
panel.setProperty("seekStart", 3600)
panel.setProperty("follow", False)
QTest.qWait(50)
assert panel.property("startTime") == 3600 and panel.property("endTime") == 3900
backend.feed_force(72001)
assert panel.property("startTime") == 3600, "Incoming samples must not move a historical viewport"
click(item("forceFollow"))
assert panel.property("startTime") == 71701
zoom.setProperty("currentIndex", 0)
QTest.qWait(50)
assert window.grabWindow().save(str(root / "output/force-endurance.png"))
backend.set_state(1, "测试已停止；请先将器械归零。")
assert panel.property("visible") and backend.force_samples, "Stop must retain the trace"
click(item("forceViewToggle"))
assert not panel.property("visible") and not enter.property("enabled")
click(item("forceViewToggle"))
assert panel.property("visible")
backend.begin_force(0)
assert not panel.property("visible"), "Pre-run must hide and reset force charts"
backend.set_state(3, "器械已归零。")

manual_mode = item("manualGripModeButton")
click(manual_mode)
assert window.property("showManualPlots")
manual_input = item("manualGripAngleInput")
manual_inputs = [manual_input, item("manualYawAngleInput"), item("manualPitchAngleInput"), item("manualRollAngleInput")]
manual_apply = item("manualGripApplyButton")
manual_photo = item("manualGripPhotoButton")
manual_photo_status = item("manualGripPhotoStatus")
manual_photo_tip = window.findChild(QObject, "manualGripPhotoTooltip")
assert manual_photo_tip is not None
manual_back = item("manualGripBackButton")
angle_selector = item("manualAngleSelector")
angle_chart = item("manualAngleChart")
assert manual_apply.property("enabled")
assert manual_photo.property("enabled"), "A homed instrument may be photographed before the first move"
click(manual_photo)
assert backend.manual_photo_requests == [(0, 0, 0, 0)]
assert not manual_photo.property("enabled") and manual_photo.property("text") == "拍照中…"
assert manual_apply.property("enabled"), "A photo must not lock angle execution"
assert not shutdown.property("enabled") and not window.close(), "Shutdown/close must wait for photo completion"
click(manual_photo)
assert len(backend.manual_photo_requests) == 1, "A duplicate photo click must be ignored"
for field, value in zip(manual_inputs, (5, 15, -10, 30)):
    field.setProperty("text", str(value))
QTest.qWait(50)
assert not backend.manual_requests, "Typing must never issue motion"
click(manual_apply)
assert backend.manual_requests == [(5, 15, -10, 30)] and backend.manual_active
assert not manual_back.property("enabled") and stop.property("enabled")
photo_path = "/home/a/Desktop/TestPhotos/Angle_Under_20/ManualGrip_" + "Command_0_" * 18 + "1730700000000.jpg"
backend.finish_manual_photo(saved_path=photo_path)
assert photo_path in manual_photo_status.property("text")
assert not manual_photo.property("enabled"), "A started manual session must wait for its first sample"
backend.feed_manual(1)
assert manual_photo.property("enabled")
for field, minimum, maximum in zip(manual_inputs, (-20, -80, -50, -60), (20, 80, 50, 60)):
    previous = field.property("text")
    for invalid in [str(maximum + .001), str(minimum - .001), "nan", "inf", "-inf", "", " "]:
        field.setProperty("text", invalid)
        QTest.qWait(20)
        assert not manual_apply.property("enabled"), (field.objectName(), invalid)
        click(manual_apply)
        assert len(backend.manual_requests) == 1, "An invalid field must block the complete pose"
    field.setProperty("text", previous)
click(manual_photo)
assert backend.manual_photo_requests[-1] == (4.5, 14.125, -9.375, 29.625)
photo_command = backend.manual_photo_requests[-1]
for field, value in zip(manual_inputs, (-5, -25, 12, 45)):
    field.setProperty("text", str(value))
QTest.qWait(20)
assert backend.manual_requests == [(5, 15, -10, 30)], "Editing a running command must not execute it"
click(manual_apply)
assert backend.manual_requests == [(5, 15, -10, 30), (-5, -25, 12, 45)]
assert not manual_photo.property("enabled") and stop.property("enabled")
assert backend.manual_photo_requests[-1] == photo_command, "Editing/applying new angles must retain request-time photo angles"
backend.finish_manual_photo(error="未检测到摄像头")
assert "未检测到摄像头" in manual_photo_status.property("text") and manual_photo.property("enabled")
click(manual_photo)
assert len(backend.manual_photo_requests) == 3, "A failed photo must allow a fresh request"
backend.finish_manual_photo(saved_path=photo_path)
assert manual_photo.property("enabled") and photo_path in manual_photo_status.property("text")
for axes in [4, 6]:
    backend.axes = axes
    backend.changed.emit()
    minimum = -10 if axes == 4 else -20
    manual_input.setProperty("text", str(minimum - .001))
    QTest.qWait(20)
    assert not manual_apply.property("enabled"), "Opening minimum must follow the instrument type"
    for pose in [(minimum, -80, -50, -60), (20, 80, 50, 60)]:
        for field, value in zip(manual_inputs, pose):
            field.setProperty("text", str(value))
        QTest.qWait(20)
        assert manual_apply.property("enabled"), (axes, pose)
        previous_count = len(backend.manual_requests)
        click(manual_apply)
        assert len(backend.manual_requests) == previous_count + 1 and backend.manual_requests[-1] == pose
    backend.feed_manual(420)
    for size in [(1280, 800), (800, 640)]:
        window.resize(*size)
        angle_selector.setProperty("currentIndex", 0)
        QTest.qWait(100)
        names = ["manualGripAngleInput", "manualYawAngleInput", "manualPitchAngleInput", "manualRollAngleInput",
                 "manualAngleSelector", "manualGripApplyButton", "manualGripPhotoButton", "manualGripPhotoStatus", "testStopButton",
                 "manualCanvas0", "manualCanvas2", "manualCanvas8" if axes == 4 else "manualCanvas6"]
        for name in names:
            control = item(name)
            top = control.mapToScene(QPointF(0, 0))
            bottom = control.mapToScene(QPointF(control.width(), control.height()))
            assert top.x() >= 0 and top.y() >= 0 and bottom.x() <= size[0] and bottom.y() <= size[1], (name, size, top, bottom)
        assert manual_photo_status.property("truncated"), "A long saved path must elide within the manual layout"
        assert manual_photo_status.height() <= 18
        assert manual_photo.mapToScene(QPointF(0, 0)).x() >= manual_apply.mapToScene(QPointF(manual_apply.width(), 0)).x()
        assert manual_photo.mapToScene(QPointF(0, 0)).y() == manual_apply.mapToScene(QPointF(0, 0)).y()
        hover = manual_photo_status.mapToScene(QPointF(manual_photo_status.width() / 2, manual_photo_status.height() / 2))
        QTest.mouseMove(window, hover.toPoint())
        QTest.qWait(600)
        assert manual_photo_tip.property("visible") and photo_path in manual_photo_tip.property("text")
        assert manual_photo_tip.property("width") <= manual_photo_status.width()
        QTest.mouseMove(window, QPointF(1, 1).toPoint())
        QTest.qWait(50)
        assert window.grabWindow().save(str(root / f"output/manual-grip-{axes}-{size[0]}.png"))
        for index, pair, expected in [(1, [12, 13], "14.125"), (2, [14, 15], "-9.375"), (3, [16, 17], "29.625")]:
            angle_selector.setProperty("currentIndex", index)
            QTest.qWait(80)
            series = angle_chart.property("series")
            series = series.toVariant() if hasattr(series, "toVariant") else series
            assert series == pair, (index, series)
            canvas = item("manualCanvas" + str(pair[0]))
            assert canvas.property("visible") and canvas.height() > 0
            # Orientation values must retain decimal precision, not encoder integer formatting.
            visible_texts = []
            descendants = [angle_chart]
            while descendants:
                child = descendants.pop()
                visible_texts.append(str(child.property("text")))
                descendants.extend(child.childItems())
            assert any(expected in text for text in visible_texts), (index, visible_texts)
        assert window.grabWindow().save(str(root / f"output/manual-roll-{axes}-{size[0]}.png"))
backend.feed_manual(421, False)
assert "失效" in item("manualGripDataStatus").property("text")
assert not manual_photo.property("enabled"), "Stale manual data must block a photo request"
photo_count = len(backend.manual_photo_requests)
click(manual_photo)
assert len(backend.manual_photo_requests) == photo_count
backend.feed_manual(422)
backend.manual_active = False
backend.manualGripDataChanged.emit()
QTest.qWait(20)
assert not manual_photo.property("enabled"), "Automatic tests must not enable the manual photo action"
backend.manual_active = True
backend.manualGripDataChanged.emit()
for state in (0, 1, 2, 5, 6, 7):
    backend.set_state(state, "拍照不可用状态")
    assert not manual_photo.property("enabled"), state
    click(manual_photo)
assert len(backend.manual_photo_requests) == photo_count
backend.set_state(4, "手动夹持进行中")
assert manual_photo.property("enabled")
click(manual_photo)
assert stop.property("enabled"), "Photo capture must keep the stop action available"
click(stop)
assert not manual_apply.property("enabled")
assert all(not field.property("enabled") for field in manual_inputs)
assert backend.state == 6 and backend.manual_photo_busy
backend.finish_manual_photo(error="照片写入失败")
assert not manual_photo.property("enabled") and "照片写入失败" in manual_photo_status.property("text")
backend.manual_active = False
backend.manualGripDataChanged.emit()
backend.set_state(1, "已停止，请重新归零")
assert not manual_apply.property("enabled") and home.property("enabled")
request_count = len(backend.manual_requests)
for field in manual_inputs:
    field.setProperty("text", "0")
click(manual_apply)
assert len(backend.manual_requests) == request_count, "Stopped pose must not restart without homing"
assert backend.manual_samples, "Stop must retain the plotted trace"
click(manual_back)
assert not window.property("showManualPlots") and not manual_mode.property("enabled")
click(home)
assert backend.state == 2 and not manual_mode.property("enabled")
backend.set_state(3, "器械已归零。")

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
print("PASS: QML, automatic modes and force charts; four-angle submission/update/limits, manual photo busy/success/failure/retry/stale-state guards, concurrent angle/stop actions, 4/6-axis charts, 2 sizes")
window.hide()
engine.deleteLater()
app.processEvents()
