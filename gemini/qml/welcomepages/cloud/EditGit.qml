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
