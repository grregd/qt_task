import QtQuick 2.12
import QtQuick.Window 2.12
import GradientTool 1.0

Window {
    visible: true
//    visibility: "Maximized"
    width: 640
    height: 480

    title: qsTr("Hello World")

    GradientTool {
        anchors.fill: parent
        penWidth: 200
        colorBegin: "#baadc0de"
        colorEnd: "blue"

//        MouseArea {
//            anchors.fill: parent
//            enabled: true
//            onClicked: {
//                parent.addPoint(mouse.mouseX, mouse.mouseY);
//            }
//        }
    }
}
