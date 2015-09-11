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

import QtQuick 2.0
import QtQuick.Controls 1.3
import org.calligra 1.0
import "components"
import "welcomepages"

Page {
    id: base;
    DocumentListModel { id: allDocumentsModel; }
    DocumentListModel { id: textDocumentsModel; filter: DocumentListModel.TextDocumentType; }
    DocumentListModel { id: presentationDocumentsModel; filter: DocumentListModel.PresentationType; }
    Component.onCompleted: {
        if(RecentFileManager.size() > 0) {
            for(var i = 1; i < sidebarList.count; i++) {
                sidebarList.setProperty(i, "selected", false);
            }
            sidebarList.setProperty(0, "selected", true);
            welcomeStack.replace(welcomePageRecent);
        }
    }
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
                text: "Calligra Gemini";
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
                ListElement { text: "Cloud"; icon: "SVG-Icon-Cloud-1"; selected: false; stackComponent: "welcomePageCloud"; }

                ListElement { header: "COMPOSE NEW"; text: "Document"; icon: "SVG-Icon-NewDocument-1"; selected: false; stackComponent: "welcomePageWords"; }
                ListElement { text: "Presentation"; icon: "SVG-Icon-NewPresentation-1"; selected: false; stackComponent: "welcomePageStage"; }
                //ListElement { text: "Spreadsheet"; icon: "SVG-Icon-NewSpreadsheet-1"; selected: false; stackComponent: "welcomePageCustom"; }
                //ListElement { text: "Sketch"; icon: "SVG-Icon-NewSketch-1"; selected: false; stackComponent: "welcomePageCustom"; }
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
                                    "welcomePageCustom": welcomePageCustom,
                                    "welcomePageCloud": welcomePageCloud
                                };
                                return elements[name];
                            }
                            onClicked: {
                                console.debug("boop! " + index);
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
        StackView {
            id: welcomeStack;
            clip: true;
            anchors {
                top: parent.top;
                left: sidebar.right;
                leftMargin: Constants.DefaultMargin * 3;
                right: parent.right;
                bottom: parent.bottom;
            }
            initialItem: welcomePageFilebrowser;
        }
        Component { id: welcomePageFilebrowser; WelcomePageFilebrowser { } }
        Component { id: welcomePageRecent; WelcomePageRecent { } }
        Component { id: welcomePageStage; WelcomePageStage { } }
        Component { id: welcomePageWords; WelcomePageWords { } }
        Component { id: welcomePageCustom; WelcomePageCustom { } }
        Component { id: welcomePageCloud; WelcomePageCloud { } }

        Component { id: mainPage; MainPage { } }
    }
    VariantSelector { id: variantSelector; }
    VariantSelector { id: wordsVariantSelector; selectorType: "words"; }
}
