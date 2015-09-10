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
    property string userForRemote;
    property string privateKeyFile;
    property string publicKeyFile;
    property bool needsPrivateKeyPassphrase;
    Component {
        id: accountComp;
        QtObject {
            property string localrepo: checkoutDir.text;
            property string userForRemote: base.userForRemote;
            property string privateKeyFile: base.privateKeyFile;
            property string publicKeyFile: base.publicKeyFile;
            property bool needsPrivateKeyPassphrase: base.needsPrivateKeyPassphrase;
        }
    }

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
        Column {
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
                bottom: parent.verticalCenter;
            }
            Text {
                id: typeChooser;
                anchors.right: parent.right;
                width: parent.width - (height / 2);
                height: existingCheckoutSelector.height + Settings.theme.adjustedPixel(16);
                verticalAlignment: Text.AlignVCenter
                font: Settings.theme.font("application");
                text: "Type of repository:";
                CohereButton {
                    id: existingCheckoutSelector;
                    anchors {
                        right: newCheckoutSelector.left;
                        verticalCenter: parent.verticalCenter;
                    }
                    text: "Existing Clone";
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    checkedColor: "#D2D4D5";
                    checked: true;
                    onClicked: {
                        if(!checked) {
                            checked = true;
                            newCheckoutSelector.checked = false;
                        }
                    }
                }
                CohereButton {
                    id: newCheckoutSelector;
                    anchors{
                        right: parent.right;
                        verticalCenter: parent.verticalCenter;
                    }
                    text: "New Clone";
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    checkedColor: "#D2D4D5";
                    onClicked: {
                        if(!checked) {
                            checked = true;
                            existingCheckoutSelector.checked = false;
                        }
                    }
                }
            }
            TextField {
                id: checkoutName;
                width: parent.width;
                placeholder: "Short name for this account";
            }
            Item {
                width: parent.width;
                height: checkoutDir.height;
                TextField {
                    id: checkoutDir;
                    anchors {
                        left: parent.left;
                        right: checkoutBrowse.left;
                    }
                    placeholder: "Local clone location";
                }
                CohereButton {
                    id: checkoutBrowse;
                    anchors {
                        verticalCenter: checkoutDir.verticalCenter;
                        right: parent.right;
                    }
                    text: "Browse...";
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    color: "#D2D4D5";
                    onClicked: {
                        var newDir = GitCheckoutCreator.getDir();
                        if(newDir !== "") {
                            checkoutDir.text = newDir;
                        }
                    }
                }
            }
            TextField {
                id: newCheckoutServer;
                width: parent.width;
                height: newCheckoutSelector.checked ? Constants.GridHeight : 0;
                Behavior on height { PropertyAnimation { duration: Constants.AnimationDuration; } }
                opacity: height === 0 ? 0 : 1;
                Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
                placeholder: "Clone URL (https or git)";
            }
            CohereButton {
                anchors.right: parent.right;
                onClicked: dlgStack.push(userCredentials);
                text: "Set User Credentials";
                textColor: "#5b6573";
                textSize: Settings.theme.adjustedPixel(18);
                color: "#D2D4D5";
            }
            Text {
                id: errorLabel;
                width: parent.width;
                height: cloneAdd.height + Settings.theme.adjustedPixel(16);
                verticalAlignment: Text.AlignVCenter
                color: "red";
                font: Settings.theme.font("application");
                CohereButton {
                    id: cloneAdd;
                    anchors {
                        verticalCenter: parent.verticalCenter;
                        right: parent.right;
                    }
                    text: "Add Now";
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    color: "#D2D4D5";
                    onClicked: {
                        if(checkoutName.text.length < 1) {
                            errorLabel.text = "You need to enter a name for the account."
                            return;
                        }
                        if(checkoutDir.text.length < 1) {
                            errorLabel.text = "You must select a location."
                            return;
                        }
                        if(base.userForRemote.length < 1 || base.privateKeyFile.length < 1 || base.publicKeyFile.length < 1) {
                            errorLabel.text = "You need to enter your credentials.";
                            return;
                        }
                        var createAccount = true;
                        if(newCheckoutSelector.checked) {
                            if(newCheckoutServer.text.length < 1) {
                                errorLabel.text = "You have to enter a remote server location for the clone.";
                                return;
                            }
                            var repoDir = GitCheckoutCreator.createClone(checkoutName.text, newCheckoutServer.text, checkoutDir.text, accountComp.createObject(cloudAccounts));
                            if(repoDir.length > 0) {
                                checkoutDir.text = repoDir;
                            }
                            else {
                                createAccount = false;
                            }
                        }
                        else {
                            if(!GitCheckoutCreator.isGitDir(checkoutDir.text)) {
                                errorLabel.text = "You must select a location which is actually a git repository."
                                return;
                            }
                        }
                        if(createAccount) {
                            cloudAccounts.addAccount(checkoutName.text, "Git", "accountsPageGit", accountComp.createObject(cloudAccounts));
                            dlgStack.replace(addEmpty);
                        }
                    }
                }
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
    Component {
        id: userCredentials;
        GetUserCredentials {
            onAccepted: {
                base.userForRemote = userForRemote;
                base.privateKeyFile = privateKeyFile;
                base.publicKeyFile = publicKeyFile;
                base.needsPrivateKeyPassphrase = needsPrivateKeyPassphrase;
                dlgStack.pop();
            }
            onCancelled: {
                dlgStack.pop();
            }
        }
    }
}
