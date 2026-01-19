import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick 2.7
import QtQuick.Controls 2.2
// first initialization page

Item {

    id: initPage_ParentItem
    width: parent.width
    height: parent.height

    property int textLeftRowOffset: -250
    property int textRightRowOffset: 310

    property int textFirstColOffset: 50
    property int textSecondColOffset: 100
    property int textThirdColOffset: 150

    property int imageLeftRowOffset:textLeftRowOffset-230
    property int imageRightRowOffset:textRightRowOffset-297

    property int imageFirstColOffset: textFirstColOffset
    property int imageSecondColOffset: textSecondColOffset
    property int imageThirdColOffset: textThirdColOffset

    property int startStep : 0


    //初始界面背景
    Image {
        id: init_freamBg
        anchors.centerIn: parent
        //source: "images/freambg.png"
        source: "images/bg_initbg.png"
        anchors.verticalCenterOffset: 0
        anchors.horizontalCenterOffset: 0
        smooth: true

        Image {
            //@disable-check M16
            objectName: "init_mainhandsta"
            id: init_mainhandsta
            y: 425
            width: 61
            height: 53
            anchors.verticalCenter: parent.verticalCenter
            //anchors.left: parent.left
            source: "images/icon_nostate.png"
            anchors.leftMargin: 330
            anchors.verticalCenterOffset: imageFirstColOffset
            anchors.horizontalCenterOffset: imageLeftRowOffset
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
            function setsta(sta)
            {
                if(sta===1)
                {
                    source= "images/icon_nostate.png"
                }
                if(sta===2)
                {
                    source= "images/icon_normal.png"
                }
                else
                {
                    source= "images/icon_warning.png"
                }
            }
        }

        Image {
            //@disable-check M16
            objectName: "init_liftingsta"
            id: init_liftingsta
            y: 487
            width: 61
            height: 54
            visible: true
            anchors.verticalCenter: parent.verticalCenter
        //    anchors.left: parent.left
            source: "images/icon_nostate.png"
            anchors.leftMargin: 330
            anchors.verticalCenterOffset: imageSecondColOffset
            anchors.horizontalCenterOffset: imageLeftRowOffset
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
            function setsta(sta)
            {
                if(sta===1)
                {
                    source= "images/icon_nostate.png"
                }
                if(sta===2)
                {
                    source= "images/icon_normal.png"
                }
                else
                {
                    source= "images/icon_warning.png"
                }
            }
        }

        Image {
            //@disable-check M16
            objectName: "init_armsta"
            id: init_armsta
            y: 745
            width: 61
            height: 53
            anchors.verticalCenter: parent.verticalCenter
           // anchors.left: parent.left
            source: "images/icon_nostate.png"
            anchors.leftMargin: 330// "qrc:/qtquickplugin/images/template_image.png"
            anchors.verticalCenterOffset: imageThirdColOffset
            anchors.horizontalCenterOffset: imageLeftRowOffset
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
            function setsta(sta)
            {
                if(sta===1)
                {
                    source= "images/icon_nostate.png"
                }
                if(sta===2)
                {
                    source= "images/icon_normal.png"
                }
                else
                {
                    source= "images/icon_warning.png"
                }
            }
        }

        Image {
            //@disable-check M16
            objectName: "init_leftfinialsta"
            id: init_leftfinialsta
            y: 809 //809
            width: 61
            height: 53
            anchors.verticalCenter: parent.verticalCenter
            //anchors.left: parent.left
            source: "images/icon_nostate.png"
            anchors.leftMargin: 330//"qrc:/qtquickplugin/images/template_image.png"
            anchors.verticalCenterOffset: imageFirstColOffset
            anchors.horizontalCenterOffset: imageRightRowOffset
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
            function setsta(sta)
            {
                if(sta===1)
                {
                    source= "images/icon_nostate.png"
                }
                if(sta===2)
                {
                    source= "images/icon_normal.png"
                }
                else
                {
                    source= "images/icon_warning.png"
                }
            }
        }

        Image {
            //@disable-check M16
            objectName: "init_rightfinialsta"
            id: init_rightfinialsta
            y: 678
            width: 61
            height: 53
            anchors.verticalCenter: parent.verticalCenter
           // anchors.left: parent.left
            source: "images/icon_nostate.png"
            anchors.leftMargin: 330
            anchors.verticalCenterOffset: imageSecondColOffset
            anchors.horizontalCenterOffset: imageRightRowOffset
            anchors.horizontalCenter: parent.horizontalCenter//"qrc:/qtquickplugin/images/template_image.png"
            fillMode: Image.PreserveAspectFit
            function setsta(sta)
            {
                if(sta===1)
                {
                    source= "images/icon_nostate.png"
                }
                if(sta===2)
                {
                    source= "images/icon_normal.png"
                }
                else
                {
                    source= "images/icon_warning.png"
                }
            }
        }
    }

    //启动按钮
    Button {
        //@disable-check M16
        objectName: "init_ContinuousBtn"
        id:init_ContinuousBtn
        y: 900
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 34
        anchors.horizontalCenterOffset: 0 // 设置字体大小
        // 设置按钮文本
        contentItem: Text {
            id: init_text_init_ContinuousBtn
            text: qsTr("继续")
            font.pixelSize: 34
            opacity: 1.0
            color: "#48FFFF"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenterOffset: 0
            elide: Text.ElideRight
            anchors.verticalCenter: parent.verticalCenter
        }
        // 设置按钮背景
        background: Rectangle {
            id:init_rect_init_ContinuousBtn
            implicitWidth: 200
            implicitHeight: 64
            opacity: 0.25
            color: "#003033"
            border.color: "#2CDFE8"
            border.width: 2
            radius:10
        }
        visible:true
        enabled: true
        function setsta(action)
        {
            switch(action)
            {
                case 3:
                    init_text_init_ContinuousBtn.text = qsTr("启动")
                    enabled = true
                    break
                case 2:
                    initPage_ParentItem.startStep = 1
                    init_text_init_ContinuousBtn.text = qsTr("继续")
                    enabled = true
                    break
                case 1:
                    init_text_init_ContinuousBtn.text = qsTr("启动中")
                    enabled = false
                    UIinterface.startSystem();
                    break
                case 0:
                    init_text_init_ContinuousBtn.text = qsTr("启动失败")
                    enabled = false
                    break
            }
        }
        onClicked: {
            if(initPage_ParentItem.startStep === 0)
            {
                init_newLogin.open();
            }
            if(initPage_ParentItem.startStep === 1)
            {
                init_rect_init_ContinuousBtn.color="#48FFFF"

                initPage.visible = false
                mainPage.visible = true
            }

        }
    }

    //输入密码弹窗
    Popup{
          id: init_newLogin
          width: 800
          height: 350
          anchors.centerIn: Overlay.overlay
          modal: true
          focus: true
          closePolicy: Popup.NoAutoClose
          background: Image {
              id: newLoginBg
              anchors.centerIn: parent
              source: "images/bg_add_win.png"
          }
          Rectangle {
              id: borderrect
              x:20
              y:-180
              width: 745
              height:220
              color: "#003033"
              border.color: "#18fefe"
              opacity: 0.8
          }
          Row {
              spacing: 10
              y: -150
              anchors.horizontalCenter: parent.horizontalCenter
              Text {
                  y: 0
                  text: qsTr("密码：") ;
                  color: "#18fefe"
                  font.pointSize: 32
                  verticalAlignment: Text.AlignVCenter
              }
              Rectangle {
                  width: 540
                  height: 64
                  color: "#003033"
                  border.color: "#2CDFE8"
                  TextInput {
                      id: docPwd
                      echoMode: TextInput.Password
                      font.pointSize: 32
                      anchors.margins: 4
                      color: "#18fefe"
                      opacity: 1
                      focus: true
                      maximumLength: 16
                      anchors.fill: parent
                      verticalAlignment: TextInput.AlignVCenter
                      onAccepted:
                      {
                        if(docPwd.text == '1234')
                        {
                          docPwd.text = ''
                          console.log("")
                          init_newLogin.close();
                          init_ContinuousBtn.setsta(1)
                        }
                         else
                         {
                             btnOkText.color = '#F56C6C'
                             btnOkBorder.border.color = "#F56C6C"
                         }
                      }
                  }
              }
          }
          //登陆按钮
          Button
          {
              id: btnOk
              x: 280
              y: -50
              // 设置按钮文本
              contentItem: Text {
                  id: btnOkText
                  text: qsTr("登录")
                  font.pixelSize: 28
                  opacity: 1.0
                  color: "#18fefe"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
              }
              // 设置按钮背景
              background: Rectangle {
                  id: btnOkBorder
                  implicitWidth: 120
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
              }
              onClicked:
              {
                if(docPwd.text == '1234')
                {
                    docPwd.text = ''
                    init_newLogin.close();
                    init_ContinuousBtn.setsta(1)
                }
                else
                {
                     btnOkText.color = '#F56C6C'
                     btnOkBorder.border.color = "#F56C6C"
                }
            }
          }
          //取消按钮
          Button {
              id: btnCancel
              x: 420
              y: -50
              // 设置按钮文本
              contentItem: Text {
                  id: btnCancelText
                  text: qsTr("取消")
                  font.pixelSize: 28
                  opacity: 1.0
                  color: "#18fefe"
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
                  docPwd.text = ''
                  init_newLogin.close()
              }
          }
      }


    //主手信息检查
    Text {
        //@disable-check M16
        objectName: "init_txtMasterConsoleCheck"
        id: init_txtMasterConsoleCheck
        y: 445
        font.pixelSize: 44
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignTop
        anchors.leftMargin: 760
        anchors.verticalCenterOffset:textFirstColOffset// -70
        anchors.horizontalCenterOffset: textLeftRowOffset-32
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.bold: true
        font.family: "Microsoft YaHei UI"
        font.letterSpacing: 20
        color: "#18FEFE"
        text: "主手检查中"
        elide: Text.ElideNone

    // anchors.left: parent.left
    }

    //升降柱信息检查
    Text {
        //@disable-check M16
        objectName: "init_txt_liftingcheck"
        id: init_txt_liftingcheck
        y: 338
        color: "#18fefe"
        text: "升降柱检查中"
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: parent.verticalCenter
        //anchors.left: parent.left
        anchors.leftMargin: 760
        anchors.verticalCenterOffset: textSecondColOffset
        anchors.horizontalCenterOffset: textLeftRowOffset
        anchors.horizontalCenter: parent.horizontalCenter
        font.letterSpacing: 20
        font.pixelSize: 44
        font.bold: true
        font.family: "Microsoft YaHei UI"
    }

    //支撑臂信息检查
    Text {
        //@disable-check M16
        objectName: "init_txt_armcheck"
        id: init_txt_armcheck
        y: 574
        color: "#18fefe"
        text: "     Ethercat检查中"
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: parent.verticalCenter
        //anchors.left: parent.left
        font.letterSpacing: 20
        font.pixelSize: 44
        anchors.leftMargin: 760
        anchors.verticalCenterOffset: textThirdColOffset
        anchors.horizontalCenterOffset: textLeftRowOffset
        anchors.horizontalCenter: parent.horizontalCenter
        font.bold: true
        font.family: "Microsoft YaHei UI"

        // Image {
        //     id: image2
        //     x: 1029
        //     y: 617
        //     width: 61
        //     height: 53
        //     source: "qrc:/qtquickplugin/images/template_image.png"
        //     fillMode: Image.PreserveAspectFit
        // }

        // Image {
        //     id: image3
        //     x: 1037
        //     y: 612
        //     width: 61
        //     height: 53
        //     source: "qrc:/qtquickplugin/images/template_image.png"
        //     fillMode: Image.PreserveAspectFit
        // }
    }

        //左侧末端信息检查
        Text {
            //@disable-check M16
            objectName: "init_txt_leftfinialcheck"
            id: init_txt_leftfinialcheck
            y: 638
            color: "#18fefe"
            text: "左执行末端检查中"
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: parent.verticalCenter
            //anchors.left: parent.left
            font.letterSpacing: 20
            font.pixelSize: 44
            anchors.leftMargin: 760
            anchors.verticalCenterOffset: textFirstColOffset
            anchors.horizontalCenterOffset: textRightRowOffset
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
            font.family: "Microsoft YaHei UI"
        }

        //右侧末端信息检查
        Text {
            //@disable-check M16
            objectName: "init_txt_rightfinialcheck"
            id: init_txt_rightfinialcheck
            y: 702
            color: "#13e0da"
            text: "右执行末端检查中"
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignCenter
            anchors.verticalCenter: parent.verticalCenter
            // anchors.left: parent.left
            font.letterSpacing: 20
            font.pixelSize: 44
            anchors.leftMargin: 760
            anchors.verticalCenterOffset: textSecondColOffset
            anchors.horizontalCenterOffset: textRightRowOffset
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
            font.family: "Microsoft YaHei UI"
        }



    //进度条
    ProgressBar {
              property color proColor: "#13e0da"//"#148014"
              property color proBackgroundColor: "#13e0da"
              property int proWidth: 0
              property real progress: 0
              property real proRadius: 50
              property alias interval: timer.interval
              property int w: 1000
              property int h: 20
              property bool run: true

              //@disable-check M16
              objectName: "init_initprogressbar"
              id: init_initprogressbar
              x:475
              y:500
             // anchors.centerIn: parent
              value: (progress/100) //进度条默认值
              padding: 0
              width: init_initprogressbar.w;
              height:init_initprogressbar.h;
              visible: true

              //启动过程信息
              Text
              {
                    id: init_initStatusText
                    width: 400
                    height: 60
                    x:340
                    anchors.top: parent.top
                    anchors.topMargin: -60  // 调整顶部的间距
                    font.letterSpacing: 20
                    font.pixelSize: 40
                    font.bold: true
                    font.family: "Microsoft YaHei UI"
                    text: qsTr("启动中")
                    color: "#48FFFF"
              }


              //取计时器状态
              function isRunning(){
                  return(timer.running)
              }

              //启动计时器来更新进度值
              function onStart(){
                  init_initprogressbar.progress = 0;
                  timer.running = true;//通知计时器
              }

              //进度已满,停止计时器
              function onStop(){
                  timer.running = false;
                  //停止后重置进度,重新开始跑进度
                  init_initprogressbar.progress=0;
                  init_initprogressbar.onStart()
              }

              //进度背景色
              background: Rectangle
              {
                  id: initprogressbar_backgroundrect
                  implicitWidth: init_initprogressbar.w
                  implicitHeight: init_initprogressbar.h
                  color: init_initprogressbar.proBackgroundColor
                  radius: init_initprogressbar.proRadius
                  border.color: "#18fefe"
                  //border.color: "#13e0da"
                  border.width: 1
                  opacity: 0.1
              }

                Rectangle {
                    id: initprogressbar_borderrect
                    x:initprogressbar_backgroundrect.x
                    y:initprogressbar_backgroundrect.y
                    width: initprogressbar_backgroundrect.width
                    height:initprogressbar_backgroundrect.height
                    radius: init_initprogressbar.proRadius
                    color: "#003033"
                    border.color: "#18fefe"
                    opacity: 0.8
                }

                //当前进度色
                contentItem: Item {
                    implicitWidth: init_initprogressbar.w
                    implicitHeight: init_initprogressbar.h
                    Rectangle {
                        width: init_initprogressbar.visualPosition * init_initprogressbard.w
                        height: init_initprogressbar.h
                        radius: init_initprogressbar.proRadius
                        color: init_initprogressbar.proColor
                    }
                }
                //@disable-check M300
                Timer{
                    id: timer
                    running: false //默认不启动
                    repeat: true //重复使用
                    interval: 10 //每50毫秒响应一次
                    onTriggered:{
                        console.log("progressbar timer trig");
                        init_initprogressbar.progress++;//响应进度
                        if (init_initprogressbar.progress > 100){
                            init_initprogressbar.onStop();
                            return;
                        }
                    }
                }
            }
}
