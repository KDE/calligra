import QtQuick 1.1
import Sailfish.Silica 1.0
import DocumentView 1.0

Page {
    id: page

    // To enable PullDownMenu, place our content in a SilicaFlickable
    SilicaFlickable {
        anchors.fill: parent

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                text: "Open File"
                onClicked: pageStack.push(Qt.resolvedUrl("FilePage.qml"))
            }
        }

        // Tell SilicaFlickable the height of its content.
//        contentHeight: childrenRect.height

        contentHeight: documentViewItem.implicitHeight
        contentWidth: Math.max(documentViewItem.implicitWidth, parent.width)

/*
        // Place our content in a Column.  The PageHeader is always placed at the top
        // of the page, followed by our content.
        Column {
            width: page.width
            spacing: theme.paddingLarge
            PageHeader {
                title: "UI Template"
            }
            Label { 
                x: theme.paddingLarge
                text: "Hello Sailors" 
                color: theme.secondaryHighlightColor
                font.pixelSize: theme.fontSizeLarge
            }
        }
*/
        DocumentViewItem {
            id: documentViewItem
        }

    }
}


