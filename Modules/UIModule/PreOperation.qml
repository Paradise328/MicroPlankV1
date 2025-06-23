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
//             anchors.verticalCenter: btnType1.verticalCenter
//             anchors.left: btnFastDown.left
//             anchors.bottom: robotArmDisable.bottom
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
             objectName: "robotArmDisable"
             id: robotArmDisable
             y: 202
             anchors.top: robotArmEnable.top
             x: 885
             font.pixelSize: 28
             anchors.topMargin: 0
             // 设置字体大小
             // 设置按钮文本
             contentItem: Text {
                 //@disable-check M16
                 objectName: "contentItem"
                 id: txtRobotArmDisable
                 text: qsTr("锁定")
                 font.pixelSize: 28
                 opacity: 1.0
                 color: "#48FFFF"
                 horizontalAlignment: Text.AlignHCenter
                 verticalAlignment: Text.AlignVCenter
                 elide: Text.ElideRight
             }
            signal triggerKinematics(int num)
             // 设置按钮背景
             background: Rectangle {
                 //@disable-check M16
                 objectName: "background"
                 id:robotArmDisable_background
                 implicitWidth: 200
                 implicitHeight: 56
                 opacity: 0.25
                 color: "#003033"
                 border.color: "#2CDFE8"
                 border.width: 2
             }
             function setsta(sta)
             {
                 switch(sta)
                 {
                    case 0x00:
                    {
                        txtRobotArmDisable.color="#7f7f7f"
                        robotArmDisable_background.border.color="#7f7f7f"
                        robotArmDisable_background.color="#7f7f7f"
                        break;
                    }
                    case 0x01:
                    {
                        txtRobotArmDisable.color="#48FFFF"
                        robotArmDisable_background.color="#003033"
                        robotArmDisable_background.border.color="#2CDFE8"
                        break;
                    }
                    case 0x02:
                    {
                        txtRobotArmDisable.color="#FFFFFF"
                        robotArmDisable_background.color="#48FFFF"
                        robotArmDisable_background.border.color="#2CDFE8"
                        break;
                    }
                    default:break;
                 }
             }
             onClicked: {
                 robotArmEnable.changecolor()
                 uiInterface.setRobotControlMode(1);
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
//           anchors.right: robotArmDisable.right
//           anchors.top: speedName.top
//           anchors.bottom: speedName.bottom

           font.pixelSize: 28
//           anchors.topMargin: 2
//           anchors.bottomMargin: 0
           // 设置字体大小
           // 设置按钮文本

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
//           x: 1540
//           y: 132
           x: 1407
           y: 125
           width: 200
           height: 53
//           anchors.left: btnSlowDown.right
//           anchors.top: speedName.top
//           anchors.bottom: speedName.bottom
           font.pixelSize: 28
//           anchors.leftMargin: 62
//           anchors.topMargin: 2
//           anchors.bottomMargin: 0
           // 设置字体大小
           // 设置按钮文本
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
//           anchors.top: btnSlowUp.top
//           anchors.bottom: speedName.bottom
           x: 1407
           font.pixelSize: 28
//           anchors.topMargin: 0
//           anchors.horizontalCenterOffset: 76
//           anchors.bottomMargin: 0
//           anchors.horizontalCenter: btnReleaseInstrument.horizontalCenter // 设置字体大小

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
//               anchors.verticalCenter: btnSlowDown.verticalCenter
//               anchors.top: btnSlowDown.top
//               anchors.bottom: speedName.bottom
               x: 931
               font.pixelSize: 28
//               anchors.bottomMargin: 0
//               anchors.topMargin: 0
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
              width: 200
              height: 53
//              anchors.left: po_robotArmEnable.left
//              anchors.right: po_robotArmEnable.right
//              anchors.bottom: po_speedName3.bottom
              font.pixelSize: 28
              anchors.bottomMargin: 0

              // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: po_btnCaliGimbalL_Text
                  text: qsTr("云台归零")
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
                  implicitWidth: 200
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
                         po_btnCaliGimbalL_Text.text = qsTr("左侧云台归零")
                         po_btnCaliGimbalL_Text.color = "#48FFFF"
                         po_btnCaliGimbalL_Background.color = "#003033"
                         po_btnCaliGimbalL_Background.border.color = "#2CDFE8"
                         break;
                     }
                     case 0x02:
                     {
                         po_btnCaliGimbalL_Text.text = qsTr("归零中")
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
              // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: po_btnCaliInstrumentL_Text
                  text: qsTr("器械归零")
                  font.pixelSize: 28
                  opacity: 1.0
                  color: "#48FFFF"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
              }
              // 设置按钮背景
              background: Rectangle {
                  id: po_btnCaliInstrumentL_Background
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
                         po_btnCaliInstrumentL_Text.color="#7f7f7f"
                         po_btnCaliInstrumentL_Background.border.color="#7f7f7f"
                         po_btnCaliInstrumentL_Background.color="#7f7f7f"
                         break;
                     }
                     case 0x01:
                     {
                         po_btnCaliInstrumentL_Text.text = qsTr("左侧器械归零")
                         po_btnCaliInstrumentL_Text.color="#48FFFF"
                         po_btnCaliInstrumentL_Background.color="#003033"
                         po_btnCaliInstrumentL_Background.border.color="#2CDFE8"
                         break;
                     }
                     case 0x02:
                     {
                         po_btnCaliInstrumentL_Text.text = qsTr("归零中yu")
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
              width: 200
              height: 53
              anchors.verticalCenter: po_btnCaliGimbalL.verticalCenter
              anchors.top: po_btnCaliGimbalL.top
              font.pixelSize: 28
              // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: po_btnCaliGimbalR_Text
                  text: qsTr("机械臂归零")
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
                         po_btnCaliGimbalR_Text.color="#7f7f7f"
                         po_btnCaliGimbalR_Background.border.color="#7f7f7f"
                         po_btnCaliGimbalR_Background.color="#7f7f7f"
                         break;
                     }
                     case 0x01:
                     {
                         po_btnCaliGimbalR_Text.text = qsTr("机械臂归零")
                         po_btnCaliGimbalR_Text.color="#48FFFF"
                         po_btnCaliGimbalR_Background.color="#003033"
                         po_btnCaliGimbalR_Background.border.color="#2CDFE8"
                         break;
                     }
                     case 0x02:
                     {
                         po_btnCaliGimbalR_Text.text = qsTr("机械臂归零中")
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
                  text: qsTr("器械归零")
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
                         po_btnCaliInstrumentR_Text.color="#7f7f7f"
                         po_btnCaliInstrumentR_Background.border.color="#7f7f7f"
                         po_btnCaliInstrumentR_Background.color="#7f7f7f"
                         break;
                     }
                     case 0x01:
                     {
                         po_btnCaliInstrumentR_Text.text = qsTr("右侧器械归零")
                         po_btnCaliInstrumentR_Text.color="#48FFFF"
                         po_btnCaliInstrumentR_Background.color="#003033"
                         po_btnCaliInstrumentR_Background.border.color="#2CDFE8"
                         break;
                     }
                     case 0x02:
                     {
                         po_btnCaliInstrumentR_Text.text = qsTr("右侧器械归零中")
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
 //           anchors.right: robotArmDisable.right
 //           anchors.top: speedName.top
 //           anchors.bottom: speedName.bottom

            font.pixelSize: 36
 //           anchors.topMargin: 2
 //           anchors.bottomMargin: 0
            // 设置字体大小
            // 设置按钮文本


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
                //  uiInterface.liftingFastup();
                  po_txt_btnEnterOpreation.color="#FFFFFF"
                  po_btnEnterOpreation_background.color="#48FFFF"
            }
            onReleased:
            {
                //uiInterface.liftingBrake();
                po_txt_btnEnterOpreation.color="#48FFFF"
                po_btnEnterOpreation_background.color="#003033"
            }
            onClicked: {
                UIinterface.setRobotControlMode(3);
                inOpreationPage.visible = true
                preOpreationPage.visible = false
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
                font.pixelSize: 28
                font.family: "Microsoft YaHei UI"
                font.bold: true
        }

        Button {
          //@disable-check M16
          objectName: "po_btnInitialInstrumentAngle30"
          id: po_btnInitialInstrumentAngle30
          x: 1250
          y: 950
          width: 200
          height: 72
          font.pixelSize: 36
          contentItem: Text {
              id:io_txt_btnGetDataFromExternalDevice
              width: 100
              text: qsTr("30°")
              font.pixelSize: 36
              opacity: 1.0
              color: isDataFromInternal ? "#48FFFF" : "#FFFFFF"
              horizontalAlignment: Text.AlignHCenter
              verticalAlignment: Text.AlignVCenter
              elide: Text.ElideRight
          }
          // 设置按钮背景
          background: Rectangle {
              id:po_btnInitialInstrumentAngle30_background
              implicitWidth: 200
              implicitHeight: 56
              opacity: 0.25
              color: isDataFromInternal ? "#003033" : "#48FFFF"
              border.color: "#2CDFE8"
              border.width: 2
              radius: 10
          }
          MouseArea {
              anchors.fill: parent
              onClicked: {
                  // isDataFromInternal = !isDataFromInternal
                  // UIinterface.setDataSource(isDataFromInternal);
              }
          }
       }

        Button {
          //@disable-check M16
          objectName: "po_btnInitialInstrumentAngle60"
          id: po_btnInitialInstrumentAngle60
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
              color: isDataFromInternal ? "#FFFFFF" : "#48FFFF"
              horizontalAlignment: Text.AlignHCenter
              verticalAlignment: Text.AlignVCenter
              elide: Text.ElideRight
          }
          // 设置按钮背景
          background: Rectangle {
              id:po_btnInitialInstrumentAngle60_background
              implicitWidth: 200
              implicitHeight: 56
              opacity: 0.25
              color: isDataFromInternal ? "#48FFFF": "#003033"
              border.color: "#2CDFE8"
              border.width: 2
              radius: 10
          }
          MouseArea {
              anchors.fill: parent
              onClicked: {
                  // isDataFromInternal = !isDataFromInternal
                  // UIinterface.setDataSource(isDataFromInternal);
              }
            }
        }


}
