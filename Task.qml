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
            height: parent.height * 0.5
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
                        //params_tf.text = "860,0,694,90,0,-90,5000"
                        params_tf.text = "860,0,846,90,0,-90,5000"
                    else if (type_tf.currentIndex === 1)
                        params_tf.text = "1164,0,694,90,0,-90,1012,0,719,90,0,-90,2000,1200"
                }
            }
            TextField {
                id: params_tf
                width: task_page.width / 2
                text: ""
            }
            TextField {
                id: params_tf_speed
                width: task_page.width / 5
                text: ""
            }
        }

        Row {
            id: row_command
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
            Button {
                text: "add Demo1"
                onClicked: {
                    onClicked: {
                        task_lm.append({"type": "line", "params": "860,0,846,90,0,-90,50,50"})
                    }
                }
            }
        }
        Row {
            id: row_demo
            anchors.top: row_command.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter

            ComboBox{
                id:demo_num_id
                width: 200
                model: ["None", "Demo1: line+arc+line", "Demo2: posture changing", "Demo3: step by step"]
            }
            Text {
                text: "  "
            }
            ComboBox{
                id:demo_speed
                width: 80
                model: ["low", "normal", "hight"]
            }
            Text {
                text: "  "
            }
            Button{
                text: "add Demo"
                onClicked: {
//                    if(demo_num_id.currentIndex == 1){
//                        if(demo_speed.currentIndex == 0){
//                            task_lm.append({"type": "line", "params": "0,1200,194,90,0,-90,100,100"})
//                            task_lm.append({"type": "arc", "params": "200,-1000,794,90,0,-90,1100,50,494,90,0,-90,50,50"})
//                            task_lm.append({"type": "line", "params": "860,0,694,90,0,-90,100,100"})
//                        }
//                        else if(demo_speed.currentIndex == 1){
//                            task_lm.append({"type": "line", "params": "0,1200,194,90,0,-90,400,400"})
//                            task_lm.append({"type": "arc", "params": "200,-1000,794,90,0,-90,1100,50,494,90,0,-90,200,200"})
//                            task_lm.append({"type": "line", "params": "860,0,694,90,0,-90,400,400"})
//                        }
//                        else if(demo_speed.currentIndex == 2){
//                            task_lm.append({"type": "line", "params": "0,1200,194,90,0,-90,400,1600"})
//                            task_lm.append({"type": "arc", "params": "200,-1000,794,90,0,-90,1100,50,494,90,0,-90,200,800"})
//                            task_lm.append({"type": "line", "params": "860,0,694,90,0,-90,400,1600"})
//                        }
//                    }
//                    else if(demo_num_id.currentIndex == 2){
//                        if(demo_speed.currentIndex == 0){
//                            task_lm.append({"type": "line", "params": "860,0,694,150,-30,-150,50,50"})
//                            task_lm.append({"type": "line", "params": "860,0,694,90,-60,-90,50,50"})
//                            task_lm.append({"type": "line", "params": "860,0,694,30,-30,-30,50,50"})
//                            task_lm.append({"type": "line", "params": "860,0,694,90,0,-90,50,50"})
//                        }
//                        else if(demo_speed.currentIndex == 1){
//                            task_lm.append({"type": "line", "params": "860,0,694,150,-30,-150,200,200"})
//                            task_lm.append({"type": "line", "params": "860,0,694,90,-60,-90,200,200"})
//                            task_lm.append({"type": "line", "params": "860,0,694,30,-30,-30,200,200"})
//                            task_lm.append({"type": "line", "params": "860,0,694,90,0,-90,200,200"})
//                        }
//                        else if(demo_speed.currentIndex == 2){
//                            task_lm.append({"type": "line", "params": "860,0,694,150,-30,-150,800,800"})
//                            task_lm.append({"type": "line", "params": "860,0,694,90,-60,-90,800,800"})
//                            task_lm.append({"type": "line", "params": "860,0,694,30,-30,-30,800,800"})
//                            task_lm.append({"type": "line", "params": "860,0,694,90,0,-90,800,800"})
//                        }
//                    }

                    // for 137
                    if(demo_num_id.currentIndex == 1){
                        if(demo_speed.currentIndex == 0){
                            task_lm.append({"type": "line", "params": "0,1200,346,0,0,0,100,100"})
                            task_lm.append({"type": "arc", "params": "200,-1000,946,0,0,0,1100,50,646,90,0,-90,50,50"})
                            task_lm.append({"type": "line", "params": "860,0,846,0,0,0,100,100"})
                        }
                        else if(demo_speed.currentIndex == 1){
                            task_lm.append({"type": "line", "params": "0,1200,346,0,0,0,400,400"})
                            task_lm.append({"type": "arc", "params": "200,-1000,946,0,0,0,1100,50,646,90,0,-90,200,200"})
                            task_lm.append({"type": "line", "params": "860,0,846,0,0,0,400,400"})
                        }
                        else if(demo_speed.currentIndex == 2){
                            task_lm.append({"type": "line", "params": "0,1200,346,0,0,0,800,1600"})
                            task_lm.append({"type": "arc", "params": "200,-1000,946,0,0,0,1100,50,646,90,0,-90,400,800"})
                            task_lm.append({"type": "line", "params": "860,0,846,0,0,0,800,1600"})
                        }
                    }
                    else if(demo_num_id.currentIndex == 2){
                        if(demo_speed.currentIndex == 0){
                            task_lm.append({"type": "line", "params": "860,0,846,150,-30,-150,50,50"})
                            task_lm.append({"type": "line", "params": "860,0,846,90,-60,-90,50,50"})
                            task_lm.append({"type": "line", "params": "860,0,846,30,-30,-30,50,50"})
                            task_lm.append({"type": "line", "params": "860,0,846,90,0,-90,50,50"})
                        }
                        else if(demo_speed.currentIndex == 1){
                            task_lm.append({"type": "line", "params": "860,0,846,150,-30,-150,200,200"})
                            task_lm.append({"type": "line", "params": "860,0,846,90,-60,-90,200,200"})
                            task_lm.append({"type": "line", "params": "860,0,846,30,-30,-30,200,200"})
                            task_lm.append({"type": "line", "params": "860,0,846,90,0,-90,200,200"})
                        }
                        else if(demo_speed.currentIndex == 2){
                            task_lm.append({"type": "line", "params": "860,0,846,150,-30,-150,800,800"})
                            task_lm.append({"type": "line", "params": "860,0,846,90,-60,-90,800,800"})
                            task_lm.append({"type": "line", "params": "860,0,846,30,-30,-30,800,800"})
                            task_lm.append({"type": "line", "params": "860,0,846,90,0,-90,800,800"})
                        }
                    }
                }
            }
        }
        Row {
            anchors.top: row_demo.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter

            ComboBox {
                id: demo_id
                model: [ "None", "Demo1"]
            }
            Button {
                text: "Switch demo"
                onClicked: Client.send_demo(demo_id.currentIndex);
            }
        }
    }
}



