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
import org.krita.sketch 1.0 as Krita
import "components"
import "panels"

Page {
    //anchors.fill: parent;
    Item {
        anchors.fill: parent;

        Krita.SketchView {
            id: sketchView;
            //anchors.fill: parent;
            width: parent.width;
            onWidthChanged: console.log("new width %1".arg(width));
            height: parent.height;
            file: Settings.currentFile;
            settings: Settings;
            Component.onCompleted: createDocument();
        }

        PanelBar { height: parent.height; width: parent.width; }

        NewImagePanel {
            id: newPanel;
            anchors.left: parent.left;
            width: Constants.GridWidth * 4;
            height: parent.height;
        }

        OpenImagePanel {
            id: openPanel;
            anchors.left: parent.left;
            width: Constants.GridWidth * 4;
            height: parent.height;
        }

        MenuPanel {
            anchors.bottom: parent.bottom;

            width: parent.width;
            z: 10;

            newButtonChecked: !newPanel.collapsed;
            openButtonChecked: !openPanel.collapsed;

            onCollapsedChanged: if( collapsed ) {
                newPanel.collapsed = true;
                openPanel.collapsed = true;
            }

            onButtonClicked: {
                switch( button ) {
                    case "new": {
                        newPanel.collapsed = !newPanel.collapsed;
                        openPanel.collapsed = true;
                    }
                    case "open": {
                        openPanel.collapsed = !openPanel.collapsed;
                        newPanel.collapsed = true;
                    }
                    case "settings":
                        pageStack.push( settingsPage );
                    case "share":
                        pageStack.push( sharePage );
                    case "help":
                        pageStack.push( helpPage );
                    case "undo":
                        sketchView.undo();
                    case "redo":
                        sketchView.redo();
                }
            }
        }
    }

    Component { id: sharePage; SharePage { } }
    Component { id: settingsPage; SettingsPage { } }
    Component { id: helpPage; HelpPage { } }
}
