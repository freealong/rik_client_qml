import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4

import RIK_Client 1.0

Window {
    visible: true
    width: Screen.width * 0.5
    height: Screen.height * 0.8
    title: qsTr("RIK Client")

    Rectangle {
        id: global_rt
        width: parent.width
        height: 20

        Switch {
            anchors.right: parent.right
            checked: false
            onCheckedChanged: Client.switch_motor(this.checked)
        }
    }

//    Switch {
//        id: motor_sw
//        anchors.right: parent.width

//        checked: false
//        onCheckedChanged: if (Client.switch_motor(motor_switch.checked) === 0) {
//                              motor_status.text = robot_switch.checked ? "started" : "not started"
//                          }
//    }

    TabView {
        width: parent.width
        height: parent.height

        Tab {
            title: "connect"
            Connect {

            }
        }

        Tab {
            title: "Settings"
            Settings {

            }
        }

        Tab {
            title: "Mode"
            Mode {

            }
        }

        Tab {
            title: "Task"
            Task {

            }
        }
    }


//    Rectangle {
//        width: parent.width
//        height: parent.height


//        Task {
//            height: parent.height -50
//            width: parent.width
//            anchors.bottom: parent.bottom
//        }

//        Settings {
//            height: parent.height -50
//            width: parent.width
//            anchors.bottom: parent.bottom
//        }

//        Connect {
//            height: parent.height * 0.8
//            width: parent.width
//            anchors.bottom: parent.top
//            anchors.bottomMargin: -50
//        }

//    }
}
