import QtQuick 2.12
import QtQuick.Window 2.12
import GradientTool 1.0
import QtQuick.Layouts 1.13
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.3

Window {
    id: window
    visible: true
    //    visibility: "Maximized"
    width: 640
    height: 480

    title: qsTr("Hello World")

    RowLayout {
        anchors.fill:
        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            RowLayout {
                Layout.fillWidth: false
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                CheckBox {
                    id: checkBox
                    text: qsTr("Show control points")
                    checked: false
                }

                Label { text: "color beg"

                }

                Label { text: "color end" }

                ColorDialog {
                    title: "select color"
                    Component.onCompleted: visible = true
                }




            }

            GradientTool {
                id: gradientTool
                Layout.fillHeight: false
                Layout.fillWidth: false
                anchors.fill: parent
                penWidth: 200
                colorBegin: "red"
                colorEnd: "yellow"
                showControlPoints: false
            }

        }
    }

    Connections {
        target: checkBox
        onClicked: gradientTool = checkBox.checked
    }

}
