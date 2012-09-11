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

        height: Constants.GridHeight * 3;
        z: 10;

        Header {
            height: Constants.GridHeight * 2;
            //text: "Krita Sketch";
            Image { anchors.fill: parent; source: "./images/header_krita_sketch.png" }
            Label { text: "KRITA SKETCH"; font.bold: true; font.pixelSize: Constants.HugeFontSize; anchors.centerIn: parent; color: "white";}
            Image { anchors.fill: parent; source: "./images/header_krita_sketch_light.png"; smooth: true; }

            //leftArea:

              /* Item {
                width: Constants.GridWidth / 2;
                height: Constants.GridHeight;
                z: 10;
                Rectangle { anchors.fill: parent; anchors.margins: Constants.DefaultMargin; color: "white"; } */

            Image {
                x: Constants.GridWidth / 2;
                y: Constants.GridHeight / 2;
                width: Constants.GridWidth / 2;
                height: Constants.GridHeight;
                source: "./images/svg/icon-krita_sketch.svg";
                fillMode: Image.PreserveAspectFit;
                smooth: true;

            }
          //}
            //rightArea:

            Button {
                x: Constants.GridWidth * 11;
                y: Constants.GridHeight / 2;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                image: "./images/svg/icon-help.svg";
                highlightColor: Constants.Theme.HighlightColor;

                onClicked: pageStack.push( help );
            }
        }

        Row {
            anchors.bottom: parent.bottom;
            width: parent.width;

            Rectangle {
                height: Constants.GridHeight;
                width: parent.width / 3;

                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "#994747"
                    }


                    GradientStop {
                        position: 1
                        color: "#862525"
                    }
                }


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

                Image { source: "./images/shadow-smooth.png"; width: parent.width; height: Constants.GridHeight / 8; anchors.top: parent.bottom;}

            }

            Rectangle {
                height: Constants.GridHeight;
                width: parent.width / 3;

                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "#3B9BA5"
                    }


                    GradientStop {
                        position: 1
                        color: "#148793"
                    }
                }

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

               Image { source: "./images/shadow-smooth.png"; width: parent.width; height: Constants.GridHeight / 8; anchors.top: parent.bottom;}

            }

            Rectangle {
                height: Constants.GridHeight;
                width: parent.width / 3;

                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "#D49A57"
                    }


                    GradientStop {
                        position: 1
                        color: "#CB8636"
                    }
                }

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

                Image { source: "./images/shadow-smooth.png"; width: parent.width; height: Constants.GridHeight / 8; anchors.top: parent.bottom;}

            }
        }
    }

    Row {
        anchors.bottom: parent.bottom;
        width: parent.width;

        RecentFilesList {
            width: parent.width / 3 - 4;
            height: Constants.GridHeight * 9;

            onClicked: button == "open" ? pageStack.push( openImage ) : pageStack.push( main );
        }

        Divider { height: Constants.GridHeight * 9; }

        NewImageList {
            width: parent.width / 3 - 8;
            height: Constants.GridHeight * 9;
            onClicked: pageStack.push( main );
        }

        Divider { height: Constants.GridHeight * 9; }

        NewsList {
            width: parent.width / 3 - 4;
            height: Constants.GridHeight * 9;
        }
    }

    Component { id: main; MainPage { } }
    Component { id: help; HelpPage { } }
    Component { id: openImage; OpenImagePage {} }
}
