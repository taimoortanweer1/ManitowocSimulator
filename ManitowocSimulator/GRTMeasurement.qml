import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0
import QtQuick.Extras 1.4

Item {


    id:grtMeasurement
    width: 1280
    height: parent.height
    property alias sliderWeightValue: sliderWeight.value

    Rectangle {


        color: "#131f2d"
        anchors.fill: parent

        Rectangle {
            id: rectangle
            x: 53
            y: 70
            width: 512
            height: 276
            color: "#131f2d"
            border.color: "#f2eaea"

            ColumnLayout {
                x: 15
                y: 15
                width: 162
                height: 262

                Text {
                    id: text7
                    color: "#ffffff"
                    text: qsTr("WEIGHT")
                    font.pixelSize: 22
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 144
                }

                Text {
                    id: text8
                    color: "#ffffff"
                    text: qsTr("BOOM LENGTH")
                    font.pixelSize: 22
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 154
                }

                Text {
                    id: text9
                    color: "#ffffff"
                    text: qsTr("RADIUS ANGLE")
                    font.pixelSize: 22
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 162
                }

                Text {
                    id: text10
                    color: "#ffffff"
                    text: qsTr("HEIGHT")
                    font.pixelSize: 22
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 162
                }

                Text {
                    id: text11
                    color: "#ffffff"
                    text: qsTr("BOOM ANGLE")
                    font.pixelSize: 22
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 162
                }

                Text {
                    id: text12
                    color: "#ffffff"
                    text: qsTr("RADIUS VALUE")
                    font.pixelSize: 22
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 162
                }
            }

            ColumnLayout {
                x: 225
                y: 8
                width: 280
                height: 260

                Slider {
                    id: sliderWeight
                    stepSize: 10
                    to: 1000
                    from: 0
                    wheelEnabled: true
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: 280
                    value: 1
                }

                Slider {
                    id: sliderBoomLength
                    wheelEnabled: true
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: 280
                    value: 0.5
                }

                Slider {
                    id: sliderRadiusAngle
                    wheelEnabled: true
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: 280
                    value: 0.5


                }

                Slider {
                    id: sliderHeight
                    wheelEnabled: true
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: 280
                    value: 0.5
                }

                Slider {
                    id: sliderBoomAngle
                    wheelEnabled: true
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: 280
                    value: 0.5
                }

                Slider {
                    id: sliderRadiusValue
                    wheelEnabled: true
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: 280
                    value: 0.5
                }
            }





        }

        Rectangle {
            id: rectangle1
            x: 53
            y: 362
            width: 512
            height: 409
            color: "#131f2d"
            border.color: "#f2eaea"

            ColumnLayout {
                x: 20
                y: 11
                width: 162
                height: 390
                spacing: 1
                Text {
                    id: text13
                    color: "#ffffff"
                    text: qsTr("Transmission Gear")
                    font.pixelSize: 22
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 144
                }

                Text {
                    id: text14
                    color: "#ffffff"
                    text: qsTr("Engine RPM")
                    font.pixelSize: 22
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 154
                }

                Text {
                    id: text15
                    color: "#ffffff"
                    text: qsTr("Speedometer")
                    font.pixelSize: 22
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 162
                }

                Text {
                    id: text16
                    color: "#ffffff"
                    text: qsTr("Park Brake")
                    font.pixelSize: 22
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 162
                }

                Text {
                    id: text17
                    color: "#ffffff"
                    text: qsTr("Steering Reversed")
                    font.pixelSize: 22
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 162
                }

                Text {
                    id: text18
                    color: "#ffffff"
                    text: qsTr("Marker Lights")
                    font.pixelSize: 22
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 162
                }

                Text {
                    id: text19
                    color: "#ffffff"
                    text: qsTr("2WD/4WD")
                    font.pixelSize: 22
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 162
                }

                Text {
                    id: text20
                    color: "#ffffff"
                    text: qsTr("Oscillation Lock")
                    font.pixelSize: 22
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 162
                }

                Text {
                    id: text21
                    color: "#ffffff"
                    text: qsTr("Eco mode")
                    font.pixelSize: 22
                    Layout.preferredHeight: 36
                    Layout.preferredWidth: 162
                }
            }

            ColumnLayout {
                x: 230
                y: -186
                width: 282
                height: 351
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: -19
                spacing: 3
                Slider {
                    id: sliderWeight1
                    Layout.preferredHeight: 40
                    stepSize: 10
                    Layout.preferredWidth: 280
                    value: 1
                    wheelEnabled: true
                    to: 1000
                    from: 0
                }

                Slider {
                    id: sliderBoomLength1
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: 280
                    value: 0.5
                    wheelEnabled: true
                }

                Slider {
                    id: sliderRadiusAngle1
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: 280
                    value: 0.5
                    wheelEnabled: true
                }

                ComboBox {
                    id: comboBox
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    editable: false
                    flat: false

                    model: ListModel {
                        id: model
                        ListElement { text: "ON" }
                        ListElement { text: "OFF" }

                    }
                    onAccepted: {
                        if (find(editText) === -1)
                            model.append({text: editText})
                    }


                }

                ComboBox {
                    id: comboBox1
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    model: ListModel {
                        id: model1
                        ListElement { text: "ON" }
                        ListElement { text: "OFF" }

                    }
                    onAccepted: {
                        if (find(editText) === -1)
                            model.append({text: editText})
                    }
                }

                ComboBox {
                    id: comboBox2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    model: ListModel {
                        id: model2
                        ListElement { text: "ON" }
                        ListElement { text: "OFF" }

                    }
                    onAccepted: {
                        if (find(editText) === -1)
                            model.append({text: editText})
                    }
                }

                ComboBox {
                    id: comboBox3
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    model: ListModel {
                        id: model3
                        ListElement { text: "2WD" }
                        ListElement { text: "4WD" }

                    }
                    onAccepted: {
                        if (find(editText) === -1)
                            model.append({text: editText})
                    }
                }

                ComboBox {
                    id: comboBox4
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    model: ListModel {
                        id: model4
                        ListElement { text: "ON" }
                        ListElement { text: "OFF" }

                    }
                    onAccepted: {
                        if (find(editText) === -1)
                            model.append({text: editText})
                    }
                }

                ComboBox {
                    id: comboBox5
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    model: ListModel {
                        id: model5
                        ListElement { text: "ON" }
                        ListElement { text: "OFF" }

                    }
                    onAccepted: {
                        if (find(editText) === -1)
                            model.append({text: editText})
                    }

                }
            }

        }


    }


}

/*##^##
Designer {
    D{i:0;formeditorZoom:0.75;height:800;width:1280}D{i:26}D{i:27}D{i:36}D{i:40}D{i:44}
D{i:48}D{i:52}D{i:17}
}
##^##*/
