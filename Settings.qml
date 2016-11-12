import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import RIK_Client 1.0

Item {
    Rectangle {
        id: settings_page
        height: parent.height
        width: parent.width

        GridLayout {
            id: joints_limit_gl
            anchors.top: parent.top
            anchors.topMargin: 50
            anchors.horizontalCenter: parent.horizontalCenter

            columns: 4

            Text {
                text: "Id"
            }
            Text {
                text: "min_angle"
            }
            Text {
                text: "max_angle"
            }
            Text {
                text: "max_speed"
            }

            Text {
                text: "J1"
            }
            TextField {
                id: j1_min_angle
            }
            TextField {
                id: j1_max_angle
            }
            TextField {
                id: j1_max_speed
            }

            Text {
                text: "J2"
            }
            TextField {
                id: j2_min_angle
            }
            TextField {
                id: j2_max_angle
            }
            TextField {
                id: j2_max_speed
            }

            Text {
                text: "J3"
            }
            TextField {
                id: j3_min_angle
            }
            TextField {
                id: j3_max_angle
            }
            TextField {
                id: j3_max_speed
            }

            Text {
                text: "J4"
            }
            TextField {
                id: j4_min_angle
            }
            TextField {
                id: j4_max_angle
            }
            TextField {
                id: j4_max_speed
            }

            Text {
                text: "J5"
            }
            TextField {
                id: j5_min_angle
            }
            TextField {
                id: j5_max_angle
            }
            TextField {
                id: j5_max_speed
            }

            Text {
                text: "J6"
            }
            TextField {
                id: j6_min_angle
            }
            TextField {
                id: j6_max_angle
            }
            TextField {
                id: j6_max_speed
            }
        }
        Button {
            id: joints_settings_btn
            anchors.top: joints_limit_gl.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            text: "send joints settings"
            background: Rectangle {
                border.color: "#888"
            }
            // @TODO: only set max speed now
            onClicked: Client.set_motors(j1_max_speed.text, j2_max_speed.text, j3_max_speed.text,
                                         j4_max_speed.text, j5_max_speed.text, j6_max_speed.text)
        }

    }
}
