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
import Qt.labs.particles 1.0
import ":/components"
import ":/panels"

Page {
    Rectangle {
        anchors.fill: parent;
        color: "grey"

        DropShadow {
            anchors.centerIn: parent;
            width: parent.width * 0.75;
            height: parent.height * 0.75;

            Rectangle {
                anchors.fill: parent;
                color: "white";

                  Particles {
                    id: particles;

                    count: 100;
                    emissionRate: 0;

                    fadeInDuration: 0;
                    fadeOutDuration: 10000;

                    lifeSpan: 15000;

                    source: Settings.currentPreset;
                }

                MouseArea {
                    anchors.fill: parent;

                    onPressed: update( mouse );
                    onPositionChanged: update( mouse );

                    function update( mouse ) {
                        particles.x = mouse.x;
                        particles.y = mouse.y;
                        particles.burst(1);
                    }
                }
            }
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
                }
            }
        }
    }

    Component { id: sharePage; SharePage { } }
    Component { id: settingsPage; SettingsPage { } }
}