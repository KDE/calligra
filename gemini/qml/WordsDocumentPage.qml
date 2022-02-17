// This file is part of the Calligra project
// SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-LicenseIdentifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQml 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.14 as Kirigami
import org.kde.calligra 1.0 as Calligra

MainPage {
    id: base;

    signal canvasInteractionStarted();
    property alias document: wordsDocument;
    property alias textEditor: wordsDocument.textEditor;
    property QtObject canvas: wordsCanvas;
    property alias source: wordsDocument.source;
    property alias navigateMode: controllerFlickable.enabled;
    property double toolbarOpacity: base.state === "readermode" ? 0.3 : 1;

    onBackRequested: {
        if (document.document.isModified()) {
            saveBeforeExitDialog.show();
        } else {
            applicationWindow().pageStack.pop();
        }
    }

    actions.contextualActions: [
        QQC2.Action {
            icon.name: "document-new"
            text: i18n("New")
        },
        QQC2.Action {
            icon.name: "document-open"
            text: i18n("Open")
        },
        QQC2.Action {
            icon.name: "document-save"
            text: i18n("Save")
            onTriggered: {
                closeToolbarMenus();
                viewLoader.item.canvas.document.save();
            }
        },
        QQC2.Action {
            icon.name: "edit-undo"
            text: i18n("Undo")
            enabled: typeof(undoaction) !== "undefined" ? undoaction.enabled : false;
            onTriggered: {
                closeToolbarMenus();
                undoaction.trigger();
            }
        },
        QQC2.Action {
            icon.name: "edit-redo"
            text: i18n("Redo")
            enabled: typeof(redoaction) !== "undefined" ? redoaction.enabled : false;
            onTriggered: {
                closeToolbarMenus();
                redoaction.trigger();
            }
        }
    ]

    Calligra.Document {
        id: wordsDocument;
        onStatusChanged: {
            if(status == Calligra.DocumentStatus.Loading) {
                baseLoadingDialog.visible = true;
            } else if(status == Calligra.DocumentStatus.Loaded) {
                console.debug("doc and part: " + wordsDocument.document + " " + wordsDocument.part);
                baseLoadingDialog.hideMe();
            }
        }
        onCurrentIndexChanged: navigatorListView.positionViewAtIndex(currentIndex - 1, ListView.Center);
    }

    onNavigateModeChanged: {
        if(navigateMode === true) {
            // This means we've just changed back from having edited stuff.
            // Consequently we want to deselect all selections. Tell the canvas about that.
            wordsCanvas.deselectEverything();
            toolManager.requestToolChange("PageToolFactory_ID");
        }
    }
    onWidthChanged: {
        if(base.state === "readermode") {
            d.zoomToFit();
        }
    }
    onHeightChanged: {
        if (base.state === "readermode") {
            d.zoomToFit();
        }
    }
    function scrollToEnd() {
        controllerFlickable.contentY = controllerFlickable.contentHeight - controllerFlickable.height;
    }

    Flickable {
        id: bgScrollArea;
        anchors.fill: parent;
        contentHeight: wordsDocument.documentSize.height;
        interactive: base.state !== "readermode";
        boundsBehavior: controllerFlickable.boundsBehavior;
        Item {
            width: parent.width;
            height: wordsDocument.documentSize.height;
            MouseArea {
                anchors.fill: parent;
                property int oldX: 0
                property int oldY: 0
                property int swipeDistance: 10
                onPressed: {
                    oldX = mouseX;
                    oldY = mouseY;
                }
                onReleased: {
                    base.canvasInteractionStarted();
                    controllerItem.pageChanging = true;
                    var xDiff = oldX - mouseX;
                    var yDiff = oldY - mouseY;
                    // Don't react if the swipe distance is too small
                    if(Math.abs(xDiff) < swipeDistance && Math.abs(yDiff) < swipeDistance) {
                        if(Math.abs(xDiff) > 2 || Math.abs(yDiff) > 2) {
                            // If the swipe distance is sort of big (2 pixels on a 1080p screen)
                            // we can assume the finger has moved some distance and should be ignored
                            // as not-a-tap.
                            controllerItem.pageChanging = false;
                            return;
                        }
                        // This might be done in onClick, but that then eats the events, which is not useful
                        // for reader mode we'll accept clicks here, to simulate an e-reader style navigation mode
                        if(mouse.x < width / 2) {
                            controllerFlickable.pageUp();
                        }
                        else if(mouse.x > width / 2) {
                            controllerFlickable.pageDown();
                        }
                    }
                    else if(base.state === "readermode") {
                        if ( Math.abs(xDiff) > Math.abs(yDiff) ) {
                            if( oldX > mouseX) {
                                // left
                                controllerFlickable.pageDown();
                            } else {
                                // right
                                controllerFlickable.pageUp()
                            }
                        } else {
                            if( oldY > mouseY) {
                                // up
                                controllerFlickable.pageDown();
                            }
                            else {
                                // down
                                controllerFlickable.pageUp();
                            }
                        }
                    }
                    controllerItem.pageChanging = false;
                }
            }
        }
    }
    Binding {
        target: controllerFlickable;
        property: "contentY";
        value: bgScrollArea.contentY;
        when: controllerFlickable.verticalVelocity === 0;
        restoreMode: Binding.RestoreBinding
    }
    Binding {
        target: bgScrollArea;
        property: "contentY";
        value: controllerFlickable.contentY;
        when: bgScrollArea.verticalVelocity === 0;
        restoreMode: Binding.RestoreBinding
    }

    QQC2.ScrollBar {
        id: scrollBar
	anchors.right: parent.right
        anchors.rightMargin: !thumbnailSidebar.modal ? thumbnailSidebar.width * thumbnailSidebar.position : 0
	height: parent.height
    }

    Item {
        id: wordsContentItem;

        anchors {
            top: parent.top
            bottom: parent.bottom
            rightMargin: (!thumbnailSidebar.modal ? thumbnailSidebar.width * thumbnailSidebar.position : 0) - (scrollBar.visible ? scrollBar.width : 0)
            left: parent.left
            right: parent.right
        }

        Calligra.View {
            id: wordsCanvas
            document: wordsDocument
            width: controllerFlickable.contentWidth
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.rightMargin: (!thumbnailSidebar.modal ? thumbnailSidebar.width * thumbnailSidebar.position / 2 : 0) - (scrollBar.visible ? scrollBar.width / 2: 0)
            height: parent.height
        }

        Flickable {
            id: controllerFlickable;
            interactive: base.state !== "readermode";
            property int fastVelocity: Kirigami.Units.gridUnit * 50
            anchors.horizontalCenter: parent.horizontalCenter
            width: Math.min(wordsDocument.documentSize.width, parent.width)
            height: parent.height
            onVerticalVelocityChanged: {
                if (Math.abs(verticalVelocity) > fastVelocity && !controllerItem.pageChanging) {
                    d.showThings();
                } else {
                    d.hideThings();
                }
            }

            boundsBehavior: wordsDocument.documentSize.width < base.width ? Flickable.StopAtBounds : Flickable.DragAndOvershootBounds;

            function pageUp() {
                if(base.state === "readermode") {
                    if(wordsDocument.currentIndex === 1) {
                        controllerFlickable.contentY = wordsCanvas.pagePosition(wordsDocument.indexCount) + 1;
                    }
                    else {
                        controllerFlickable.contentY = wordsCanvas.pagePosition(wordsDocument.currentIndex - 1) + 1;
                    }
                }
                else {
                    controllerFlickable.contentY = Math.max(0, controllerFlickable.contentY - controllerFlickable.height + (Kirigami.Units.gridUnit * 1.5));
                }
            }
            function pageDown() {
                if(base.state === "readermode") {
                    controllerFlickable.contentY = wordsCanvas.pagePosition(wordsDocument.currentIndex + 1) + 1;
                }
                else {
                    controllerFlickable.contentY = Math.min(controllerFlickable.contentHeight - controllerFlickable.height, controllerFlickable.contentY + controllerFlickable.height - (Kirigami.Units.gridUnit * 1.5));
                }
            }

            PinchArea {
                x: controllerFlickable.contentX;
                y: controllerFlickable.contentY;
                height: controllerFlickable.height;
                width: controllerFlickable.width;

                enabled: base.state !== "readermode";

                onPinchStarted: {
                    base.canvasInteractionStarted();
                }
                onPinchUpdated: {
                    var newCenter = mapToItem( controllerFlickable, pinch.center.x, pinch.center.y );
                    controllerItem.zoomAroundPoint(pinch.scale - pinch.previousScale, newCenter.x, newCenter.y );
                }
                onPinchFinished: {
                    controllerFlickable.returnToBounds();
                }

                MouseArea {
                    anchors.fill: parent;
                    onDoubleClicked: {
                        if(base.state === "readermode") {
                            // for reader mode, we don't accept editing input
                            base.canvasInteractionStarted();
                            return;
                        }
                        toolManager.requestToolChange("TextToolFactory_ID");
                        base.navigateMode = false;
                        base.canvasInteractionStarted();
                    }

                    property int oldX: 0
                    property int oldY: 0
                    property int swipeDistance: 10
                    onPressed: {
                        oldX = mouseX;
                        oldY = mouseY;
                    }
                    onReleased: {
                        base.canvasInteractionStarted();
                        if(base.state !== "readermode") {
                            return;
                        }
                        var xDiff = oldX - mouseX;
                        var yDiff = oldY - mouseY;
                        controllerItem.pageChanging = true;
                        // Don't react if the swipe distance is too small
                        if(Math.abs(xDiff) < swipeDistance && Math.abs(yDiff) < swipeDistance) {
                            if(Math.abs(xDiff) > 2 || Math.abs(yDiff) > 2) {
                                // If the swipe distance is sort of big (2 pixels on a 1080p screen)
                                // we can assume the finger has moved some distance and should be ignored
                                // as not-a-tap.
                                controllerItem.pageChanging = false;
                                return;
                            }
                            // This might be done in onClick, but that then eats the events, which is not useful
                            // for reader mode we'll accept clicks here, to simulate an e-reader style navigation mode
                            if(mouse.x < width / 4) {
                                controllerFlickable.pageUp();
                            }
                            else if(mouse.x > width * 3 / 4) {
                                controllerFlickable.pageDown();
                            }
                        } else if( Math.abs(xDiff) > Math.abs(yDiff) ) {
                            if( oldX > mouseX) {
                                // left
                                controllerFlickable.pageDown();
                            } else {
                                // right
                                controllerFlickable.pageUp();
                            }
                        } else {
                            if( oldY > mouseY) {
                                // up
                                controllerFlickable.pageDown();
                            }
                            else {
                                // down
                                controllerFlickable.pageUp();
                            }
                        }
                        controllerItem.pageChanging = false;
                    }
                }
            }

            Calligra.ViewController {
                id: controllerItem;
                objectName: "controllerItem";
                view: wordsCanvas;
                flickable: controllerFlickable;
                property bool pageChanging: false;
                zoom: 1.0;
                minimumZoom: 0.5;
            }
        }
    }

    QtObject {
        id: d;
        property double previouszoom: 0;
        function restoreZoom() {
            controllerItem.zoom = previouszoom;
            previouszoom = 0;
        }
        function zoomToFit() {
            if(previouszoom === 0) {
                previouszoom = controllerItem.zoom;
            }
            controllerItem.zoomToPage();
        }
        function showThings() {
            if(base.state === "readermode") {
                return;
            }
            base.state = "sidebarShown";
        }
        function hideThings() {
            /*if(navigatorSidebar.containsMouse) {
                return;
            }*/
            hideTimer.start();
            hidePageNumTimer.start();
        }
    }

    transitions: [
        Transition {
            AnchorAnimation { duration: Kirigami.Units.shortDuration; easing.type: Easing.InOutQuad; }
        },
        Transition {
            to: "readermode";
            ScriptAction {
                script: {
                    d.zoomToFit();
                    controllerFlickable.contentY = wordsCanvas.pagePosition(wordsDocument.currentIndex) + 1;
					base.canvasInteractionStarted();
                }
            }
        },
        Transition {
            from: "readermode";
            ScriptAction {
                script: {
                    d.restoreZoom();
                    base.canvasInteractionStarted();
                }
            }
        }
    ]

    footer: QQC2.ToolBar {
        contentItem: RowLayout {
            Item { 
                Layout.fillWidth: true
            }

            QQC2.Label {
                id: zoomLabel
                text: (controllerItem.zoom * 100).toFixed(2) + "%"
            }

	    QQC2.Label {
                text: (wordsDocument === null) ? 0 : wordsDocument.currentIndex + "/" + wordsDocument.indexCount;
            }
        }
    }

    //Connections {
    //    target: applicationWindow()
    //    onWidthChanged: thumbnailSidebar.modal = width < Kirigami.Units.gridUnit * 50
    //}

    /// Sidebar containing the preview of the slides
    Kirigami.OverlayDrawer {
        id: thumbnailSidebar
        edge: Qt.application.layoutDirection == Qt.RightToLeft ? Qt.LeftEdge : Qt.RightEdge
        onModalChanged: drawerOpen != modal
        drawerOpen: true
        handleVisible: true
        parent: base.QQC2.overlay
        height: base.height
        width: contentItem.implicitWidth
        modal:false
        contentItem: QQC2.ScrollView {
            implicitWidth: Kirigami.Units.gridUnit * 15
            ListView {
                id: navigatorListView;
                clip: true;
                model: Calligra.ContentsModel {
                    id: wordsContentModel;
                    document: wordsDocument;
                    useToC: false;
                    thumbnailSize: Qt.size(Kirigami.Units.gridUnit * 10, Kirigami.Units.gridUnit * 14);
                }

                delegate: QQC2.ItemDelegate {
                    onClicked: {
                        wordsDocument.currentIndex = model.contentIndex;
                        // controllerFlickable.contentY = wordsCanvas.pagePosition(index + 1) + 1;
                        base.canvasInteractionStarted();
                    }
                    width: Kirigami.Units.gridUnit * 15
                    height: Kirigami.Units.gridUnit * 15
                    Calligra.ImageDataItem {
                        id: navigatorThumbnail;
                        data: model.thumbnail;
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        width: Kirigami.Units.gridUnit * 11
                    }
                    QQC2.Label {
                        Layout.fillWidth: true
                        text: index + 1;
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }
        }
    }

    /// Close dialog
    Kirigami.OverlaySheet {
        id: saveBeforeExitDialog;
        header: Kirigami.Heading {
            text: i18n("Save?")
        }
        contentItem: QQC2.Label {
            text: i18n("The document was modified. Would you like to save it before closing it?");
        }
        footer: QQC2.DialogButtonBox {
            standardButtons: QQC2.DialogButtonBox.Ok | QQC2.DialogButtonBox.Cancel | QQC2.DialogButtonBox.Discard
            onAccepted: {
                document.save();
                applicationWindow().pageStack.pop();
            }
            onRejected: saveBeforeExitDialog.visible = false;
            onDiscarded: {
                document.setModified(false);
                applicationWindow().pageStack.pop();
            }
        }
    }
}
