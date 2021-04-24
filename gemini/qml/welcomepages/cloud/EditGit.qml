/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.11
import QtQuick.Controls 2.5 as QtControls
import org.kde.kirigami 2.7 as Kirigami
import org.calligra 1.0
import "../../components"
import "git"

Kirigami.OverlaySheet {
    id: component;
    property int accountIndex: -1;
    onAccountIndexChanged: {
        accountDetails = cloudAccounts.accountDetails(accountIndex);
    }
    property string text;
    onTextChanged: {
        nameField.text = text;
    }
    property QtObject accountDetails;

    header: Kirigami.Heading {
        text: "Edit Git Account"
        width: component.width / 2
    }

    Kirigami.FormLayout {
        QtControls.TextField {
            id: nameField;
            Kirigami.FormData.label: "Account Name";
        }

        QtControls.Button {
            onClicked: {
                dlgLoader.item.close();
                userCredentials.open();
                if(accountDetails.readProperty("userForRemote") !== undefined) {
                    credentialsGetter.userForRemote = accountDetails.readProperty("userForRemote");
                }
                if(accountDetails.readProperty("privateKeyFile") !== undefined) {
                    credentialsGetter.privateKeyFile = accountDetails.readProperty("privateKeyFile");
                }
                if(accountDetails.readProperty("publicKeyFile") !== undefined) {
                    credentialsGetter.publicKeyFile = accountDetails.readProperty("publicKeyFile");
                }
                if(accountDetails.readProperty("needsPrivateKeyPassphrase") !== undefined) {
                    credentialsGetter.needsPrivateKeyPassphrase = accountDetails.readProperty("needsPrivateKeyPassphrase");
                }
            }
            text: "Edit User Credentials";
            Kirigami.FormData.label: "User Credentials"
        }

        Kirigami.Separator {
        }

        QtControls.Button {
            text: "Save";
            onClicked: {
                cloudAccounts.renameAccount(component.accountIndex, nameField.text);
                dlgLoader.item.close();
            }
        }

        Kirigami.OverlaySheet {
            id: userCredentials;
            header: Kirigami.Heading { text: "User Credentials" }
            GetUserCredentials {
                id: credentialsGetter
                onAccepted: {
                    component.userForRemote = userForRemote;
                    component.privateKeyFile = privateKeyFile;
                    component.publicKeyFile = publicKeyFile;
                    component.needsPrivateKeyPassphrase = needsPrivateKeyPassphrase;
                    userCredentials.close();
                }
            }
            onSheetOpenChanged: {
                if (sheetOpen === false) {
                    dlgLoader.item.open();
                }
            }
        }
    }
}
