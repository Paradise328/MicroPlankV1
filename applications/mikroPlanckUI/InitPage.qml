import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
// first initialization page
Item {
    width: parent.width
    height: parent.height
    Image {
        id: initBg
        anchors.centerIn: parent
        source: "images/initbg.png"
        smooth: true
    }
    Image {
        id: freamBg
        anchors.centerIn: parent
        source: "images/freambg.png"
        smooth: true
    }
    Image {
        id: logoBg
        y: 250
        anchors.horizontalCenter: parent.horizontalCenter
        source: "images/logo.png"
        smooth: true
    }

    //@disable-check M300
    CanvasWave {
        id: progressBar
        y: 500
        anchors.horizontalCenter: parent.horizontalCenter
        waveColor: "#5fed6c"
    }
        //状态栏文本赋值：statusText.text
        Rectangle{
            id: statusRect
            width: 400
            height: 50
            y:780
            anchors.horizontalCenter: parent.horizontalCenter
            color: "transparent"
            clip: true
            visible: true
            Text {
                id: statusText
                text: qsTr("机器人开始启动")
                color: "#FFFFFF"
                font.family: "Microsoft YaHei UI"
                font.pixelSize: 32
                anchors.horizontalCenter: parent.horizontalCenter
                verticalAlignment: Text.AlignVCenter
                SequentialAnimation on y{
                    loops: Animation.Infinite
                    PropertyAnimation {
                        from: statusRect.height
                        to: -statusRect.height
                        duration: 1000
                    }
                }
            }
        }
        //复位按钮
        Button {
            id: reserve
            y: 780
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 34 // 设置字体大小
            // 设置按钮文本
            contentItem: Text {
                id: text2
                text: qsTr("复位")
                font.pixelSize: 34
                opacity: 1.0
                color: "#48FFFF"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            // 设置按钮背景
            background: Rectangle {
                id:btn
                implicitWidth: 200
                implicitHeight: 64
                opacity: 0.25
                color: "#003033"
                border.color: "#2CDFE8"
                border.width: 2
            }
            visible:false
//            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                btn.color="#48FFFF"
                if(uiInterface.systemStart()===1){
                initPage.visible = false
                mainPage.visible = true
                }else{
                    btn.color="#003033"
                }
            }
        }
        Text {
                 id: infoVersion
                 width: 400
                 height: 40
                 x: 960
                 y: 1000
                 font.pixelSize: 34
                 text: qsTr("Mikro Elena v1.0@ 2022(R) Version")
                 color: "#48FFFF"
             }
        //测试进度条
        //@disable-check M300
    Timer {
        id: timer
        running: true //进度条启动
        repeat: true
        interval: 30
        onTriggered: {
            if(progressBar.curValue < progressBar.maxValue){
                progressBar.curValue += 5
            }
            if(progressBar.curValue >= progressBar.maxValue){
                statusRect.visible = false
                progressBar.curValue = progressBar.maxValue
                reserve.visible = true
            }
        }
    }
}
