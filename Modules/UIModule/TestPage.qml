import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Window {
    id: window
    visible: true
    visibility: Window.Maximized
    width: 1280
    height: 800
    minimumWidth: 800
    minimumHeight: 640
    title: "器械测试"
    color: "#0C151C"

    property int selectedMode: -1
    property bool shutdownRequested: false
    readonly property int testState: UIinterface.testState
    readonly property bool busy: testState === 2 || testState === 4 || testState === 6 || testState === 7
    onTestStateChanged: {
        if (testState !== 3 && testState !== 4) window.selectedMode = -1
    }
    readonly property var modes: [
        { title: "器械预跑", duration: "02 小时", detail: "基础动作预跑，不进入夹持力采集步骤" },
        { title: "夹持力测试（入库前力检测）", duration: "3.5mm负角度为-20°，4.5mm负角度为-10°", detail: "执行夹持动作，读取并记录压力数据" },
        { title: "器械耐久性测试", duration: "20 小时", detail: "完整动作循环，包含夹持力采集步骤" }
    ]

    // Closing the window is not a motor stop command.
    onClosing: {
        if (busy || shutdownRequested) close.accepted = false
    }

    Button {
        id: shutdownButton
        objectName: "testShutdownButton"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 20
        anchors.rightMargin: 40
        width: 120
        height: 40
        text: window.shutdownRequested ? "正在关机…" : "关机"
        enabled: !window.busy && window.testState !== 0 && !window.shutdownRequested
        onClicked: shutdownDialog.open()
        contentItem: Text {
            text: shutdownButton.text
            color: shutdownButton.enabled ? "#FFE8E4" : "#80949F"
            font.pixelSize: 16
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            radius: 8
            color: shutdownButton.down ? "#913D36" : "#3B292B"
            border.color: shutdownButton.activeFocus ? "#FFE8E4" : "#85514D"
        }
    }

    Dialog {
        id: shutdownDialog
        objectName: "testShutdownDialog"
        x: (window.width - width) / 2
        y: (window.height - height) / 2
        width: 360
        title: "确认关机？"
        modal: true
        focus: true
        contentItem: Label {
            text: "将关闭电机并退出控制程序。"
            wrapMode: Text.WordWrap
        }
        footer: DialogButtonBox {
            Button {
                objectName: "testConfirmShutdownButton"
                text: "确认关机"
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            }
            Button {
                objectName: "testCancelShutdownButton"
                text: "取消"
                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            }
        }
        onAccepted: {
            if (!shutdownButton.enabled) return
            window.shutdownRequested = true
            UIinterface.onButton_PowerOff()
        }
    }

    ColumnLayout {
        enabled: !window.shutdownRequested
        width: Math.min(parent.width - 80, 1120)
        anchors.centerIn: parent
        spacing: window.height < 760 ? 10 : 20

        ColumnLayout {
            spacing: window.height < 760 ? 6 : 10
            Text {
                text: "INSTRUMENT TEST"
                color: "#5BD7BD"
                font.pixelSize: 12
                font.letterSpacing: 3
            }
            Text {
                text: "器械测试"
                color: "#F0F5F7"
                font.pixelSize: 34
                font.weight: Font.DemiBold
            }
            Text {
                text: "选择轴数 → 器械归零 → 选择测试模式 → 进入测试"
                color: "#8C9FAE"
                font.pixelSize: 15
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            Text {
                text: "被测器械"
                color: "#D6E1E8"
                font.pixelSize: 16
            }
            Repeater {
                model: [4, 6]
                delegate: Button {
                    id: axesButton
                    objectName: "testAxesButton" + modelData
                    implicitWidth: 180
                    implicitHeight: 40
                    text: modelData === 4 ? "四轴（4 Maxon）" : "六轴（6 Maxon）"
                    checked: UIinterface.instrumentAxes === modelData
                    enabled: window.testState === 1 || window.testState === 3
                    onClicked: UIinterface.selectInstrumentAxes(modelData)
                    contentItem: Text {
                        text: axesButton.text
                        color: axesButton.enabled ? "#DDF9F2" : "#71818C"
                        font.pixelSize: 16
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        radius: 8
                        color: axesButton.checked ? "#193C39" : "#15232D"
                        border.width: axesButton.checked || axesButton.activeFocus ? 2 : 1
                        border.color: axesButton.activeFocus ? "#E1FFF8" : (axesButton.checked ? "#5BD7BD" : "#2A3B46")
                    }
                }
            }
            Item { Layout.fillWidth: true }
        }

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: window.height < 760 ? 80 : 100
            radius: 14
            color: "#15232D"
            border.color: "#263944"
            RowLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 20
                Rectangle {
                    width: 9; height: 9; radius: 5
                    color: UIinterface.testState === 3 ? "#5BD7BD" : "#E8B66A"
                }
                Text {
                    Layout.fillWidth: true
                    text: window.shutdownRequested ? "正在关闭电机并退出控制程序…" : UIinterface.testStatus
                    color: "#D6E1E8"
                    font.pixelSize: 15
                    wrapMode: Text.WordWrap
                }
                Button {
                    id: homeButton
                    objectName: "testHomeRightButton"
                    implicitWidth: 160
                    implicitHeight: 48
                    text: UIinterface.testState === 2 ? "归零中…" : "器械归零"
                    enabled: UIinterface.instrumentAxes > 0 && (UIinterface.testState === 1 || UIinterface.testState === 3)
                    onClicked: UIinterface.homeRightInstrument()
                    contentItem: Text {
                        text: homeButton.text
                        color: homeButton.enabled ? "#DDF9F2" : "#71818C"
                        font.pixelSize: 16
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        radius: 8
                        color: homeButton.down ? "#265849" : "#203B3A"
                        border.color: homeButton.activeFocus ? "#E1FFF8" : "#39635A"
                    }
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 14
            RowLayout {
                Layout.fillWidth: true
                Text {
                    Layout.fillWidth: true
                    text: "选择测试模式"
                    color: "#D6E1E8"
                    font.pixelSize: 17
                }
                Text {
                    objectName: "testElapsedLabel"
                    text: "已测试 " + UIinterface.testElapsed
                    color: "#5BD7BD"
                    font.pixelSize: 17
                }
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 16
                Repeater {
                    model: window.modes
                    delegate: Button {
                        id: modeButton
                        objectName: "testModeButton" + index
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                        implicitHeight: 214
                        // Selection is owned by selectedMode, never by an independent toggle.
                        checked: window.selectedMode === index
                        enabled: UIinterface.testState === 3
                        onClicked: window.selectedMode = index
                        Accessible.name: modelData.title + "，" + modelData.duration
                        background: Rectangle {
                            radius: 14
                            color: modeButton.checked ? "#193C39" : (modeButton.hovered ? "#1A2D38" : "#15232D")
                            border.width: modeButton.checked || modeButton.activeFocus ? 2 : 1
                            border.color: modeButton.activeFocus ? "#E1FFF8" : (modeButton.checked ? "#5BD7BD" : "#2A3B46")
                            opacity: modeButton.enabled ? 1 : 0.7
                        }
                        contentItem: Item {
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 12
                                RowLayout {
                                    Layout.fillWidth: true
                                    Text {
                                        Layout.fillWidth: true
                                        text: "0" + (index + 1)
                                        color: modeButton.checked ? "#5BD7BD" : "#78909E"
                                        font.pixelSize: 16
                                    }
                                    Rectangle {
                                        width: 20; height: 20; radius: 10
                                        color: "transparent"
                                        border.color: modeButton.checked ? "#5BD7BD" : "#5A707E"
                                        Rectangle {
                                            anchors.centerIn: parent
                                            width: 10; height: 10; radius: 5
                                            color: "#5BD7BD"
                                            visible: modeButton.checked
                                        }
                                    }
                                }
                                Text {
                                    Layout.fillWidth: true
                                    text: modelData.title
                                    color: "#F0F5F7"
                                    font.pixelSize: window.width < 1000 ? 19 : 23
                                    font.weight: Font.DemiBold
                                    wrapMode: Text.WordWrap
                                }
                                Text {
                                    Layout.fillWidth: true
                                    text: modelData.duration
                                    color: "#5BD7BD"
                                    font.pixelSize: 13
                                    wrapMode: Text.WordWrap
                                }
                                Text {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    text: modelData.detail
                                    color: "#93A8B6"
                                    font.pixelSize: 13
                                    wrapMode: Text.WordWrap
                                }
                            }
                        }
                    }
                }
            }
        }

        Button {
            id: enterButton
            objectName: "testEnterButton"
            Layout.fillWidth: true
            implicitHeight: 60
            text: UIinterface.testState === 4 ? "测试进行中…" : "进入测试"
            enabled: UIinterface.testState === 3 && window.selectedMode >= 0
            onClicked: UIinterface.enterInstrumentTest(window.selectedMode)
            contentItem: Text {
                text: enterButton.text
                color: enterButton.enabled ? "#0B2924" : "#80949F"
                font.pixelSize: 19
                font.weight: Font.DemiBold
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                radius: 10
                color: enterButton.enabled ? (enterButton.down ? "#42BFA6" : "#5BD7BD") : "#263B43"
                border.color: enterButton.activeFocus ? "#E1FFF8" : "transparent"
            }
        }

        Button {
            id: stopButton
            objectName: "testStopButton"
            Layout.fillWidth: true
            implicitHeight: 48
            text: UIinterface.testState === 6 ? "正在停止…" : "停止"
            enabled: UIinterface.testState === 4
            onClicked: UIinterface.stopInstrumentTest()
            contentItem: Text {
                text: stopButton.text
                color: stopButton.enabled ? "#FFE8E4" : "#80949F"
                font.pixelSize: 19
                font.weight: Font.DemiBold
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                radius: 10
                color: stopButton.enabled ? (stopButton.down ? "#913D36" : "#B34F45") : "#263B43"
                border.color: stopButton.activeFocus ? "#FFE8E4" : "transparent"
            }
        }
    }
}
