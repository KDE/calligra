/* This file is part of the KDE project
 * Copyright (C) 2014 Stuart Dickson <stuartmd@kogmbh.com>
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

Rectangle {
    id: steamBase;
    property color steamBorderColor: "#383635";
    property color steamBackLightColor: "#3E3D3C";
    property color steamBackDarkColor: "#262626";
    property color steamHeadingTextColor: "#C7C4C2";
    property color steamActiveHeadingTextColor: "#FFFFFF";

     width: 640; height: 480;
     gradient: Gradient {
         GradientStop { position: 0.0; color: "black" }
         GradientStop { position: 0.2; color: steamBorderColor }
         GradientStop { position: 1.0; color: steamBorderColor }
     }

    Row {
        id: steamTabs;
        property int activeTabIndex: 0
        x:20;  y: 20;
        spacing: 16
        Text {
            id: storageText
            text: "STORAGE"
            color: (steamTabs.activeTabIndex==0) ? steamBase.steamActiveHeadingTextColor : steamBase.steamHeadingTextColor;
            font.pointSize: 16; font.bold: false

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    steamTabs.activeTabIndex = 0;
                }
            }
        }
        Text {
            id: storeText
            text: "STORE"
            color: (steamTabs.activeTabIndex==1) ? steamBase.steamActiveHeadingTextColor : steamBase.steamHeadingTextColor;
            font.pointSize: 16; font.bold: false

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    steamTabs.activeTabIndex = 1;
                }
            }
        }
        Text {
            id: workshopText
            text: "WORKSHOP"
            color: (steamTabs.activeTabIndex==2) ? steamBase.steamActiveHeadingTextColor : steamBase.steamHeadingTextColor;
            font.pointSize: 16; font.bold: false

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    steamTabs.activeTabIndex = 2;
                }
            }
        }
        Text {
            id: transfersText
            text: "TRANSFERS"
            color: (steamTabs.activeTabIndex==3) ? steamBase.steamActiveHeadingTextColor : steamBase.steamHeadingTextColor;
            font.pointSize: 16; font.bold: false

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    steamTabs.activeTabIndex = 3;
                }
            }
        }
    }

     Rectangle {
        id: steamContents
        anchors.left: parent.left
        anchors.leftMargin: 16
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.top: steamTabs.bottom
        anchors.topMargin: 16
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 16
        gradient: Gradient {
            GradientStop { position: 0.0; color: steamBase.steamBackLightColor }
            GradientStop { position: 1.0; color: steamBase.steamBackDarkColor }
        }

        SteamStoragePage {
            visible: steamTabs.activeTabIndex==0
        }
        SteamStorePage {
            visible: steamTabs.activeTabIndex==1
        }
        SteamWorkshopPage {
            visible: steamTabs.activeTabIndex==2
        }
        SteamTransfersPage {
            visible: steamTabs.activeTabIndex==3
        }
    }
}
