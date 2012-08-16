/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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
import "../components"

Panel {
    id: base;
    name: "Presets";
    panelColor: "#1e0058"

    actions: [
        Button {
            id: addButton;
            anchors.top: parent.top
            anchors.left: parent.left

            width: Constants.GridWidth / 2
            height: Constants.GridHeight;

            color: "transparent";
            image: ":/images/svg/icon-add.svg"
            textColor: "white";
            shadow: false;
            highlight: false;
            onClicked: base.toggleEdit();

            states: State {
                name: "edit"
                when: base.state == "edit";

                PropertyChanges { target: addButton; text: "Cancel"; color: Constants.Theme.NegativeColor; width: Constants.GridWidth * 2; }
            }

            transitions: Transition {
                to: "edit";
                reversible: true;

                ParallelAnimation {
                    NumberAnimation { target: addButton; properties: "width"; duration: 250; }
                    ColorAnimation { target: addButton; properties: "color"; duration: 250; }
                }
            }
        },
        Button {
            id: editButton;
            anchors.top: parent.top
            anchors.right: parent.right

            width: Constants.GridWidth / 2
            height: Constants.GridHeight;

            text: ""
            image: ":/images/svg/icon-edit.svg"
            color: "transparent";
            textColor: "white";
            shadow: false;
            highlight: false;
            onClicked: base.toggleEdit();

            states: State {
                name: "edit"
                when: base.state == "edit";

                PropertyChanges { target: editButton; text: "Save"; color: Constants.Theme.PositiveColor; width: Constants.GridWidth * 2; }
            }

            transitions: Transition {
                to: "edit";
                reversible: true;

                ParallelAnimation {
                    NumberAnimation { target: editButton; properties: "width"; duration: 250; }
                    ColorAnimation { target: editButton; properties: "color"; duration: 250; }
                }
            }
        }
    ]

    peekContents: GridView {
        x: 0
        y: 0
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        keyNavigationWraps: false

        model: PresetsModel;
        delegate: delegate;

        cellWidth: (Constants.GridWidth / 2) - 8
        cellHeight: (Constants.GridHeight * 1) - 8
    }

    fullContents: PageStack {
        id: contentArea;
        anchors.fill: parent;
        initialPage: GridView {
            anchors.fill: parent;

            model: PresetsModel;
            delegate: delegate;

            cellWidth: Constants.GridWidth;
            cellHeight: Constants.GridHeight;
        }
    }

    onStateChanged: if( state != "edit" && contentArea.depth > 1 ) {
        contentArea.pop();
    }

    ListModel {
        id: model;

        ListElement { text: "Red"; icon: ":/images/red.png"; }
        ListElement { text: "Green"; icon: ":/images/green.png"; }
        ListElement { text: "Blue"; icon: ":/images/blue.png"; }
    }

    Component {
        id: delegate;

        Button {
            width: Constants.GridWidth;
            height: Constants.GridHeight;

            checked: GridView.isCurrentItem;

            text: model.text;
            shadow: false
            textSize: 10;
            image: model.image;

            highlightColor: Constants.Theme.HighlightColor;

            onClicked: {
                GridView.view.currentIndex = index;
                Settings.currentPreset = model.image;
            }
        }
    }

    function toggleEdit() {
        if( base.state == "edit" ) {
            base.state = "full";
            contentArea.pop();
        } else if( base.state == "full" ) {
            base.state = "edit";
            contentArea.push( editPresetPage );
        }
    }

    Component { id: editPresetPage; EditPresetPage { } }
}
