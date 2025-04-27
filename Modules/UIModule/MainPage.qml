import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Controls 2.12


Item {
    id: parentPage
    property bool isRobotPage: true
    property color itemColor:  "#48FFFF"
    property color tabBg: "transparent"
    property color chooseTabBg:  "#48FFFF"
    property string systemTime: "2022.01.25 09:44:36"
    property variant currentInfo
    property variant updateVal
    property variant currentLang

    function readJson(){
        robotPage.json = updateVal;
        robotPage.readPara();
    }
    //通用设置背景图
//    Image {
//        id: commonPageBg
//      //  source: "images/bg_mainpage_common.png"
//        width: Screen.desktopAvailableWidth
//        height: Screen.desktopAvailableheight
//        smooth: true
//    }
//    Image {
//        id: logoName
//     //   source: "images/logo_title.png"
//        x: 0
//        y: 0
//        smooth: true
//    }

    PreOperation{
        id: preOpreationPage
        visible:true
    }

    InOpreation{
        id: inOpreationPage
        visible:false
    }

//    //机器人设置tab
//    Rectangle{
//        id: tabRobot
//        width: 120
//        height: 42
//        x: 435
//        y: 24
//        color: Qt.rgba(0/255,135/255,255/255, 0.25)
//        border.color: "#1A9AD9"
//        border.width: 1
//        state: "robotSetting"
//        Text {
//            id: robotSettingText
//            font.pixelSize: 30
//            width: parent.width
//            height: parent.height
//            verticalAlignment: Text.AlignVCenter
//            horizontalAlignment: Text.AlignHCenter
//            text: qsTr( "术前准备")
//            color: "#FFFFFF"
//        }
//        MouseArea{
//            anchors.fill: parent;
//            acceptedButtons: Qt.LeftButton;
//            onClicked: {
//              //TODO: end thread of controlling
//              uiInterface.setCurrentPage(0);
//              tabRobot.color = Qt.rgba(0/255,135/255,255/255, 0.25)
//              tabImage.color = "transparent"
//              imageVideoText.color = itemColor
//              robotSettingText.color = "#FFFFFF"
//              isRobotPage = true
//            }
//        }
//    }

//    //成像视频tab
//    Rectangle{
//        id: tabImage
//        width: 120
//        height: 42
//        x: 605
//        y: 24
//        color: "transparent"
//        border.color: "#1A9AD9"
//        border.width: 1
//        state: "robotSetting"
//        Text {
//            id: imageVideoText
//            font.pixelSize: 30
//            width: parent.width
//            height: parent.height
//            verticalAlignment: Text.AlignVCenter
//            horizontalAlignment: Text.AlignHCenter
//            text: qsTr( "手术中")
//            opacity: 1
//            color: itemColor

//        }
//        MouseArea{
//            anchors.fill: parent;
//            acceptedButtons: Qt.LeftButton;
//            onClicked: {
//              //TODO: start thread of controlling
//              uiInterface.setCurrentPage(1);
//              tabRobot.color = "transparent"
//              tabImage.color = Qt.rgba(0/255,135/255,255/255, 0.25)
//              imageVideoText.color = "#FFFFFF"
//              robotSettingText.color = itemColor
//              isRobotPage = false
//            }
//        }
//    }
    //系统时间图标
//    Image {
//        x: 870
//        y: 36
//        width: 32
//        height: 32
//        id: iconTime
//        source: "images/icon_time.png"
//    }

    //系统时间文本赋值：textDateTime.text
    Item{
         x: 1320
         y: 32
         id: systemTimeText
         Text {
             id: textDateTime
             color: itemColor
             font.pixelSize: 30
             font.bold: true
             text: systemTime
         }
         //@disable-check M300
         Timer {
             id:nowTimer;
             interval: 1000;
             running: true;
             repeat: true
             onTriggered: {
             textDateTime.text = systemTimeText.getCurDate()//此处即是让时间显示到文本中去
            }
         }
         function getCurDate()
         {
             var d = new Date();
             var years = d.getFullYear();
             var month = systemTimeText.add_zero(d.getMonth()+1);
             var days = systemTimeText.add_zero(d.getDate());
             var hours = systemTimeText.add_zero(d.getHours());
             var minutes = systemTimeText.add_zero(d.getMinutes());
             var seconds= systemTimeText.add_zero(d.getSeconds());
             var ndate = years+"."+month+"."+days+" "+hours+":"+minutes+":"+seconds;
             return ndate;
         }
         function add_zero(temp)
         {
             if(temp<10) return "0"+temp;
             else return temp;
         }
     }
    Button {
        x: 1720
        y: 20
//        width: 36
//        height: 36
        Image {
             //anchors.centerIn: parent
             x:30
             y:12
             fillMode: Image.PreserveAspectFit
             source: "images/shutdown.png"
         }
        background:Rectangle {
            //@disable-check M16
            objectName: "background"
            implicitWidth: 150
            implicitHeight: 56
            opacity: 0.25
            color: "#003033"
            border.color: "#2CDFE8"
            border.width: 2
            radius:20
        }
        contentItem: Text {
            //@disable-check M16
            objectName: "contentItem"
            text: qsTr("关机")+"  "
            font.pixelSize: 28
            font.family: "Microsoft YaHei UI"
            opacity: 1.0
            color: "#48FFFF"
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            clip: false
            elide: Text.ElideRight
        }
        onClicked: sureClose.open()
    }
    //确认关闭弹窗
     Popup{
         id: sureClose
         width: 518
         height: 312
         anchors.centerIn: Overlay.overlay
         modal: true
         focus: true
         closePolicy: Popup.NoAutoClose
         background: Image {
             anchors.centerIn: parent
             source: "images/bg_tab_win.png"
         }
         Rectangle {
             id: sureCloseBoard
             x:-50
             y:50
             width: 745
             height:220
             color: "#003033"
             border.color: "#18fefe"
             opacity: 0.8
         }
         Text {
             y: 80
             anchors.horizontalCenter: parent.horizontalCenter
             text: qsTr("确认关闭系统？") ;
             color: itemColor
             font.pointSize: 32
             verticalAlignment: Text.AlignVCenter
         }
         //确定按钮
         Button {
             //@disable-check M16
             objectName: "btn_sureShutDownSystem"
             id: btn_sureShutDownSystem
             x: 205
             y: 175
             // 设置按钮文本
             contentItem: Text {
                 id: btnDelText
                 text: qsTr("确定")
                 font.pixelSize: 28
                 opacity: 1.0
                 color: itemColor
                 horizontalAlignment: Text.AlignHCenter
                 verticalAlignment: Text.AlignVCenter
                 elide: Text.ElideRight
             }
             // 设置按钮背景
             background: Rectangle {
                 implicitWidth: 120
                 implicitHeight: 56
                 opacity: 0.25
                 color: "#003033"
                 border.color: "#2CDFE8"
                 border.width: 2
             }
             onClicked: {
                 console.log("关机 in qml！！！！！")
                 UIinterface.onButton_PowerOff();
                 sureClose.close()
             }
         }
         //取消关机按钮
         Button {
             id: btnDelCancel
             x: 345
             y: 175
             // 设置按钮文本
             contentItem: Text {
                 id: btnDelCancelText
                 text: qsTr("取消")
                 font.pixelSize: 28
                 opacity: 1.0
                 color: itemColor
                 horizontalAlignment: Text.AlignHCenter
                 verticalAlignment: Text.AlignVCenter
                 elide: Text.ElideRight
             }
             // 设置按钮背景
             background: Rectangle {
                 implicitWidth: 120
                 implicitHeight: 56
                 opacity: 0.25
                 color: "#003033"
                 border.color: "#2CDFE8"
                 border.width: 2
             }
             onClicked: {
                 sureClose.close()
             }
         }
     }

    Text {
             id: infoVersion
         //    width: 200
             height: 40
             x: 30
             y: 1000
             font.pixelSize: 34
             text: qsTr("MikroPlanck v0.0@ 2023(R) Version")
             color: "#48FFFF"
         }
    //语言切换
//    BasicComboBox{
//        id:combox
//        x: 1620
//        y: 12
//        width: 175
//        height: 60
//        model: ["中文", "English", "Deutsch"]
//        onActivated: {
//            qmlLanguage.setLanguage(currentIndex);
//            currentLang = currentIndex;
//        }
//    }
    //@disable-check M16
    Component.onCompleted:{
        currentInfo = robotPage.updateJson;
        updateVal = currentInfo;

    }
}
