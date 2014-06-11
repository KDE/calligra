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
            ListElement { text: "File Browser"; icon: "SVG-Icon-PlayPresentation-1"; selected: true; stackComponent: "welcomePageFilebrowser"; }
            ListElement { text: "Recent Documents"; icon: "SVG-Icon-PlayPresentation-1"; selected: false; stackComponent: "welcomePageRecent"; }
            ListElement { text: "Stage Templates"; icon: "SVG-Icon-PlayPresentation-1"; selected: false; stackComponent: "welcomePageStage"; }
            ListElement { text: "Words Templates"; icon: "SVG-Icon-PlayPresentation-1"; selected: false; stackComponent: "welcomePageWords"; }
            ListElement { text: "Custom"; icon: "SVG-Icon-PlayPresentation-1"; selected: false; stackComponent: "welcomePageCustom"; }
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
                height: Constants.GridHeight;
                Rectangle {
                    anchors.fill: parent;
                    color: "#00adf5";
                    opacity: model.selected ? 0.6 : 0;
                    PropertyAnimation on opacity { duration: Constants.AnimationDuration; }
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
    PageStack {
        id: welcomeStack;
        clip: true;
        anchors {
            top: parent.top;
            left: sidebar.right;
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
