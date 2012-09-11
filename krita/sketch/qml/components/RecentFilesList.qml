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

Item {
    id: base;
    signal clicked(string button);

    RecentImagesModel {
        id: recentImagesModel;
        recentFileManager: RecentFileManager;
    }

    ListView {
        id: view;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;

        height: Constants.GridHeight * 7;

        clip: true;

        delegate: Rectangle {
            width: Constants.GridWidth * 4;
            height: Constants.GridHeight * 1.75;

               Rectangle{
                   x: Constants.GridWidth / 4;
                   y: Constants.GridHeight * 0.25
                   width: parent.width - (Constants.GridWidth / 2);
                   height: parent.height - (Constants.GridHeight * 0.5);
                   Image { source: "../images/shadow-smooth.png"; width: parent.width; height: Constants.GridHeight / 8; anchors.top: parent.bottom;}
                   gradient: Gradient {
                       GradientStop {
                           position: 0
                           color: "#FBFBFD"
                       }

                       GradientStop {
                           position: 0.4
                           color: "#FAF0F5"
                       }
                   }
               }


            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    recentImagesModel.addRecent(model.url);
                    Settings.currentFile = model.url;
                    base.clicked("");
                }
            }


            Image {
                id: thumbnail
                source: model.image;
                x: Constants.GridWidth * 0.375
                y: Constants.GridHeight * 0.5
		asynchronous: true;
                Image { source: "../images/shadow-smooth.png"; width: parent.width; height: Constants.GridHeight / 8; anchors.top: parent.bottom;}
            }

            Label {
                anchors {
                    top: parent.top;
                    topMargin: Constants.GridHeight * 0.5;
                    left: thumbnail.right;
                    leftMargin: Constants.GridWidth * 0.25;
                }

                text: model.text;
                verticalAlignment: Text.AlignTop;
            }

            Label {
                anchors {
                    bottom: parent.bottom;
                    bottomMargin:Constants.GridHeight * 0.5;
                    left: thumbnail.right;
                    leftMargin: Constants.GridWidth * 0.25;
                }

                text: model.url;
                color: Constants.Theme.SecondaryTextColor;
                font.pixelSize: Constants.SmallFontSize;
                verticalAlignment: Text.AlignBottom;
            }
        }

        model: recentImagesModel;
    }

    Item {
        anchors.top: view.bottom;
        width: parent.width;
        height: Constants.GridHeight * 1.75;

        Rectangle{
            x: Constants.GridWidth / 4;
            y: Constants.GridHeight * 0.25
            width: parent.width - (Constants.GridWidth / 2);
            height: parent.height - (Constants.GridHeight * 0.5);
            Image { source: "../images/shadow-smooth.png"; width: parent.width; height: Constants.GridHeight / 8; anchors.top: parent.bottom;}
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#FBFBFD"
                }

                GradientStop {
                    position: 0.4
                    color: "#FAF0F5"
                }
            }
        }


        MouseArea {
            anchors.fill: parent;
            onClicked: base.clicked();
        }

        Button {
            id: icon;
            onClicked: {
                base.clicked("open");
            }

            image: "../images/svg/icon-fileopen-red.svg";
            x: Constants.GridWidth * 0.125;
            y: Constants.GridHeight * 0.375;
        }

        Label {
            anchors {
                left: icon.right;

               }

            y: Constants.GridHeight * 0.25;
            text: "Open Image";
        }
    }
}
