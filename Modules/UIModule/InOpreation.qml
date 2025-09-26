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

//    Image {
//        id: freamBg
//        anchors.centerIn: parent
//        //source: "images/freambg.png"
//        source: "images/initbg.png"
//        anchors.verticalCenterOffset: 0
//        anchors.horizontalCenterOffset: 0
//        smooth: true
//     }

    Image {
        id: io_inOpreationPageBg
        source: "images/bg_inopreation.png"
        width: 1920//parent.width//Screen.desktopAvailableWidth
        height: 1080//Screen.desktopAvailableheight
        smooth: true
    }
    //指示灯状态id为signal1～signal7：id.state == "Normal/Warning"，显示为正常/警示灯

    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_A_Front_l"
        id: io_picCondition_Axis_A_Front_l
        x:767
        y:485
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp1l
        Image {
            id: io_image_Axis_A_Front_l
            anchors.centerIn: parent;
            source: "images/icon_axis_free_1.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_A_Front_l.source="images/icon_axis_free_1.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_A_Front_l.source="images/icon_axis_limited_1.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_A_Front_l; source: "images/icon_axis_limited_1.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_A_Front_l; source: "images/icon_axis_free_1.png"}
            }
        ]
    }

    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_A_Back_l"
        id: io_picCondition_Axis_A_Back_l
        x:727
        y:445
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp1l
        Image {
            id: io_image_Axis_A_Back_l
            anchors.centerIn: parent;
            source: "images/icon_axis_free_2.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_A_Back_l.source="images/icon_axis_free_2.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_A_Back_l.source="images/icon_axis_limited_2.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_A_Back_l; source: "images/icon_axis_limited_2.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_A_Back_l; source: "images/icon_axis_free_2.png"}
            }
        ]
    }

    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_B_Front_l"
        id: io_picCondition_Axis_B_Front_l
        x:493
        y:525
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp2l
        Image {
            id: io_image_Axis_B_Front_l
            anchors.centerIn: parent;
            source: "images/icon_axis_free_4.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_B_Front_l.source="images/icon_axis_free_4.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_B_Front_l.source="images/icon_axis_limited_4.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_B_Front_l; source: "images/icon_axis_limited_4.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_B_Front_l; source: "images/icon_axis_free_4.png"}
            }
        ]
    }

    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_B_Back_l"
        id: io_picCondition_Axis_B_Back_l
        x:531
        y:487
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp2l
        Image {
            id: io_image_Axis_B_Back_l
            anchors.centerIn: parent;
            source: "images/icon_axis_free_3.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_B_Back_l.source="images/icon_axis_free_3.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_B_Back_l.source="images/icon_axis_limited_3.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_B_Back_l; source: "images/icon_axis_limited_3.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_B_Back_l; source: "images/icon_axis_free_3.png"}
            }
        ]
    }

    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_C_l"
        id: io_picCondition_Axis_C_l
        x:610
        y:500
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp3l
        Image {
            id: io_image_Axis_C_l
            anchors.centerIn: parent;
            source: "images/icon_axis_free_5.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
               states.name="Normal"
            }
            else if(sta === 1)
            {
                 states.name="Warning"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_C_l; source: "images/icon_axis_limited_5.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_C_l; source: "images/icon_axis_free_5.png"}
            }
        ]
    }

    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_D_l"
        id: io_picCondition_Axis_D_l
        x:715
        y:600
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp4l
        Image {
            id: io_image_Axis_D_l
            anchors.centerIn: parent;
            source: "images/icon_axis_free_6.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
               states.name="Normal"
            }
            else if(sta === 1)
            {
                 states.name="Warning"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_D_l; source: "images/icon_axis_limited_6.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_D_l; source: "images/icon_axis_free_6.png"}
            }
        ]
    }

    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_E_Front_l"
        id: io_picCondition_Axis_E_Front_l
        x:222
        y:657
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp5l
        Image {
            id: io_image_Axis_E_Front_l
            anchors.centerIn: parent;
            source: "images/icon_axis_free_13.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_E_Front_l.source="images/icon_axis_free_13.png"

            }
            else if(sta === 1)
            {
                io_image_Axis_E_Front_l.source="images/icon_axis_limited_13.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_E_Front_l; source: "images/icon_axis_limited_13.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_E_Front_l; source: "images/icon_axis_free_13.png"}
            }
        ]
    }


    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_E_Back_l"
        id: io_picCondition_Axis_E_Back_l
        x:352
        y:528
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp5l
        Image {
            id: io_image_Axis_E_Back_l
            anchors.centerIn: parent;
            source: "images/icon_axis_free_14.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_E_Back_l.source="images/icon_axis_free_14.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_E_Back_l.source="images/icon_axis_limited_14.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_E_Back_l; source: "images/icon_axis_limited_14.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_E_Back_l; source: "images/icon_axis_free_14.png"}
            }
        ]
    }

    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_F_Front_l"
        id: io_picCondition_Axis_F_Front_l
        x:190
        y:585
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp6l
        Image {
            id: io_image_Axis_F_Front_l
            anchors.centerIn: parent;
            source: "images/icon_axis_free_11.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_F_Front_l.source="images/icon_axis_free_11.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_F_Front_l.source="images/icon_axis_limited_11.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_F_Front_l; source: "images/icon_axis_limited_11.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_F_Front_l; source: "images/icon_axis_free_11.png"}
            }
        ]
    }


    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_F_Back_l"
        id: io_picCondition_Axis_F_Back_l
        x:412
        y:585
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp6l
        Image {
            id: io_image_Axis_F_Back_l
            anchors.centerIn: parent;
            source: "images/icon_axis_free_12.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_F_Back_l.source="images/icon_axis_free_12.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_F_Back_l.source="images/icon_axis_limited_12.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_F_Back_l; source: "images/icon_axis_limited_12.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_F_Back_l; source: "images/icon_axis_free_12.png"}
            }
        ]
    }

    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_G_Front_l"
        id: io_picCondition_Axis_G_Front_l
        x:293
        y:685
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp7l
        Image {
            id: io_image_Axis_G_Front_l
            anchors.centerIn: parent;
            source: "images/icon_axis_free_10.png"
        }
        function setsta(sta)
        {
//            console.log("sta = ")
//            console.log(sta)
            if(sta === 0)
            {
                io_image_Axis_G_Front_l.source="images/icon_axis_free_10.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_G_Front_l.source="images/icon_axis_limited_10.png"

            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_G_Front_l; source: "images/icon_axis_limited_10.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_G_Front_l; source: "images/icon_axis_free_10.png"}
            }
        ]
    }

    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_G_Back_l"
        id: io_picCondition_Axis_G_Back_l
        x:293
        y:468
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp7l
        Image {
            id: io_image_Axis_G_Back_l
            anchors.centerIn: parent;
            source: "images/icon_axis_free_9.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_G_Back_l.source="images/icon_axis_free_9.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_G_Back_l.source="images/icon_axis_limited_9.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_G_Back_l; source: "images/icon_axis_limited_9.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_G_Back_l; source: "images/icon_axis_free_9.png"}
            }
        ]
    }

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------


    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_A_Front_r"
        id: io_picCondition_Axis_A_Front_r
        x:1053
        y:485
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp7l
        Image {
            id: io_image_Axis_A_Front_r
            anchors.centerIn: parent;
            source: "images/icon_axis_free_4.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_A_Front_r.source="images/icon_axis_free_4.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_A_Front_r.source="images/icon_axis_limited_4.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_A_Front_r; source: "images/icon_axis_limited_4.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_A_Front_r; source: "images/icon_axis_free_4.png"}
            }
        ]
    }


    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_A_Back_r"
        id: io_picCondition_Axis_A_Back_r
        x:1092
        y:445
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp7l
        Image {
            id: io_image_Axis_A_Back_r
            anchors.centerIn: parent;
            source: "images/icon_axis_free_3.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_A_Back_r.source="images/icon_axis_free_3.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_A_Back_r.source="images/icon_axis_limited_3.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_A_Back_r; source: "images/icon_axis_limited_3.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_A_Back_r; source: "images/icon_axis_free_3.png"}
            }
        ]
    }

    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_B_Front_r"
        id: io_picCondition_Axis_B_Front_r
        x:1288
        y:489
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp2r
        Image {
            id: io_image_Axis_B_Front_r
            anchors.centerIn: parent;
            source: "images/icon_axis_free_2.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
               states.name="Normal"
            }
            else if(sta === 1)
            {
                 states.name="Warning"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_B_Front_r; source: "images/icon_axis_limited_2.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_B_Front_r; source: "images/icon_axis_free_2.png"}
            }
        ]
    }


    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_B_Back_r"
        id: io_picCondition_Axis_B_Back_r
        x:1325
        y:525
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp2r
        Image {
            id: io_image_Axis_B_Back_r
            anchors.centerIn: parent;
            source: "images/icon_axis_free_1.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
               states.name="Normal"
            }
            else if(sta === 1)
            {
                 states.name="Warning"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_B_Back_r; source: "images/icon_axis_limited_1.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_B_Back_r; source: "images/icon_axis_free_1.png"}
            }
        ]
    }

    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_C_r"
        id: io_picCondition_Axis_C_r
        width: 100
        height: 100
        x:1099
        y:600
        color: "transparent"
        state: signalLamp3r
        Image {
            id: io_image_Axis_C_r
            anchors.centerIn: parent;
            source: "images/icon_axis_free_7.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
               states.name="Normal"
            }
            else if(sta === 1)
            {
                 states.name="Warning"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_C_r; source: "images/icon_axis_limited_7.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_C_r; source: "images/icon_axis_free_7.png"}
            }
        ]
    }


    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_D_r"
        id: io_picCondition_Axis_D_r
        x:1212
        y:495
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp4r
        Image {
            id: io_image_Axis_D_r
            anchors.centerIn: parent;
            source: "images/icon_axis_free_8.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
               states.name="Normal"
            }
            else if(sta === 1)
            {
                 states.name="Warning"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_D_r; source: "images/icon_axis_limited_8.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_D_r; source: "images/icon_axis_free_8.png"}
            }
        ]
    }


    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_E_Front_r"
        id: io_picCondition_Axis_E_Front_r
        x:1443
        y:657
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp5r
        Image {
            id: io_image_Axis_E_Front_r
            anchors.centerIn: parent;
            source: "images/icon_axis_free_13.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_E_Front_r.source="images/icon_axis_free_13.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_E_Front_r.source="images/icon_axis_limited_13.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_E_Front_r; source: "images/icon_axis_limited_13.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_E_Front_r; source: "images/icon_axis_free_13.png"}
            }
        ]
    }


    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_E_Back_r"
        id: io_picCondition_Axis_E_Back_r
        x:1570
        y:528
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp5r
        Image {
            id: io_image_Axis_E_Back_r
            anchors.centerIn: parent;
            source: "images/icon_axis_free_14.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_E_Back_r.source="images/icon_axis_free_14.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_E_Back_r.source="images/icon_axis_limited_14.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_E_Back_r; source: "images/icon_axis_limited_14.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_E_Back_r; source: "images/icon_axis_free_14.png"}
            }
        ]
    }


    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_F_Front_r"
        id: io_picCondition_Axis_F_Front_r
        x:1410
        y:585
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp6l
        Image {
            id: io_image_Axis_F_Front_r
            anchors.centerIn: parent;
            source: "images/icon_axis_free_11.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_F_Front_r.source="images/icon_axis_free_11.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_F_Front_r.source="images/icon_axis_limited_11.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_F_Front_r; source: "images/icon_axis_limited_11.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_F_Front_r; source: "images/icon_axis_free_11.png"}
            }
        ]
    }

    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_F_Back_r"
        id: io_picCondition_Axis_F_Back_r
        x:1635
        y:585
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp6l
        Image {
            id: io_image_Axis_F_Back_r
            anchors.centerIn: parent;
            source: "images/icon_axis_free_12.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_F_Back_r.source="images/icon_axis_free_12.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_F_Back_r.source="images/icon_axis_limited_12.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_F_Back_r; source: "images/icon_axis_limited_12.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_F_Back_r; source: "images/icon_axis_free_12.png"}
            }
        ]
    }


    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_G_Front_r"
        id: io_picCondition_Axis_G_Front_r
        x:1515
        y:685
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp7r
        Image {
            id: io_image_Axis_G_Front_r
            anchors.centerIn: parent;
            source: "images/icon_axis_free_10.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_G_Front_r.source="images/icon_axis_free_10.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_G_Front_r.source="images/icon_axis_limited_10.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_G_Front_r; source: "images/icon_axis_limited_10.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_G_Front_r; source: "images/icon_axis_free_10.png"}
            }
        ]
    }

    Rectangle{
        //@disable-check M16
        objectName: "io_picCondition_Axis_G_Back_r"
        id: io_picCondition_Axis_G_Back_r
        x:1515
        y:466
        width: 100
        height: 100
        color: "transparent"
        state: signalLamp7r
        Image {
            id: io_image_Axis_G_Back_r
            anchors.centerIn: parent;
            source: "images/icon_axis_free_9.png"
        }
        function setsta(sta)
        {
            if(sta === 0)
            {
                io_image_Axis_G_Back_r.source="images/icon_axis_free_9.png"
            }
            else if(sta === 1)
            {
                io_image_Axis_G_Back_r.source="images/icon_axis_limited_9.png"
            }
        }
        states: [
            State {
                name: "Warning"
                PropertyChanges { target: io_image_Axis_G_Back_r; source: "images/icon_axis_limited_9.png"}
            },
            State {
                name: "Normal"
                PropertyChanges { target: io_image_Axis_G_Back_r; source: "images/icon_axis_free_9.png"}
            }
        ]
    }
    //锁状态修改：lock.state == "Lock/Unlock"
     //设置手术类型信息  type为"positive"正向；"opposite"逆向
        Rectangle{
            id: io_btnType1
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
            id: io_picConditionF_r
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

//        Text {
//            x: 180
//            y: 200
//            id: lblSpeedCount
//            font.pixelSize: 44
//            font.bold: true
//            font.family: "Microsoft YaHei UI"
//            font.letterSpacing: 20
//            text: qsTr("缩放比例")
//            color: "#18FEFE"
//        }
        //Level按钮
//        Rectangle{
//            id: io_level1
//            width: 100
//            height: 100
//            color: "transparent"
//            state: "selected"
//            x: 518
//            y: 172
//            visible: false
//            Image {
//                id: io_levelBg1
//                anchors.centerIn: parent;
//                source: "images/btn_level_hover.png"
//            }
//            Text {
//                id: io_levelVal1
//                y: 25
//                font.pixelSize: 30
//                font.bold: true
//                width: parent.width
//                height: parent.height
//                verticalAlignment: Text.AlignTop
//                horizontalAlignment: Text.AlignHCenter
//                text: "1/60"
//                color: "#050C0D"
//            }
//            MouseArea{
//                anchors.left: parent.left
//                anchors.right: parent.right
//                anchors.top: parent.top
//                anchors.bottom: parent.bottom
//                acceptedButtons: Qt.LeftButton;
////                onClicked: {
////                    robotMain.initBtn();
////                    robotMain.setSpeed(currentAxis,currentLevel);
////                }
//            }
//            states: [
//                State {
//                    name: "selected"
//                    PropertyChanges { target: io_levelBg1; source: "images/btn_level_hover.png"}
//                    PropertyChanges { target: io_levelVal1; color: "#050C0D"}
//                },
//                State {
//                    name: "unselected"
//                    PropertyChanges { target: io_levelBg1;source: "images/btn_level.png"}
//                    PropertyChanges { target: io_levelVal1; color: "#48FFFF"}
//                }
//            ]
//        }
//        Rectangle{
//            id: io_level2
//            width: 100
//            height: 100
//            color: "transparent"
//            anchors.verticalCenter: io_level1.verticalCenter
//            anchors.top: io_level1.top
//            anchors.bottom: io_level1.bottom
//            state: "unselected"
//            x: 670
//            y: 172
//            Image {
//                id: io_levelBg2
//                anchors.centerIn: parent;
//                source: "images/btn_level.png"
//            }
//            Text {
//                id: io_levelVal2
//                y: 25
//                font.pixelSize: 30
//                font.bold: true
//                width: parent.width
//                height: parent.height
//                verticalAlignment: Text.AlignTop
//                horizontalAlignment: Text.AlignHCenter
//                text: "1/40"
//                color: "#48FFFF"
//                }
//            MouseArea{
//                anchors.fill: parent;
//                acceptedButtons: Qt.LeftButton;
////                onClicked: {
////                    currentLevel = "level2";
////                    robotMain.setSpeed(currentAxis,currentLevel);
////                    level2.state = "selected"
////                    level1.state = "unselected"
////                    level3.state = "unselected"
////                    level4.state = "unselected"
////                    level5.state = "unselected"
////                }
//            }
//            states: [
//                State {
//                    name: "selected"
//                    PropertyChanges { target: io_levelBg2; source: "images/btn_level_hover.png"}
//                    PropertyChanges { target: io_levelVal2; color: "#050C0D"}
//                },
//                State {
//                    name: "unselected"
//                    PropertyChanges { target: io_levelBg2;source: "images/btn_level.png"}
//                    PropertyChanges { target: io_levelVal2; color: "#48FFFF"}
//                }
//            ]
//        }
        Rectangle{
            //@disable-check M16
            objectName: "io_level1"
            id: io_level1
            width: 100
            height: 100
            color: "transparent"
            state: "unselected"
            x: 750
            y: 145
            Image {
                id: io_levelBg1
                anchors.centerIn: parent;
                source: "images/btn_level.png"
            }
            Text {
                id: io_levelVal1
                y: 25
                font.pixelSize: 30
                font.bold: true
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                text: "12 X"
                color: "#48FFFF"
            }
            MouseArea{
                anchors.fill: parent;
                acceptedButtons: Qt.LeftButton;
            }
            function setsta(sta)
            {
                if(sta === 0)
                {
                   io_levelBg1.source="images/btn_level.png"
                   io_levelVal1.color="#48FFFF"
                }
                else if(sta === 1)
                {
                    io_levelBg1.source="images/btn_level_hover.png"
                    io_levelVal1.color="#050C0D"
                }
            }
        }
        Rectangle{
            //@disable-check M16
            objectName: "io_level2"
            id: io_level2
            width: 100
            height: 100
            color: "transparent"
            anchors.verticalCenter: io_level1.verticalCenter
            anchors.top: io_level1.top
            anchors.bottom: io_level1.bottom
            state: "unselected"
            x: 905
            y: 145
            Image {
                id: io_levelBg2
                anchors.centerIn: parent;
                source: "images/btn_level.png"
            }
            Text {
                id: io_levelVal2
                y: 25
                font.pixelSize: 30
                font.bold: true
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                text: "10 X"
                color: "#48FFFF"
            }
            MouseArea{
                anchors.fill: parent;
                acceptedButtons: Qt.LeftButton;
            }
            function setsta(sta)
            {
                if(sta === 0)
                {
                   io_levelBg2.source="images/btn_level.png"
                   io_levelVal2.color="#48FFFF"
                }
                else if(sta === 1)
                {
                    io_levelBg2.source="images/btn_level_hover.png"
                    io_levelVal2.color="#050C0D"
                }
            }
        }
        Rectangle{
            //@disable-check M16
            objectName: "io_level3"
            id: io_level3
            width: 100
            height: 100
            color: "transparent"
            state: "unselected"
            x: 1060
            y: 145
            Image {
                id: io_levelBg3
                anchors.centerIn: parent;
                source: "images/btn_level.png"
            }
            Text {
                id: io_levelVal3
                y: 25
                font.pixelSize: 30
                font.bold: true
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                text: "7 X"
                color: "#48FFFF"
            }
            MouseArea{
                anchors.fill: parent;
                acceptedButtons: Qt.LeftButton;
            }
            function setsta(sta)
            {
                if(sta === 0)
                {
                   io_levelBg3.source="images/btn_level.png"
                   io_levelVal3.color="#48FFFF"
                }
                else if(sta === 1)
                {
                    io_levelBg3.source="images/btn_level_hover.png"
                    io_levelVal3.color="#050C0D"
                }
            }
        }

        Rectangle{
            //@disable-check M16
            objectName: "io_level4"
            id: io_level4
            width: 100
            height: 100
            color: "transparent"
            state: "unselected"
            x: 1215
            y: 145
            Image {
                id: io_levelBg4
                anchors.centerIn: parent;
                source: "images/btn_level.png"
            }
            Text {
                id: io_levelVal4
                y: 25
                font.pixelSize: 30
                font.bold: true
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                text: "5 X"
                color: "#48FFFF"
            }
            MouseArea{
                anchors.fill: parent;
                acceptedButtons: Qt.LeftButton;
            }
            function setsta(sta)
            {
                if(sta === 0)
                {
                   io_levelBg4.source="images/btn_level.png"
                   io_levelVal4.color="#48FFFF"
                }
                else if(sta === 1)
                {
                    io_levelBg4.source="images/btn_level_hover.png"
                    io_levelVal4.color="#050C0D"
                }
            }
        }


        // @disable-check M300
//       Connections
//       {
//           target: uiInterface
//           onLevelChanged:{
//           // console.log("diu ni da ba")
//               switch(uiInterface.getlevel()){
//                               case 1:
//                                   console.log("level1")
//                                   currentLevel = "level1";
//                                   io_level1.state = "selected"
//                                   io_level2.state = "unselected"
//                                   io_level3.state = "unselected"
//                                   io_level4.state = "unselected"
//                                   io_level5.state = "unselected"
//                                   break;
//                               case 2:
//                                    console.log("level 2 ")
//                                   currentLevel = "level2";
//                                   io_level2.state = "selected"
//                                   io_level1.state = "unselected"
//                                   io_level3.state = "unselected"
//                                   io_level4.state = "unselected"
//                                   io_level5.state = "unselected"
//                                   break;
//                               case 3:
//                                   currentLevel = "level3";
//                                   io_level3.state = "selected"
//                                   io_level1.state = "unselected"
//                                   io_level2.state = "unselected"
//                                   io_level4.state = "unselected"
//                                   io_level5.state = "unselected"
//                                   break;
//                               case 4:
//                                   console.log("level 4 ")
//                                   currentLevel = "level4";
//                                   io_level4.state = "selected"
//                                   io_level1.state = "unselected"
//                                   io_level2.state = "unselected"
//                                   io_level3.state = "unselected"
//                                   io_level5.state = "unselected"
//                                   break;
//                               case 5:
//                                   currentLevel = "level5";
//                                   io_level5.state = "selected"
//                                   io_level1.state = "unselected"
//                                   io_level2.state = "unselected"
//                                   io_level3.state = "unselected"
//                                   io_level4.state = "unselected"
//                                   break;
//                               default:
//                                   console.log("无效的速度设置")
//                               }
//           }
//       }

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

       Text {
                 id: io_txtEnable_L
                 //@disable-check M16
                 objectName: "io_txtEnable_L"
                 x: 455
                 y: 378
                 color: "#18fefe"
                 text: qsTr("左侧未使能")
//                     anchors.left: io_operationName.left
//                     anchors.right: io_speedName1.right
                // font.letterSpacing: 20
                 font.pixelSize: 28
                 font.family: "Microsoft YaHei UI"
                 font.bold: true

                 function setsta(sta)
                 {
                     if(sta===true)
                     {
                         io_txtEnable_L.text=qsTr("左侧使能")
                         io_txtEnable_L.color="#FFFFFF"
                     }
                     else
                     {
                         io_txtEnable_L.text=qsTr("左侧未使能")
                         io_txtEnable_L.color="#48FFFF"
                     }
                 }
            }
//         //测试按钮1->kinematics
//          Button {
//               // @disable-check M16
//              objectName: "io_btnEnable_l"
//              id: io_btnEnable_l
//              y: 773
//              width: 179
//              height: 56
//              x: 81
//              font.pixelSize: 28
//              anchors.horizontalCenter: io_btnUsedTime_l.horizontalCenter // 设置字体大小
//              // 设置按钮文本
//              contentItem: Text {
//                  id: io_txtEnable_l
//                  x: 7
//                  y: 6
//                  text: qsTr("左侧未使能")
//                  font.pixelSize: 28
//                  opacity: 1.0
//                  color: "#48FFFF"
//                  horizontalAlignment: Text.AlignHCenter
//                  verticalAlignment: Text.AlignVCenter
//                  elide: Text.ElideRight
//              }
//             signal triggerKinematics(int num)
//              // 设置按钮背景
//              background: Rectangle {
//                  id: io_btnEnable_l_background
//                  implicitWidth: 200
//                  implicitHeight: 56
//                  opacity: 0.25
//                  color: "#003033"
//                  border.color: "#2CDFE8"
//                  border.width: 2
//              }
//              function setsta(sta)
//              {
//                  if(sta===true)
//                  {
//                      io_btnEnable_l.enabled=true
//                      io_txtEnable_l.text=qsTr("左侧使能")
//                      io_txtEnable_l.color="#FFFFFF"
//                      io_btnEnable_l_background.color="#48FFFF"
//                      io_btnEnable_l_background.border.color="#2CDFE8"
//                  }
//                  else
//                  {
//                      io_btnEnable_l.enabled=true
//                      io_txtEnable_l.text=qsTr("左侧未使能")
//                      io_txtEnable_l.color="#48FFFF"
//                      io_btnEnable_l_background.color="#003033"
//                      io_btnEnable_l_background.border.color="#2CDFE8"
//                  }
//              }
//              onClicked: {
//                  triggerKinematics(2);
//              }
//          }

       Text {
                 id: io_txtEnable_R
                 //@disable-check M16
                 objectName: "io_txtEnable_R"
                 x: 1320
                 y: 378
                 color: "#18fefe"
                 text: qsTr("右侧未使能")
//                     anchors.left: io_operationName.left
//                     anchors.right: io_speedName1.right
                // font.letterSpacing: 20
                 font.pixelSize: 28
                 font.family: "Microsoft YaHei UI"
                 font.bold: true

                 function setsta(sta)
                 {
                     if(sta===true)
                     {
                         io_txtEnable_R.text=qsTr("右侧使能")
                         io_txtEnable_R.color="#FFFFFF"
                     }
                     else
                     {
                         io_txtEnable_R.text=qsTr("右侧未使能")
                         io_txtEnable_R.color="#48FFFF"
                     }
                 }
            }
//          //测试按钮1->kinematics
//           Button {
//                // @disable-check M16
//               objectName: "io_btnEnable_r"
//               id: io_btnEnable_r
//               y: 773
//               width: 172
//               height: 56
//               anchors.verticalCenter: io_btnEnable_l.verticalCenter
//               x: 1026
//               font.pixelSize: 28
//               anchors.horizontalCenterOffset: 0
//               anchors.horizontalCenter: io_blUsedtime_r.horizontalCenter // 设置字体大小
//               // 设置按钮文本
//               contentItem: Text {
//                   id: io_txtEnable_r
//                   x: 8
//                   y: 6
//                   text: qsTr("右侧未使能")
//                   font.pixelSize: 28
//                   opacity: 1.0
//                   color: "#48FFFF"
//                   horizontalAlignment: Text.AlignHCenter
//                   verticalAlignment: Text.AlignVCenter
//                   elide: Text.ElideRight
//               }
//              signal triggerKinematics(int num)
//               // 设置按钮背景
//               background: Rectangle {
//                   id:io_btnEnable_r_background
//                   implicitWidth: 200
//                   implicitHeight: 56
//                   opacity: 0.25
//                   color: "#003033"
//                   border.color: "#2CDFE8"
//                   border.width: 2
//               }
//               function setsta(sta)
//               {
//                   if(sta===true)
//                   {
//                       io_btnEnable_r.enabled=true
//                       io_txtEnable_r.text=qsTr("右侧使能")
//                       io_txtEnable_r.color="#FFFFFF"
//                       io_btnEnable_r_background.color="#48FFFF"
//                       io_btnEnable_r_background.border.color="#2CDFE8"
//                   }
//                   else
//                   {
//                       io_btnEnable_r.enabled=true
//                       io_txtEnable_r.text=qsTr("右侧未使能")
//                       io_txtEnable_r.color="#48FFFF"
//                       io_btnEnable_r_background.color="#003033"
//                       io_btnEnable_r_background.border.color="#2CDFE8"
//                   }
//               }
//               onClicked: {
//                   triggerKinematics(3);
//               }
//           }

           Text {
                     id: io_txtInstrumentType_L
                     //@disable-check M16
                     objectName: "io_txtInstrumentType_L"
                     x: 380
                     y: 838
                     color: "#18fefe"
                     text: qsTr("持针镊")
//                     anchors.left: io_operationName.left
//                     anchors.right: io_speedName1.right
                    // font.letterSpacing: 20
                     font.pixelSize: 28
                     font.family: "Microsoft YaHei UI"
                     font.bold: true
           }
           //测试按钮1->kinematics
//           Button {
//                // @disable-check M16
//               objectName: "btnInstrument_l"
//               id: btnInstrument_l
//               y: 465
//               width: 172
//               height: 56
//               anchors.right: btnUsedTime_l.right
//               x: 84
//               font.pixelSize: 28
//               anchors.horizontalCenter: btnUsedTime_l.horizontalCenter // 设置字体大小
//               // 设置按钮文本
//               contentItem: Text {
//                   id: txtInstrument_l
//                   x: 8
//                   y: 6
//                   text: qsTr("持针镊")
//                   font.pixelSize: 28
//                   opacity: 1.0
//                   color: "#48FFFF"
//                   horizontalAlignment: Text.AlignHCenter
//                   verticalAlignment: Text.AlignVCenter
//                   elide: Text.ElideRight
//               }
//              signal triggerKinematics(int num)
//               // 设置按钮背景
//               background: Rectangle {
//                   implicitWidth: 200
//                   implicitHeight: 56
//                   opacity: 0.25
//                   color: "#003033"
//                   border.color: "#2CDFE8"
//                   border.width: 2
//               }
//               onClicked: {
//                   triggerKinematics(3);
//               }
//           }
           Text {
                     id: io_txtUsedTime_L
                     //@disable-check M16
                     objectName: "io_txtUsedTime_L"
                     x: 740
                     y: 845
                     color: "#18fefe"
                     text: qsTr("20.4h")
//                     anchors.left: io_operationName.left
//                     anchors.right: io_speedName1.right
                    // font.letterSpacing: 20
                     font.pixelSize: 28
                     font.family: "Microsoft YaHei UI"
                     font.bold: true
           }
//           Button {
//               // @disable-check M16
//              objectName: "io_btnUsedTime_l"
//              id: io_btnUsedTime_l
//              y: 614
//              width: 172
//              height: 56
//              anchors.top: io_btnUsedTime_r.top
//              anchors.bottom: io_btnType1.bottom
//              x: 84
//              font.pixelSize: 28 // 设置字体大小
//              // 设置按钮文本
//              contentItem: Text {
//                  id: io_txtUsedTime_l
//                  x: 8
//                  y: 6
//                  text: qsTr("20.4h")
//                  font.pixelSize: 28
//                  opacity: 1.0
//                  color: "#48FFFF"
//                  horizontalAlignment: Text.AlignHCenter
//                  verticalAlignment: Text.AlignVCenter
//                  elide: Text.ElideRight
//              }
//             signal triggerKinematics(int num)
//              // 设置按钮背景
//              background: Rectangle {
//                  implicitWidth: 200
//                  implicitHeight: 56
//                  opacity: 0.25
//                  color: "#003033"
//                  border.color: "#2CDFE8"
//                  border.width: 2
//              }
//              onClicked: {
//                  triggerKinematics(3);
//              }
//          }
           Text {
                     id: io_txtInstrumentType_R
                     //@disable-check M16
                     objectName: "io_txtInstrumentType_R"
                     x: 1250
                     y: 838
                     color: "#18fefe"
                     text: qsTr("显微剪")
//                     anchors.left: io_operationName.left
//                     anchors.right: io_speedName1.right
                    // font.letterSpacing: 20
                     font.pixelSize: 28
                     font.family: "Microsoft YaHei UI"
                     font.bold: true
           }

//           //测试按钮1->kinematics
//           Button {
//                // @disable-check M16
//               objectName: "io_btnInstrument_r"
//               id: io_btnInstrument_r
//               y: 465
//               width: 172
//               height: 56
//               anchors.verticalCenter: io_btnInstrument_l.verticalCenter
//               x: 1026
//               font.pixelSize: 28
//               anchors.horizontalCenter: io_btnUsedTime_r.horizontalCenter // 设置字体大小
//               // 设置按钮文本
//               contentItem: Text {
//                   id: io_txtInstrument_r
//                   x: 8
//                   text: qsTr("显微剪")
//                   font.pixelSize: 28
//                   opacity: 1.0
//                   color: "#48FFFF"
//                   horizontalAlignment: Text.AlignHCenter
//                   verticalAlignment: Text.AlignVCenter
//                   elide: Text.ElideRight
//               }
//              signal triggerKinematics(int num)
//               // 设置按钮背景
//               background: Rectangle {
//                   implicitWidth: 200
//                   implicitHeight: 56
//                   opacity: 0.25
//                   color: "#003033"
//                   border.color: "#2CDFE8"
//                   border.width: 2
//               }
//               onClicked: {
//                   triggerKinematics(3);
//               }
//           }


           Text {
                     id: io_txtUsedTime_R
                     //@disable-check M16
                     objectName: "io_txtUsedTime_R"
                     x: 1600
                     y: 845
                     color: "#18fefe"
                     text: qsTr("0.4h")
//                     anchors.left: io_operationName.left
//                     anchors.right: io_speedName1.right
                    // font.letterSpacing: 20
                     font.pixelSize: 28
                     font.family: "Microsoft YaHei UI"
                     font.bold: true
           }
//           Button {
//               // @disable-check M16
//              objectName: "io_btnUsedTime_r"
//              id: io_btnUsedTime_r
//              y: 614
//              width: 172
//              height: 56
//              anchors.verticalCenter: io_btnUsedTime_l.verticalCenter
//              x: 1026
//              font.pixelSize: 28 // 设置字体大小
//              // 设置按钮文本
//              contentItem: Text {
//                  id: io_txtUsedTime_r
//                  x: 8
//                  y: 6
//                  text: qsTr("0.4h")
//                  font.pixelSize: 28
//                  opacity: 1.0
//                  color: "#48FFFF"
//                  horizontalAlignment: Text.AlignHCenter
//                  verticalAlignment: Text.AlignVCenter
//                  elide: Text.ElideRight
//              }
//             signal triggerKinematics(int num)
//              // 设置按钮背景
//              background: Rectangle {
//                  implicitWidth: 200
//                  implicitHeight: 56
//                  opacity: 0.25
//                  color: "#003033"
//                  border.color: "#2CDFE8"
//                  border.width: 2
//              }
//              onClicked: {
//                  triggerKinematics(3);
//              }
//           }



//          Text {
//              id: lblEnable_l
//              x: 90
//              y: 736
//              color: "#18fefe"
//              text: qsTr("使能状态")
//              font.letterSpacing: 16
//              font.pixelSize: 24
//              horizontalAlignment: Text.AlignLeft
//              clip: false
//              font.family: "Microsoft YaHei UI"
//              font.bold: true
//          }

//          Text {
//              id: lblInstrument_r
//              x: 1034
//              y: 418
//              color: "#18fefe"
//              text: qsTr("器械类型")
//              font.letterSpacing: 16
//              font.pixelSize: 24
//              font.family: "Microsoft YaHei UI"
//              font.bold: true
//          }
//          Text {
//              id: lblInstrument_l
//              x: 90
//              y: 418
//              color: "#18fefe"
//              text: qsTr("器械类型")
//              anchors.verticalCenter: lblInstrument_r.verticalCenter
//              font.letterSpacing: 16
//              font.pixelSize: 24
//              horizontalAlignment: Text.AlignLeft
//              clip: false
//              font.family: "Microsoft YaHei UI"
//              font.bold: true
//          }
//          Text {
//              id: lblEnable_r
//              x: 1034
//              y: 736
//              color: "#18fefe"
//              text: qsTr("使能状态")
//              font.letterSpacing: 16
//              font.pixelSize: 24
//              font.family: "Microsoft YaHei UI"
//              font.bold: true
//          }

//          Text {
//              id: lblUsedTime_l
//              x: 90
//              y: 577
//              width: 108
//              height: 31
//              color: "#18fefe"
//              text: qsTr("使用时长")
//              font.letterSpacing: 16
//              font.pixelSize: 24
//              horizontalAlignment: Text.AlignLeft
//              clip: false
//              font.family: "Microsoft YaHei UI"
//              font.bold: true
//          }

//          Text {
//              id: lblUsedtime_r
//              x: 1034
//              y: 577
//              color: "#18fefe"
//              text: qsTr("使用时长")
//              font.letterSpacing: 16
//              font.pixelSize: 24
//              font.family: "Microsoft YaHei UI"
//              font.bold: true
//          }

          Button {
            //@disable-check M16
            objectName: "io_btnReturnPreOpreation"
            id: io_btnReturnPreOpreation
            x: 1550
            y: 950
            width: 300
            height: 72
            font.pixelSize: 36
            contentItem: Text {
                id:io_txt_btnReturnPreOpreation
                width: 100
                text: qsTr("返回术前准备")
                font.pixelSize: 36
                opacity: 1.0
                color: "#48FFFF"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            // 设置按钮背景
            background: Rectangle {
                id:io_btnReturnPreOpreation_background
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
                  io_txt_btnReturnPreOpreation.color="#FFFFFF"
                  io_btnReturnPreOpreation_background.color="#48FFFF"
            }
            onReleased:
            {
                io_txt_btnReturnPreOpreation.color="#48FFFF"
                io_btnReturnPreOpreation_background.color="#003033"
            }
            onClicked: {
                io_sureBackToOperation.open()

            }
            //确认返回术前准备
            Popup{
                id: io_sureBackToOperation
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
                    id: io_sureBackToOperationBoard
                    x:-50
                    y:50
                    width: 745
                    height: 220
                    color: "#003033"
                    border.color: "#18fefe"
                    opacity: 0.8
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        text: qsTr("确认返回术前准备页面？") ;
                        color: itemColor
                        font.pointSize: 40
                        anchors.topMargin: 40  // 调整顶部的间距
                    }
                }

                 //确定按钮
                Button {
                    id: io_btnSureBackToOperationOk
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
                    background: Rectangle {
                        implicitWidth: 120
                        implicitHeight: 56
                        opacity: 0.25
                        color: "#003033"
                        border.color: "#2CDFE8"
                        border.width: 2
                    }
                     onClicked: {
                        io_sureBackToOperation.close()
                        UIinterface.setRobotControlMode(1)
                        inOpreationPage.visible = false
                        preOpreationPage.visible = true
                    }
                }
                 //删除取消按钮
                 Button {
                     id: io_btnsureBackToOperationCancel
                     x: 345
                     y: 175
                     // 设置按钮文本
                     contentItem: Text {
                         id: io_btnsureBackToOperationCancelText
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
                         io_sureBackToOperation.close()
                     }
                 }
              }
        }
}

/*##^##
Designer {
    D{i:0;autoSize:true;formeditorZoom:0.66;height:480;width:640}
}
##^##*/
