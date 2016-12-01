import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import RIK_Client 1.0

Item {
    Rectangle {
        id: task_page
        height: parent.height
        width: parent.width

        ListModel {
            id: task_lm
        }

        TableView {
            id: task_tv
            width: parent.width * 0.9
            height: parent.height * 0.7
            anchors.top: parent.top
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter

            TableViewColumn {
                role: "type"
                title: "Type"
                width: 100
            }

            TableViewColumn {
                role: "params"
                title: "Params"
                width: 300
            }

//            itemDelegate:
//                TextInput {
//                    text: ""
//                    onAccepted: model.setProperty(rowIndex, itemProperty, text)
//                }

            model: task_lm
        }

        Row {
            id: loop_setting
            anchors.top: task_tv.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 20

            Text {
                text: "loop times(-1 for infinity): "
            }
            TextField{
                id: loop_times
                text: "-1"
            }
        }

        Row {
            id: row_task
            anchors.top: loop_setting.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 20

            TextField {
                id: type_tf
                text: "0"
            }
            TextField {
                id: params_tf
                text: "860,0,846,0,0,0"
            }
        }

        Row {
            anchors.top: row_task.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: "add row"
                onClicked: task_lm.append({"type": type_tf.text, "params": params_tf.text})
            }
            Button {
                text: "delete row"
                onClicked: task_lm.remove(task_tv.currentRow)
            }
            Button {
                text: "send task"
                onClicked: {
                    var s1 = "", s2 = ""
                    for (var i = 0; i < task_lm.count; i++) {
                        s1 += task_lm.get(i).type;
                        s1 += ";"
                        s2 += task_lm.get(i).params;
                        s2 += ";"
                    }
                    //console.log(s1, s2)
                    Client.send_task(s1, s2, loop_times.text)
                }
            }
        }
    }
}



