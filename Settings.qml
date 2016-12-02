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
            anchors.topMargin: 30
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
                text: " "
            }
            Text {
                text: " "
            }
            Text {
                text: " "
            }
            ComboBox {
                id: speed_comb
                model: [ "low speed", "middle speed", "high speed"]
                onCurrentIndexChanged: {
                    if (speed_comb.currentIndex === 0) {
                        j1_max_speed.text = "200"
                        j2_max_speed.text = "200"
                        j3_max_speed.text = "200"
                        j4_max_speed.text = "200"
                        j5_max_speed.text = "200"
                        j6_max_speed.text = "200"
                    }
                    else if (speed_comb.currentIndex === 1) {
                        j1_max_speed.text = "1000"
                        j2_max_speed.text = "1000"
                        j3_max_speed.text = "1000"
                        j4_max_speed.text = "1000"
                        j5_max_speed.text = "1000"
                        j6_max_speed.text = "1000"
                    }
                    else if (speed_comb.currentIndex === 2) {
                        j1_max_speed.text = "60000"
                        j2_max_speed.text = "60000"
                        j3_max_speed.text = "60000"
                        j4_max_speed.text = "60000"
                        j5_max_speed.text = "60000"
                        j6_max_speed.text = "60000"
                    }
                }
            }

            Text {
                text: "J1"
            }
            TextField {
                id: j1_min_angle
                text: "-160"
            }
            TextField {
                id: j1_max_angle
                text: "160"
            }
            TextField {
                id: j1_max_speed
                text: "600"
            }

            Text {
                text: "J2"
            }
            TextField {
                id: j2_min_angle
                text: "-150"
            }
            TextField {
                id: j2_max_angle
                text: "-45"
            }
            TextField {
                id: j2_max_speed
                text: "600"
            }

            Text {
                text: "J3"
            }
            TextField {
                id: j3_min_angle
                text: "-135"
            }
            TextField {
                id: j3_max_angle
                text: "60"
            }
            TextField {
                id: j3_max_speed
                text: "600"
            }

            Text {
                text: "J4"
            }
            TextField {
                id: j4_min_angle
                text: "-180"
            }
            TextField {
                id: j4_max_angle
                text: "180"
            }
            TextField {
                id: j4_max_speed
                text: "600"
            }

            Text {
                text: "J5"
            }
            TextField {
                id: j5_min_angle
                text: "-180"
            }
            TextField {
                id: j5_max_angle
                text: "110"
            }
            TextField {
                id: j5_max_speed
                text: "600"
            }

            Text {
                text: "J6"
            }
            TextField {
                id: j6_min_angle
                text: "-180"
            }
            TextField {
                id: j6_max_angle
                text: "180"
            }
            TextField {
                id: j6_max_speed
                text: "600"
            }
        }
        Button {
            id: joints_settings_btn
            anchors.top: joints_limit_gl.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            text: "send joints settings"

            // @TODO: only set max speed now
            onClicked: Client.set_motors(j1_max_speed.text, j2_max_speed.text, j3_max_speed.text,
                                         j4_max_speed.text, j5_max_speed.text, j6_max_speed.text)
        }

    }
}
