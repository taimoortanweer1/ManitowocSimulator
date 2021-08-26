import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.12
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.11
import QtQuick.Controls.Styles 1.4

ApplicationWindow {
    id: applicationWindow
    width: 1280
    height: 800
    visible: true
    color: "#131f2d"
    title: qsTr("Manitowoc Simulator")

    TabBar {
        id: bar
        width: parent.width
        height: 40
        font.pointSize: 18
        TabButton {
            text: qsTr("CONFIGURATIONS")
        }
        TabButton {
            text: qsTr("GRT MEASUREMENTS")
        }
        TabButton {
            text: qsTr("CONTROLS")
        }


    }

    StackLayout {
        width: parent.width
        currentIndex: bar.currentIndex
        Item {

            id: configTab
        }
        GRTMeasurement {
            id: grtMeasurementTab
        }
        Item {
            id: controlsTab
        }


    }





}



/*##^##
Designer {
    D{i:0;formeditorZoom:0.66}
}
##^##*/
