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
    initialPage: createNewListPage;
    clip: true;

    signal clicked();

    Component { id: createNewListPage; Page {
        ListView {
            anchors.fill: parent;

            delegate: Rectangle {
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
                            color: "#FAFCFD"
                        }

                        GradientStop {
                            position: 0.4
                            color: "#F0F5FA"
                        }
                    }
                }

                Button {
                    id: thumbnail;

                    x: Constants.GridWidth * 0.125;
                    y: Constants.GridHeight * 0.375;

                    image: model.image;

                    enabled: model.name === "clip" ? KisClipBoard.clip : true;

                    onClicked: {
                        if (model.bnrole === "a4p") {
                            Settings.currentFile = "";
                            Settings.imageWidth = 2408;
                            Settings.imageHeight = 3509;
                            Settings.imageResolution = 300;
                            onClicked: base.clicked();
                        }
                        else if (model.bnrole === "a4l") {
                            Settings.currentFile = "";
                            Settings.imageWidth = 3509;
                            Settings.imageHeight = 2408;
                            Settings.imageResolution = 300;
                            onClicked: base.clicked();
                        }
                        else if (model.bnrole === "custom") {
                           pageStack.push( createNewPage );
                        }
                        else if (model.bnrole === "clip") {
                            Settings.currentFile = "";
                            Settings.useClipBoard = true;
                            onClicked: base.clicked();
                        }
                        else if (model.bnrole === "webcam") {
                            Settings.currentFile = "";
                            Settings.useWebCam = true;
                            onClicked: base.clicked();
                        }
                    }
                }

                Label {
                    anchors {
                        left: thumbnail.right;
                        leftMargin: Constants.DefaultMargin;
                        verticalCenter: parent.verticalCenter;
                    }

                    text: model.name;
                }
            }


            model: ListModel {
                ListElement { bnrole: "a4p";    name: "Blank Image (A4 Portrait)"; image: "../images/svg/icon-A4portrait-green.svg" }
                ListElement { bnrole: "a4l";    name: "Blank Image (A4 Landscape)"; image: "../images/svg/icon-A4landscape-green.svg" }
                ListElement { bnrole: "custom"; name: "Custom Size"; image: "../images/svg/icon-filenew-green.svg" }
                ListElement { bnrole: "clip";   name: "From Clipboard"; image: "../images/svg/icon-fileclip-green.svg" }
                ListElement { bnrole: "webcam"; name: "From Camera"; image: "../images/svg/icon-camera-green.svg" }
            }
        }
    } }

    Component { id: createNewPage; Page {
        Column {
            anchors.fill: parent;
            Item {
                width: parent.width;
                height: Constants.GridHeight;
                Image {
                    id: titleImage;
                    anchors.left: parent.left;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.margins: Constants.DefaultMargin;
                    source: "../images/svg/icon-filenew-green.svg"
                }
                Label {
                    anchors.left: titleImage.right;
                    anchors.margins: Constants.DefaultMargin;
                    anchors.verticalCenter: parent.verticalCenter;
                    text: "Custom Size";
                    font.pixelSize: Constants.LargeFontSize;
                }
            }
            TextField {
                id: width;
                placeholder: "Width";
                validator: IntValidator{bottom: 0; top: 10000;}
            }
            TextField {
                id: height;
                placeholder: "Height"
                validator: IntValidator{bottom: 0; top: 10000;}
            }
            TextField {
                id: resolution;
                placeholder: "Resolution"
                validator: IntValidator{bottom: 0; top: 600;}
            }
            Item { width: parent.width; height: Constants.GridHeight; }
            Row {
                width: parent.width;
                Button {
                    width: parent.width / 2;
                    color: Constants.Theme.NegativeColor;
                    text: "Cancel";
                    textColor: "white";
                    onClicked: pageStack.pop();
                }
                Button {
                    width: parent.width / 2;
                    color: Constants.Theme.PositiveColor;
                    text: "Create";
                    textColor: "white";
                    onClicked: {
                        Settings.currentFile = "";
                        Settings.imageWidth = width.text;
                        Settings.imageHeight = height.text;
                        Settings.imageResolution = resolution.text;
                        base.clicked();
                    }
                }
            }
        }
    } }
}
