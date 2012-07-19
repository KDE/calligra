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
import "components"
import "panels"

Page {

    DropShadow {
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }

        height: Constants.GridHeight * 2;
        z: 10;

        Header {
            text: "Krita Sketch";

            leftArea: Item {
                width: Constants.GridWidth;
                height: Constants.GridHeight;

                Rectangle { anchors.fill: parent; anchors.margins: Constants.DefaultMargin; color: "white"; }
                Image { anchors.centerIn: parent; source: ":/images/krita.png" }
            }
            rightArea: Button {
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                text: "?";
                highlightColor: Constants.Theme.HighlightColor;
            }
        }

        Row {
            anchors.bottom: parent.bottom;

            Rectangle {
                height: Constants.GridHeight;
                width: Constants.GridWidth * 4;

                color: Constants.Theme.SecondaryColor;

                Label {
                    anchors {
                        left: parent.left;
                        leftMargin: Constants.DefaultMargin;
                        verticalCenter: parent.verticalCenter;
                    }
                    text: "Recent Images";
                    font.pixelSize: Constants.LargeFontSize;
                    color: "white";
                }
            }

            Rectangle {
                height: Constants.GridHeight;
                width: Constants.GridWidth * 4;

                color: Constants.Theme.TertiaryColor;

                Label {
                    anchors {
                        left: parent.left;
                        leftMargin: Constants.DefaultMargin;
                        verticalCenter: parent.verticalCenter;
                    }
                    text: "Create New";
                    font.pixelSize: Constants.LargeFontSize;
                    color: "white";
                }
            }

            Rectangle {
                height: Constants.GridHeight;
                width: Constants.GridWidth * 4;

                color: Constants.Theme.QuaternaryColor;

                Label {
                    anchors {
                        left: parent.left;
                        leftMargin: Constants.DefaultMargin;
                        verticalCenter: parent.verticalCenter;
                    }
                    text: "Community News";
                    font.pixelSize: Constants.LargeFontSize;
                    color: "white";
                }
            }
        }
    }

    Row {
        anchors.bottom: parent.bottom;

        RecentFilesList {
            width: Constants.GridWidth * 4 - 1;
            height: Constants.GridHeight * 6;

            onClicked: pageStack.push( main );
        }

        Divider { height: Constants.GridHeight * 6; }

        CreateNewSection {
            width: Constants.GridWidth * 4 - 2;
            height: Constants.GridHeight * 6;
            onClicked: pageStack.push( main );
        }

        Divider { height: Constants.GridHeight * 6; }

        ListView {
            width: Constants.GridWidth * 4 - 1;
            height: Constants.GridHeight * 6;
        }
    }

    Component { id: main; MainPage { } }
}
