import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Layouts 1.13
import QtQuick.Controls 1.4


Window {
    id: window
    visible: true
    //    visibility: "Maximized"
    width: 640
    height: 480

    title: qsTr("Hello World")

    ColumnLayout
    {
        anchors.fill: parent

        RowLayout {
            Layout.alignment: Qt.AlignLeft
            Rectangle {
//                Layout.alignment: Qt.AlignCenter
                color: "red"
                width: 100
                height: 40
//                Layout.preferredWidth: 40
//                Layout.preferredHeight: 40
            }

            Rectangle {
//                Layout.alignment: Qt.AlignRight
                color: "green"
                width: 100
                height: 70
//                Layout.preferredWidth: 40
//                Layout.preferredHeight: 70
            }
        }

        Rectangle {
            Layout.alignment: Qt.AlignBottom
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "grey"
            width: 70
            height: 40
//            Layout.preferredWidth: 70
//            Layout.preferredHeight: 40
        }
    }

}
