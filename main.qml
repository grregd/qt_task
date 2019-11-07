import QtQuick 2.12
import QtQuick.Window 2.12
import GradientTool 1.0
import QtQuick.Layouts 1.13
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.3


Window {
    id: window
    visible: true
    width: 640
    height: 480

    title: qsTr("Hello World")

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            id: layoutMenu

            CheckBox {
                id: checkBoxCtrlPoints
                text: qsTr("Show control points")
                onClicked: gradientTool.showControlPoints = checkBoxCtrlPoints.checked
                Component.onCompleted: checkBoxCtrlPoints.checked = gradientTool.showControlPoints
                Connections {
                    target: gradientTool
                    onShowControlPointsChanged: checkBoxCtrlPoints.checked = gradientTool.showControlPoints
                } }

            Label { text: qsTr("Pen width [pixels]: ") }

            Slider {
                id: penWidthSlider
                Layout.preferredWidth: 150
                stepSize: 1
                minimumValue: 1
                maximumValue: 150
                value: 100
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
            penWidthMax: 150
            hoverSelectColor: "#3300cc" // kolor chabrowy
            defaultColorBegin: "#8fbcbc"
            defaultColorEnd: "#180a18"
            showControlPoints: false
            onRequestColorChange: {
                openColorDialog(controlPointColorDialog, initColor)
            }
            onPointSelectionChanged: {
                controlPointColorDialog.currentColor = pointColor
            }
        }

        ColorDialog {
            id: controlPointColorDialog
            title: qsTr("Choose color for selected control point")
            showAlphaChannel: true

            onCurrentColorChanged: if (visible) gradientTool.setColorOfSelectedPoint(currentColor)
            Component.onCompleted: visible = false
        }

       Shortcut {
            sequence: "Ctrl+F"
            onActivated: window.showFullScreen()
        }
       Shortcut {
            sequence: "ESC"
            onActivated: window.showNormal()
        }
       Shortcut {
            sequence: "Ctrl+D"
            onActivated: {
                if (layoutMenu.visible == false)
                    layoutMenu.visible = true
                else
                    layoutMenu.visible = false
            }
        }
       Shortcut {
           sequence: "Ctrl+T"
           onActivated: {
               checkBoxCtrlPoints.checked = !checkBoxCtrlPoints.checked
               gradientTool.showControlPoints = !gradientTool.showControlPoints
           }
       }
    }

    function openColorDialog(colorDialog, color)
    {
        colorDialog.currentColor = color
        colorDialog.open()
    }
}
