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

            ComboBox {
                id: type_tf
                model: [ "line", "arc"]
                onCurrentIndexChanged: {
                    if (type_tf.currentIndex === 0)
                        params_tf.text = "860,0,694,90,0,-90,5000"
                    else if (type_tf.currentIndex === 1)
                        params_tf.text = "860,0,694,90,0,-90,860,0,694,90,0,-90,50,100"
                }
            }
            TextField {
                id: params_tf
                width: task_page.width / 2
                text: ""
            }
        }

        Row {
            anchors.top: row_task.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: "add row"
                onClicked: task_lm.append({"type": type_tf.currentText, "params": params_tf.text})
            }
            Button {
                text: "delete row"
                onClicked: task_lm.remove(task_tv.currentRow)
            }
            Button {
                text: "clear rows"
                onClicked: task_lm.clear()
            }
            Text {
                text: "  "
            }
            Button {
                text: "send task"
                onClicked: {
                    var s1 = "", s2 = ""
                    for (var i = 0; i < task_lm.count; i++) {
                        s1 += task_lm.get(i).type == "line" ? 0 : 1;
                        s1 += ";"
                        s2 += task_lm.get(i).params;
                        s2 += ";"
                    }
                    //console.log(s1, s2)
                    Client.send_task(s1, s2, loop_times.text)
                }
            }
            Button {
                text: "abort Task"
                onClicked: {
                    Client.send_msg("abort task")
                }
            }
        }
    }
}



