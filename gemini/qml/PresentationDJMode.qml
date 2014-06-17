/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

import QtQuick 1.1
import "components"
import org.calligra 1.0
import org.calligra.CalligraComponents 0.1

Rectangle {
    id: base;
    //property alias canvas: presentationModel.canvas;
    color: "#22282f";
    Item {
        id: currentSlideContainer;
        anchors {
            fill: parent;
            topMargin: Constants.GridHeight + Constants.DefaultMargin;
            leftMargin: Constants.DefaultMargin;
            rightMargin: parent.width / 3;
            bottomMargin: Constants.GridHeight + Constants.DefaultMargin;
        }
        Rectangle {
            anchors.fill: parent;
            opacity: 0.6;
        }
        Thumbnail {
            anchors.fill: parent;
            content: presentationModel.thumbnail(0);
        }
        Image {
            id: fakePieChart;
            opacity: 0;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
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
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
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
            id: scribbler;
            anchors.fill: parent;
            opacity: 0;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
            Timer {
                id: scribbleTimer;
                repeat: false; interval: Constants.AnimationDuration;
                onTriggered: scribbler.clear();
            }
        }
    }
    Item {
        id: nextSlideContainer;
        anchors {
            top: parent.top;
            topMargin: Constants.GridHeight + Constants.DefaultMargin;
            left: currentSlideContainer.right;
            leftMargin: Constants.DefaultMargin;
            right: parent.right;
            rightMargin: Constants.DefaultMargin;
        }
        height: width * (currentSlideContainer.height / currentSlideContainer.width);
        Rectangle {
            anchors.fill: parent;
            opacity: 0.6;
        }
        Thumbnail {
            anchors.fill: parent;
            content: presentationModel.thumbnail(1);
        }
    }
    Button {
        anchors {
            top: parent.top;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        width: Constants.GridWidth * 2
        text: "End Presentation"
        textColor: "white";
        onClicked: mainPageStack.pop();
    }
    Label {
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        font: Settings.theme.font("pageHeader");
        color: "white";
        Timer {
            interval: 500;
            running: true;
            repeat: true;
            triggeredOnStart: true;
            onTriggered: parent.text = Qt.formatTime(new Date(), "hh:mm:ss");
        }
    }
    Row {
        anchors {
            left: parent.left;
            bottom: parent.bottom;
            margins: Constants.DefaultMargin;
        }
        height: Constants.GridHeight;
        spacing: Constants.DefaultMargin;
        Button {
            height: parent.height;
            width: height;
            image: Settings.theme.icon("SVG-Icon-DesktopMode-1");
            checked: fakePieChart.opacity === 1 || fakePieChartTapped.opacity === 1;
            radius: 4;
            onClicked: {
                if(fakePieChart.opacity === 0) {
                    fakePieChart.opacity = 1;
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
            image: Settings.theme.icon("SVG-Label-Red-1");
        }
        Button {
            height: parent.height;
            width: height;
            image: Settings.theme.icon("edit");
            checked: scribbler.opacity === 1;
            radius: 4;
            onClicked: {
                if(scribbler.opacity === 0) {
                    scribbler.opacity = 1;
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
            margins: Constants.DefaultMargin;
        }
        height: Constants.GridHeight;
        spacing: Constants.DefaultMargin;
        Button {
            height: parent.height;
            width: height;
            image: Settings.theme.icon("back");
        }
        Button {
            height: parent.height;
            width: height;
            image: Settings.theme.icon("forward");
        }
    }
    Button {
        anchors {
            right: parent.right;
            bottom: parent.bottom;
            margins: Constants.DefaultMargin;
        }
        height: Constants.GridHeight;
        width: height;
        text: "(fx)";
        onClicked: base.state = "sidebarShown";
    }
    states: State {
        name: "sidebarShown"
        AnchorChanges { target: fxSidebar; anchors.left: undefined; anchors.right: parent.right; }
    }
    transitions: Transition {
        AnchorAnimation { duration: Constants.AnimationDuration; }
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
                Image {
                    anchors.centerIn: parent;
                    height: Constants.GridHeight * 1.5;
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
                        margins: Constants.DefaultMargin;
                    }
                    height: font.pixelSize;
                    horizontalAlignment: Text.AlignHCenter;
                    verticalAlignment: Text.AlignVCenter;
                    color: "#c1cdd1";
                    font: Settings.theme.font("small");
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
