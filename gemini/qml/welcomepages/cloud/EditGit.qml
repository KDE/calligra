/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QtControls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.calligra
import "../../components"
import "git"

FormCard.FormCardDialog {
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

    title: i18nc("@title:dialog", "Edit Git Account")

    standardButtons: QtControls.Dialog.Save

    onAccepted: {
        cloudAccounts.renameAccount(component.accountIndex, nameField.text);
        dlgLoader.item.close();
    }

    FormCard.FormTextFieldDelegate {
        id: nameField;
        text: i18nc("@label:textbox", "Account Name")
    }

    FormCard.FormDelegateSeparator {}

    FormCard.FormButtonDelegate {
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
        text: i18nc("@action:button", "Edit User Credentials")
    }


    Kirigami.Dialog {
        id: userCredentials;
        title: "User Credentials"
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
        onClosed: {
            dlgLoader.item.open();
        }
    }
}
