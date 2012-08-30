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
import org.krita.sketch 1.0
import "../components"

Panel {
    id: base;
    name: "Presets";
    panelColor: "#1e0058";
    dragDelegate: Component {
        Rectangle {
            color: "#1e0058";
            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;
            radius: 8

            Label {
                anchors.centerIn: parent;

                text: "Presets";
                color: "white";

                font.pixelSize: Constants.DefaultFontSize;
            }
        }
    }

    actions: [
        Button {
            id: addButton;

            width: Constants.GridWidth / 2
            height: Constants.GridHeight;

            color: "transparent";
            image: "../images/svg/icon-add.svg"
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

            width: Constants.GridWidth / 2
            height: Constants.GridHeight;

            text: ""
            image: "../images/svg/icon-edit.svg"
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

    PresetModel {
        id: presetsModel;
        view: sketchView.view;
    }
    
    peekContents: GridView {
        anchors.fill: parent;
        keyNavigationWraps: false

        model: presetsModel;
        delegate: delegate;

        cellWidth: Constants.GridWidth - 8
        cellHeight: Constants.GridHeight - 8
    }

    fullContents: PageStack {
        id: contentArea;
        anchors.fill: parent;
        initialPage: GridView {
            anchors.fill: parent;

            model: presetsModel;
            delegate: delegate;

            cellWidth: Constants.GridWidth - 8
            cellHeight: Constants.GridHeight - 8
        }
    }

    onStateChanged: if( state != "edit" && contentArea.depth > 1 ) {
        contentArea.pop();
    }

    Component {
        id: delegate;

        Button {
            width: Constants.GridWidth;
            height: Constants.GridHeight;

            checked: GridView.isCurrentItem;

            color: "transparent";
            text: model.text;
            shadow: false
            textSize: 10;
            image: model.image;

            highlightColor: Constants.Theme.HighlightColor;

            onClicked: {
                presetsModel.activatePreset(index);
                GridView.view.currentIndex = index;
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
