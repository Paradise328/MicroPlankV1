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
    Image {
        id: commonPageBg
        source: "images/bg_mainpage_common.png"
        width: Screen.desktopAvailableWidth
        height: Screen.desktopAvailableheight
        smooth: true
    }
    Image {
        id: logoName
        source: "images/logo_title.png"
        x: 0
        y: 0
        smooth: true
    }

    RobotPage{
        id: robotPage
        visible:isRobotPage
    }

    IntroducePage{
        id: introducePage
        visible:!isRobotPage
    }

    //机器人设置tab
    Rectangle{
        id: tabRobot
        width: 120
        height: 42
        x: 435
        y: 24
        color: Qt.rgba(0/255,135/255,255/255, 0.25)
        border.color: "#1A9AD9"
        border.width: 1
        state: "robotSetting"
        Text {
            id: robotSettingText
            font.pixelSize: 30
            width: parent.width
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: qsTr( "术前准备")
            color: "#FFFFFF"
        }
        MouseArea{
            anchors.fill: parent;
            acceptedButtons: Qt.LeftButton;
            onClicked: {
<<<<<<< HEAD
=======
              //TODO: end thread of controlling
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
              uiInterface.setCurrentPage(0);
              tabRobot.color = Qt.rgba(0/255,135/255,255/255, 0.25)
              tabImage.color = "transparent"
              imageVideoText.color = itemColor
              robotSettingText.color = "#FFFFFF"
              isRobotPage = true
            }
        }
    }

    //成像视频tab
    Rectangle{
        id: tabImage
        width: 120
        height: 42
        x: 605
        y: 24
        color: "transparent"
        border.color: "#1A9AD9"
        border.width: 1
        state: "robotSetting"
        Text {
            id: imageVideoText
            font.pixelSize: 30
            width: parent.width
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: qsTr( "手术中")
            opacity: 1
            color: itemColor

        }
        MouseArea{
            anchors.fill: parent;
            acceptedButtons: Qt.LeftButton;
            onClicked: {
<<<<<<< HEAD
=======
              //TODO: start thread of controlling
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
              uiInterface.setCurrentPage(1);
              tabRobot.color = "transparent"
              tabImage.color = Qt.rgba(0/255,135/255,255/255, 0.25)
              imageVideoText.color = "#FFFFFF"
              robotSettingText.color = itemColor
              isRobotPage = false
            }
        }
    }
    //系统时间图标
    Image {
        x: 870
        y: 36
        width: 32
        height: 32
        id: iconTime
        source: "images/icon_time.png"
    }

    //系统时间文本赋值：textDateTime.text
    Item{
         x: 906
         y: 30
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
        x: 1220
        y: 30
        width: 36
        height: 36
        background:Image {
             id: newDocBg
             anchors.centerIn: parent
             source: "images/shutdown.png"
         }
        onClicked: Qt.quit()
    }
    Text {
             id: infoVersion
             width: 400
             height: 40
             x: 960
             y: 1000
             font.pixelSize: 34
<<<<<<< HEAD
             text: qsTr("Mikro Elena v1.0@ 2022(R) Version")
=======
             text: qsTr("MikroPlanck v0.0@ 2023(R) Version")
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
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
