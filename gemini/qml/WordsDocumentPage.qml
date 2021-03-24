// This file is part of the Calligra project
// SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-LicenseIdentifier: GPL-2.0-or-later

import QtQuick 2.15
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
                mainWindow.setDocAndPart(wordsDocument.document, wordsDocument.part);
                baseLoadingDialog.hideMe();
            }
        }
        onCurrentIndexChanged: navigatorListView.positionViewAtIndex(currentIndex - 1, ListView.Center);
    }

    Calligra.ContentsModel {
        id: wordsContentModel;
        document: wordsDocument;
        useToC: false;
        thumbnailSize: Qt.size(Kirigami.Units.gridUnit * 10, Kirigami.Units.gridUnit * 10);
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


    Calligra.View {
        id: wordsCanvas;
        document: wordsDocument;
        anchors {
            top: parent.top
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
        width: controllerFlickable.width
    }

    QQC2.ScrollView {
        anchors {
            top: parent.top
            bottom: parent.bottom
        }
        width: parent.width - (!thumbnailSidebar.modal && thumbnailSidebar.drawerOpen ? thumbnailSidebar.implicitWidth : 0);

        Flickable {
            id: controllerFlickable;
            width: Math.min(parent.width, base.width);
            interactive: base.state !== "readermode";
            property int fastVelocity: Kirigami.Units.gridUnit * 50
            anchors.horizontalCenter: parent.horizontalCenter
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
                    property int swipeDistance: Settings.theme.adjustedPixel(10);
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
                            if(Math.abs(xDiff) > Settings.theme.adjustedPixel(2) || Math.abs(yDiff) > Settings.theme.adjustedPixel(2)) {
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
            pageNumber.opacity = 1;
            hideTimer.stop();
            hidePageNumTimer.stop();
        }
        function hideThings() {
            /*if(navigatorSidebar.containsMouse) {
                return;
            }*/
            hideTimer.start();
            hidePageNumTimer.start();
        }
    }
    Timer {
        id: hidePageNumTimer;
        running: false;
        repeat: false;
        interval: 500;
        onTriggered: {
            pageNumber.opacity = 0;
        }
    }
    Timer {
        id: hideTimer;
        running: false;
        repeat: false;
        interval: 2000;
        onTriggered: {
            if (base.state === "readermode") {
                return;
            }
            base.state = "";
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
                    if(mainWindow.maximized) {
                        mainWindow.fullScreen = true;
                    }
                }
            }
        },
        Transition {
            from: "readermode";
            ScriptAction {
                script: {
                    d.restoreZoom();
					base.canvasInteractionStarted();
                    mainWindow.fullScreen = false;
                }
            }
        }
    ]

    /*
        Item {
            anchors.fill: navigatorListView;
            clip: true;
            Item {
                id: visualiserContainer;
                property double scale: height / controllerFlickable.contentHeight;
                width: parent.width;
                height: (wordsDocument === null) ? 0 : wordsDocument.indexCount * Settings.theme.adjustedPixel(190);
                x: 0;
                y: -navigatorListView.contentY;
                Rectangle {
                    x: 0;
                    y: controllerFlickable.contentY * visualiserContainer.scale;
                    width: Settings.theme.adjustedPixel(190);
                    height: visualiserContainer.scale * controllerFlickable.height;
                    color: "#00adf5"
                    opacity: 0.4;
                }
            }
        }
    }*/

    Item {
        id: pageNumber;
        anchors {
            right: parent.right;
            bottom: parent.bottom;
            margins: Kirigami.Units.largeSpacing;
        }
        opacity: 0;
        Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
        height: Constants.GridHeight / 2;
        width: Constants.GridWidth;
        Rectangle {
            anchors.fill: parent;
            radius: Kirigami.Units.largeSpacing;
            color: Settings.theme.color("components/overlay/base");
            opacity: 0.7;
        }
        QQC2.Label {
            anchors.centerIn: parent;
            color: Settings.theme.color("components/overlay/text");
            text: (wordsDocument === null) ? 0 : wordsDocument.currentIndex + " of " + wordsDocument.indexCount;
        }
    }
    Rectangle {
        id: zoomLevel;
        radius: Kirigami.Units.largeSpacing;
        color: Kirigami.Units.alternateBackgroundColor
        opacity: 0;
        anchors {
            right: parent.right;
            bottom: (pageNumber.opacity > 0) ? pageNumber.top : parent.bottom;
            margins: Kirigami.Units.largeSpacing;
        }
        Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
        height: zoomLabel.implicitHeight + Kirigami.Units.largeSpacing
        width: Math.max(Kirigami.Units.gridUnit * 4, zoomLabel)
        Timer {
            id: hideZoomLevelTimer;
            repeat: false; running: false; interval: 1000;
            onTriggered: zoomLevel.opacity = 0;
        }
        QQC2.Label {
            id: zoomLabel
            anchors.centerIn: parent;
            text: wordsCanvas.zoomAction ? (wordsCanvas.zoomAction.effectiveZoom * 100).toFixed(2) + "%" : "";
            onTextChanged: {
                zoomLevel.opacity = 1;
                hideZoomLevelTimer.start();
            }
        }
    }

    Connections {
        target: applicationWindow()
        onWidthChanged: thumbnailSidebar.modal = width < Kirigami.Units.gridUnit * 50
    }

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
        contentItem: QQC2.ScrollView {
            implicitWidth: Kirigami.Units.gridUnit * 15
            ListView {
                id: navigatorListView;
                clip: true;
                model: wordsContentModel;
                delegate: QQC2.ItemDelegate {
                    onClicked: {
                        wordsDocument.currentIndex = model.contentIndex;
                        // controllerFlickable.contentY = wordsCanvas.pagePosition(index + 1) + 1;
                        base.canvasInteractionStarted();
                    }
                    width: navigatorListView.width
                    contentItem: ColumnLayout {
                        Calligra.ImageDataItem {
                            id: navigatorThumbnail;
                            Layout.preferredWidth: Kirigami.Units.gridUnit * 11
                            Layout.alignment: Qt.AlignHCenter
                            Layout.fillHeight: true
                            data: model.thumbnail;
                        }
                        QQC2.Label {
                            Layout.fillWidth: true
                            text: index + 1;
                        }
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
