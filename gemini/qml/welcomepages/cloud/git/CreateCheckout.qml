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

import QtQuick 1.1
import org.calligra 1.0
import Calligra.Gemini.Git 1.0
import "../../../components"

Item {
    Text {
        id: pageTitle;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right
        }
        height: font.pixelHeight + Settings.theme.adjustedPixel(16);
        font: Settings.theme.font("pageHeader");
        verticalAlignment: Text.AlignVCenter;
        horizontalAlignment: Text.AlignHCenter;
        text: "Add Git Account";
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
            top: pageTitle.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
        Item {
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
                bottom: parent.verticalCenter;
            }
            Text {
                id: existingDir
                anchors {
                    left: parent.left;
                    right: parent.right
                    bottom: existingCheckoutName.top;
                    bottomMargin: Settings.theme.adjustedPixel(8);
                }
                height: font.pixelHeight + Settings.theme.adjustedPixel(16);
                font: Settings.theme.font("pageHeader");
                verticalAlignment: Text.AlignVCenter;
                horizontalAlignment: Text.AlignHCenter;
                text: "Add existing checkout";
            }
            TextField {
                id: existingCheckoutName;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.verticalCenter;
                }
                placeholder: "Short name for this account";
            }
            TextField {
                id: existingCheckoutDir;
                anchors {
                    top: existingCheckoutName.bottom
                    topMargin: Settings.theme.adjustedPixel(8);
                    left: parent.left;
                    right: existingCheckoutBrowse.left;
                }
                placeholder: "Location of existing checkout";
            }
            CohereButton {
                id: existingCheckoutBrowse;
                anchors {
                    verticalCenter: existingCheckoutDir.verticalCenter;
                    right: parent.right;
                }
                text: "Browse...";
                textColor: "#5b6573";
                textSize: Settings.theme.adjustedPixel(18);
                color: "#D2D4D5";
                onClicked: {
                    var newDir = GitCheckoutCreator.getDir();
                    if(newDir !== "") {
                        existingCheckoutDir.text = newDir;
                    }
                }
            }
            Text {
                id: errorLabel;
                anchors {
                    verticalCenter: existingCheckoutAdd.verticalCenter;
                    left: parent.left;
                    right: parent.right;
                    leftMargin: existingCheckoutDir.height / 2;
                }
                height: existingCheckoutAdd.height;
                color: "red";
                font: Settings.theme.font("application");
            }
            CohereButton {
                id: existingCheckoutAdd;
                anchors {
                    top: existingCheckoutDir.bottom;
                    topMargin: Settings.theme.adjustedPixel(8);
                    right: parent.right;
                }
                text: "Add Now";
                textColor: "#5b6573";
                textSize: Settings.theme.adjustedPixel(18);
                color: "#D2D4D5";
                Component {
                    id: accountComp;
                    QtObject {
                        property string localrepo: existingCheckoutDir.text;
                    }
                }
                onClicked: {
                    if(existingCheckoutName.text.length < 1) {
                        errorLabel.text = "You need to enter a name for the account."
                        return;
                    }
                    if(existingCheckoutDir.text.length < 1) {
                        errorLabel.text = "You must select a location."
                        return;
                    }
                    if(!GitCheckoutCreator.isGitDir(existingCheckoutDir.text)) {
                        errorLabel.text = "You must select a location which is actually a git repository."
                        return;
                    }
                    cloudAccounts.addAccount(existingCheckoutName.text, "Git", "accountsPageGit", accountComp.createObject(cloudAccounts));
                    dlgStack.replace(addEmpty);
                }
            }
        }
        Item {
            anchors {
                top: parent.verticalCenter;
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            Text {
                id: newCheckout
                anchors {
                    left: parent.left;
                    right: parent.right
                    bottom: newCheckoutName.top;
                    bottomMargin: Settings.theme.adjustedPixel(8);
                }
                height: font.pixelHeight + Settings.theme.adjustedPixel(16);
                font: Settings.theme.font("pageHeader");
                verticalAlignment: Text.AlignVCenter;
                horizontalAlignment: Text.AlignHCenter;
                text: "Create a new clone";
            }
            TextField {
                id: newCheckoutName;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.verticalCenter;
                }
                placeholder: "Short name for this account";
            }
            TextField {
                id: newCheckoutServer;
                anchors {
                    top: newCheckoutName.bottom;
                    topMargin: Settings.theme.adjustedPixel(8);
                    left: parent.left;
                    right: parent.right;
                }
                placeholder: "Clone URL (https or git)";
            }
            CohereButton {
                id: newCloneAdd;
                anchors {
                    top: newCheckoutServer.bottom;
                    topMargin: Settings.theme.adjustedPixel(8);
                    right: parent.right;
                }
                text: "Clone and Add Now";
                textColor: "#5b6573";
                textSize: Settings.theme.adjustedPixel(18);
                color: "#D2D4D5";
                onClicked: console.debug(GitCheckoutCreator);
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
}
