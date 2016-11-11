import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.0

Window {
    visible: true
    width: Screen.width * 0.5
    height: Screen.height * 0.5
    title: qsTr("RIK Client")

    Rectangle {
        width: parent.width
        height: parent.height


        Task {
            height: parent.height -50
            width: parent.width
            anchors.bottom: parent.bottom
        }

        Connect {
            height: parent.height * 0.8
            width: parent.width
            anchors.bottom: parent.top
            anchors.bottomMargin: -50
        }

    }
}
