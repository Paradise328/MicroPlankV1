import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
// in operation page haosibo

Rectangle {
    id: inOperationMain
    property string signalLamp1: "Normal"
    property string signalLamp2: "Warning"
    property string signalLamp3: "Normal"
    property string lockState :"Unlock"
    property string currentAxis: "axis0"
    property string currentLevel: "level3"
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
        currentLevel = "level3";
        level1.state = "unselected"
        level2.state = "unselected"
        level3.state = "selected"
        level4.state = "unselected"
        level5.state = "unselected"
    }
    function readPara(){
        btnType1.state = json.type;
        picConditionF_r.state = json.type;
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

    //指示灯状态id为signal1～signal7：id.state == "Normal/Warning"，显示为正常/警示灯
     Column{
         spacing: 16
         anchors.fill: parent
         anchors.rightMargin: -91
         anchors.bottomMargin: -132
         anchors.leftMargin: 480
         anchors.topMargin: 360
         Rectangle{
             id: picConditionA_l
             width: 100
             height: 100
             color: "transparent"
             state: signalLamp1l
             Image {
                 id: normal1l
                 anchors.centerIn: parent;
                 source: "images/icon_normal.png"
             }
             states: [
                 State {
                     name: "Warning"
                     PropertyChanges { target: normal1l; source: "images/icon_warning.png"}
                 },
                 State {
                     name: "Normal"
                     PropertyChanges { target: normal1l; source: "images/icon_normal.png"}
                 }
             ]
         }
         Text {
             id: txtConditionA_l
             color: itemColor
             text: "左侧A轴"
             font.pixelSize: 16
             font.bold: true
         }
         //2l
         Rectangle{
             id: picConditionB_l
             width: 100
             height: 100
             color: "transparent"
             state: signalLamp2l
             Image {
                 id: normal2l
                 anchors.centerIn: parent;
                 source: "images/icon_normal.png"
             }
             states: [
                 State {
                     name: "Warning"
                     PropertyChanges { target: normal2; source: "images/icon_warning.png"}
                 },
                 State {
                     name: "Normal"
                     PropertyChanges { target: normal2; source: "images/icon_normal.png"}
                 }
             ]
         }
         Text {
             id: txtConditionB_l
             color: itemColor
             text: "左侧B轴"
             font.pixelSize: 16
             font.bold: true
         }
         //3l
         Rectangle{
             id: picConditionC_l
             width: 100
             height: 100
             color: "transparent"
             state: signalLamp3l
             Image {
                 id: normal3l
                 anchors.centerIn: parent;
                 source: "images/icon_normal.png"
             }
             states: [
                 State {
                     name: "Warning"
                     PropertyChanges { target: normal3l; source: "images/icon_warning.png"}
                 },
                 State {
                     name: "Normal"
                     PropertyChanges { target: normal3l; source: "images/icon_normal.png"}
                 }
             ]
         }
         Text {
             id: txtConditionC_l
             color: itemColor
             text: "左侧C轴"
             font.pixelSize: 16
             font.bold: true
         }
         //4l
         Rectangle{
             id: picConditionD_l
             width: 100
             height: 100
             color: "transparent"
             state: signalLamp4l
             Image {
                 id: normal4l
                 anchors.centerIn: parent;
                 source: "images/icon_normal.png"
             }
             states: [
                 State {
                     name: "Warning"
                     PropertyChanges { target: normal4l; source: "images/icon_warning.png"}
                 },
                 State {
                     name: "Normal"
                     PropertyChanges { target: normal4l; source: "images/icon_normal.png"}
                 }
             ]
         }
         Text {
             id: txtConditionD_l
             color: itemColor
             text: "左侧D轴"
             font.pixelSize: 16
             font.bold: true
         }

     }
     //指示灯状态id为signal1～signal7：id.state == "Normal/Warning"，显示为正常/警示灯
      Column{
          spacing: 16
          anchors.fill: parent
          anchors.rightMargin: -93
          anchors.bottomMargin: -132
          anchors.leftMargin: 300
          anchors.topMargin: 360

          //5l
          Rectangle{
              id: picConditionE_l
              width: 100
              height: 100
              color: "transparent"
              state: signalLamp5l
              Image {
                  id: normal5l
                  anchors.centerIn: parent;
                  source: "images/icon_normal.png"
              }
              states: [
                  State {
                      name: "Warning"
                      PropertyChanges { target: normal5l; source: "images/icon_warning.png"}
                  },
                  State {
                      name: "Normal"
                      PropertyChanges { target: normal5l; source: "images/icon_normal.png"}
                  }
              ]
          }
          Text {
              id: txtConditionE_l
              color: itemColor
              text: "左侧E轴"
              font.pixelSize: 16
              font.bold: true
          }
          Rectangle{
              id: picConditionF_l
              width: 100
              height: 100
              color: "transparent"
              state: signalLamp6l
              Image {
                  id: normal6l
                  anchors.centerIn: parent;
                  source: "images/icon_normal.png"
              }
              states: [
                  State {
                      name: "Warning"
                      PropertyChanges { target: normal6l; source: "images/icon_warning.png"}
                  },
                  State {
                      name: "Normal"
                      PropertyChanges { target: normal6l; source: "images/icon_normal.png"}
                  }
              ]
          }
          Text {
              id: txtConditionF_l
              color: itemColor
              text: "左侧F轴"
              font.pixelSize: 16
              font.bold: true
          }
          Rectangle{
              id: picConditionG_l
              width: 100
              height: 100
              color: "transparent"
              state: signalLamp7l
              Image {
                  id: normal7l
                  anchors.centerIn: parent;
                  source: "images/icon_normal.png"
              }
              states: [
                  State {
                      name: "Warning"
                      PropertyChanges { target: normal7l; source: "images/icon_warning.png"}
                  },
                  State {
                      name: "Normal"
                      PropertyChanges { target: normal7l; source: "images/icon_normal.png"}
                  }
              ]
          }
          Text {
              id: txtConditionG_l
              color: itemColor
              text: "左侧G轴"
              font.pixelSize: 16
              font.bold: true
          }
      }
     //右侧指示灯状态id为signal1r～signal3r：id.state == "Normal/Warning"，显示为正常/警示灯

      //指示灯状态id为signal1～signal7：id.state == "Normal/Warning"，显示为正常/警示灯
      Column{
          id: column1
           spacing: 16
           anchors.fill: parent
           anchors.rightMargin: -212
           anchors.bottomMargin: -134
           anchors.leftMargin: 720
           anchors.topMargin: 360
           Rectangle{
               id: picConditionA_r
               width: 100
               height: 100
               color: "transparent"
               state: signalLamp1r
               Image {
                   id: normal1r
                   anchors.verticalCenter: parent.verticalCenter
                   source: "images/icon_normal.png"
                   anchors.horizontalCenter: parent.horizontalCenter
               }
               states: [
                   State {
                       name: "Warning"
                       PropertyChanges { target: normal1r; source: "images/icon_warning.png"}
                   },
                   State {
                       name: "Normal"
                       PropertyChanges { target: normal1r; source: "images/icon_normal.png"}
                   }
               ]
           }
           Text {
               id: txtConditionA_r
               color: itemColor
               text: "右侧A轴"
               font.pixelSize: 16
               font.bold: true
           }
           //2l
           Rectangle{
               id: picConditionB_r
               width: 100
               height: 100
               color: "transparent"
               state: signalLamp2r
               Image {
                   id: normal2r
                   anchors.centerIn: parent;
                   source: "images/icon_normal.png"
               }
               states: [
                   State {
                       name: "Warning"
                       PropertyChanges { target: normal2r; source: "images/icon_warning.png"}
                   },
                   State {
                       name: "Normal"
                       PropertyChanges { target: normal2r; source: "images/icon_normal.png"}
                   }
               ]
           }
           Text {
               id: txtConditionB_r
               color: itemColor
               text: "右侧B轴"
               font.pixelSize: 16
               font.bold: true
           }
           //3l
           Rectangle{
               id: picConditionC_r
               width: 100
               height: 100
               color: "transparent"
               state: signalLamp3r
               Image {
                   id: normal3r
                   anchors.centerIn: parent;
                   source: "images/icon_normal.png"
               }
               states: [
                   State {
                       name: "Warning"
                       PropertyChanges { target: normal3r; source: "images/icon_warning.png"}
                   },
                   State {
                       name: "Normal"
                       PropertyChanges { target: normal3r; source: "images/icon_normal.png"}
                   }
               ]
           }
           Text {
               id: txtConditionC_r
               color: itemColor
               text: "右侧C轴"
               font.pixelSize: 16
               font.bold: true
           }
           //4l
           Rectangle{
               id: picConditionD_r
               width: 100
               height: 100
               color: "transparent"
               state: signalLamp4r
               Image {
                   id: normal4r
                   anchors.centerIn: parent;
                   source: "images/icon_normal.png"
               }
               states: [
                   State {
                       name: "Warning"
                       PropertyChanges { target: normal4r; source: "images/icon_warning.png"}
                   },
                   State {
                       name: "Normal"
                       PropertyChanges { target: normal4r; source: "images/icon_normal.png"}
                   }
               ]
           }
           Text {
               id: txtConditionD_r
               color: itemColor
               text: "右侧D轴"
               font.pixelSize: 16
               font.bold: true
           }

       }
       //指示灯状态id为signal1～signal7：id.state == "Normal/Warning"，显示为正常/警示灯
      Column{
            id: column
            spacing: 16
            anchors.fill: parent
            anchors.rightMargin: -315
            anchors.bottomMargin: -132
            anchors.leftMargin: 900
            anchors.topMargin: 360

            //5l
            Rectangle{
                id: picConditionE_r
                width: 100
                height: 100
                color: "transparent"
                state: signalLamp5r
                Image {
                    id: normal5r
                    anchors.centerIn: parent;
                    source: "images/icon_normal.png"
                }
                states: [
                    State {
                        name: "Warning"
                        PropertyChanges { target: normal5r; source: "images/icon_warning.png"}
                    },
                    State {
                        name: "Normal"
                        PropertyChanges { target: normal5r; source: "images/icon_normal.png"}
                    }
                ]
            }
            Text {
                id: txtConditionE_r
                color: itemColor
                text: "右侧E轴"
                font.pixelSize: 16
                font.bold: true
            }
            Rectangle{
                id: signal6r
                width: 100
                height: 100
                color: "transparent"
                state: signalLamp6l
                Image {
                    id: normal6r
                    anchors.centerIn: parent;
                    source: "images/icon_normal.png"
                }
                states: [
                    State {
                        name: "Warning"
                        PropertyChanges { target: normal6r; source: "images/icon_warning.png"}
                    },
                    State {
                        name: "Normal"
                        PropertyChanges { target: normal6r; source: "images/icon_normal.png"}
                    }
                ]
            }
            Text {
                id: txtConditionF_r
                color: itemColor
                text: "右侧F轴"
                font.pixelSize: 16
                font.bold: true
            }
            Rectangle{
                id: picConditionG_r
                width: 100
                height: 100
                color: "transparent"
                state: signalLamp7r
                Image {
                    id: normal7r
                    anchors.centerIn: parent;
                    source: "images/icon_normal.png"
                }
                states: [
                    State {
                        name: "Warning"
                        PropertyChanges { target: normal7r; source: "images/icon_warning.png"}
                    },
                    State {
                        name: "Normal"
                        PropertyChanges { target: normal7r; source: "images/icon_normal.png"}
                    }
                ]
            }
            Text {
                id: txtConditionG_r
                color: itemColor
                text: "右侧G轴"
                font.pixelSize: 16
                font.bold: true
            }
        }

    //锁状态修改：lock.state == "Lock/Unlock"
     //设置手术类型信息  type为"positive"正向；"opposite"逆向
        Rectangle{
            id: btnType1
            x: -95
            y: 570
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
            id: picConditionF_r
            width: 100
            height: 100
            x: 900
            y:514
            color: "transparent"
            anchors.left: column.left
            anchors.bottom: column1.bottom
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
            x: 180
            y: 200
            id: lblSpeedCount
            font.pixelSize: 44
            font.bold: true
            font.family: "Microsoft YaHei UI"
            font.letterSpacing: 20
            text: qsTr("缩放比例")
            color: "#18FEFE"
        }
        //Level按钮
        Rectangle{
            id: level1
            width: 100
            height: 100
            color: "transparent"
            state: "selected"
            x: 518
            y: 172
            Image {
                id: levelBg1
                anchors.centerIn: parent;
                source: "images/btn_level_hover.png"
            }
            Text {
                id: levelVal1
                y: 25
                font.pixelSize: 30
                font.bold: true
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                text: "1/60"
                color: "#050C0D"
            }
            MouseArea{
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                acceptedButtons: Qt.LeftButton;
//                onClicked: {
//                    robotMain.initBtn();
//                    robotMain.setSpeed(currentAxis,currentLevel);
//                }
            }
            states: [
                State {
                    name: "selected"
                    PropertyChanges { target: levelBg1; source: "images/btn_level_hover.png"}
                    PropertyChanges { target: levelVal1; color: "#050C0D"}
                },
                State {
                    name: "unselected"
                    PropertyChanges { target: levelBg1;source: "images/btn_level.png"}
                    PropertyChanges { target: levelVal1; color: "#48FFFF"}
                }
            ]
        }
        Rectangle{
            id: level2
            width: 100
            height: 100
            color: "transparent"
            anchors.verticalCenter: level1.verticalCenter
            anchors.top: level1.top
            anchors.bottom: level1.bottom
            state: "unselected"
            x: 670
            y: 172
            Image {
                id: levelBg2
                anchors.centerIn: parent;
                source: "images/btn_level.png"
            }
            Text {
                id: levelVal2
                y: 25
                font.pixelSize: 30
                font.bold: true
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                text: "1/40"
                color: "#48FFFF"
                }
            MouseArea{
                anchors.fill: parent;
                acceptedButtons: Qt.LeftButton;
//                onClicked: {
//                    currentLevel = "level2";
//                    robotMain.setSpeed(currentAxis,currentLevel);
//                    level2.state = "selected"
//                    level1.state = "unselected"
//                    level3.state = "unselected"
//                    level4.state = "unselected"
//                    level5.state = "unselected"
//                }
            }
            states: [
                State {
                    name: "selected"
                    PropertyChanges { target: levelBg2; source: "images/btn_level_hover.png"}
                    PropertyChanges { target: levelVal2; color: "#050C0D"}
                },
                State {
                    name: "unselected"
                    PropertyChanges { target: levelBg2;source: "images/btn_level.png"}
                    PropertyChanges { target: levelVal2; color: "#48FFFF"}
                }
            ]
        }
        Rectangle{
            id: level3
            width: 100
            height: 100
            color: "transparent"
            anchors.verticalCenter: level1.verticalCenter
            anchors.top: level1.top
            anchors.bottom: level1.bottom
            state: "unselected"
            x: 821
            y: 172
            Image {
                id: levelBg3
                anchors.centerIn: parent;
                source: "images/btn_level.png"
            }
            Text {
                id: levelVal3
                y: 25
                font.pixelSize: 25
                font.bold: true
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                text: "1/25"
                color: "#48FFFF"
            }
            MouseArea{
                anchors.fill: parent;
                acceptedButtons: Qt.LeftButton;
//                onClicked: {
//                    currentLevel = "level3";
//                    robotMain.setSpeed(currentAxis,currentLevel);
//                    level3.state = "selected"
//                    level1.state = "unselected"
//                    level2.state = "unselected"
//                    level4.state = "unselected"
//                    level5.state = "unselected"
//                }
            }
            states: [
                State {
                    name: "selected"
                    PropertyChanges { target: levelBg3; source: "images/btn_level_hover.png"}
                    PropertyChanges { target: levelVal3; color: "#050C0D"}
                },
                State {
                    name: "unselected"
                    PropertyChanges { target: levelBg3;source: "images/btn_level.png"}
                    PropertyChanges { target: levelVal3; color: "#48FFFF"}
                }
            ]
        }
        Rectangle{
            id: level4
            width: 100
            height: 100
            color: "transparent"
            anchors.verticalCenter: level1.verticalCenter
            anchors.top: level1.top
            anchors.bottom: level1.bottom
            state: "unselected"
            x: 974
            y: 172
            Image {
                id: levelBg4
                anchors.centerIn: parent;
                source: "images/btn_level.png"
            }
            Text {
                id: levelVal4
                y: 25
                font.pixelSize: 30
                font.bold: true
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                text: "1/15"
                color: "#48FFFF"
            }
            MouseArea{
                anchors.fill: parent;
                acceptedButtons: Qt.LeftButton;
//                onClicked: {
//                    currentLevel = "level4";
//                    robotMain.setSpeed(currentAxis,currentLevel);
//                    level4.state = "selected"
//                    level1.state = "unselected"
//                    level2.state = "unselected"
//                    level3.state = "unselected"
//                    level5.state = "unselected"
//                }
            }
            states: [
                State {
                    name: "selected"
                    PropertyChanges { target: levelBg4; source: "images/btn_level_hover.png"}
                    PropertyChanges { target: levelVal4; color: "#050C0D"}
                },
                State {
                    name: "unselected"
                    PropertyChanges { target: levelBg4;source: "images/btn_level.png"}
                    PropertyChanges { target: levelVal4; color: "#48FFFF"}
                }
            ]
        }
        Rectangle{
            id: level5
            width: 100
            height: 100
            color: "transparent"
            anchors.verticalCenter: level1.verticalCenter
            anchors.top: level1.top
            anchors.bottom: level1.bottom
            state: "unselected"
            x: 1130
            y: 172
            Image {
                id: levelBg5
                anchors.centerIn: parent;
                source: "images/btn_level.png"
            }
            Text {
                id: levelVal5
                y: 25
                font.pixelSize: 30
                font.bold: true
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                text: "1/7"
                color: "#48FFFF"
            }
            MouseArea{
                anchors.fill: parent;
                acceptedButtons: Qt.LeftButton;
//                onClicked: {
//                    currentLevel = "level5";
//                    robotMain.setSpeed(currentAxis,currentLevel);
//                    level5.state = "selected"
//                    level1.state = "unselected"
//                    level2.state = "unselected"
//                    level3.state = "unselected"
//                    level4.state = "unselected"
//                }
            }
            states: [
                State {
                    name: "selected"
                    PropertyChanges { target: levelBg5; source: "images/btn_level_hover.png"}
                    PropertyChanges { target: levelVal5; color: "#050C0D"}
                },
                State {
                    name: "unselected"
                    PropertyChanges { target: levelBg5;source: "images/btn_level.png"}
                    PropertyChanges { target: levelVal5; color: "#48FFFF"}
                }
            ]
        }
        // @disable-check M300
       Connections
       {
           target: uiInterface
           onLevelChanged:{

               switch(uiInterface.getlevel()){
                               case 1:
                                   currentLevel = "level1";
                                   level1.state = "selected"
                                   level2.state = "unselected"
                                   level3.state = "unselected"
                                   level4.state = "unselected"
                                   level5.state = "unselected"
//                                   robotMain.initBtn();
//               //                    robotMain.setSpeed(currentAxis,currentLevel);
                                   break;
                               case 2:
                                    console.log("level 2 ")
                                   currentLevel = "level2";
               //                    robotMain.setSpeed(currentAxis,currentLevel);
                                   level2.state = "selected"
                                   level1.state = "unselected"
                                   level3.state = "unselected"
                                   level4.state = "unselected"
                                   level5.state = "unselected"
                                   break;
                               case 3:
                                   currentLevel = "level3";
               //                    robotMain.setSpeed(currentAxis,currentLevel);
                                   level3.state = "selected"
                                   level1.state = "unselected"
                                   level2.state = "unselected"
                                   level4.state = "unselected"
                                   level5.state = "unselected"
                                   break;
                               case 4:
                                   console.log("level 4 ")
                                   currentLevel = "level4";
               //                    robotMain.setSpeed(currentAxis,currentLevel);
                                   level4.state = "selected"
                                   level1.state = "unselected"
                                   level2.state = "unselected"
                                   level3.state = "unselected"
                                   level5.state = "unselected"
                                   break;
                               case 5:
                                   currentLevel = "level5";
               //                    robotMain.setSpeed(currentAxis,currentLevel);
                                   level5.state = "selected"
                                   level1.state = "unselected"
                                   level2.state = "unselected"
                                   level3.state = "unselected"
                                   level4.state = "unselected"
                                   break;
                               default:
                                   console.log("无效的速度设置")
                               }
           }
       }

//        Connections{
//            target: uiInterface
//            onUiSignalChanged:{
//                console.log("trigger uiSignalChanged:"); //+ uiInterface.getSignal(0))
//                switch(uiInterface.getSignal(0)){
//                case 1:
//                    robotMain.initBtn();
////                    robotMain.setSpeed(currentAxis,currentLevel);
//                    break;
//                case 2:
//                    currentLevel = "level2";
////                    robotMain.setSpeed(currentAxis,currentLevel);
//                    level2.state = "selected"
//                    level1.state = "unselected"
//                    level3.state = "unselected"
//                    level4.state = "unselected"
//                    level5.state = "unselected"
//                    break;
//                case 3:
//                    currentLevel = "level3";
////                    robotMain.setSpeed(currentAxis,currentLevel);
//                    level3.state = "selected"
//                    level1.state = "unselected"
//                    level2.state = "unselected"
//                    level4.state = "unselected"
//                    level5.state = "unselected"
//                    break;
//                case 4:
//                    currentLevel = "level4";
////                    robotMain.setSpeed(currentAxis,currentLevel);
//                    level4.state = "selected"
//                    level1.state = "unselected"
//                    level2.state = "unselected"
//                    level3.state = "unselected"
//                    level5.state = "unselected"
//                    break;
//                case 5:
//                    currentLevel = "level5";
////                    robotMain.setSpeed(currentAxis,currentLevel);
//                    level5.state = "selected"
//                    level1.state = "unselected"
//                    level2.state = "unselected"
//                    level3.state = "unselected"
//                    level4.state = "unselected"
//                    break;
//                default:
//                    console.log("无效的速度设置")
//                }
//            }
//        }


         //测试按钮1->kinematics
          Button {
               // @disable-check M16
<<<<<<< HEAD
              objectName: "kinematics"
=======
              objectName: "btnEnable_l"
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
              id: btnEnable_l
              y: 773
              width: 179
              height: 56
              x: 81
              font.pixelSize: 28
              anchors.horizontalCenter: btnUsedTime_l.horizontalCenter // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: txtEnable_l
                  x: 7
                  y: 6
                  text: qsTr("左侧未使能")
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
<<<<<<< HEAD
=======
                  id: btnEnable_l_background
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
                  implicitWidth: 200
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
              }
<<<<<<< HEAD
=======
              function setsta(sta)
              {
                  if(sta===true)
                  {
                      btnEnable_l.enabled=true
                      txtEnable_l.text=qsTr("左侧使能")
                      txtEnable_l.color="#FFFFFF"
                      btnEnable_l_background.color="#48FFFF"
                      btnEnable_l_background.border.color="#2CDFE8"
                  }
                  else
                  {
                      btnEnable_l.enabled=true
                      txtEnable_l.text=qsTr("左侧未使能")
                      txtEnable_l.color="#48FFFF"
                      btnEnable_l_background.color="#003033"
                      btnEnable_l_background.border.color="#2CDFE8"
                  }
              }
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
              onClicked: {
                  triggerKinematics(2);
              }
          }
          //测试按钮1->kinematics
           Button {
                // @disable-check M16
<<<<<<< HEAD
               objectName: "kinematics"
=======
               objectName: "btnEnable_r"
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
               id: btnEnable_r
               y: 773
               width: 172
               height: 56
               anchors.verticalCenter: btnEnable_l.verticalCenter
               x: 1026
               font.pixelSize: 28
               anchors.horizontalCenterOffset: 0
               anchors.horizontalCenter: lblUsedtime_r.horizontalCenter // 设置字体大小
               // 设置按钮文本
               contentItem: Text {
                   id: txtEnable_r
                   x: 8
                   y: 6
                   text: qsTr("右侧未使能")
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
<<<<<<< HEAD
=======
                   id:btnEnable_r_background
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
                   implicitWidth: 200
                   implicitHeight: 56
                   opacity: 0.25
                   color: "#003033"
                   border.color: "#2CDFE8"
                   border.width: 2
               }
<<<<<<< HEAD
=======
               function setsta(sta)
               {
                   if(sta===true)
                   {
                       btnEnable_r.enabled=true
                       txtEnable_r.text=qsTr("右侧使能")
                       txtEnable_r.color="#FFFFFF"
                       btnEnable_r_background.color="#48FFFF"
                       btnEnable_r_background.border.color="#2CDFE8"
                   }
                   else
                   {
                       btnEnable_r.enabled=true
                       txtEnable_r.text=qsTr("右侧未使能")
                       txtEnable_r.color="#48FFFF"
                       btnEnable_r_background.color="#003033"
                       btnEnable_r_background.border.color="#2CDFE8"
                   }
               }
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
               onClicked: {
                   triggerKinematics(3);
               }
           }
           //测试按钮1->kinematics
           Button {
                // @disable-check M16
               objectName: "kinematics"
               id: btnInstrument_l
               y: 465
               width: 172
               height: 56
               anchors.right: btnUsedTime_l.right
               x: 84
               font.pixelSize: 28
               anchors.horizontalCenter: btnUsedTime_l.horizontalCenter // 设置字体大小
               // 设置按钮文本
               contentItem: Text {
                   id: txtInstrument_l
                   x: 8
                   y: 6
                   text: qsTr("持针镊")
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
                   implicitWidth: 200
                   implicitHeight: 56
                   opacity: 0.25
                   color: "#003033"
                   border.color: "#2CDFE8"
                   border.width: 2
               }
               onClicked: {
                   triggerKinematics(3);
               }
           }Button {
               // @disable-check M16
              objectName: "kinematics"
              id: btnUsedTime_l
              y: 614
              width: 172
              height: 56
              anchors.top: btnUsedTime_r.top
              anchors.bottom: btnType1.bottom
              x: 84
              font.pixelSize: 28 // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: txtUsedTime_l
                  x: 8
                  y: 6
                  text: qsTr("20.4h")
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
                  implicitWidth: 200
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
              }
              onClicked: {
                  triggerKinematics(3);
              }
          }//测试按钮1->kinematics
           Button {
                // @disable-check M16
               objectName: "kinematics"
               id: btnInstrument_r
               y: 465
               width: 172
               height: 56
               anchors.verticalCenter: btnInstrument_l.verticalCenter
               x: 1026
               font.pixelSize: 28
               anchors.horizontalCenter: btnUsedTime_r.horizontalCenter // 设置字体大小
               // 设置按钮文本
               contentItem: Text {
                   id: txtInstrument_r
                   x: 8
                   text: qsTr("显微剪")
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
                   implicitWidth: 200
                   implicitHeight: 56
                   opacity: 0.25
                   color: "#003033"
                   border.color: "#2CDFE8"
                   border.width: 2
               }
               onClicked: {
                   triggerKinematics(3);
               }
           }Button {
               // @disable-check M16
              objectName: "kinematics"
              id: btnUsedTime_r
              y: 614
              width: 172
              height: 56
              anchors.verticalCenter: btnUsedTime_l.verticalCenter
              x: 1026
              font.pixelSize: 28 // 设置字体大小
              // 设置按钮文本
              contentItem: Text {
                  id: txtUsedTime_r
                  x: 8
                  y: 6
                  text: qsTr("0.4h")
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
                  implicitWidth: 200
                  implicitHeight: 56
                  opacity: 0.25
                  color: "#003033"
                  border.color: "#2CDFE8"
                  border.width: 2
              }
              onClicked: {
                  triggerKinematics(3);
              }
           }



          Text {
              id: lblEnable_l
              x: 90
              y: 736
              color: "#18fefe"
              text: qsTr("使能状态")
              font.letterSpacing: 16
              font.pixelSize: 24
              horizontalAlignment: Text.AlignLeft
              clip: false
              font.family: "Microsoft YaHei UI"
              font.bold: true
          }

          Text {
              id: lblInstrument_r
              x: 1034
              y: 418
              color: "#18fefe"
              text: qsTr("器械类型")
              font.letterSpacing: 16
              font.pixelSize: 24
              font.family: "Microsoft YaHei UI"
              font.bold: true
          }
          Text {
              id: lblInstrument_l
              x: 90
              y: 418
              color: "#18fefe"
              text: qsTr("器械类型")
              anchors.verticalCenter: lblInstrument_r.verticalCenter
              font.letterSpacing: 16
              font.pixelSize: 24
              horizontalAlignment: Text.AlignLeft
              clip: false
              font.family: "Microsoft YaHei UI"
              font.bold: true
          }
          Text {
              id: lblEnable_r
              x: 1034
              y: 736
              color: "#18fefe"
              text: qsTr("使能状态")
              font.letterSpacing: 16
              font.pixelSize: 24
              font.family: "Microsoft YaHei UI"
              font.bold: true
          }

          Text {
              id: lblUsedTime_l
              x: 90
              y: 577
              width: 108
              height: 31
              color: "#18fefe"
              text: qsTr("使用时长")
              font.letterSpacing: 16
              font.pixelSize: 24
              horizontalAlignment: Text.AlignLeft
              clip: false
              font.family: "Microsoft YaHei UI"
              font.bold: true
          }

          Text {
              id: lblUsedtime_r
              x: 1034
              y: 577
              color: "#18fefe"
              text: qsTr("使用时长")
              font.letterSpacing: 16
              font.pixelSize: 24
              font.family: "Microsoft YaHei UI"
              font.bold: true
          }
}

/*##^##
Designer {
    D{i:0;autoSize:true;formeditorZoom:0.66;height:480;width:640}
}
##^##*/
