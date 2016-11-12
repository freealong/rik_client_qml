import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import RIK_Client 1.0

Item {
    id: connect_wgt

    Rectangle {
        id: connect_page
        height: parent.height
        width: parent.width

        GridLayout {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            columns: 2
            columnSpacing: 3
            rowSpacing: 5

            Text {
                text: "IP address:"
            }
            Text {
                text: "Port:"
            }
            TextField {
                id: ip
                text: "127.0.0.1"

            }
            TextField {
                id: port
                text: "8888"
            }
            Button {
                id: connect_btn
                text: "connect"
                background: Rectangle {
                    border.color: "#888"
                }
                onClicked: if (Client.connect_server(ip.text, port.text) === 0) {
                               server_status.text = "connected"
                           }
            }
            Button {
                id: disconnect_btn
                text: "disconnect"
                background: Rectangle {
                    border.color: "#666"
                }
                onClicked: if (Client.disconnect_server() === 0) {
                               server_status.text = "disconnected"
                           }
            }

            Text {
                text: "server status:"
            }
            Text {
                id: server_status
                text: "disconnected"
            }
            Text {
                text: "robot status:"
            }
            Text {
                id: robot_status
                text: "not started"
            }
            Text {
                text: "motor status:"
            }
            Text {
                id: motor_status
                text: "not started"
            }

            Text {
                text: "Robot"
            }
            Switch {
                id: robot_switch
                checked: false
                onCheckedChanged: if (Client.switch_robot(robot_switch.checked) === 0) {
                                      robot_status.text = robot_switch.checked ? "started" : "not started"
                                  }
            }
            Text {
                text: "Motor"
            }
            Switch {
                id: motor_switch
                checked: false
                onCheckedChanged: if (Client.switch_motor(motor_switch.checked) === 0) {
                                      motor_status.text = robot_switch.checked ? "started" : "not started"
                                  }
            }

        } // GridLayout

        Rectangle {
            id: bottom_rect
            height: 50
            width: parent.width
            anchors.bottom: parent.bottom
            Rectangle {
                height: parent.height
                width: 80
                anchors.horizontalCenter: parent.horizontalCenter
                color: "lightyellow"
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
//                        if (connect_wgt.anchors.bottomMargin === -height) {
//                            connect_wgt.anchors.bottomMargin = -connect_wgt.height
//                            connect_page.activeFocus
//                        }
//                        else
//                            connect_wgt.anchors.bottomMargin = -height
                    }
                }
            }
        }

    } // Rectangle
}
