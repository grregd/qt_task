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

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            CheckBox {
                id: checkBox
                text: qsTr("Show control points")
                onClicked: gradientTool.showControlPoints = checkBox.checked
                Component.onCompleted: checkBox.checked = gradientTool.showControlPoints
                Connections {
                    target: gradientTool
                    onShowControlPointsChanged: checkBox.checked = gradientTool.showControlPoints
                }
            }

            Button {
                text: "Select begin color"

                onClicked: {
                    selectColorBegin.currentColor = gradientTool.colorBegin
                    selectColorBegin.open()
                }
            }

            Button {
                text: "Select end color"

                onClicked: {
                    selectColorEnd.currentColor = gradientTool.colorEnd
                    selectColorEnd.open()
                }
            }

            Label { text: qsTr("Pen width [pixels]") }
            Slider {
                id: penWidthSlider
                stepSize: 1
                minimumValue: 1
                maximumValue: 100
                value: 1
                Component.onCompleted: penWidthSlider.value = gradientTool.penWidth
                onValueChanged: gradientTool.penWidth = penWidthSlider.value
                Connections {
                    target: gradientTool
                    onPenWidthChanged: penWidthSlider.value = gradientTool.penWidth
                }
            }
        }

        GradientTool {
            id: gradientTool
            Layout.fillHeight: true
            Layout.fillWidth: true
            penWidthMax: 100
            colorBegin: "red"
            colorEnd: "yellow"
            showControlPoints: false
        }

        ColorDialog {
            id: selectColorBegin
            title: "Select begin color"
            showAlphaChannel: true

            onCurrentColorChanged: gradientTool.colorBegin = currentColor
            Component.onCompleted: visible = false
        }

        ColorDialog {
            id: selectColorEnd
            title: "Select end color"
            showAlphaChannel: true

            onCurrentColorChanged: gradientTool.colorEnd = currentColor
            Component.onCompleted: visible = false
        }

       Shortcut {
           sequence: "Ctrl+Z"
           onActivated: gradientTool.removeLastPoint()
       }
       Shortcut {
           sequence: "Ctrl+Y"
           onActivated: gradientTool.redoLastPoint()
       }
       Shortcut {
            sequence: "Ctrl+F"
            onActivated: window.showFullScreen()
        }
        Shortcut {
            sequence: "ESC"
            onActivated: window.showNormal()
        }
    }


}
