/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts  as QtLayouts
import QtQuick.Controls as QtControls
import org.kde.kirigami as Kirigami
import org.calligra 1.0
import Calligra.Gemini.Git 1.0
import "../../../components"

Kirigami.FormLayout {
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

    QtLayouts.ColumnLayout {
        id: typeChooser;
        spacing: Kirigami.Units.smallSpacing
        Kirigami.FormData.label: "Type of repository:";
        Kirigami.FormData.buddyFor: existingCheckoutSelector
        QtControls.RadioButton {
            id: existingCheckoutSelector;
            text: "Existing Clone";
            checked: true;
        }
        QtControls.RadioButton {
            id: newCheckoutSelector;
            text: "New Clone";
        }
    }

    QtControls.TextField {
        id: checkoutName;
        Kirigami.FormData.label: "Short name for this account";
    }

    QtControls.TextField {
        id: checkoutDir;
        Kirigami.FormData.label: "Local clone location";
        QtControls.Button {
            id: checkoutBrowse;
            anchors {
                top: checkoutDir.top;
                right: parent.right;
                bottom: checkoutDir.bottom;
            }
            text: "Browse...";
            onClicked: {
                var newDir = GitCheckoutCreator.getDir();
                if(newDir !== "") {
                    checkoutDir.text = newDir;
                }
            }
        }
    }

    QtControls.TextField {
        id: newCheckoutServer;
        opacity: newCheckoutSelector.checked ? 1 : 0;
        Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
        visible: opacity > 0
        Kirigami.FormData.label: "Clone URL (https or git)";
    }

    QtControls.Button {
        onClicked: { dlgLoader.item.close(); userCredentials.open(); }
        text: "Set User Credentials";
        Kirigami.FormData.label: "User Credentials"
    }

    QtControls.Label {
        id: errorLabel;
        visible: text !== ""
        color: "red";
    }
    QtControls.Button {
        id: cloneAdd;
        text: "Add Now";
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
                dlgLoader.item.close();
            }
        }
    }

    Kirigami.Dialog {
        id: userCredentials;

        title: "User Credentials"

        contentItem: GetUserCredentials {
            onAccepted: {
                base.userForRemote = userForRemote;
                base.privateKeyFile = privateKeyFile;
                base.publicKeyFile = publicKeyFile;
                base.needsPrivateKeyPassphrase = needsPrivateKeyPassphrase;
                userCredentials.close();
            }
        }
        onClosed: {
            dlgLoader.item.open();
        }
    }
}
