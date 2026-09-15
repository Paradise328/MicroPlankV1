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

    def __init__(self):
        super().__init__()
        self.state = 0
        self.status = "正在初始化设备，请稍候…"
        self.home_requests = 0
        self.test_requests = []

    testState = Property(int, lambda self: self.state, notify=changed)
    testStatus = Property(str, lambda self: self.status, notify=changed)

    def set_state(self, state, text):
        self.state, self.status = state, text
        self.changed.emit()
        QTest.qWait(50)

    @Slot()
    def homeRightInstrument(self):
        self.home_requests += 1
        self.set_state(2, "右器械归零中，请勿触碰器械…")

    @Slot(int)
    def enterInstrumentTest(self, mode):
        self.test_requests.append(mode)
        self.set_state(4, "测试进行中，归零和模式切换已锁定。")


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
modes = [item("testModeButton" + str(i)) for i in range(3)]
assert not home.property("enabled") and not enter.property("enabled")
backend.set_state(1, "控制程序已就绪，请确认设备连接并将右器械归零。")
assert home.property("enabled") and not enter.property("enabled")
click(home)
assert backend.home_requests == 1 and not home.property("enabled")
assert all(not m.property("enabled") for m in modes)
backend.set_state(3, "右器械已归零，选择模式后即可进入测试。")
assert not enter.property("enabled"), "Must select a mode first"

for index in range(3):
    if index:
        backend.set_state(3, "右器械已归零，选择模式后即可进入测试。")
    click(modes[index])
    assert window.property("selectedMode") == index
    assert [m.property("checked") for m in modes] == [i == index for i in range(3)]
    # Clicking the same card must not deselect it.
    click(modes[index])
    assert modes[index].property("checked") and enter.property("enabled")
    if index == 0:
        output = root / "output/test-ui-preview.png"
        assert window.grabWindow().save(str(output))
    click(enter)
    assert backend.test_requests[-1] == index
    assert not enter.property("enabled") and not home.property("enabled")
    assert all(not m.property("enabled") for m in modes)
    click(enter)
    assert len(backend.test_requests) == index + 1, "Duplicate start was not blocked"
    backend.set_state(1, "测试已完成；再次测试前，请重新归零。")
    assert not enter.property("enabled") and home.property("enabled")

backend.set_state(5, "收到异常信号，请使用硬件急停并检查设备。")
assert not home.property("enabled") and not enter.property("enabled")
assert all(not m.property("enabled") for m in modes)
backend.set_state(3, "右器械已归零，选择模式后即可进入测试。")
window.resize(800, 640)
QTest.qWait(100)
assert window.grabWindow().save(str(root / "output/test-ui-preview-small.png"))
assert not warnings, "\n".join(warnings)
print("PASS: QML load, 3 modes, exclusive selection, homing/run/fault locks, duplicate clicks, 2 preview sizes")
window.hide()
engine.deleteLater()
app.processEvents()
