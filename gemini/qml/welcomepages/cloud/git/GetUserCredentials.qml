/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

import QtQuick 2.0
import org.calligra 1.0
import Calligra.Gemini.Git 1.0
import "../../../components"

Item {
    id: base;
    signal accepted();
    signal cancelled();
    property alias userForRemote: userName.text;
    property alias privateKeyFile: privateKey.text;
    property alias needsPrivateKeyPassphrase: needsPassphrase.checked;
    property alias publicKeyFile: publicKey.text;
    anchors.margins: Settings.theme.adjustedPixel(32);
    Rectangle {
        anchors {
            fill: parent;
            margins: -Settings.theme.adjustedPixel(48);
        }
        opacity: 0.7;
        color: "white";
        MouseArea { anchors.fill: parent; onClicked: { /*nothing */ } }
        SimpleTouchArea { anchors.fill: parent; onTouched: { /*nothing */ } }
    }
    Text {
        id: pageTitle;
        anchors {
            left: parent.left;
            right: parent.right
            bottom: pageContent.top;
            bottomMargin: Settings.theme.adjustedPixel(8);
        }
        height: font.pixelHeight + Settings.theme.adjustedPixel(16);
        font: Settings.theme.font("pageHeader");
        verticalAlignment: Text.AlignVCenter;
        horizontalAlignment: Text.AlignHCenter;
        text: "User Credentials";
        Rectangle {
            anchors {
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            height: 1;
            color: "black";
            opacity: 0.1;
        }
    }
    Item {
        id: pageContent;
        anchors {
            left: parent.left;
            right: parent.right;
            verticalCenter: parent.verticalCenter;
        }
        height: contentColumn.height;
        Column {
            id: contentColumn;
            anchors {
                verticalCenter: parent.verticalCenter;
                left: parent.left;
                right: parent.right;
            }
            height: childrenRect.height;
            TextField {
                id: userName;
                width: parent.width;
                placeholder: "Username";
            }
            Item {
                height: privateKey.height;
                width: parent.width;
                TextField {
                    id: privateKey;
                    width: parent.width - privateKeyBrowse.width;
                    anchors.right: privateKeyBrowse.left;
                    placeholder: "Private Key File";
                }
                CohereButton {
                    id: privateKeyBrowse;
                    anchors {
                        verticalCenter: privateKey.verticalCenter;
                        right: parent.right;
                    }
                    text: "Browse...";
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    color: "#D2D4D5";
                    onClicked: {
                        var newFile = GitCheckoutCreator.getFile("Private Key File", "*", ".ssh");
                        if(newFile !== "") {
                            privateKey.text = newFile;
                        }
                    }
                }
            }
            Item {
                height: publicKey.height;
                width: parent.width;
                TextField {
                    id: publicKey;
                    width: parent.width - privateKeyBrowse.width;
                    anchors.right: publicKeyBrowse.left;
                    placeholder: "Public Key File";
                }
                CohereButton {
                    id: publicKeyBrowse;
                    anchors {
                        verticalCenter: publicKey.verticalCenter;
                        right: parent.right;
                    }
                    text: "Browse...";
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    color: "#D2D4D5";
                    onClicked: {
                        var newFile = GitCheckoutCreator.getFile("Public Key File", "*.pub", ".ssh");
                        if(newFile !== "") {
                            publicKey.text = newFile;
                        }
                    }
                }
            }
            CheckBox {
                id: needsPassphrase;
                width: parent.width;
                text: "Does the private key require a password to unlock it?";
            }
        }
        Rectangle {
            anchors {
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            height: 1;
            color: "black";
            opacity: 0.1;
        }
    }

    CohereButton {
        id: acceptButton;
        anchors {
            top: pageContent.bottom;
            right: cancelButton.left;
            margins: Settings.theme.adjustedPixel(8);
        }
        text: "Accept";
        textColor: "#5b6573";
        textSize: Settings.theme.adjustedPixel(18);
        color: "#D2D4D5";
        onClicked: base.accepted();
    }
    CohereButton {
        id: cancelButton;
        anchors {
            top: pageContent.bottom;
            right: parent.right;
            margins: Settings.theme.adjustedPixel(8);
        }
        text: "Cancel";
        textColor: "#5b6573";
        textSize: Settings.theme.adjustedPixel(18);
        color: "#D2D4D5";
        onClicked: base.cancelled();
    }
}

