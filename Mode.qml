import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import RIK_Client 1.0

Item {
    onVisibleChanged: {
        if (visible)
            Client.send_mode(mode_comb.currentIndex)
    }

    Rectangle {
        id: mode_page
        height: parent.height
        width: parent.width

        Row {
            id: mode_row
            anchors.top: parent.top
            anchors.topMargin: 10
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
                    Client.send_mode(currentIndex)
                }
            }
        }

        GridLayout {
            id: show_ly
            anchors.top: mode_row.bottom
            anchors.topMargin: 10
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

                onClicked: Client.get_joints()

            }
            Button {
                text: "show current pose"

                onClicked: Client.get_pose()
            }
        } // show joints and pose

        Rectangle {
            id: joints_mode_ly
            anchors.top: show_ly.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            visible: false

            GridLayout {
                id: joints_grid
                anchors.horizontalCenter: parent.horizontalCenter
                columns: 4
                Text {
                    text: "joints1:"
                }
                TextField {
                    id: j1
                    text: "0"
                }
                Button {
                    text: "-"
                    onPressed: Client.send_increasing_mode(true, 0, 0, -1)
                    onReleased: Client.send_increasing_mode(false, 0, 0, -1)
                }
                Button {
                    text: "+"
                    onPressed: Client.send_increasing_mode(true, 0, 0, 1)
                    onReleased: Client.send_increasing_mode(false, 0, 0, 1)
                }


                Text {
                    text: "joints2:"
                }
                TextField {
                    id: j2
                    text: "-90"
                }
                Button {
                    text: "-"
                    onPressed: Client.send_increasing_mode(true, 0, 1, -1)
                    onReleased: Client.send_increasing_mode(false, 0, 1, -1)
                }
                Button {
                    text: "+"
                    onPressed: Client.send_increasing_mode(true, 0, 1, 1)
                    onReleased: Client.send_increasing_mode(false, 0, 1, 1)
                }

                Text {
                    text: "joints3:"
                }
                TextField {
                    id: j3
                    text: "-90"
                }
                Button {
                    text: "-"
                    onPressed: Client.send_increasing_mode(true, 0, 2, -1)
                    onReleased: Client.send_increasing_mode(false, 0, 2, -1)
                }
                Button {
                    text: "+"
                    onPressed: Client.send_increasing_mode(true, 0, 2, 1)
                    onReleased: Client.send_increasing_mode(false, 0, 2, 1)
                }

                Text {
                    text: "joints4:"
                }
                TextField {
                    id: j4
                    text: "0"
                }
                Button {
                    text: "-"
                    onPressed: Client.send_increasing_mode(true, 0, 3, -1)
                    onReleased: Client.send_increasing_mode(false, 0, 3, -1)
                }
                Button {
                    text: "+"
                    onPressed: Client.send_increasing_mode(true, 0, 3, 1)
                    onReleased: Client.send_increasing_mode(false, 0, 3, 1)
                }

                Text {
                    text: "joints5:"
                }
                TextField {
                    id: j5
                    text: "-90"
                }
                Button {
                    text: "-"
                    onPressed: Client.send_increasing_mode(true, 0, 4, -1)
                    onReleased: Client.send_increasing_mode(false, 0, 4, -1)
                }
                Button {
                    text: "+"
                    onPressed: Client.send_increasing_mode(true, 0, 4, 1)
                    onReleased: Client.send_increasing_mode(false, 0, 4, 1)
                }

                Text {
                    text: "joints6:"
                }
                TextField {
                    id: j6
                    text: "0"
                }
                Button {
                    text: "-"
                    onPressed: Client.send_increasing_mode(true, 0, 5, -1)
                    onReleased: Client.send_increasing_mode(false, 0, 5, -1)
                }
                Button {
                    text: "+"
                    onPressed: Client.send_increasing_mode(true, 0, 5, 1)
                    onReleased: Client.send_increasing_mode(false, 0, 5, 1)
                }
            }
            Button {
                anchors.top: joints_grid.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                text: "send target joints"

                onClicked: Client.send_target_joints(j1.text, j2.text, j3.text, j4.text, j5.text, j6.text)
            }
        } // joints

        Rectangle {
            id: pose_mode_ly
            anchors.top: show_ly.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            visible: false


            GridLayout {
                id: pose_grid
                anchors.horizontalCenter: parent.horizontalCenter
                columns: 4
                Text {
                    text: "x:"
                }
                TextField {
                    id: px
                    text: "860"
                }
                Button {
                    text: "-"
                    onPressed: Client.send_increasing_mode(true, 1, 0, -1)
                    onReleased: Client.send_increasing_mode(false, 1, 0, -1)
                }
                Button {
                    text: "+"
                    onPressed: Client.send_increasing_mode(true, 1, 0, 1)
                    onReleased: Client.send_increasing_mode(false, 1, 0, 1)
                }

                Text {
                    text: "y:"
                }
                TextField {
                    id: py
                    text: "0"
                }
                Button {
                    text: "-"
                    onPressed: Client.send_increasing_mode(true, 1, 1, -1)
                    onReleased: Client.send_increasing_mode(false, 1, 1, -1)
                }
                Button {
                    text: "+"
                    onPressed: Client.send_increasing_mode(true, 1, 1, 1)
                    onReleased: Client.send_increasing_mode(false, 1, 1, 1)
                }

                Text {
                    text: "z:"
                }
                TextField {
                    id: pz
                    text: "694"
                }
                Button {
                    text: "-"
                    onPressed: Client.send_increasing_mode(true, 1, 2, -1)
                    onReleased: Client.send_increasing_mode(false, 1, 2, -1)
                }
                Button {
                    text: "+"
                    onPressed: Client.send_increasing_mode(true, 1, 2, 1)
                    onReleased: Client.send_increasing_mode(false, 1, 2, 1)
                }

                Text {
                    text: "a:"
                }
                TextField {
                    id: pa
                    text: "0"
                }
                Button {
                    text: "-"
                    onPressed: Client.send_increasing_mode(true, 1, 3, -1)
                    onReleased: Client.send_increasing_mode(false, 1, 3, -1)
                }
                Button {
                    text: "+"
                    onPressed: Client.send_increasing_mode(true, 1, 3, 1)
                    onReleased: Client.send_increasing_mode(false, 1, 3, 1)
                }

                Text {
                    text: "b:"
                }
                TextField {
                    id: pb
                    text: "0"
                }
                Button {
                    text: "-"
                    onPressed: Client.send_increasing_mode(true, 1, 4, -1)
                    onReleased: Client.send_increasing_mode(false, 1, 4, -1)
                }
                Button {
                    text: "+"
                    onPressed: Client.send_increasing_mode(true, 1, 4, 1)
                    onReleased: Client.send_increasing_mode(false, 1, 4, 1)
                }

                Text {
                    text: "c:"
                }
                TextField {
                    id: pc
                    text: "0"
                }
                Button {
                    text: "-"
                    onPressed: Client.send_increasing_mode(true, 1, 5, -1)
                    onReleased: Client.send_increasing_mode(false, 1, 5, -1)
                }
                Button {
                    text: "+"
                    onPressed: Client.send_increasing_mode(true, 1, 5, 1)
                    onReleased: Client.send_increasing_mode(false, 1, 5, 1)
                }
            }
            Button {
                anchors.top: pose_grid.bottom
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                text: "send target pose"

                onClicked: Client.send_target_pose(Qt.vector3d(px.text, py.text, pz.text),
                                                   Qt.vector3d(pa.text, pb.text, pc.text))
            }
        } // pose

        Rectangle {
            id: path_mode_ly
            anchors.top: show_ly.bottom
            anchors.topMargin: 10
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
                anchors.topMargin: 10
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
                anchors.topMargin: 10
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
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                text: "send target pose"

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
