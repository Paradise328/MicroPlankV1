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
    readonly property bool busy: UIinterface.testState === 2 || UIinterface.testState === 4
    readonly property var modes: [
        { title: "器械预跑", duration: "02 小时", detail: "基础动作预跑，不进入夹持力采集步骤" },
        { title: "夹持力测试", duration: "17 小时 · 沿用现有设置", detail: "执行夹持动作，读取并记录压力数据" },
        { title: "器械耐久性测试", duration: "20 小时", detail: "完整动作循环，包含夹持力采集步骤" }
    ]

    // Closing the window is not a motor stop command.
    onClosing: {
        if (busy) close.accepted = false
    }

    ColumnLayout {
        width: Math.min(parent.width - 80, 1120)
        anchors.centerIn: parent
        spacing: window.height < 720 ? 18 : 28

        ColumnLayout {
            spacing: 10
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
                text: "右器械归零 → 选择测试模式 → 进入测试"
                color: "#8C9FAE"
                font.pixelSize: 15
            }
        }

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: 100
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
                    text: UIinterface.testStatus
                    color: "#D6E1E8"
                    font.pixelSize: 15
                    wrapMode: Text.WordWrap
                }
                Button {
                    id: homeButton
                    objectName: "testHomeRightButton"
                    implicitWidth: 160
                    implicitHeight: 48
                    text: UIinterface.testState === 2 ? "归零中…" : "右器械归零"
                    enabled: UIinterface.testState === 1 || UIinterface.testState === 3
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
            Text {
                text: "选择测试模式"
                color: "#D6E1E8"
                font.pixelSize: 17
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
                        enabled: !window.busy && UIinterface.testState !== 5
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
                                    text: modelData.duration
                                    color: "#5BD7BD"
                                    font.pixelSize: 13
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

        Text {
            Layout.fillWidth: true
            text: "测试时长在完整循环结束时检查。运行前确认运动区域无干涉，硬件急停可用。"
            color: "#7C929F"
            font.pixelSize: 12
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
