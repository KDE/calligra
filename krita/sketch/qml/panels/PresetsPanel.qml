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

    actions: [
        Button {
            id: addButton;

            width: Constants.GridWidth;
            height: Constants.GridHeight;

            text: "Add";
            color: "transparent";
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

            width: Constants.GridWidth;
            height: Constants.GridHeight;

            text: "Edit";
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
        anchors.fill: parent;

        model: model;
        delegate: delegate;

        cellWidth: Constants.GridWidth;
        cellHeight: Constants.GridHeight;
    }

    fullContents: PageStack {
        id: contentArea;
        anchors.fill: parent;
        initialPage: GridView {
            anchors.fill: parent;

            model: model;
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

        ListElement { name: "Red"; file: "images/red.png"; }
        ListElement { name: "Green"; file: "images/green.png"; }
        ListElement { name: "Blue"; file: "images/blue.png"; }
    }

    Component {
        id: delegate;

        Button {
            width: Constants.GridWidth;
            height: Constants.GridHeight;

            checked: GridView.isCurrentItem;

            text: model.name;

            highlightColor: Constants.Theme.HighlightColor;

            onClicked: {
                GridView.view.currentIndex = index;
                Settings.currentPreset = model.file;
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
