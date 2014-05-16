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
import org.krita.steam 1.0
import org.krita.sketch 1.0
import org.krita.sketch.components 1.0

Rectangle {
    id: base;
    property color steamBorderColor: "#383635";
    property color steamBackLightColor: "#3E3D3C";
    property color steamBackDarkColor: "#262626";
    property color steamHeadingTextColor: "#C7C4C2";
    property color steamActiveHeadingTextColor: "#FFFFFF";

     width: 640; height: 480;

     Component.onCompleted: {
         Constants.setGridWidth( width / Constants.GridColumns );
         Constants.setGridHeight( height / Constants.GridRows );
     }
     onWidthChanged: Constants.setGridWidth( width / Constants.GridColumns );
     onHeightChanged: Constants.setGridHeight( height / Constants.GridRows );


     gradient: Gradient {
         GradientStop { position: 0.0; color: "black" }
         GradientStop { position: 0.2; color: steamBorderColor }
         GradientStop { position: 1.0; color: steamBorderColor }
     }


    StorageListModel {
        id: storageListModel;
        kritaSteamClient: SteamClientInstance;
    }



    Column {
        id: header;
        spacing: 5;

        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }

        Row {
            id: steamTabs;
            x:20;  y: 20;
            spacing: 16
            Text {
                id: storageText
                text: "OPEN FILE FROM CLOUD"
                color: (steamTabs.activeTabIndex==0) ? base.steamActiveHeadingTextColor : base.steamHeadingTextColor;
                font.pointSize: 16; font.bold: false

                MouseArea {
                    anchors.fill: parent
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
        anchors.top: header.bottom
        anchors.topMargin: 16
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 16
        gradient: Gradient {
            GradientStop { position: 0.0; color: base.steamBackLightColor }
            GradientStop { position: 1.0; color: base.steamBackDarkColor }
        }

        ListView {
            id: filesView
            anchors.fill: parent

            clip: true;

            delegate: ListItem {
                title: model.fileName;
                description: "";
                width: filesView.width;
            }

            model: {return storageListModel;}

            ScrollDecorator { }
        }
    }
}
