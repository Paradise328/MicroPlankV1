import QtQuick 2.12
import QtQuick.Layouts 1.12

ColumnLayout {
    id: panel
    property bool compact: false
    readonly property real end: Math.max(300, UIinterface.manualGripTime)
    readonly property real start: Math.max(0, end - 300)
    spacing: 6
    Text {
        Layout.fillWidth: true
        text: "最近5分钟 · 10Hz · 最新缓存，非同步采样 · CH1~4对应逻辑轴0~3；轴4/5无对应扭力"
        wrapMode: Text.WordWrap; color: "#93A8B6"; font.pixelSize: 11
    }
    ManualGripChart {
        Layout.fillWidth: true; Layout.preferredHeight: panel.compact ? 90 : 126
        title: "夹持指令角 / °（非实测）"; series: [0, 1]; labels: ["设定", "指令"]
        rangeStart: panel.start; rangeEnd: panel.end
    }
    ManualGripChart {
        Layout.fillWidth: true; Layout.preferredHeight: panel.compact ? 90 : 126
        title: "扭力原始值（单位待核实）"; series: [2, 3, 4, 5]; labels: ["CH1", "CH2", "CH3", "CH4"]
        rangeStart: panel.start; rangeEnd: panel.end
    }
    ManualGripChart {
        Layout.fillWidth: true; Layout.preferredHeight: panel.compact ? 90 : 126
        title: "实际编码器 / count"
        series: (UIinterface.manualGripAxes || UIinterface.instrumentAxes) === 4 ? [8, 9, 10, 11] : [6, 7, 8, 9, 10, 11]
        labels: (UIinterface.manualGripAxes || UIinterface.instrumentAxes) === 4 ? ["轴2", "轴3", "轴4", "轴5"] : ["轴0", "轴1", "轴2", "轴3", "轴4", "轴5"]
        rangeStart: panel.start; rangeEnd: panel.end
    }
    Text {
        objectName: "manualGripDataStatus"
        text: UIinterface.manualGripActive ? (UIinterface.manualGripStale ? "数据未更新：当前数值已失效，请检查通信" : "采集中 · 扭力仅监测，不作力闭环或超扭保护 · 已测试 " + UIinterface.testElapsed) : "未运行 / 已停止 · 曲线仅保留本次会话，不自动保存文件"
        color: UIinterface.manualGripStale ? "#E8B66A" : "#93A8B6"; font.pixelSize: 11
    }
}
