import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.XmlListModel 2.0

Window {
    id: window
    visible: true
    visibility: "Maximized"
//    width: 1280
//    height: 1024
    width:Screen.desktopAvailableWidth
    height: Screen.desktopAvailableHeight
    flags: Qt.FramelessWindowHint
    color: "#000C0A"

    property bool isInitPage: true
    property string applicationDirPath

    InitPage{
        id: initPage
        visible: isInitPage
    }

    MainPage{
        id: mainPage
        visible: !isInitPage
    }
}
