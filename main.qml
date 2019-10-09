import QtQuick 2.12
import QtQuick.Window 2.12
import GradientTool 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    GradientTool {
        width: 640
        height: 480
        penWidth: 20

//        MouseArea {
//            anchors.fill: parent
//            enabled: true
//            onClicked: {
//                parent.addPoint(mouse.mouseX, mouse.mouseY);
//            }
//        }
    }
}
