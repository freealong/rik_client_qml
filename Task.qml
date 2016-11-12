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
            width: 400
            height: 600
            anchors.top: parent.top
            anchors.topMargin: 50
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

            itemDelegate:
                TextInput {
                    text: ""
                    onAccepted: model.setProperty(rowIndex, itemProperty, text)
                }

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
            anchors.top: loop_setting.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: "add row"
                onClicked: task_lm.append({"type": "Line", "Params": "860,0,846,0,0,0,5000"})
            }
            Button {
                text: "delete row"
                onClicked: task_lm.remove(task_tv.currentRow)
            }
            Button {
                text: "send task"
                onClicked: {

                }
            }
        }
    }
}
