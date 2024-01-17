import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
// in operation page

Rectangle {
    id: robotMain
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
     Text {
         id: operationName
         x: 150
         y: 200
         font.pixelSize: 44
         font.bold: true
         font.family: "Microsoft YaHei UI"
         font.letterSpacing: 20
         color: "#18FEFE"
         text: "水平增程臂"
     }
        Rectangle{
            id: btnType1
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
            id: btnType2
            width: 100
            height: 100
            x: 880
            y:180
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

        Text {
            x: 150
            y: 326
            id: speedName
            font.pixelSize: 44
            font.bold: true
            font.family: "Microsoft YaHei UI"
            font.letterSpacing: 20
            text: qsTr("竖直升降柱")
            color: "#18FEFE"
        }
        //Level按钮

         //测试按钮1->kinematics
         Button {
             //@disable-check M16
             objectName: "robotArmEnable"
             id: robotArmEnable
             y: 202
             anchors.verticalCenter: btnType1.verticalCenter
             anchors.left: btnFastDown.left
             anchors.bottom: robotArmDisable.bottom
             x: 575
             font.pixelSize: 28
             font.italic: false
             rotation: 0
             anchors.verticalCenterOffset: 0
             anchors.bottomMargin: 0
             anchors.horizontalCenter: btnReleaseInstrument.horizontalCenter // 设置字体大小

             // 设置按钮文本
             contentItem: Text {
                 //@disable-check M16
                 objectName: "contentItem"
                 id: txtRobotArmEnable
                 text: qsTr("使能")
                 font.pixelSize: 28
                 opacity: 1.0
                 color: "#48FFFF"
                 horizontalAlignment: Text.AlignHCenter
                 verticalAlignment: Text.AlignVCenter
                 clip: false
                 elide: Text.ElideRight
             }
            signal triggerKinematics(int num)
             // 设置按钮背景
             background: Rectangle {
                 //@disable-check M16
                 objectName: "background"
                 id: robotArmEnable_background
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
                        txtRobotArmEnable.color="#7f7f7f"
                        robotArmEnable_background.border.color="#7f7f7f"
                        robotArmEnable_background.color="#7f7f7f"
                        break;
                    }
                    case 0x01:
                    {
                        txtRobotArmEnable.color="#48FFFF"
                        robotArmEnable_background.color="#003033"
                        robotArmEnable_background.border.color="#2CDFE8"
                        break;
                    }
                    case 0x02:
                    {
                        txtRobotArmEnable.color="#FFFFFF"
                        robotArmEnable_background.color="#48FFFF"
                        robotArmEnable_background.border.color="#2CDFE8"
                        break;
                    }
                    default:break;
                 }
             }
             onClicked: {
                 uiInterface.enableRobotArm();

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
                 //robotArmEnable.changecolor()
                 uiInterface.disableRobotArm();
             }
         }

         Button {
           //@disable-check M16
           objectName: "btnFastUp"
           id: btnFastUp
           y: 326
           width: 100
           height: 56
           anchors.right: robotArmDisable.right
           anchors.top: speedName.top
           anchors.bottom: speedName.bottom
           x: 985
           font.pixelSize: 28
           anchors.topMargin: 2
           anchors.bottomMargin: 0
           // 设置字体大小
           // 设置按钮文本
           contentItem: Text {
               id: txtFastUp
               width: 100
               text: qsTr("快升")
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
               id:btnFastUp_background
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
                       btnFastUp.enabled=false
                       txtFastUp.color="#7f7f7f"
                       btnFastUp_background.border.color="#7f7f7f"
                       btnFastUp_background.color="#7f7f7f"
                       break;
                   }
                  case 0x01:
                  {
                      btnFastUp.enabled=true
                      txtFastUp.color="#48FFFF"
                      btnFastUp_background.color="#003033"
                      btnFastUp_background.border.color="#2CDFE8"
                      break;
                  }
                  case 0x02:
                  {
                      btnFastUp.enabled=true
                      txtFastUp.color="#FFFFFF"
                      btnFastUp_background.color="#48FFFF"
                      btnFastUp_background.border.color="#2CDFE8"
                      break;
                  }
                  default:break;
               }
           }
           onPressed:
           {
                 uiInterface.liftingFastup();
                 txtFastUp.color="#FFFFFF"
                 btnFastUp_background.color="#48FFFF"
           }
           onReleased:
           {
               uiInterface.liftingBrake();
               txtFastUp.color="#48FFFF"
               btnFastUp_background.color="#003033"
           }
           onClicked: {
               triggerKinematics(1);
           }
        }
         Button {
           //@disable-check M16
           objectName: "btnSlowUp"
           id: btnSlowUp
           y: 326
           width: 100
           height: 56
           anchors.left: btnSlowDown.right
           anchors.top: speedName.top
           anchors.bottom: speedName.bottom
           x: 812
           font.pixelSize: 28
           anchors.leftMargin: 62
           anchors.topMargin: 2
           anchors.bottomMargin: 0
           // 设置字体大小
           // 设置按钮文本
           contentItem: Text {
               id: txtSlowUp
               width: 100
               text: qsTr("慢升")
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
               id:btnSlowUp_background
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
                   btnSlowUp.enabled=false
                   txtSlowUp.color="#7f7f7f"
                   btnSlowUp_background.border.color="#7f7f7f"
                   btnSlowUp_background.color="#7f7f7f"
                   break;
               }
                  case 0x01:
                  {
                      btnSlowUp.enabled=true
                      txtSlowUp.color="#48FFFF"
                      btnSlowUp_background.color="#003033"
                      btnSlowUp_background.border.color="#2CDFE8"
                      break;
                  }
                  case 0x02:
                  {
                      btnSlowUp.enabled=true
                      txtSlowUp.color="#FFFFFF"
                      btnSlowUp_background.color="#48FFFF"
                      btnSlowUp_background.border.color="#2CDFE8"
                      break;
                  }
                  default:break;
               }
           }

           onPressed:
           {
               uiInterface.liftingFastup()
                 txtSlowUp.color="#FFFFFF"
                 btnSlowUp_background.color="#48FFFF"
           }
           onReleased:
           {
              uiInterface.liftingBrake();
               txtSlowUp.color="#48FFFF"
               btnSlowUp_background.color="#003033"
           }
           onClicked: {
               triggerKinematics(1);
           }
        }
         Button {
           //@disable-check M16
           objectName: "btnSlowDown"
           id: btnSlowDown
           y: 328
           width: 100
           height: 56
           anchors.top: btnSlowUp.top
           anchors.bottom: speedName.bottom
           x: 550
           font.pixelSize: 28
           anchors.topMargin: 0
           anchors.horizontalCenterOffset: 76
           anchors.bottomMargin: 0
           anchors.horizontalCenter: btnReleaseInstrument.horizontalCenter // 设置字体大小
           // 设置按钮文本
           contentItem: Text {
               id: txtSlowDown
               width: 100
               text: qsTr("慢降")
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
               id:btnSlowDown_background
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
                   btnSlowDown.enabled=false
                   txtSlowDown.color="#7f7f7f"
                   btnSlowDown_background.border.color="#7f7f7f"
                   btnSlowDown_background.color="#7f7f7f"
                   break;
               }
                  case 0x01:
                  {
                      btnSlowDown.enabled=true
                      txtSlowDown.color="#48FFFF"
                      btnSlowDown_background.color="#003033"
                      btnSlowDown_background.border.color="#2CDFE8"
                      break;
                  }
                  case 0x02:
                  {

                      btnSlowDown.enabled=true
                      txtSlowDown.color="#FFFFFF"
                      btnSlowDown_background.color="#48FFFF"
                      btnSlowDown_background.border.color="#2CDFE8"
                      break;
                  }
                  default:break;
               }
           }

           onPressed:
           {
               uiInterface.liftingSlowdown();
                 txtSlowDown.color="#FFFFFF"
                 btnSlowDown_background.color="#48FFFF"
           }
           onReleased:
           {
               uiInterface.liftingBrake();
               txtSlowDown.color="#48FFFF"
               btnSlowDown_background.color="#003033"
           }
           onClicked: {
               triggerKinematics(1);
           }
        }
         Button {
               //@disable-check M16
               objectName: "btnFastDown"
               id: btnFastDown
               y: 328
               width: 100
               anchors.verticalCenter: btnSlowDown.verticalCenter
               anchors.top: btnSlowDown.top
               anchors.bottom: speedName.bottom
               x: 575
               font.pixelSize: 28
               anchors.bottomMargin: 0
               anchors.topMargin: 0
               // 设置字体大小
               // 设置按钮文本
               contentItem: Text {
                   id: txtFastDown
                   text: qsTr("快降")
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
                   id:btnFastDown_background
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
                           btnFastDown.enabled=false
                           txtFastDown.color="#7f7f7f"
                           btnFastDown_background.border.color="#7f7f7f"
                           btnFastDown_background.color="#7f7f7f"
                           break;
                       }
                      case 0x01:
                      {
                          btnFastDown.enabled=true
                          txtFastDown.color="#48FFFF"
                          btnFastDown_background.color="#003033"
                          btnFastDown_background.border.color="#2CDFE8"
                          break;
                      }
                      case 0x02:
                      {
                          btnFastDown.enabled=true
                          txtFastDown.color="#FFFFFF"
                          btnFastDown_background.color="#48FFFF"
                          btnFastDown_background.border.color="#2CDFE8"
                          break;
                      }
                      default:break;
                   }
               }
               onPressed:
               {
                     uiInterface.liftingFastdown()
                     txtFastDown.color="#FFFFFF"
                     btnFastDown_background.color="#48FFFF"
               }
               onReleased:
               {
                   uiInterface.liftingBrake();
                   txtFastDown.color="#48FFFF"
                   btnFastDown_background.color="#003033"
               }
               onClicked: {
                   triggerKinematics(2);
               }
           }

          //测试按钮1->kinematics
          Button {
              //@disable-check M16
              objectName: "btnCaliGimbalL"
              id: btnCaliGimbalL
              y: 751
              width: 200
              height: 53
              anchors.left: robotArmEnable.left
              anchors.right: robotArmEnable.right
              anchors.bottom: speedName3.bottom
              x: 575
              font.pixelSize: 28
              anchors.bottomMargin: 0
              // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: txtCaliGimbalL
                  text: qsTr("左侧云台归零")
                  font.pixelSize: 28
                  opacity: 1.0
                  color: "#48FFFF"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
              }
              // 设置按钮背景
              background: Rectangle {
                  implicitWidth: 200
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
              }
          }
          Button {
              //@disable-check M16
              objectName: "btnCaliInstrumentL"
              id: btnCaliInstrumentL
              y: 843
              x: 650
              font.pixelSize: 28
              anchors.rightMargin: 0
              anchors.horizontalCenter: btnCaliGimbalL.horizontalCenter
              // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: txtCaliInstrumentL
                  text: qsTr("左侧器械归零")
                  font.pixelSize: 28
                  opacity: 1.0
                  color: "#48FFFF"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
              }
              // 设置按钮背景
              background: Rectangle {
                  implicitWidth: 200
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
              }
          }

          Button {
              //@disable-check M16
              objectName: "btnCaliGimbalR"
              id: btnCaliGimbalR
              y: 751
              width: 200
              height: 53
              anchors.verticalCenter: btnCaliGimbalL.verticalCenter
              anchors.top: btnCaliGimbalL.top
              x: 885
              font.pixelSize: 28
              // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: txtCaliGimbalR
                  text: qsTr("右侧云台归零")
                  font.pixelSize: 28
                  opacity: 1.0
                  color: "#48FFFF"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
              }
              // 设置按钮背景
              background: Rectangle {
                  implicitWidth: 200
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
              }
          }
          Button {
              //@disable-check M16
              objectName: "btnCaliInstrumentR"
              id: btnCaliInstrumentR
              y: 843
              x: 600
              font.pixelSize: 28
              anchors.horizontalCenterOffset: 0
              anchors.horizontalCenter: btnCaliGimbalR.horizontalCenter
              // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: txtCaliInstrumentR
                  text: qsTr("右侧器械归零")
                  font.pixelSize: 28
                  opacity: 1.0
                  color: "#48FFFF"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
              }
              // 设置按钮背景
              background: Rectangle {
                  implicitWidth: 200
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
<<<<<<< HEAD
=======

              }onClicked:{

>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
              }
          }

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

          Button {
              //@disable-check M16
              objectName: "btnUsedTime_L"
              id: btnUsedTime_L
              y: 630

              x: 575
              font.pixelSize: 28
              anchors.horizontalCenterOffset: 0
              anchors.horizontalCenter: robotArmEnable.horizontalCenter
              anchors.bottomMargin: 0
              anchors.topMargin: 0
              // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: txtUsedTime_L
                  text: qsTr("10.4h")
                  font.pixelSize: 28
                  opacity: 1.0
                  color: "#48FFFF"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
              }
              // 设置按钮背景
              background: Rectangle {
                  implicitWidth: 200
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
              }
          }

          Button {
              //@disable-check M16
              objectName: "btnUsedTime_R"
              id: btnUsedTime_R
              y: 630

              x: 575
              font.pixelSize: 28
              anchors.horizontalCenterOffset: 310
              anchors.horizontalCenter: robotArmEnable.horizontalCenter
              anchors.bottomMargin: 0
              anchors.topMargin: 0
              // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: txtUsedTime_R
                  text: qsTr("30.8h")
                  font.pixelSize: 28
                  opacity: 1.0
                  color: "#48FFFF"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
              }
              // 设置按钮背景
              background: Rectangle {
                  implicitWidth: 200
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
              }
          }

          Button {
              //@disable-check M16
              objectName: "btnInstrumentType_R"
              id: btnInstrumentType_R
              y: 524

              x: 575
              font.pixelSize: 28
              anchors.horizontalCenterOffset: 310
              anchors.horizontalCenter: robotArmEnable.horizontalCenter
              anchors.bottomMargin: 0
              anchors.topMargin: 0
              // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  //@disable-check M16
                  objectName: "txtInstrumentType_R"
                  id: txtInstrumentType_R
                  text: qsTr("持针钳")
                  font.pixelSize: 28
                  opacity: 1.0
                  color: "#48FFFF"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
              }
              // 设置按钮背景
              background: Rectangle {
                  implicitWidth: 200
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
              }
          }

          Button {
              //@disable-check M16
              objectName: "btnInstrumentType_L"
              id: btnInstrumentType_L
              y: 524

              x: 575
              font.pixelSize: 28
              anchors.horizontalCenterOffset: 0
              anchors.horizontalCenter: robotArmEnable.horizontalCenter
              anchors.bottomMargin: 0
              anchors.topMargin: 0
              // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  //@disable-check M16
                  objectName: "txtInstrumentType_L"
                  id: txtInstrumentType_L
                  text: qsTr("显微剪")
                  font.pixelSize: 28
                  opacity: 1.0
                  color: "#48FFFF"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
              }
              // 设置按钮背景
              background: Rectangle {
                  implicitWidth: 200
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
              }
          }

          Button {
              //@disable-check M16
              objectName: "btnInstrumentInstalled"
              id: btnInstrumentInstalled
              y: 424
              x: 885
              font.pixelSize: 28
              // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: txtInstrumentInstalled
                  text: qsTr("器械已安装")
                  font.pixelSize: 28
                  opacity: 1.0
                  color: "#48FFFF"
                  horizontalAlignment: Text.AlignHCenter
                  verticalAlignment: Text.AlignVCenter
                  elide: Text.ElideRight
              }
              // 设置按钮背景
              background: Rectangle {
                  id: instrumentInstalled_background
                  implicitWidth: 200
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
              }
              onPressed:
              {
                    txtInstrumentInstalled.color="#FFFFFF"
                    instrumentInstalled_background.color="#48FFFF"
              }
              onReleased:
              {
                  txtInstrumentInstalled.color="#48FFFF"
                  instrumentInstalled_background.color="#003033"
              }
          }

          Text {
              //@disable-check M16
              objectName: "speedName1"
              id: speedName1
              x: 150
              y: 503
              color: "#18fefe"
              text: "器械类型"
              anchors.left: operationName.left
              font.letterSpacing: 20
              font.pixelSize: 44
              font.styleName: "Regular"
              font.family: "Microsoft YaHei UI"
              font.bold: true
          }

          Text {
              id: speedName2
              x: 150
              y: 628
              color: "#18fefe"
              text: "使用时长"
              anchors.left: operationName.left
              anchors.right: speedName1.right
              font.letterSpacing: 20
              font.pixelSize: 44
              font.family: "Microsoft YaHei UI"
              font.bold: true
          }
          //测试按钮1->kinematics
           //测试按钮1->kinematics
           //测试按钮1->kinematics

          Text {
              id: speedName3
              x: 150
              y: 746
              color: "#18fefe"
              text: "执行器初始化"
              anchors.left: operationName.left
              font.letterSpacing: 20
              font.pixelSize: 44
              anchors.leftMargin: 0
              font.family: "Microsoft YaHei UI"
              font.bold: true
          }

}



/*##^##
Designer {
    D{i:0;autoSize:true;formeditorZoom:0.5;height:480;width:640}D{i:61;annotation:"1 //;;//  //;;//  //;;// <!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html> //;;// 1681695823";customId:"speedName1"}
}
##^##*/
