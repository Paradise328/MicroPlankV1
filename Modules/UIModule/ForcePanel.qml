import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ColumnLayout {
    id: panel
    objectName: "forcePanel"
    property bool compact: false
    property bool follow: true
    property real seekStart: 0
    readonly property real span: UIinterface.forceMode === 1 ? 300 : [0, 60, 300, 1800, 3600][zoom.currentIndex]
    readonly property real endTime: span === 0 ? Math.max(1, UIinterface.forceTime)
        : (follow ? Math.max(span, UIinterface.forceTime) : seekStart + span)
    readonly property real startTime: span === 0 ? 0 : Math.max(0, endTime - span)
    spacing: 8
    Connections {
        target: UIinterface
        function onForceReset() { panel.follow = true; panel.seekStart = 0; zoom.currentIndex = 0 }
    }
    RowLayout {
        Layout.fillWidth: true
        Text {
            text: UIinterface.forceMode === 1 ? "夹持力 · 最近5分钟" : "耐久性 · 全程记录"
            color: "#D6E1E8"
            font.pixelSize: 15
        }
        Item { Layout.fillWidth: true }
        ComboBox {
            id: zoom
            objectName: "forceZoom"
            visible: UIinterface.forceMode === 2
            model: ["全程", "1分钟", "5分钟", "30分钟", "1小时"]
            implicitWidth: 110
            implicitHeight: 32
            onActivated: panel.follow = true
            palette.button: "#203B3A"
            palette.buttonText: "#DDF9F2"
            palette.base: "#15232D"
            palette.text: "#D6E1E8"
            palette.highlight: "#39635A"
            palette.highlightedText: "#FFFFFF"
        }
        Button {
            objectName: "forceFollow"
            visible: UIinterface.forceMode === 2
            text: panel.follow ? "跟随中" : "跟随最新"
            implicitHeight: 32
            onClicked: panel.follow = true
            contentItem: Text { text: parent.text; color: "#DDF9F2"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            background: Rectangle { radius: 6; color: "#203B3A"; border.color: "#39635A" }
        }
    }
    Slider {
        id: historySlider
        objectName: "forceHistorySlider"
        visible: UIinterface.forceMode === 2
        Layout.fillWidth: true
        Layout.preferredHeight: 20
        from: 0
        to: Math.max(0, UIinterface.forceTime - panel.span)
        enabled: panel.span > 0 && to > 0
        value: panel.follow ? to : panel.seekStart
        onMoved: { panel.seekStart = value; panel.follow = false }
        ToolTip.visible: hovered
        ToolTip.text: "选择局部时间范围后，拖动查看历史数据"
        background: Rectangle {
            x: historySlider.leftPadding
            y: historySlider.topPadding + historySlider.availableHeight / 2 - height / 2
            width: historySlider.availableWidth
            height: 3
            radius: 2
            color: "#2A3B46"
        }
        handle: Rectangle {
            x: historySlider.leftPadding + historySlider.visualPosition * (historySlider.availableWidth - width)
            y: historySlider.topPadding + historySlider.availableHeight / 2 - height / 2
            width: 14
            height: 14
            radius: 7
            color: historySlider.enabled ? "#5BD7BD" : "#526873"
            border.color: historySlider.activeFocus ? "#E1FFF8" : "#39635A"
        }
    }
    ForceChart {
        objectName: "forceTotalChart"
        Layout.fillWidth: true
        Layout.preferredHeight: panel.compact ? 120 : 160
        title: "总力"
        channel: 0
        rangeStart: panel.startTime
        rangeEnd: panel.endTime
    }
    RowLayout {
        Layout.fillWidth: true
        spacing: 10
        ForceChart {
            objectName: "forceSensor1Chart"
            Layout.fillWidth: true
            Layout.preferredWidth: 1
            Layout.preferredHeight: panel.compact ? 120 : 150
            title: "力传感器1"
            channel: 1
            lineColor: "#6AB8F5"
            rangeStart: panel.startTime
            rangeEnd: panel.endTime
        }
        ForceChart {
            objectName: "forceSensor2Chart"
            Layout.fillWidth: true
            Layout.preferredWidth: 1
            Layout.preferredHeight: panel.compact ? 120 : 150
            title: "力传感器2"
            channel: 2
            lineColor: "#E8B66A"
            rangeStart: panel.startTime
            rangeEnd: panel.endTime
        }
    }
    Text {
        Layout.fillWidth: true
        text: (UIinterface.forceValues.length === 3 ? "力值沿用现有标定；横轴为测试时间。" : "传感器无有效数据或测试基线无效，曲线断开。")
            + "  " + UIinterface.forceRecordStatus
        color: UIinterface.forceValues.length === 3 && UIinterface.forceRecordStatus.indexOf("失败") < 0 ? "#93A8B6" : "#E8B66A"
        font.pixelSize: 12
        wrapMode: Text.WrapAnywhere
        maximumLineCount: 3
        elide: Text.ElideMiddle
    }
}
