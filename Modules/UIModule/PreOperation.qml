import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

/*术前准备页面*/
Rectangle {
    property string signalLamp1: "Normal"
    property string signalLamp2: "Warning"
    property string signalLamp3: "Normal"
    property string lockState :"Unlock"
    property string currentAxis: "axis0"
    property string currentLevel: "level1"
    property real currentMinSpeed: 0
    property real currentMaxSpeed: 100
    property int currentSpeedVal: 0
    property string timeText: "00:01:35"
    property bool isInstrumentAngle30: false

    function setSpeed(axis,level){
        currentMinSpeed = json[axis][level].speedMin;
        currentMaxSpeed = json[axis][level].speedMax;
        currentSpeedVal = json[axis][level].speedVal;
    }

    function changeSpeed(axis,level,val){
        updateJson[axis][level].speedVal = val;
        currentSpeedVal = val;
    }
    function initBtn(){
        currentLevel = "level1";
        level1.state = "selected"
        level2.state = "unselected"
        level3.state = "unselected"
        level4.state = "unselected"
        level5.state = "unselected"
    }

    function readPara(){
        btnType1.state = json.type;
        btnType2.state = json.type;
        voiceSlider.value = json.voice;
        initBtn();
        currentAxis = "axis0";
        axis0.state = "selected";
        axis1.state = "unselected";
        axis2.state = "unselected";
        axis3.state = "unselected";
        axis4.state = "unselected";
        axis5.state = "unselected";
        setSpeed("axis0","level1");
    }

    Image
    {
        id: preOpreationPageBg
        source: "images/bg_preopration.png"
        width: 1920//parent.width//Screen.desktopAvailableWidth
        height: 1080//Screen.desktopAvailableheight
        smooth: true
    }

    //指示灯状态id为signal1～signal5：id.state == "Normal/Warning"，显示为正常/警示灯
     Column{
         spacing: 32
         x: 40
         y: 169
         Rectangle{
             id: signal3
             width: 100
             height: 100
             color: "transparent"
             state: signalLamp3
             states: [
                 State {
                     name: "Warning"
                 },
                 State {
                     name: "Normal"
                 }
             ]
         }
     }
    //锁状态修改：lock.state == "Lock/Unlock"
     //设置手术类型信息  type为"positive"正向；"opposite"逆向
//     Text {
//         id: operationName
//         x: 150
//         y: 200
//         font.pixelSize: 44
//         font.bold: true
//         font.family: "Microsoft YaHei UI"
//         font.letterSpacing: 20
//         color: "#18FEFE"
//         text: "水平增程臂"
//     }
        Rectangle{
            id: po_btnType1
//            x: 415
//            y: 125
            x: 660
            y: 180
            width: 100
            height: 100
            color: "transparent"
            state: "positive"
//            MouseArea{
//                anchors.fill: parent;
//                acceptedButtons: Qt.LeftButton;
//                onClicked: {
//                  btnType1.state = "positive"
//                  btnType2.state = "positive"
//                }
//            }
            states: [
                State {
                    name: "positive"
                },
                State {
                    name: "opposite"
                }
            ]
//            Connections{
//                target: uiInterface
//                onKinematicsStateChanged:{
//                   console.log("kinematics state was "+kinematicsState)
//                    btnType1.state = !kinematicsState ? "positive" : "opposite"
//                    btnType2.state = !kinematicsState ? "positive" : "opposite"
//                   console.log("kinematics state was "+kinematicsState)
//                }
//            }
        }
        Rectangle{
            id: po_btnType2
            width: 100
            height: 100
            x: 880
            y:180
//            x: 931
//            y:125
            color: "transparent"
            state: "positive"
//            MouseArea{
//                anchors.fill: parent;
//                acceptedButtons: Qt.LeftButton;
//                onClicked: {
//                  btnType1.state = "opposite"
//                  btnType2.state = "opposite"
//                }
//            }
            states: [
                State {
                    name: "positive"
                },
                State {
                    name: "opposite"
                }
            ]
        }

//        Text {
//            x: 150
//            y: 326
//            id: speedName
//            font.pixelSize: 44
//            font.bold: true
//            font.family: "Microsoft YaHei UI"
//            font.letterSpacing: 20
//            text: qsTr("竖直升降柱")
//            color: "#18FEFE"
//        }
        //Level按钮

         //测试按钮1->kinematics
         Button {
             //@disable-check M16
             objectName: "po_robotArmEnable"
             id: po_robotArmEnable
             y: 125
             x: 415
             font.pixelSize: 28
             font.italic: false
             rotation: 0
             anchors.verticalCenterOffset: 0
             anchors.bottomMargin: 0
             anchors.horizontalCenter: po_btnReleaseInstrument.horizontalCenter // 设置字体大小

             Image {
                 //@disable-check M16
                 objectName: "po_mainhandsta"
                 id: po_imageRobotArmEnable
                 x:50
                 y:12
                 width: 25
                 height: 32
                 source: "images/icon_unlock.png"
                 fillMode: Image.PreserveAspectFit
             }
             // 设置按钮文本
             contentItem: Text {
                 //@disable-check M16
                 objectName: "po_contentItem"
                 id: po_txtRobotArmEnable
                 text: qsTr("使能")+"     "
                 font.pixelSize: 28
                 opacity: 1.0
                 color: "#48FFFF"
                 horizontalAlignment: Text.AlignRight
                 verticalAlignment: Text.AlignVCenter
                 clip: false
                 elide: Text.ElideRight
             }
            signal triggerKinematics(int num)
             // 设置按钮背景
             background: Rectangle {
                 //@disable-check M16
                 objectName: "po_robotArmEnable_background"
                 id: po_robotArmEnable_background
                 implicitWidth: 200
                 implicitHeight: 56
                 opacity: 0.25
                 color: "#003033"
                 border.color: "#2CDFE8"
                 border.width: 2
                 radius:10
             }
             function setsta(sta)
             {
                 switch(sta)
                 {
                    case 0x00:
                    {
                        po_txtRobotArmEnable.color="#7f7f7f"
                        po_robotArmEnable_background.border.color="#7f7f7f"
                        po_robotArmEnable_background.color="#7f7f7f"
                        break;
                    }
                    case 0x01:
                    {
                        po_txtRobotArmEnable.color="#48FFFF"
                        po_robotArmEnable_background.color="#003033"
                        po_robotArmEnable_background.border.color="#2CDFE8"
                        break;
                    }
                    case 0x02:
                    {
                        po_txtRobotArmEnable.color="#FFFFFF"
                        po_robotArmEnable_background.color="#48FFFF"
                        po_robotArmEnable_background.border.color="#2CDFE8"
                        break;
                    }
                    default:break;
                 }
            }
            onClicked: {
                uiInterface.setRobotControlMode(2);
            }
        }

        Button {
           //@disable-check M16
           objectName: "po_btnFastUp"
           id: po_btnFastUp
           x: 931
           y: 125
           width: 200
           height: 53
           font.pixelSize: 28

           Image {
               //@disable-check M16
               objectName: "po_imageFastUp"
               id: po_imageFastUp
               x:50
               y:14
               width: 25
               height: 26
               source: "images/icon_fastup.png"
               fillMode: Image.PreserveAspectFit
           }

           contentItem: Text {
               id: po_txtFastUp
               width: 100
               text: qsTr("上升")+"     "
               font.pixelSize: 28
               opacity: 1.0
               color: "#48FFFF"
               horizontalAlignment: Text.AlignRight
               verticalAlignment: Text.AlignVCenter
               elide: Text.ElideRight
           }
          signal triggerKinematics(int num)
           // 设置按钮背景
           background: Rectangle {
               id:po_btnFastUp_background
               implicitWidth: 200
               implicitHeight: 56
               opacity: 0.25
               color: "#003033"
               border.color: "#2CDFE8"
               border.width: 2
               radius: 10
           }
           function setsta(sta)
           {
               switch(sta)
               {
                   case 0x00:
                   {
                       po_btnFastUp.enabled=false
                       po_txtFastUp.color="#7f7f7f"
                       po_btnFastUp_background.border.color="#7f7f7f"
                       po_btnFastUp_background.color="#7f7f7f"
                       break;
                   }
                  case 0x01:
                  {
                      po_btnFastUp.enabled=true
                      po_txtFastUp.color="#48FFFF"
                      po_btnFastUp_background.color="#003033"
                      po_btnFastUp_background.border.color="#2CDFE8"
                      break;
                  }
                  case 0x02:
                  {
                      po_btnFastUp.enabled=true
                      po_txtFastUp.color="#FFFFFF"
                      po_btnFastUp_background.color="#48FFFF"
                      po_btnFastUp_background.border.color="#2CDFE8"
                      break;
                  }
                  default:break;
               }
           }
           onPressed:
           {
               //  uiInterface.liftingFastup();
                 po_txtFastUp.color="#FFFFFF"
                 po_btnFastUp_background.color="#48FFFF"
           }
           onReleased:
           {
               //uiInterface.liftingBrake();
               po_txtFastUp.color="#48FFFF"
               po_btnFastUp_background.color="#003033"
           }
           onClicked: {
               triggerKinematics(1);
           }
        }
         Button {
           //@disable-check M16
           objectName: "po_btnSlowUp"
           id: po_btnSlowUp
           x: 1407
           y: 125
           width: 200
           height: 53
           font.pixelSize: 28
           Image {
               //@disable-check M16
               objectName: "po_imageFastUp"
               id: po_imageSlowUp
               x:50
               y:14
               width: 25
               height: 26
               source: "images/icon_slowup.png"
               fillMode: Image.PreserveAspectFit
           }

           contentItem: Text {
               id: po_txtSlowUp
               width: 100
               text: qsTr("上升")+"     "
               font.pixelSize: 28
               opacity: 1.0
               color: "#48FFFF"
               horizontalAlignment: Text.AlignRight
               verticalAlignment: Text.AlignVCenter
               elide: Text.ElideRight
           }
          signal triggerKinematics(int num)
           // 设置按钮背景
           background: Rectangle {
               id:po_btnSlowUp_background
               implicitWidth: 200
               implicitHeight: 56
               opacity: 0.25
               color: "#003033"
               border.color: "#2CDFE8"
               border.width: 2
               radius: 10
           }
           function setsta(sta)
           {
               switch(sta)
               {
               case 0x00:
               {
                   po_btnSlowUp.enabled=false
                   po_txtSlowUp.color="#7f7f7f"
                   po_btnSlowUp_background.border.color="#7f7f7f"
                   po_btnSlowUp_background.color="#7f7f7f"
                   break;
               }
                  case 0x01:
                  {
                      po_btnSlowUp.enabled=true
                      po_txtSlowUp.color="#48FFFF"
                      po_btnSlowUp_background.color="#003033"
                      po_btnSlowUp_background.border.color="#2CDFE8"
                      break;
                  }
                  case 0x02:
                  {
                      po_btnSlowUp.enabled=true
                      po_txtSlowUp.color="#FFFFFF"
                      po_btnSlowUp_background.color="#48FFFF"
                      po_btnSlowUp_background.border.color="#2CDFE8"
                      break;
                  }
                  default:break;
               }
           }

           onPressed:
           {
              // uiInterface.liftingFastup()
                 po_txtSlowUp.color="#FFFFFF"
                 po_btnSlowUp_background.color="#48FFFF"
           }
           onReleased:
           {
             // uiInterface.liftingBrake();
               po_txtSlowUp.color="#48FFFF"
               po_btnSlowUp_background.color="#003033"
           }
           onClicked: {
               triggerKinematics(1);
           }
        }
         Button {
           //@disable-check M16
           objectName: "po_btnSlowDown"
           id: po_btnSlowDown
           y: 214
           width: 200
           height: 53
           x: 1407
           font.pixelSize: 28

           Image {
               //@disable-check M16
               objectName: "po_imageFastUp"
               id: po_imagebtnSlowDown
               x:50
               y:14
               width: 25
               height: 26
               source: "images/icon_slowdown.png"
               fillMode: Image.PreserveAspectFit
           }
           // 设置按钮文本
           contentItem: Text {
               id: po_txtSlowDown
               width: 100
               text: qsTr("下降")+"     "
               font.pixelSize: 28
               opacity: 1.0
               color: "#48FFFF"
               horizontalAlignment: Text.AlignRight
               verticalAlignment: Text.AlignVCenter
               elide: Text.ElideRight
           }
          signal triggerKinematics(int num)
           // 设置按钮背景
           background: Rectangle {
               id:po_btnSlowDown_background
               implicitWidth: 250
               implicitHeight: 56
               opacity: 0.25
               color: "#003033"
               border.color: "#2CDFE8"
               border.width: 2
               radius: 10
           }
            function setsta(sta)
            {
                switch(sta)
                {
                    case 0x00:
                    {
                        po_btnSlowDown.enabled=false
                        po_txtSlowDown.color="#7f7f7f"
                        po_btnSlowDown_background.border.color="#7f7f7f"
                        po_btnSlowDown_background.color="#7f7f7f"
                        break;
                    }
                    case 0x01:
                    {
                        po_btnSlowDown.enabled=true
                        po_txtSlowDown.color="#48FFFF"
                        po_btnSlowDown_background.color="#003033"
                        po_btnSlowDown_background.border.color="#2CDFE8"
                        break;
                    }
                    case 0x02:
                    {

                        po_btnSlowDown.enabled=true
                        po_txtSlowDown.color="#FFFFFF"
                        po_btnSlowDown_background.color="#48FFFF"
                        po_btnSlowDown_background.border.color="#2CDFE8"
                        break;
                    }
                    default:break;
                }
            }

           onPressed:
           {
               console.log("slowdown pressed")
               //uiInterface.liftingSlowdown();
                 po_txtSlowDown.color="#FFFFFF"
                 po_btnSlowDown_background.color="#48FFFF"
           }
           onReleased:
           {
                console.log("slowdown onReleased")
            //   uiInterface.liftingBrake();
               po_txtSlowDown.color="#48FFFF"
               po_btnSlowDown_background.color="#003033"
           }
           onClicked: {

           }
        }

        Button {
               //@disable-check M16
               objectName: "po_btnFastDown"
               id: po_btnFastDown
               y: 214
               width: 200
               x: 931
               font.pixelSize: 28
               // 设置字体大小
               Image {
                   //@disable-check M16
                   objectName: "po_imageFastUp"
                   id: po_imagebtnFastDown
                   x:50
                   y:14
                   width: 25
                   height: 26
                   source: "images/icon_fastdown.png"
                   fillMode: Image.PreserveAspectFit
               }
               // 设置按钮文本
               contentItem: Text {
                   id: po_txtFastDown
                   text: qsTr("下降")+"     "
                   font.pixelSize: 28
                   opacity: 1.0
                   color: "#48FFFF"
                   horizontalAlignment: Text.AlignRight
                   verticalAlignment: Text.AlignVCenter
                   elide: Text.ElideRight
               }
              signal triggerKinematics(int num)
               // 设置按钮背景
               background: Rectangle {
                   id:po_btnFastDown_background
                   implicitWidth: 250
                   implicitHeight: 56
                   opacity: 0.25
                   color: "#003033"
                   border.color: "#2CDFE8"
                   border.width: 2
                   radius: 10
               }
               function setsta(sta)
               {
                   switch(sta)
                   {
                       case 0x00:
                       {
                           po_btnFastDown.enabled=false
                           po_txtFastDown.color="#7f7f7f"
                           po_btnFastDown_background.border.color="#7f7f7f"
                           po_btnFastDown_background.color="#7f7f7f"
                           break;
                       }
                      case 0x01:
                      {
                          po_btnFastDown.enabled=true
                          po_txtFastDown.color="#48FFFF"
                          po_btnFastDown_background.color="#003033"
                          po_btnFastDown_background.border.color="#2CDFE8"
                          break;
                      }
                      case 0x02:
                      {
                          po_btnFastDown.enabled=true
                          po_txtFastDown.color="#FFFFFF"
                          vbtnFastDown_background.color="#48FFFF"
                          po_btnFastDown_background.border.color="#2CDFE8"
                          break;
                      }
                      default:break;
                   }
               }
               onPressed:
               {
                  //   uiInterface.liftingFastdown()
                     po_txtFastDown.color="#FFFFFF"
                     po_btnFastDown_background.color="#48FFFF"
               }
               onReleased:
               {
                 //  uiInterface.liftingBrake();
                   po_txtFastDown.color="#48FFFF"
                   po_btnFastDown_background.color="#003033"
               }
               onClicked: {

               }
           }

          //左侧云台归零按钮
            Button {
                //@disable-check M16
                objectName: "po_btnCaliGimbalL"
                id: po_btnCaliGimbalL
                y: 665
                x: 575
                width: 250
                height: 53
                font.pixelSize: 28
                anchors.bottomMargin: 0

                contentItem: Text {
                    id: po_btnCaliGimbalL_Text
                    text: qsTr("左侧机械臂复位")
                    font.pixelSize: 28
                    opacity: 1.0
                    color: "#48FFFF"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
              // 设置按钮背景
                background: Rectangle {
                    id: po_btnCaliGimbalL_Background
                    implicitWidth: 250
                    implicitHeight: 56
                    opacity: 0.25
                    color: "#003033"//#7f7f7f
                    border.color: "#2CDFE8"
                    border.width: 2
                    radius:10
                }
                function setsta(sta)
                {
                    switch(sta)
                    {
                        case 0x00:
                        {
                            po_btnCaliGimbalL_Text.color = "#7f7f7f"
                            po_btnCaliGimbalL_Background.border.color = "#7f7f7f"
                            po_btnCaliGimbalL_Background.color = "#7f7f7f"
                            break;
                        }
                        case 0x01:
                        {
                            po_btnCaliGimbalL_Text.text = qsTr("左侧机械臂复位")
                            po_btnCaliGimbalL_Text.color = "#48FFFF"
                            po_btnCaliGimbalL_Background.color = "#003033"
                            po_btnCaliGimbalL_Background.border.color = "#2CDFE8"
                            break;
                        }
                        case 0x02:
                        {
                            po_btnCaliGimbalL_Text.text = qsTr("左侧机械臂复位中")
                            po_btnCaliGimbalL_Text.color = "#FFFFFF"
                            po_btnCaliGimbalL_Background.color = "#48FFFF"
                            po_btnCaliGimbalL_Background.border.color = "#2CDFE8"
                            break;
                        }
                        default:break;
                    }
                }
            }

            //左侧器械归零按钮
            Button {
                //@disable-check M16
                objectName: "po_btnCaliInstrumentL"
                id: po_btnCaliInstrumentL
                y: 750//843
                x: 575
                font.pixelSize: 28
                anchors.rightMargin: 0
                anchors.horizontalCenter: po_btnCaliGimbalL.horizontalCenter
                contentItem: Text {
                    id: po_btnCaliInstrumentL_Text
                    text: qsTr("左侧器械复位")
                    font.pixelSize: 28
                    opacity: 1.0
                    color: "#48FFFF"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
                background: Rectangle {
                    id: po_btnCaliInstrumentL_Background
                    implicitWidth: 250
                    implicitHeight: 56
                    opacity: 0.25
                    color: "#003033"
                    border.color: "#2CDFE8"
                    border.width: 2
                    radius:10
                }
                function setsta(sta)
                {
                    switch(sta)
                    {
                        case 0x00:
                        {
                            po_btnCaliInstrumentL_Text.color="#7f7f7f"
                            po_btnCaliInstrumentL_Background.border.color="#7f7f7f"
                            po_btnCaliInstrumentL_Background.color="#7f7f7f"
                            break;
                        }
                    case 0x01:
                    {
                        po_btnCaliInstrumentL_Text.text = qsTr("左侧器械复位")
                        po_btnCaliInstrumentL_Text.color="#48FFFF"
                        po_btnCaliInstrumentL_Background.color="#003033"
                        po_btnCaliInstrumentL_Background.border.color="#2CDFE8"
                        break;
                    }
                    case 0x02:
                    {
                        po_btnCaliInstrumentL_Text.text = qsTr("左侧器械复位中")
                        po_btnCaliInstrumentL_Text.color="#FFFFFF"
                        po_btnCaliInstrumentL_Background.color="#48FFFF"
                        po_btnCaliInstrumentL_Background.border.color="#2CDFE8"
                        break;
                    }
                    default:break;
                }
            }
        }
          //右侧云台归零
          Button {
              //@disable-check M16
              objectName: "po_btnCaliGimbalR"
              id: po_btnCaliGimbalR
              x: 1430
              y: 665
              width: 250
              height: 53
              anchors.verticalCenter: po_btnCaliGimbalL.verticalCenter
              anchors.top: po_btnCaliGimbalL.top
              font.pixelSize: 28
              // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: po_btnCaliGimbalR_Text
                  text: qsTr("右侧机械臂复位")
                  font.pixelSize: 28
                  opacity: 1.0
                  color: "#48FFFF"
                  horizontalAlignment: Text.AlignHCenter//水平居中
                  verticalAlignment: Text.AlignVCenter//垂直居中
                  elide: Text.ElideRight//文本过长会在后面加上省略号
              }
              // 设置按钮背景
              background: Rectangle {
                  id:po_btnCaliGimbalR_Background
                  implicitWidth: 250
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
                  radius: 10
              }
              function setsta(sta)
              {
                  switch(sta)
                  {
                     case 0x00:
                     {
                         po_btnCaliGimbalR_Text.color="#7f7f7f"
                         po_btnCaliGimbalR_Background.border.color="#7f7f7f"
                         po_btnCaliGimbalR_Background.color="#7f7f7f"
                         break;
                     }
                     case 0x01:
                     {
                         po_btnCaliGimbalR_Text.text = qsTr("右侧机械臂复位")
                         po_btnCaliGimbalR_Text.color="#48FFFF"
                         po_btnCaliGimbalR_Background.color="#003033"
                         po_btnCaliGimbalR_Background.border.color="#2CDFE8"
                         break;
                     }
                     case 0x02:
                     {
                         po_btnCaliGimbalR_Text.text = qsTr("右侧机械臂复位中")
                         po_btnCaliGimbalR_Text.color="#FFFFFF"
                         po_btnCaliGimbalR_Background.color="#48FFFF"
                         po_btnCaliGimbalR_Background.border.color="#2CDFE8"
                         break;
                     }
                     default:break;
                  }
              }
          }
          //右侧器械归零
          Button {
              //@disable-check M16
              objectName: "po_btnCaliInstrumentR"
              id: po_btnCaliInstrumentR
              y: 750
              x: 1430
              font.pixelSize: 28
              anchors.horizontalCenterOffset: 0
              anchors.horizontalCenter: po_btnCaliGimbalR.horizontalCenter
              // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: po_btnCaliInstrumentR_Text
                  text: qsTr("右侧器械复位")
                  font.pixelSize: 28
                  opacity: 1.0
                  color: "#48FFFF"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
              }
              // 设置按钮背景
              background: Rectangle {
                  id:po_btnCaliInstrumentR_Background
                  implicitWidth: 250
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
                  radius: 10
              }
              function setsta(sta)
              {
                  switch(sta)
                  {
                     case 0x00:
                     {
                         po_btnCaliInstrumentR_Text.color="#7f7f7f"
                         po_btnCaliInstrumentR_Background.border.color="#7f7f7f"
                         po_btnCaliInstrumentR_Background.color="#7f7f7f"
                         break;
                     }
                     case 0x01:
                     {
                         po_btnCaliInstrumentR_Text.text = qsTr("右侧器械复位")
                         po_btnCaliInstrumentR_Text.color="#48FFFF"
                         po_btnCaliInstrumentR_Background.color="#003033"
                         po_btnCaliInstrumentR_Background.border.color="#2CDFE8"
                         break;
                     }
                     case 0x02:
                     {
                         po_btnCaliInstrumentR_Text.text = qsTr("右侧器械复位中")
                         po_btnCaliInstrumentR_Text.color="#FFFFFF"
                         po_btnCaliInstrumentR_Background.color="#48FFFF"
                         po_btnCaliInstrumentR_Background.border.color="#2CDFE8"
                         break;
                     }
                     default:break;
                  }
              }
              onClicked: {}
          }
/*
          Button {
              //@disable-check M16
              objectName: "btnReleaseInstrument"
              id: btnReleaseInstrument
              y: 424
              anchors.top: btnInstrumentInstalled.top
              x: 575
              font.pixelSize: 28
              anchors.topMargin: 0
              // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: txtReleaseInstrument
                  text: qsTr("释放器械")
                  font.pixelSize: 28
                  opacity: 1.0
                  color: "#48FFFF"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
              }
              // 设置按钮背景
              background: Rectangle {
                  id: releaseInstrument_background
                  implicitWidth: 200
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
              }
              onPressed:
              {
                    txtReleaseInstrument.color="#FFFFFF"
                    releaseInstrument_background.color="#48FFFF"
              }
              onReleased:
              {
                    txtReleaseInstrument.color="#48FFFF"
                    releaseInstrument_background.color="#003033"
              }
          }
*/
          Text {
                    id: po_txtUsedTime_L
                    //@disable-check M16
                    objectName: "po_txtUsedTime_L"
                    x: 625
                    y: 575
                    color: "#18fefe"
                    text: "10.5h"
                    anchors.left: po_operationName.left
                    anchors.right: po_speedName1.right
                   // font.letterSpacing: 20
                    font.pixelSize: 28
                    font.family: "Microsoft YaHei UI"
                    font.bold: true
          }

//          Button {
//              //@disable-check M16
//              objectName: "btnUsedTime_L"
//              id: btnUsedTime_L
//              y: 630

//              x: 575
//              font.pixelSize: 28
//              anchors.horizontalCenterOffset: 0
//              anchors.horizontalCenter: robotArmEnable.horizontalCenter
//              anchors.bottomMargin: 0
//              anchors.topMargin: 0
//              // 设置字体大小
//              // 设置按钮文本
//              contentItem: Text {
//                  id: txtUsedTime_L
//                  text: qsTr("10.4h")
//                  font.pixelSize: 28
//                  opacity: 1.0
//                  color: "#48FFFF"
//                  horizontalAlignment: Text.AlignHCenter
//                  verticalAlignment: Text.AlignVCenter
//                  elide: Text.ElideRight
//              }
              // 设置按钮背景
//              background: Rectangle {
//                  implicitWidth: 200
//                  implicitHeight: 56
//                  opacity: 0.25
//                  color: "#003033"
//                  border.color: "#2CDFE8"
//                  border.width: 2
//              }
//          }
          Text {
                    id: po_txtUsedTime_R
                    //@disable-check M16
                    objectName: "po_txtUsedTime_R"
                    x: 1475
                    y: 575
                    color: "#18fefe"
                    text: qsTr("30.8h")
                    anchors.left: po_operationName.left
                    anchors.right: po_speedName1.right
                   // font.letterSpacing: 20
                    font.pixelSize: 28
                    font.family: "Microsoft YaHei UI"
                    font.bold: true
          }
//          Button {
//              //@disable-check M16
//              objectName: "btnUsedTime_R"
//              id: btnUsedTime_R
//              y: 630

//              x: 575
//              font.pixelSize: 28
//              anchors.horizontalCenterOffset: 310
//              anchors.horizontalCenter: robotArmEnable.horizontalCenter
//              anchors.bottomMargin: 0
//              anchors.topMargin: 0
//              // 设置字体大小
//              // 设置按钮文本
//              contentItem: Text {
//                  id: txtUsedTime_R
//                  text: qsTr("30.8h")
//                  font.pixelSize: 28
//                  opacity: 1.0
//                  color: "#48FFFF"
//                  horizontalAlignment: Text.AlignHCenter
//                  verticalAlignment: Text.AlignVCenter
//                  elide: Text.ElideRight
//              }
//              // 设置按钮背景
//              background: Rectangle {
//                  implicitWidth: 200
//                  implicitHeight: 56
//                  opacity: 0.25
//                  color: "#003033"
//                  border.color: "#2CDFE8"
//                  border.width: 2
//              }
//          }
          Text {

                    //@disable-check M16
                    objectName: "po_txtInstrumentType_L"
                    id: po_txtInstrumentType_L
                    x: 625
                    y: 480
                    color: "#18fefe"
                    text: "显微剪"
                    anchors.left: po_operationName.left
                    anchors.right: po_speedName1.right
                   // font.letterSpacing: 20
                    font.pixelSize: 28
                    font.family: "Microsoft YaHei UI"
                    font.bold: true
          }
          Text {
                    //@disable-check M16
                    objectName: "po_txtInstrumentType_R"
                    id: po_txtInstrumentType_R

                    x: 1475
                    y: 480
                    color: "#18fefe"
                    text: qsTr("持针钳")
                    anchors.left: po_operationName.left
                    anchors.right: po_speedName1.right
                   // font.letterSpacing: 20
                    font.pixelSize: 28
                    font.family: "Microsoft YaHei UI"
                    font.bold: true
          }
//          Button {
//              //@disable-check M16
//              objectName: "btnInstrumentType_R"
//              id: btnInstrumentType_R
//              y: 524

//              x: 575
//              font.pixelSize: 28
//              anchors.horizontalCenterOffset: 310
//              anchors.horizontalCenter: robotArmEnable.horizontalCenter
//              anchors.bottomMargin: 0
//              anchors.topMargin: 0
//              // 设置字体大小
//              // 设置按钮文本
//              contentItem: Text {
//                  //@disable-check M16
//                  objectName: "txtInstrumentType_R"
//                  id: txtInstrumentType_R
//                  text: qsTr("持针钳")
//                  font.pixelSize: 28
//                  opacity: 1.0
//                  color: "#48FFFF"
//                  horizontalAlignment: Text.AlignHCenter
//                  verticalAlignment: Text.AlignVCenter
//                  elide: Text.ElideRight
//              }
//              // 设置按钮背景
//              background: Rectangle {
//                  implicitWidth: 200
//                  implicitHeight: 56
//                  opacity: 0.25
//                  color: "#003033"
//                  border.color: "#2CDFE8"
//                  border.width: 2
//              }
//          }


//          Button {
//              //@disable-check M16
//              objectName: "btnInstrumentType_L"
//              id: btnInstrumentType_L
//              y: 524

//              x: 575
//              font.pixelSize: 28
//              anchors.horizontalCenterOffset: 0
//              anchors.horizontalCenter: robotArmEnable.horizontalCenter
//              anchors.bottomMargin: 0
//              anchors.topMargin: 0
//              // 设置字体大小
//              // 设置按钮文本
//              contentItem: Text {
//                  //@disable-check M16
//                  objectName: "txtInstrumentType_L"
//                  id: txtInstrumentType_L
//                  text: qsTr("显微剪")
//                  font.pixelSize: 28
//                  opacity: 1.0
//                  color: "#48FFFF"
//                  horizontalAlignment: Text.AlignHCenter
//                  verticalAlignment: Text.AlignVCenter
//                  elide: Text.ElideRight
//              }
//              // 设置按钮背景
//              background: Rectangle {
//                  implicitWidth: 200
//                  implicitHeight: 56
//                  opacity: 0.25
//                  color: "#003033"
//                  border.color: "#2CDFE8"
//                  border.width: 2
//              }
//          }
          Text {
                    id: po_txtInstrumentInstallSta_L
                    //@disable-check M16
                    objectName: "po_txtInstrumentInstallSta_L"
                    x: 455
                    y: 380
                    color: "#18fefe"
                    text: "器械已安装"
                    anchors.left: po_operationName.left
                    anchors.right: po_speedName1.right
                   // font.letterSpacing: 20
                    font.pixelSize: 28
                    font.family: "Microsoft YaHei UI"
                    font.bold: true
          }

          Text {
                    id: po_txtInstrumentInstallSta_R
                    //@disable-check M16
                    objectName: "po_txtInstrumentInstallSta_R"
                    x: 1320
                    y: 380
                    color: "#18fefe"
                    text: "器械已安装"
                    anchors.left: po_operationName.left
                    anchors.right: po_speedName1.right
                   // font.letterSpacing: 20
                    font.pixelSize: 28
                    font.family: "Microsoft YaHei UI"
                    font.bold: true
          }
//          Button {
//              //@disable-check M16
//              objectName: "btnInstrumentInstalled"
//              id: btnInstrumentInstalled
//              y: 424
//              x: 885
//              font.pixelSize: 28
//              // 设置字体大小
//              // 设置按钮文本
//              contentItem: Text {
//                  id: txtInstrumentInstalled
//                  text: qsTr("器械已安装")
//                  font.pixelSize: 28
//                  opacity: 1.0
//                  color: "#48FFFF"
//                  horizontalAlignment: Text.AlignHCenter
//                  verticalAlignment: Text.AlignVCenter
//                  elide: Text.ElideRight
//              }
//              // 设置按钮背景
//              background: Rectangle {
//                  id: instrumentInstalled_background
//                  implicitWidth: 200
//                  implicitHeight: 56
//                  opacity: 0.25
//                  color: "#003033"
//                  border.color: "#2CDFE8"
//                  border.width: 2
//              }
//              onPressed:
//              {
//                    txtInstrumentInstalled.color="#FFFFFF"
//                    instrumentInstalled_background.color="#48FFFF"
//              }
//              onReleased:
//              {
//                  txtInstrumentInstalled.color="#48FFFF"
//                  instrumentInstalled_background.color="#003033"
//              }
//          }

//          Text {
//              //@disable-check M16
//              objectName: "speedName1"
//              id: speedName1
//              x: 150
//              y: 503
//              color: "#18fefe"
//              text: "器械类型"
//              anchors.left: operationName.left
//              font.letterSpacing: 20
//              font.pixelSize: 44
//              font.styleName: "Regular"
//              font.family: "Microsoft YaHei UI"
//              font.bold: true
//          }

//          Text {
//              id: speedName2
//              x: 150
//              y: 628
//              color: "#18fefe"
//              text: "使用时长"
//              anchors.left: operationName.left
//              anchors.right: speedName1.right
//              font.letterSpacing: 20
//              font.pixelSize: 44
//              font.family: "Microsoft YaHei UI"
//              font.bold: true
//          }
          //测试按钮1->kinematics
           //测试按钮1->kinematics
           //测试按钮1->kinematics

//          Text {
//              id: speedName3
//              x: 150
//              y: 746
//              color: "#18fefe"
//              text: "执行器初始化"
//              anchors.left: operationName.left
//              font.letterSpacing: 20
//              font.pixelSize: 44
//              anchors.leftMargin: 0
//              font.family: "Microsoft YaHei UI"
//              font.bold: true
//          }
        Button {
            //@disable-check M16
            objectName: "po_btnEnterOpreation"
            id: po_btnEnterOpreation
            x: 1550
            y: 950
            width: 300
            height: 72
            font.pixelSize: 36

            contentItem: Text {
                id:po_txt_btnEnterOpreation
                width: 100
                text: qsTr("开始手术")
                font.pixelSize: 36
                opacity: 1.0
                color: "#48FFFF"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            // 设置按钮背景
            background: Rectangle {
                id:po_btnEnterOpreation_background
                implicitWidth: 200
                implicitHeight: 56
                opacity: 0.25
                color: "#003033"
                border.color: "#2CDFE8"
                border.width: 2
                radius: 10
            }
            onPressed:
            {
                po_txt_btnEnterOpreation.color="#FFFFFF"
                po_btnEnterOpreation_background.color="#48FFFF"
            }
            onReleased:
            {
                po_txt_btnEnterOpreation.color="#48FFFF"
                po_btnEnterOpreation_background.color="#003033"
            }
            onClicked: {
                onClicked: po_sureEnterOperation.open()
            }
            //确认进入手术中
            Popup{
                    id: po_sureEnterOperation
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
                    id: po_sureEnterOperationBoard
                    x:-50
                    y:50
                    width: 745
                    height:220
                    color: "#003033"
                    border.color: "#18fefe"
                    opacity: 0.8
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        text: qsTr("确认进入手术中页面？") ;
                        color: itemColor
                        font.pixelSize: 40
                        anchors.topMargin: 40  // 调整顶部的间距
                    }
                }

                 //确定按钮
                Button {
                    //@disable-check M16
                    objectName: "po_btnSureEnterOperationOk"
                    id: po_btnSureEnterOperationOk
                    x: 205
                    y: 175
                    contentItem: Text {
                        id: po_btnSureEnterOperationOkText
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
                        id: po_btnSureEnterOperationOkText_background
                        implicitWidth: 120
                        implicitHeight: 56
                        opacity: 0.25
                        color: "#003033"
                        border.color: "#2CDFE8"
                        border.width: 2
                    }
                    onPressed:
                    {
                        po_btnSureEnterOperationOkText.color="#FFFFFF"
                        po_btnSureEnterOperationOkText_background.color="#48FFFF"
                    }
                    onReleased:
                    {
                        po_btnSureEnterOperationOkText.color="#48FFFF"
                        po_btnSureEnterOperationOkText_background.color="#003033"
                    }
                    onClicked: {
                        console.log("sure go to operation")
                        UIinterface.setRobotControlMode(3);
                        po_sureEnterOperation.close()
                        inOpreationPage.visible = true
                        preOpreationPage.visible = false
                    }
                }
                //取消按钮
                Button {
                    id: po_btnSureEnterOperationCancel
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
                        po_sureEnterOperation.close()
                    }
                }
            }
        }

        Text {
            id: po_txtInitialInstrumentAngle
            //@disable-check M16
            objectName: "po_txtInitialInstrumentAngle"
            x: 650
            y: 950
            color: "#18fefe"
            text: "器械夹角"
            font.pixelSize: 36
            font.family: "Microsoft YaHei UI"
            font.bold: true
        }

        Button {
            //@disable-check M16
            objectName: "po_btnInitialInstrumentAngle30"
            id: po_btnInitialInstrumentAngle30
            enabled: !isInstrumentAngle30
            x: 1250
            y: 950
            width: 200
            height: 72
            font.pixelSize: 36
            contentItem: Text {
                id:po_txtInitialInstrumentAngle30
                width: 100
                text: qsTr("30°")
                font.pixelSize: 36
                opacity: 1.0
                color: isInstrumentAngle30 ? "#FFFFFF" : "#48FFFF"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            background: Rectangle {
                id:po_btnInitialInstrumentAngle30_background
                implicitWidth: 200
                implicitHeight: 56
                opacity: 0.25
                color: isInstrumentAngle30 ? "#48FFFF" : "#003033"
                border.color: "#2CDFE8"
                border.width: 2
                radius: 10
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    po_sureToSwitchInstrumentAngle_pop.open()
                }
            }
        }

        Button {
            id: po_btnInitialInstrumentAngle60
            enabled: isInstrumentAngle30
            x: 950
            y: 950
            width: 200
            height: 72
            font.pixelSize: 36
            contentItem: Text {
                id:po_txtInitialInstrumentAngle60
                width: 100
                text: qsTr("60°")
                font.pixelSize: 36
                opacity: 1.0
                color: isInstrumentAngle30 ? "#48FFFF" : "#FFFFFF"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            background: Rectangle {
                id:po_btnInitialInstrumentAngle60_background
                implicitWidth: 200
                implicitHeight: 56
                opacity: 0.25
                color: isInstrumentAngle30 ? "#003033" : "#48FFFF"
                border.color: "#2CDFE8"
                border.width: 2
                radius: 10
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    po_sureToSwitchInstrumentAngle_pop.open()
                }
            }
        }

        //确认切换器械角度
        Popup{
                id: po_sureToSwitchInstrumentAngle_pop
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
                    id: po_sureToSwitchInstrumentAngle_Board
                    x:-50
                    y:50
                    width: 745
                    height:220
                    color: "#003033"
                    border.color: "#18fefe"
                    opacity: 0.8
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: isInstrumentAngle30? qsTr("确认更改末端器械角度为60度？") : qsTr("确认更改末端器械角度为30度？")
                        anchors.top: parent.top
                        anchors.topMargin: 40  // 调整顶部的间距
                        font.pixelSize: 40
                        color: itemColor
                    }
                }

                 //确定按钮
                Button {
                    id: po_sureToSwitchInstrumentAngleOk
                    x: 205
                    y: 175
                    contentItem: Text {
                        id: po_sureToSwitchInstrumentAngleOk_Text
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
                        id: po_sureToSwitchInstrumentAngleOk_Text_background
                        implicitWidth: 120
                        implicitHeight: 56
                        opacity: 0.25
                        color: "#003033"
                        border.color: "#2CDFE8"
                        border.width: 2
                    }
                    onPressed:
                    {
                        po_sureToSwitchInstrumentAngleOk_Text.color="#FFFFFF"
                        po_sureToSwitchInstrumentAngleOk_Text_background.color="#48FFFF"
                    }
                    onReleased:
                    {
                        po_sureToSwitchInstrumentAngleOk_Text.color="#48FFFF"
                        po_sureToSwitchInstrumentAngleOk_Text_background.color="#003033"
                    }
                    onClicked: {
                        if(isInstrumentAngle30 === true)
                        {
                            UIinterface.setInstrumentAngle(60)
                            isInstrumentAngle30 = false
                            po_sureToSwitchInstrumentAngle_pop.close()
                            return;
                        }
                        if(isInstrumentAngle30 == false)
                        {
                            UIinterface.setInstrumentAngle(30)
                            isInstrumentAngle30 = true
                            po_sureToSwitchInstrumentAngle_pop.close()
                            return;
                        }

                    }
                }
                //取消按钮
                Button {
                    id: po_cancelSwitchInstrumentAngle30Ok
                    x: 345
                    y: 175
                    // 设置按钮文本
                    contentItem: Text {
                        id: po_cancelSwitchInstrumentAngle30OkText
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
                        po_sureToSwitchInstrumentAngle_pop.close()
                    }
                }
        }
}
