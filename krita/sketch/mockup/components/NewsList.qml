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

PageStack {
    id: base;
    clip: true;

    initialPage: Page {
        ListView {
            anchors.fill: parent;

            delegate: delegate;
            model: model;
        }
    }

    Component {
        id: delegate;

        Rectangle {
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
                        color: "#FDFBFA"
                    }

                    GradientStop {
                        position: 0.4
                        color: "#FAF5F0"
                    }
                }
            }


            color: mouse.pressed ? Constants.Theme.HighlightColor : "transparent";

            Label {
                anchors {
                    top: parent.top;
                    topMargin: Constants.GridHeight * 0.5;
                    left: parent.left;
                    leftMargin: Constants.GridWidth * 0.5;
                }

                text: model.title;
                verticalAlignment: Text.AlignTop;
            }

            Label {
                anchors {
                    bottom: parent.bottom;
                    bottomMargin: Constants.GridHeight * 0.5;
                    left: parent.left;
                    leftMargin: Constants.GridWidth * 0.5;
                }

                text: model.date;
                font.pixelSize: Constants.SmallFontSize;
                color: Constants.Theme.SecondaryTextColor;
                verticalAlignment: Text.AlignBottom;
            }

            Label {
                anchors.right: parent.right;
                anchors.rightMargin: Constants.GridWidth * 0.5;
                y: Constants.GridHeight * 0.5;
                text: "More >";
                color: Constants.Theme.SecondaryTextColor;
                font.pixelSize: Constants.SmallFontSize;
            }

            MouseArea {
                id: mouse;

                anchors.fill: parent;

                onClicked: base.push( detailsPage );
            }
        }
    }

    ListModel {
        id: model;

        ListElement { title: "Krita Sketch Released"; date: "12-10-2012 13:45"; }
        ListElement { title: "Krita 2.6 Released"; date: "08-09-2012 18:24"; }
    }

    Component { id: detailsPage; Page {
        Flickable {
            anchors.fill: parent;
            anchors.leftMargin: Constants.DefaultMargin;
            anchors.rightMargin: Constants.DefaultMargin;
            anchors.bottomMargin: Constants.DefaultMargin;

            contentWidth: width;
            contentHeight: contents.height;

            Column {
                id: contents;
                width: parent.width;

                Item {
                    width: parent.width;
                    height: Constants.GridHeight;

                    Label {
                        anchors {
                            top: parent.top;
                            topMargin: Constants.DefaultMargin;
                        }

                        text: "Krita Sketch Released"
                        verticalAlignment: Text.AlignTop;
                    }

                    Label {
                        anchors {
                            bottom: parent.bottom;
                            bottomMargin: Constants.DefaultMargin;
                        }

                        text: "12-10-2012 13:45";
                        font.pixelSize: Constants.SmallFontSize;
                        color: Constants.Theme.SecondaryTextColor;
                        verticalAlignment: Text.AlignBottom;
                    }


                }

                Label {
                    width: parent.width;
                    height: paintedHeight;

                    elide: Text.ElideNone;
                    wrapMode: Text.WordWrap;
                    horizontalAlignment: Text.AlignJustify;

                    text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam id nisi eget diam viverra dictum sed vel lacus. Donec sollicitudin mollis libero, quis cursus dolor volutpat ac. Sed sagittis varius dolor, vitae bibendum nibh malesuada sed. Praesent imperdiet mi ac dui fermentum tempor. Vivamus porta risus ac felis iaculis mattis. Mauris molestie lacus a nisl elementum porttitor. Quisque placerat euismod sodales. Nunc non tristique nulla. Fusce nec neque mattis orci faucibus placerat at ut metus. Donec blandit purus sit amet quam bibendum a laoreet leo ullamcorper. Etiam eget congue ante."
                }

                Label {
                    text: "< Back";
                    font.pixelSize: Constants.SmallFontSize;
                    color: Constants.Theme.SecondaryTextColor;
                }
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: pageStack.pop();
            }
        }
    } }
}
