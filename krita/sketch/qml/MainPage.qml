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
import "components"
import "panels"

Page {
    SketchView {
        id: sketchView;
        onFloatingMessageRequested: {
            console.debug(message,iconName);
        }
        width: parent.width;
        height: parent.height;
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

        onOpenClicked: pageStack.push(openImagePage);
    }

    MenuPanel {
        id: menuPanel;

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
                case "save":
                    if(!Settings.temporaryFile) {
                        sketchView.save();
                    } else {
                        pageStack.push( saveAsPage, { view: sketchView } );
                    }
                case "saveAs":
                    pageStack.push( saveAsPage, { view: sketchView } );
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

    Connections {
        target: Settings;

        onCurrentFileChanged: {
            if(sketchView.modified) {
                //Show modified dialog
            }

            if(Settings.temporaryFile) {
                Krita.ImageBuilder.discardImage(sketchView.file);
            }
            sketchView.file = Settings.currentFile;
            menuPanel.collapsed = true;
        }
    }

    onStatusChanged: if(status == 0) sketchView.file = Settings.currentFile;

    Connections {
        target: Krita.Window;

        onCloseRequested: {
            if(sketchView.modified) {
                //Show modified dialog
            }

            if(Settings.temporaryFile) {
                Krita.ImageBuilder.discardImage(Settings.currentFile);
            }

            Krita.Window.allowClose = true;
            Krita.Window.closeWindow();
        }
    }

    Component.onCompleted: {
        Krita.Window.allowClose = false;
    }

    Component { id: openImagePage; OpenImagePage { } }
    Component { id: sharePage; SharePage { } }
    Component { id: settingsPage; SettingsPage { } }
    Component { id: helpPage; HelpPage { } }
    Component { id: saveAsPage; SaveImagePage { } }
}
