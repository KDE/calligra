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

Item {
    id: base
    anchors.fill: parent

    StorageListModel {
        id: storageListModel
    }

    Column {
        ListView {
            id: storageListView
            model: storageListModel
            //onModelChanged: currentIndex = 0;
            onCurrentIndexChanged: {
                //storageListModel.itemSelected(currentIndex);
            }

            delegate: ListItem {
                title: model.name;
                //image: model.image;
                //imageShadow: false;
                //imageSmooth: false;
                //imageFillMode: Image.PreserveAspectCrop;
                //imageCache: false;

                onClicked: {

                }
                Image {
                /*
                    id: cloudIcon;
                    visible: model.cloudSynced;
                    source: "../images/icon_steam_cloud-black.png"

                    anchors.right: parent.right;
                    anchors.rightMargin: Constants.GridWidth * 0.25;
                    anchors.top: parent.top;
                    anchors.topMargin: Constants.GridHeight * 0.25;

                    width: Constants.GridWidth * 0.5;

                    asynchronous: true;
                    fillMode: Image.PreserveAspectFit;
                    clip: true;
                    smooth: true;

                    sourceSize.width: width > height ? height : width;
                    sourceSize.height: width > height ? height : width;
                    */
                }
            }
        }

        Row {
            anchors.bottom: parent.bottom
            Text {
                id: usedAndMaxStorage
                text: "54MB of 1.0 GB"
                color: "green"
            }
        }
    }
}
