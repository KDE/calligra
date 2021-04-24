/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import org.kde.kirigami 2.7 as Kirigami
import "components"
import org.calligra 1.0
import org.kde.calligra 1.0 as Calligra

Rectangle {
    id: base;
    color: "#22282f";
    property int currentSlide: 0;
    property int timePassed: 0;
    Connections {
        target: pageStack.layers;
        onCurrentPageChanged: timePassed = 0;
    }
    function changeSlide(goToSlide) {
        fakePieChart.opacity = 0;
        fakePieChartTapped.opacity = 0;
        laserScribbler.opacity = 1;
        laserScribbler.clear();
        scribbler.opacity = 0;
        scribbler.clear();
        if(mainWindow.lastScreen() > 0) {
            mainWindow.desktopKoView.presentationMode().navigateToPage(goToSlide);
        }
        base.currentSlide = goToSlide;
    }
    Item {
        id: currentSlideContainer;
        anchors {
            fill: parent;
            topMargin: Settings.theme.adjustedPixel(166);
            leftMargin: Settings.theme.adjustedPixel(10);
            rightMargin: parent.width / 3;
            bottomMargin: Settings.theme.adjustedPixel(200);
        }
        Rectangle {
            anchors.fill: parent;
            opacity: 0.6;
        }
        Calligra.ImageDataItem {
//             id: thumbnail;
            anchors.fill: parent;
//             anchors.horizontalCenter: parent.horizontalCenter;

//             width: parent.ListView.view.thumbnailWidth;
//             height: parent.ListView.view.thumbnailHeight;

            data: presentationModel.thumbnail(currentSlide);
            visible: fakePieChart.opacity === 0 && fakePieChartTapped.opacity === 0;
        }
//         Thumbnail {
//             anchors.fill: parent;
//             content: presentationModel.thumbnail(currentSlide);
//         }
        Image {
            id: fakePieChart;
            opacity: 0;
            Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
            anchors.fill: parent;
            fillMode: Image.PreserveAspectFit
            source: Settings.theme.image("intel-Stage-Slide-PieChart-Default.svg");
            MouseArea {
                anchors.fill: parent;
                enabled: parent.opacity > 0;
                onClicked: fakePieChartTapped.opacity = 1;
            }
        }
        Image {
            id: fakePieChartTapped;
            opacity: 0;
            Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
            anchors.fill: parent;
            fillMode: Image.PreserveAspectFit
            source: Settings.theme.image("intel-Stage-Slide-PieChart-PieceTappedOn.svg");
            MouseArea {
                anchors.fill: parent;
                enabled: parent.opacity > 0;
                onClicked: fakePieChartTapped.opacity = 0;
            }
        }
        ScribbleArea {
            id: laserScribbler;
            anchors.fill: parent;
            opacity: 1;
            color: "#ff0000";
            penWidth: 10;
            Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
            onPaintingStarted: laserTimer2.stop();
            onPaintingStopped: laserTimer2.start();
            Timer {
                id: laserTimer;
                repeat: false; interval: Kirigami.Units.shortDuration;
                onTriggered: laserScribbler.clear();
            }
            Timer {
                id: laserTimer2;
                repeat: false; interval: 1000;
                onTriggered: laserScribbler.clear();
            }
        }
        ScribbleArea {
            id: scribbler;
            anchors.fill: parent;
            opacity: 0;
            color: "#dff03c";
            penWidth: 40;
            Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
            Timer {
                id: scribbleTimer;
                repeat: false; interval: Kirigami.Units.shortDuration;
                onTriggered: scribbler.clear();
            }
        }
    }
    Item {
        id: nextSlideContainer;
        anchors {
            top: parent.top;
            topMargin: Settings.theme.adjustedPixel(166);
            left: currentSlideContainer.right;
            leftMargin: Settings.theme.adjustedPixel(10);
            right: parent.right;
            rightMargin: Settings.theme.adjustedPixel(10);
        }
        height: width * (currentSlideContainer.height / currentSlideContainer.width);
        Rectangle {
            anchors.fill: parent;
            opacity: 0.6;
        }
        Calligra.ImageDataItem {
//             id: thumbnail;
            anchors.fill: parent;
//             anchors.horizontalCenter: parent.horizontalCenter;

//             width: parent.ListView.view.thumbnailWidth;
//             height: parent.ListView.view.thumbnailHeight;

            data: presentationModel.thumbnail(currentSlide + 1);
//             visible: fakePieChart.opacity === 0 && fakePieChartTapped.opacity === 0;
        }
//         Thumbnail {
//             anchors.fill: parent;
//             content: presentationModel.thumbnail(currentSlide + 1);
//         }
        MouseArea {
            anchors.fill: parent;
            onClicked: {
                if(base.currentSlide < presentationModel.canvas.slideCount()) {
                    base.changeSlide(base.currentSlide + 1);
                }
            }
        }
    }
    Image {
        anchors {
            top: parent.top;
            right: parent.right;
            rightMargin: Settings.theme.adjustedPixel(20);
        }
        height: Settings.theme.adjustedPixel(166);
        width: Settings.theme.adjustedPixel(136);
        source: Settings.theme.icon("SVG-Exit-1");
        fillMode: Image.PreserveAspectFit
        MouseArea {
            anchors.fill: parent;
            onClicked: {
                mainWindow.desktopKoView.stopPresentation();
                pageStack.layers.pop();
                if(mainWindow.fullScreen === true) {
                    mainWindow.fullScreen = false;
                }
            }
        }
    }
    Label {
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;
        height: Settings.theme.adjustedPixel(166)
        font: Settings.theme.font("presentationTime");
        color: "#c1cdd1";
        Timer {
            interval: 1000;
            running: true;
            repeat: true;
            triggeredOnStart: true;
            function offset() {return new Date().getTimezoneOffset()*60000}
            onTriggered: {
                base.timePassed = base.timePassed + 1000;
                parent.text = Qt.formatTime(new Date(base.timePassed + offset()), "hh:mm:ss");
            }
        }
    }
    Row {
        anchors {
            left: parent.left;
            bottom: parent.bottom;
            margins: Kirigami.Units.largeSpacing;
        }
        height: Settings.theme.adjustedPixel(136);
        spacing: Kirigami.Units.largeSpacing;
        Button {
            height: parent.height;
            width: height;
            image: Settings.theme.icon("SVG-Nudge-1");
            checked: fakePieChart.opacity === 1 || fakePieChartTapped.opacity === 1;
            checkedColor: "#00adf5"; checkedOpacity: 0.6;
            radius: 8;
            onClicked: {
                if(fakePieChart.opacity === 0) {
                    fakePieChart.opacity = 1;
                    laserScribbler.opacity = 0;
                    scribbler.opacity = 0;
                    scribbleTimer.start();
                }
                else {
                    fakePieChart.opacity = 0;
                    fakePieChartTapped.opacity = 0;
                }
            }
        }
        Button {
            height: parent.height;
            width: height;
            image: Settings.theme.icon("SVG-Pointer-1");
            checked: laserScribbler.opacity === 1;
            checkedColor: "#00adf5"; checkedOpacity: 0.6;
            radius: 8;
            onClicked: {
                if(laserScribbler.opacity === 0) {
                    fakePieChart.opacity = 0;
                    fakePieChartTapped.opacity = 0;
                    laserScribbler.opacity = 1;
                    scribbler.opacity = 0;
                    scribbleTimer.start();
                }
                else {
                    laserScribbler.opacity = 0;
                }
            }
        }
        Button {
            height: parent.height;
            width: height;
            image: Settings.theme.icon("SVG-Highlighter-1");
            checked: scribbler.opacity === 0.7;
            checkedColor: "#00adf5"; checkedOpacity: 0.6;
            radius: 8;
            onClicked: {
                if(scribbler.opacity === 0) {
                    fakePieChart.opacity = 0;
                    fakePieChartTapped.opacity = 0;
                    laserScribbler.opacity = 0;
                    scribbler.opacity = 0.7;
                    scribbleTimer.stop();
                }
                else {
                    scribbler.opacity = 0;
                    scribbleTimer.start();
                }
            }
        }
    }
    Row {
        anchors {
            horizontalCenter: parent.horizontalCenter;
            bottom: parent.bottom;
            margins: Kirigami.Units.largeSpacing;
        }
        height: Settings.theme.adjustedPixel(136);
        spacing: Kirigami.Units.largeSpacing;
        Button {
            height: parent.height;
            width: height;
            image: Settings.theme.icon("SVG-PreviousSlide-1");
            onClicked: {
                if(base.currentSlide > 0) {
                    base.changeSlide(base.currentSlide - 1);
                }
            }
        }
        Button {
            height: parent.height;
            width: height;
            image: Settings.theme.icon("SVG-NextSlide-1");
            onClicked: {
                if(base.currentSlide < presentationModel.canvas.slideCount()) {
                    base.changeSlide(base.currentSlide + 1);
                }
            }
        }
    }
    Button {
        anchors {
            right: parent.right;
            bottom: parent.bottom;
            margins: Kirigami.Units.largeSpacing;
        }
        height: Settings.theme.adjustedPixel(136);
        width: height;
        image: Settings.theme.icon("SVG-FXTransition-1");
        onClicked: base.state = "sidebarShown";
    }
    states: State {
        name: "sidebarShown"
        AnchorChanges { target: fxSidebar; anchors.left: undefined; anchors.right: parent.right; }
    }
    transitions: Transition {
        AnchorAnimation { duration: Kirigami.Units.shortDuration; easing.type: Easing.InOutQuad; }
    }
    Item {
        id: fxSidebar;
        anchors {
            top: parent.top;
            left: parent.right;
            bottom: parent.bottom;
        }
        Rectangle {
            anchors.fill: parent;
            color: "#22282f";
            opacity: 0.96;
        }
        Rectangle {
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            height: Constants.GridHeight;
            color: "#4e5359";
            Label {
                anchors.fill: parent;
                horizontalAlignment: Text.AlignHCenter;
                verticalAlignment: Text.AlignVCenter;
                color: "#c1cdd1";
                font: Settings.theme.font("small");
                text: "SELECT A NEW\nTRANSITION EFFECT";
            }
            Rectangle {
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.bottom;
                }
                height: 1;
                color: "#c1cccd";
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: base.state = "";
            }
        }
        width: Constants.GridWidth * 2;
        Rectangle {
            anchors {
                top: parent.top;
                left: parent.left;
                bottom: parent.bottom;
            }
            width: 1;
            color: "white";
        }
        ListView {
            anchors {
                fill: parent;
                topMargin: Constants.GridHeight;
                leftMargin: 1;
            }
            clip: true;
            model: ListModel {
                ListElement { text: "Fade"; }
                ListElement { text: "Dissolve"; }
                ListElement { text: "Slide in"; }
                ListElement { text: "Bounce"; }
                ListElement { text: "Fan"; }
                ListElement { text: "Snake"; }
                ListElement { text: "Spiral"; }
                ListElement { text: "Swap effect"; }
            }
            delegate: Item {
                height: Constants.GridHeight * 2;
                width: ListView.view.width;
                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        base.state = "";
                        if(base.currentSlide < presentationModel.canvas.slideCount()) {
                            base.changeSlide(base.currentSlide + 1);
                        }
                    }
                    Rectangle {
                        anchors.fill: parent;
                        opacity: parent.pressed ? 0.6 : 0;
                        Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
                        radius: Settings.theme.adjustedPixel(8);
                        color: "#00adf5";
                    }
                }
                Image {
                    anchors.centerIn: parent;
                    height: Constants.GridHeight * 1.3;
                    width: height;
                    source: Settings.theme.icon("SVG-Icon-Animations-1");
                    sourceSize.width: width > height ? height : width;
                    sourceSize.height: width > height ? height : width;
                }
                Label {
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        bottom: parent.bottom;
                        margins: Kirigami.Units.largeSpacing;
                    }
                    height: font.pixelSize;
                    horizontalAlignment: Text.AlignHCenter;
                    verticalAlignment: Text.AlignVCenter;
                    color: "#c1cdd1";
                    font: Settings.theme.font("presentationFx");
                    text: model.text;
                }
                Rectangle {
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        bottom: parent.bottom;
                    }
                    height: 1;
                    color: "#c1cccd";
                }
            }
        }
    }
}
