import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Templates 2.12 as T
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12

T.ComboBox {
    id: control

    //checked选中状态，down按下状态，hovered悬停状态
    property color backgroundTheme: "#48FFFF"
    //下拉框背景色
    property color backgroundColor: "transparent"
    //边框颜色
    property color borderColor: Qt.darker(backgroundTheme)
    //item高亮颜色
    property color itemHighlightColor: Qt.darker(backgroundTheme)
    //item普通颜色
    property color itemNormalColor: backgroundTheme
    //每个item的高度
    property int itemHeight: height
    //每个item文本的左右padding
    property int itemPadding: 15
    //下拉按钮颜色
    property color indicatorColor: "#FFFFFF"
    //下拉按钮左右距离
    property int indicatorPadding: 3
    //圆角
    property int radius: 0
    //最多显示的item个数
    property int showCount: 5
    //文字颜色
    property color textColor: "#48FFFF"
    //model数据左侧附加的文字
    property string textLeft: ""
    //model数据右侧附加的文字
    property string textRight: ""

    implicitWidth: 175
    implicitHeight: 60
    spacing: 0
    leftPadding: padding
    rightPadding: padding
    font {
        family: "SimSun"
        pixelSize: 32
    }

    //各item
    delegate: ItemDelegate {
        id: box_item
        height: control.itemHeight
        //Popup如果有padding，这里要减掉2*pop.padding
        width: control.width
        leftPadding: itemPadding
        rightPadding: itemPadding
        contentItem: Text {
            text: control.textLeft + (control.textRole ? (Array.isArray(
                                                              control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData) + control.textRight
            color: control.textColor
            font: control.font
            elide: Text.ElideRight
            renderType: Text.NativeRendering
            verticalAlignment: Text.AlignVCenter
        }
        hoverEnabled: control.hoverEnabled
        background: Rectangle {
            radius: control.radius
            color: (control.highlightedIndex === index) ? control.itemHighlightColor : Qt.rgba(
                                                              0, 0, 0, 0.95)
            //item底部的线
            Rectangle {
                height: 0
                width: parent.width - 2 * control.radius
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                color: Qt.lighter(control.itemNormalColor)
            }
        }
    }

    //box显示item
    contentItem: T.TextField {
        //control的leftPadding会挤过来，不要设置control的padding
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: control.editable ? control.editText : (control.textLeft + control.displayText
                                                     + control.textRight)
        font: control.font
        color: control.textColor
        //默认鼠标选取文本设置为false
        selectByMouse: true
        //选中文本的颜色
        selectedTextColor: "green"
        //选中文本背景色
        selectionColor: "white"
        clip: true
        //renderType: Text.NativeRendering
        enabled: control.editable
        autoScroll: control.editable
        readOnly: control.down
        inputMethodHints: control.inputMethodHints
        validator: control.validator
        renderType: Text.NativeRendering
        background: Rectangle {
            visible: control.enabled && control.editable
            border.width: parent && parent.activeFocus ? 1 : 0
            border.color: control.borderColor
            color: "transparent"
        }
    }

    //box框背景
    background: Rectangle {
        implicitWidth: control.implicitWidth
        implicitHeight: control.implicitHeight
        radius: control.radius
        //        color: control.backgroundColor
        //颜色渐变
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: Qt.rgba(0, 135, 255, 0.05)
            }
            GradientStop {
                position: 0.5
                color: Qt.rgba(0, 135, 255, 0.15)
            }
            GradientStop {
                position: 1.0
                color: Qt.rgba(0, 135, 255, 0.4)
            }
        }
        border.width: 1
        border.color: control.borderColor
    }

    //弹出框
    popup: T.Popup {
        //默认向下弹出，如果距离不够，y会自动调整（）
        y: control.height
        width: control.width
        //根据showCount来设置最多显示item个数
        implicitHeight: control.delegateModel ? ((control.delegateModel.count < showCount) ? contentItem.implicitHeight : control.showCount * control.itemHeight) + 2 : 0
        //用于边框留的padding
        padding: 1
        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            //按行滚动SnapToItem ;像素移动SnapPosition
            snapMode: ListView.SnapToItem
            //ScrollBar.horizontal: ScrollBar { visible: false }
            ScrollBar.vertical: ScrollBar {
                //定制滚动条
                id: box_bar
                implicitWidth: 10
                visible: control.delegateModel
                         && (control.delegateModel.count > showCount)
                //background: Rectangle{} //这是整体的背景
                contentItem: Rectangle {
                    implicitWidth: 10
                    radius: width / 2
                    color: box_bar.pressed ? Qt.rgba(0.6, 0.6,
                                                     0.6) : Qt.rgba(0.6, 0.6,
                                                                    0.6, 0.5)
                }
            }
        }

        //弹出框背景（只有border显示出来了，其余部分被delegate背景遮挡）
        background: Rectangle {
            border.width: 1
            border.color: control.borderColor
            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: Qt.rgba(0, 48, 51, 0.05)
                }
                GradientStop {
                    position: 0.5
                    color: Qt.rgba(26, 154, 219, 0.1)
                }
                GradientStop {
                    position: 1.0
                    color: Qt.rgba(26, 154, 217, 0.2)
                }
            }
            //color: Qt.lighter(themeColor)
            radius: control.radius
        }
    }
}
