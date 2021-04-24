/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.11
import org.kde.kirigami 2.7 as Kirigami
import "components"
import org.kde.calligra 1.0 as Calligra

Item {
    id: base;
    signal canvasInteractionStarted();
    property alias document: wordsDocument;
    property alias textEditor: wordsDocument.textEditor;
    property QtObject canvas: wordsCanvas;
    property alias source: wordsDocument.source;
    property alias navigateMode: controllerFlickable.enabled;
    property double toolbarOpacity: base.state === "readermode" ? 0.3 : 1;
    Calligra.Document {
        id: wordsDocument;
        onStatusChanged: {
            if(status == Calligra.DocumentStatus.Loading) {
                baseLoadingDialog.visible = true;
            }
            else if(status == Calligra.DocumentStatus.Loaded) {
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
        thumbnailSize: Qt.size(Settings.theme.adjustedPixel(280), Settings.theme.adjustedPixel(360));
    }
    onNavigateModeChanged: {
        if(navigateMode === true) {
            // This means we've just changed back from having edited stuff.
            // Consequently we want to deselect all selections. Tell the canvas about that.
            wordsCanvas.deselectEverything();
            toolManager.requestToolChange("PageToolFactory_ID");
        }
    }
    Connections {
        target: Constants;
        onGridSizeChanged: {
            var newState = (Constants.IsLandscape ? "" : "readermode");
            if(base.state !== newState) {
                base.state = newState;
            }
        }
    }
    onWidthChanged: {
        if(base.state === "readermode") {
            d.zoomToFit();
        }
    }
    onHeightChanged: {
        if(base.state === "readermode") {
            d.zoomToFit();
        }
    }
    function scrollToEnd() {
        controllerFlickable.contentY = controllerFlickable.contentHeight - controllerFlickable.height;
    }
    Rectangle {
        anchors.fill: parent;
        color: "#e8e9ea";
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
                property int swipeDistance: Settings.theme.adjustedPixel(10);
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
                        if(Math.abs(xDiff) > Settings.theme.adjustedPixel(2) || Math.abs(yDiff) > Settings.theme.adjustedPixel(2)) {
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
    }
    Binding {
        target: bgScrollArea;
        property: "contentY";
        value: controllerFlickable.contentY;
        when: bgScrollArea.verticalVelocity === 0;
    }
    Item {
        id: wordsContentItem;
        anchors {
            top: parent.top;
            bottom: parent.bottom;
            horizontalCenter: parent.horizontalCenter;
        }
        width: Math.min(wordsDocument.documentSize.width, base.width);

        Calligra.View {
            id: wordsCanvas;
            anchors.fill: parent;
            document: wordsDocument;
        }

        Flickable {
            id: controllerFlickable;
            anchors {
                top: parent.top;
                topMargin: Settings.theme.adjustedPixel(86);
                right: parent.right;
                bottom: enabled ? parent.bottom : parent.top;
                bottomMargin: enabled ? 0 : -Settings.theme.adjustedPixel(86);
            }
            width: Math.min(parent.width, base.width - navigatorHandle.width);
            interactive: base.state !== "readermode";
            property int fastVelocity: Settings.theme.adjustedPixel(1000);
            onVerticalVelocityChanged: {
                if(Math.abs(verticalVelocity) > fastVelocity && !controllerItem.pageChanging) {
                    d.showThings();
                }
                else {
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
                    controllerFlickable.contentY = Math.max(0, controllerFlickable.contentY - controllerFlickable.height + (Constants.GridHeight * 1.5));
                }
            }
            function pageDown() {
                if(base.state === "readermode") {
                    controllerFlickable.contentY = wordsCanvas.pagePosition(wordsDocument.currentIndex + 1) + 1;
                }
                else {
                    controllerFlickable.contentY = Math.min(controllerFlickable.contentHeight - controllerFlickable.height, controllerFlickable.contentY + controllerFlickable.height - (Constants.GridHeight * 1.5));
                }
            }

            Image {
                height: Settings.theme.adjustedPixel(40);
                width: Settings.theme.adjustedPixel(40);
                source: Settings.theme.icon("intel-Words-Handle-cursor");
                opacity: wordsCanvas.hasSelection ? 1 : 0;
                Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
                x: wordsCanvas.hasSelection ? wordsCanvas.selectionStartPos.x - width / 2 : 0;
                y: wordsCanvas.hasSelection ? wordsCanvas.selectionStartPos.y - (height - 4) : 0;
                Rectangle {
                    anchors {
                        top: parent.bottom;
                        horizontalCenter: parent.horizontalCenter;
                    }
                    height: wordsCanvas.hasSelection ? wordsCanvas.selectionStartPos.height - 4 : 0;
                    width: 4;
                    color: "#009bcd";
                }
            }
            Image {
                height: Settings.theme.adjustedPixel(40);
                width: Settings.theme.adjustedPixel(40);
                source: Settings.theme.icon("intel-Words-Handle-cursor");
                opacity: wordsCanvas.hasSelection ? 1 : 0;
                Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
                x: wordsCanvas.hasSelection ? wordsCanvas.selectionEndPos.x - width / 2 : 0;
                y: wordsCanvas.hasSelection ? wordsCanvas.selectionEndPos.y + (wordsCanvas.selectionEndPos.height - 4) : 0;
                Rectangle {
                    anchors {
                        bottom: parent.top;
                        horizontalCenter: parent.horizontalCenter;
                    }
                    height: wordsCanvas.hasSelection ? wordsCanvas.selectionEndPos.height - 4 : 0;
                    width: 4;
                    color: "#009bcd";
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
                        }
                        else if( Math.abs(xDiff) > Math.abs(yDiff) ) {
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
            if(navigatorSidebar.containsMouse) {
                return;
            }
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
            if(base.state === "readermode") {
                return;
            }
            base.state = "";
        }
    }
    states: [
        State {
            name: "sidebarShown"
            AnchorChanges { target: navigatorSidebar; anchors.left: parent.left; anchors.right: undefined; }
        },
        State {
            name: "readermode"
            PropertyChanges { target: navigatorSidebar; opacity: 0; }
        }
        ]
    transitions: [ Transition {
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
    Item {
        id: navigatorSidebar;
        property alias containsMouse: listViewMouseArea.containsMouse;
        anchors {
            top: parent.top;
            right: parent.left;
            bottom: parent.bottom;
            topMargin: Settings.theme.adjustedPixel(40) + Constants.ToolbarHeight;
            bottomMargin: Settings.theme.adjustedPixel(40);
        }
        width: Settings.theme.adjustedPixel(190);
        BorderImage {
            anchors {
                fill: parent;
                topMargin: -28;
                leftMargin: -36;
                rightMargin: -36;
                bottomMargin: -44;
            }
            border { left: 36; top: 28; right: 36; bottom: 44; }
            horizontalTileMode: BorderImage.Stretch;
            verticalTileMode: BorderImage.Stretch;
            source: Settings.theme.image("drop-shadows.png");
            opacity: (base.state === "sidebarShown") ? 1 : 0;
            Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
            BorderImage {
                anchors {
                    fill: parent;
                    topMargin: 28;
                    leftMargin: 36;
                    rightMargin: 36;
                    bottomMargin: 44;
                }
                border { left: 8; top: 8; right: 8; bottom: 8; }
                horizontalTileMode: BorderImage.Stretch;
                verticalTileMode: BorderImage.Stretch;
                source: Settings.theme.image("drop-corners.png");
            }
        }
        Item {
            id: navigatorHandle;
            anchors {
                left: parent.right;
                verticalCenter: parent.verticalCenter;
            }
            height: Constants.GridHeight * 2;
            width: Kirigami.Units.largeSpacing * 3;
            clip: true;
            Rectangle {
                anchors {
                    fill: parent;
                    leftMargin: -(radius + 1);
                }
                radius: Kirigami.Units.largeSpacing;
                color: "#55595e";
                opacity: 0.5;
            }
            Rectangle {
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                    margins: Kirigami.Units.largeSpacing;
                    horizontalCenter: parent.horizontalCenter;
                }
                width: 4;
                radius: 2;
                color: "white";
                opacity: 0.5;
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    if(base.state === "sidebarShown" && base.state !== "readermode") {
                        base.state = "";
                        pageNumber.opacity = 0;
                    }
                    else {
                        d.showThings();
                    }
                    base.canvasInteractionStarted();
                }
            }
        }
        Rectangle {
            anchors {
                fill: parent;
                leftMargin: -Kirigami.Units.largeSpacing + 1;
            }
            radius: Kirigami.Units.largeSpacing;
            color: "#55595e";
            opacity: 0.5;
            Rectangle {
                anchors.fill: parent;
                radius: parent.radius;
                color: "transparent";
                border.width: 1;
                border.color: "black";
                opacity: 0.6;
            }
        }
        MouseArea {
            id: listViewMouseArea;
            anchors.fill: parent;
            hoverEnabled: true;
        }
        Button {
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            height: Constants.GridHeight / 2;
            image: Settings.theme.icon("Arrow-ScrollUp-1");
            imageMargin: 2;
            Rectangle {
                anchors {
                    left: parent.left;
                    right: parent.right;
                    rightMargin: 1;
                    bottom: parent.bottom;
                }
                height: 1;
                color: "black";
                opacity: 0.3;
            }
        }
        Button {
            anchors {
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            height: Constants.GridHeight / 2;
            image: Settings.theme.icon("Arrow-ScrollDown-1");
            imageMargin: 2;
            Rectangle {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    rightMargin: 1;
                }
                height: 1;
                color: "black";
                opacity: 0.3;
            }
        }
        ListView {
            id: navigatorListView;
            anchors {
                fill: parent;
                topMargin: Constants.GridHeight / 2;
                bottomMargin: Constants.GridHeight / 2;
                rightMargin: 1;
            }
            clip: true;
            model: wordsContentModel;
            delegate: Item {
                width: Settings.theme.adjustedPixel(190);
                height: Settings.theme.adjustedPixel(190);
                Calligra.ImageDataItem {
                    id: navigatorThumbnail;
                    anchors {
                        top: parent.top;
                        right: parent.right;
                        bottom: parent.bottom;
                        margins: Settings.theme.adjustedPixel(5);
                    }
                    width: Settings.theme.adjustedPixel(140);
                    data: model.thumbnail;
                }
                Rectangle {
                    anchors.fill: navigatorThumbnail;
                    color: "transparent";
                    border.width: 1;
                    border.color: "black";
                    opacity: 0.1;
                }
                Label {
                    anchors {
                        left: parent.left;
                        leftMargin: Kirigami.Units.largeSpacing;
                        verticalCenter: parent.verticalCenter;
                    }
                    text: index + 1;
                    font: Settings.theme.font("applicationSemi");
                    color: "#c1cdd1";
                }
                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        wordsDocument.currentIndex = model.contentIndex;
//                         controllerFlickable.contentY = wordsCanvas.pagePosition(index + 1) + 1;
                        base.canvasInteractionStarted();
                    }
                }
            }
        }
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
    }
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
        Label {
            anchors.centerIn: parent;
            color: Settings.theme.color("components/overlay/text");
            text: (wordsDocument === null) ? 0 : wordsDocument.currentIndex + " of " + wordsDocument.indexCount;
        }
    }
    Item {
        id: zoomLevel;
        anchors {
            right: parent.right;
            bottom: (pageNumber.opacity > 0) ? pageNumber.top : parent.bottom;
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
        Timer {
            id: hideZoomLevelTimer;
            repeat: false; running: false; interval: 1000;
            onTriggered: zoomLevel.opacity = 0;
        }
        Label {
            anchors.centerIn: parent;
            color: Settings.theme.color("components/overlay/text");
            text: wordsCanvas.zoomAction ? (wordsCanvas.zoomAction.effectiveZoom * 100).toFixed(2) + "%" : "";
            onTextChanged: {
                zoomLevel.opacity = 1;
                hideZoomLevelTimer.start();
            }
        }
    }
}
