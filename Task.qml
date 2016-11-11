import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import RIK_Client 1.0

Item {
    Rectangle {
        id: task_page
        height: parent.height
        width: parent.width

        Row {
            id: mode_row
            anchors.top: parent.top
            anchors.topMargin: 50
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10

            ComboBox {
                id: mode_comb
//                width: parent.width * 0.4
                model: [ "joints mode", "pose mode", "path mode"]
                onCurrentIndexChanged: {
                    if (mode_comb.currentIndex === 0) {
                        joints_mode_ly.visible = true;
                        pose_mode_ly.visible = false;
                        path_mode_ly.visible = false;
                    }
                    else if (mode_comb.currentIndex === 1) {
                        pose_mode_ly.visible = true;
                        joints_mode_ly.visible = false;
                        path_mode_ly.visible = false;
                    }
                    else if (mode_comb.currentIndex === 2) {
                        pose_mode_ly.visible = false;
                        joints_mode_ly.visible = false;
                        path_mode_ly.visible = true;
                    }

                }
            }
            Button {
                text: "set mode"
                background: Rectangle {
                    border.color: "#888"
                }
                onClicked: Client.send_mode(mode_comb.currentIndex)
            }
        }

        GridLayout {
            id: show_ly
            anchors.top: mode_row.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter

            columns: 2

            Text {
                text: "joints:"
            }
            Text {
                text: "pose:"
            }

            Column {
                id: current_joints_cln
                Text {
                    text: "0"
                }
                Text {
                    text: "0"
                }
                Text {
                    text: "0"
                }
                Text {
                    text: "0"
                }
                Text {
                    text: "0"
                }
                Text {
                    text: "0"
                }
            }

            Column {
                id: current_pose_cln
                Text {
                    text: "0"
                }
                Text {
                    text: "0"
                }
                Text {
                    text: "0"
                }
                Text {
                    text: "0"
                }
                Text {
                    text: "0"
                }
                Text {
                    text: "0"
                }
            }

            Button {
                text: "show current joints"
                background: Rectangle {
                    border.color: "#888"
                }
                onClicked: Client.get_joints()

            }
            Button {
                text: "show current pose"
                background: Rectangle {
                    border.color: "#888"
                }
                onClicked: Client.get_pose()
            }
        } // show joints and pose

        Rectangle {
            id: joints_mode_ly
            anchors.top: show_ly.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            visible: false

            GridLayout {
                id: joints_grid
                anchors.horizontalCenter: parent.horizontalCenter
                columns: 2
                Text {
                    text: "joints1:"
                }
                TextField {
                    text: ""
                }
                Text {
                    text: "joints2:"
                }
                TextField {
                    text: ""
                }
                Text {
                    text: "joints3:"
                }
                TextField {
                    text: ""
                }
                Text {
                    text: "joints4:"
                }
                TextField {
                    text: ""
                }
                Text {
                    text: "joints5:"
                }
                TextField {
                    text: ""
                }
                Text {
                    text: "joints6:"
                }
                TextField {
                    text: ""
                }
            }
            Button {
                anchors.top: joints_grid.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                text: "send target joints"
                background: Rectangle {
                    border.color: "#888"
                }
                onClicked: Client.send_target_joints(joints_grid.children[1].text, joints_grid.children[3].text,
                                                     joints_grid.children[5].text, joints_grid.children[7].text,
                                                     joints_grid.children[9].text, joints_grid.children[11].text)
            }
        } // joints

        Rectangle {
            id: pose_mode_ly
            anchors.top: show_ly.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            visible: false


            GridLayout {
                id: pose_grid
                anchors.horizontalCenter: parent.horizontalCenter
                columns: 2
                Text {
                    text: "x:"
                }
                TextField {
                    text: ""
                }
                Text {
                    text: "y:"
                }
                TextField {
                    text: ""
                }
                Text {
                    text: "z:"
                }
                TextField {
                    text: ""
                }
                Text {
                    text: "a:"
                }
                TextField {
                    text: ""
                }
                Text {
                    text: "b:"
                }
                TextField {
                    text: ""
                }
                Text {
                    text: "c:"
                }
                TextField {
                    text: ""
                }
            }
            Button {
                anchors.top: pose_grid.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                text: "send target pose"
                background: Rectangle {
                    border.color: "#888"
                }
                onClicked: Client.send_target_pose(Qt.vector3d(pose_grid.children[1].text, pose_grid.children[3].text, pose_grid.children[5].text),
                                                   Qt.vector3d(pose_grid.children[7].text, pose_grid.children[9].text, pose_grid.children[11].text))
            }
        } // pose

        Rectangle {
            id: path_mode_ly
            anchors.top: show_ly.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter

            ComboBox {
                id: path_type
                anchors.horizontalCenter: parent.horizontalCenter
                model: ["line path", "arc path"]
                onCurrentIndexChanged: {
                    if (path_type.currentIndex === 0) {
                        line_grid.visible = true
                        arc_grid.visible = false
                        path_send_btn.anchors.top = line_grid.bottom
                    }
                    else if (path_type.currentIndex === 1)
                    {
                        arc_grid.visible = true
                        line_grid.visible = false
                        path_send_btn.anchors.top = arc_grid.bottom

                    }
                }
            }

            GridLayout {
                id: line_grid
                anchors.top: path_type.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                columns: 2
                Text {
                    text: "x:"
                }
                TextField {
                    id: line_x
                    text: ""
                }
                Text {
                    text: "y:"
                }
                TextField {
                    id: line_y
                    text: ""
                }
                Text {
                    text: "z:"
                }
                TextField {
                    id: line_z
                    text: ""
                }
                Text {
                    text: "a:"
                }
                TextField {
                    id: line_a
                    text: ""
                }
                Text {
                    text: "b:"
                }
                TextField {
                    id: line_b
                    text: ""
                }
                Text {
                    text: "c:"
                }
                TextField {
                    id: line_c
                    text: ""
                }
                Text {
                    text: "Time:"
                }
                TextField {
                    id: line_t
                    text: ""
                }
            }
            GridLayout {
                id: arc_grid
                anchors.top: path_type.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                columns: 3

                Text {
                    text: " "
                }
                Text {
                    text: "final pose"
                }
                Text {
                    text: "middle pose"
                }


                Text {
                    text: "x:"
                }
                TextField {
                    id: arc_fx
                    text: ""
                }
                TextField {
                    id: arc_mx
                    text: ""
                }

                Text {
                    text: "y:"
                }
                TextField {
                    id: arc_fy
                    text: ""
                }
                TextField {
                    id: arc_my
                    text: ""
                }

                Text {
                    text: "z:"
                }
                TextField {
                    id: arc_fz
                    text: ""
                }
                TextField {
                    id: arc_mz
                    text: ""
                }

                Text {
                    text: "a:"
                }
                TextField {
                    id: arc_fa
                    text: ""
                }
                TextField {
                    id: arc_ma
                    text: ""
                }

                Text {
                    text: "b:"
                }
                TextField {
                    id: arc_fb
                    text: ""
                }
                TextField {
                    id: arc_mb
                    text: ""
                }

                Text {
                    text: "c:"
                }
                TextField {
                    id: arc_fc
                    text: ""
                }
                TextField {
                    id: arc_mc
                    text: ""
                }

                Text {
                    text: "M_Spd:"
                }
                TextField {
                    id: arc_spd
                    text: ""
                }
                Text {
                    text: " "
                }

                Text {
                    text: "M_Acc:"
                }
                TextField {
                    id: arc_acc
                    text: ""
                }
                Text {
                    text: " "
                }
            }

            Button {
                id: path_send_btn
                anchors.top: line_grid.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                text: "send target pose"
                background: Rectangle {
                    border.color: "#888"
                }
                onClicked: {
                    if (path_type.currentIndex === 0) {
                        Client.send_target_path(Qt.vector3d(line_x.text, line_y.text, line_z.text),
                                                Qt.vector3d(line_a.text, line_b.text, line_c.text), line_t.text)
                    }
                    else if (path_type.currentIndex === 1) {
                        Client.send_target_path(Qt.vector3d(arc_fx.text, arc_fy.text, arc_fz.text),
                                                Qt.vector3d(arc_fa.text, arc_fb.text, arc_fc.text),
                                                Qt.vector3d(arc_mx.text, arc_my.text, arc_mz.text),
                                                Qt.vector3d(arc_ma.text, arc_mb.text, arc_mc.text),
                                                arc_spd.text, arc_acc.text)
                    }
                }
            }
        } // path

    }
}
