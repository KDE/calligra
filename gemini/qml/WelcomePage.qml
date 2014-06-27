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
import org.calligra 1.0
import "components"
import "welcomepages"

Page {
    id: base;
    DocumentListModel { id: allDocumentsModel; }
    DocumentListModel { id: textDocumentsModel; filter: DocumentListModel.TextDocumentType; }
    DocumentListModel { id: presentationDocumentsModel; filter: DocumentListModel.PresentationType; }
    Item {
        id: welcomeToolbar;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: Settings.theme.adjustedPixel(86);
        Rectangle {
            anchors.fill: parent;
            color: "#f2b200";
        }
        Image {
            anchors {
                top: parent.top;
                left: parent.left;
                bottom: parent.bottom;
                margins: Constants.DefaultMargin;
            }
            width: height;
            source: Settings.theme.icon("Calligra-MockIcon-1");
            sourceSize.width: width > height ? height : width;
            sourceSize.height: width > height ? height : width;
            Label {
                anchors {
                    left: parent.right;
                    leftMargin: Constants.DefaultMargin;
                    verticalCenter: parent.verticalCenter;
                }
                text: "Calligra Suite";
                font: Settings.theme.font("welcomeHeader");
                color: "white";
            }
        }
        Image {
            id: settingsButton;
            anchors {
                right: parent.right;
                rightMargin: Settings.theme.adjustedPixel(19);
                verticalCenter: parent.verticalCenter;
            }
            width: Settings.theme.adjustedPixel(48);
            height: width;
            source: Settings.theme.icon("SVG-Icon-OptionsWhite-1");
            sourceSize.width: width > height ? height : width;
            sourceSize.height: width > height ? height : width;
        }
        Rectangle {
            anchors {
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            height: 1;
            color: "black";
            opacity: 0.1;
        }
    }
    Item {
        anchors {
            top: welcomeToolbar.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
        Item {
            id: sidebar;
            anchors {
                top: parent.top;
                left: parent.left;
                bottom: parent.bottom;
            }
            width: parent.width / 5;
            ListModel {
                id: sidebarList;
                ListElement { header: "OPEN"; text: "Recent Documents"; icon: "SVG-Icon-RecentDocuments-1"; selected: false; stackComponent: "welcomePageRecent"; }
                ListElement { text: "Library"; icon: "SVG-Icon-MyDocuments-1"; selected: true; stackComponent: "welcomePageFilebrowser"; }
                ListElement { text: "Cloud"; icon: "SVG-Icon-Cloud-1"; selected: false; stackComponent: "welcomePageFilebrowser"; }

                ListElement { header: "COMPOSE NEW"; text: "Document"; icon: "SVG-Icon-NewDocument-1"; selected: false; stackComponent: "welcomePageWords"; }
                ListElement { text: "Presentation"; icon: "SVG-Icon-NewPresentation-1"; selected: false; stackComponent: "welcomePageStage"; }
                ListElement { text: "Spreadsheet"; icon: "SVG-Icon-NewSpreadsheet-1"; selected: false; stackComponent: "welcomePageCustom"; }
                ListElement { text: "Sketch"; icon: "SVG-Icon-NewSketch-1"; selected: false; stackComponent: "welcomePageCustom"; }
            }
            Rectangle {
                anchors.fill: parent;
                color: "#e8e9ea";
            }
            ListView {
                anchors.fill: parent;
                clip: true;
                model: sidebarList;
                delegate: Item {
                    width: ListView.view.width;
                    height: model.header ? Constants.GridHeight * 1.5 : Constants.GridHeight;
                    Item {
                        id: delegateHeader;
                        height: model.header ? Constants.GridHeight / 2 : 0;
                        width: parent.width;
                        Label {
                            anchors {
                                fill: parent;
                                leftMargin: Constants.DefaultMargin;
                            }
                            verticalAlignment: Text.AlignBottom;
                            horizontalAlignment: Text.AlignLeft;
                            text: model.header ? model.header : "";
                            font: Settings.theme.font("filelistheader");
                            color: "#f2b200";
                        }
                    }
                    Item {
                        anchors.top: delegateHeader.bottom;
                        width: parent.width;
                        height: Constants.GridHeight;
                        Rectangle {
                            anchors.fill: parent;
                            color: "#00adf5";
                            opacity: model.selected ? 0.6 : 0;
                            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
                        }
                        Image {
                            anchors {
                                left: parent.left;
                                leftMargin: Constants.DefaultMargin;
                                verticalCenter: parent.verticalCenter;
                            }
                            height: parent.height - Constants.DefaultMargin * 2;
                            width: height;
                            source: Settings.theme.icon(model.icon);
                            sourceSize.width: width > height ? height : width;
                            sourceSize.height: width > height ? height : width;
                        }
                        Label {
                            anchors {
                                left: parent.left;
                                leftMargin: parent.height;
                                verticalCenter: parent.verticalCenter;
                            }
                            text: model.text;
                            font: Settings.theme.font("welcomeSidebar");
                        }
                        MouseArea {
                            anchors.fill: parent;
                            function elementFromName(name) {
                                var elements = {
                                    "welcomePageFilebrowser": welcomePageFilebrowser,
                                    "welcomePageRecent": welcomePageRecent,
                                    "welcomePageStage": welcomePageStage,
                                    "welcomePageWords": welcomePageWords,
                                    "welcomePageCustom": welcomePageCustom
                                };
                                return elements[name];
                            }
                            onClicked: {
                                if(model.selected) {
                                    return;
                                }
                                for(var i = 0; i < sidebarList.count; i++) {
                                    sidebarList.setProperty(i, "selected", false);
                                }
                                sidebarList.setProperty(index, "selected", true);
                                welcomeStack.replace(elementFromName(model.stackComponent));
                            }
                        }
                    }
                }
            }
            Rectangle {
                anchors {
                    top: parent.top;
                    right: parent.right;
                    bottom: parent.bottom;
                }
                width: 1;
                color: "black";
                opacity: 0.1;
            }
        }
        Rectangle {
            anchors {
                top: parent.top;
                left: sidebar.right;
                right: parent.right;
                bottom: parent.bottom;
            }
            color: "#e8e9ea";
        }
        PageStack {
            id: welcomeStack;
            clip: true;
            anchors {
                top: parent.top;
                left: sidebar.right;
                leftMargin: Constants.DefaultMargin * 3;
                right: parent.right;
                bottom: parent.bottom;
            }
            initialPage: welcomePageFilebrowser;
        }
        Component { id: welcomePageFilebrowser; WelcomePageFilebrowser { } }
        Component { id: welcomePageRecent; WelcomePageRecent { } }
        Component { id: welcomePageStage; WelcomePageStage { } }
        Component { id: welcomePageWords; WelcomePageWords { } }
        Component { id: welcomePageCustom; WelcomePageCustom { } }

        Component { id: mainPage; MainPage { } }
    }
    Item {
        id: variantSelector;
        property alias model: variantView.model;
        anchors.fill: parent;
        opacity: 0;
        Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
        MouseArea { anchors.fill: parent; onClicked: {} }
        SimpleTouchArea { anchors.fill: parent; onTouched: {} }
        Rectangle {
            anchors.fill: parent;
            color: "#e8e9ea"
            opacity: 0.6;
        }
        Item {
            anchors {
                centerIn: parent;
                topMargin: -(height/2);
            }
            height: Settings.theme.adjustedPixel(475);
            width: Settings.theme.adjustedPixel(1600);
            Rectangle {
                anchors.fill: parent;
                color: "#22282f";
                opacity: 0.7;
                radius: 8;
            }
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
            Label {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                }
                height: Settings.theme.adjustedPixel(120);
                horizontalAlignment: Text.AlignHCenter;
                verticalAlignment: Text.AlignVCenter;
                color: "white";
                font: Settings.theme.font("title");
                text: "Select a Style Option";
            }
            Button {
                anchors {
                    top: parent.top;
                    right: parent.right;
                    margins: Settings.theme.adjustedPixel(20);
                }
                height: Settings.theme.adjustedPixel(40);
                width: height;
                image: Settings.theme.icon("SVG-Icon-SmallX");
                onClicked: variantSelector.opacity = 0;
            }
            Flickable {
                id: variantFlickable;
                anchors {
                    fill: parent;
                    topMargin: Settings.theme.adjustedPixel(120);
                    leftMargin: Settings.theme.adjustedPixel(50);
                    rightMargin: Settings.theme.adjustedPixel(50);
                    bottomMargin: Settings.theme.adjustedPixel(40);
                }
                clip: true;
                contentHeight: variantFlow.height;
                contentWidth: variantFlow.width;
                Flow {
                    id: variantFlow;
                    width: variantFlickable.width;
                    Repeater {
                        id: variantView;
                        delegate: Item {
                            height: Settings.theme.adjustedPixel(310);
                            width: Settings.theme.adjustedPixel(375);
                            MouseArea {
                                anchors.fill: parent;
                                onClicked: {
                                    variantSelector.opacity = 0;
                                    var file = Settings.stageTemplateLocation(model.templateFile);
                                    if(file.slice(-1) === "/" || file === "") {
                                        return;
                                    }
                                    baseLoadingDialog.visible = true;
                                    openFile(file);
                                }
                                Rectangle {
                                    anchors.fill: parent;
                                    opacity: parent.pressed ? 0.6 : 0;
                                    Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
                                    radius: Settings.theme.adjustedPixel(8);
                                    color: "#00adf5";
                                }
                            }
                            Image {
                                anchors {
                                    top: parent.top;
                                    left: parent.left;
                                    right: parent.right;
                                    margins: Settings.theme.adjustedPixel(16);
                                }
                                height: Settings.theme.adjustedPixel(192);
                                source: Settings.theme.icon(model.thumbnail);
                                fillMode: Image.PreserveAspectFit
                                smooth: true;
                            }
                            Image {
                                anchors {
                                    horizontalCenter: parent.horizontalCenter;
                                    bottom: parent.bottom;
                                    margins: Settings.theme.adjustedPixel(16);
                                }
                                height: Settings.theme.adjustedPixel(64);
                                width: Settings.theme.adjustedPixel(190);
                                source: Settings.theme.icon(model.swatch);
                                fillMode: Image.PreserveAspectFit
                                smooth: true;
                            }
                        }
                    }
                }
            }
            ScrollDecorator { flickableItem: variantFlickable; anchors.fill: variantFlickable; }
        }
    }
}
