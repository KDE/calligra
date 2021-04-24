/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.11
import QtQuick.Controls 2.5 as QtControls
import org.kde.kirigami 2.7 as Kirigami
import Calligra.Gemini.Dropbox 1.0
import "../../../components"

Kirigami.FormLayout {
    id: component
    QtControls.Label {
        visible: !controllerMIT.needAuthenticate;
        wrapMode: Text.Wrap;
        text: "You are already signed into DropBox, and we only support one DropBox account. To sign in as another user, please click on the button above to sign out of your current account first.";
    }
    Kirigami.Separator {
        visible: !controllerMIT.needAuthenticate;
        Kirigami.FormData.isSection: true
    }
    QtControls.Button {
        visible: !controllerMIT.needAuthenticate;
        Kirigami.FormData.label: "Log out of DropBox?"
        text: "Log Out"
        onClicked: {
            if (!controllerMIT.is_transfer()) {
                controllerMIT.logout();
            } else {
                applicationWindow().showPassiveNotification("Please complete the upload/download tasks in Files Transfer before Sign out.");
                dlgLoader.item.close();
            }
        }
    }

    QtControls.Button {
        visible: controllerMIT.needAuthenticate
        text: "Log in to DropBox";
        onClicked: {
            dlgLoader.item.close();
            pageStack.push(loginPage);
        }
    }
    Component {
        id: loginPage;
        LoginPage { }
    }

    InfoBanner { id: i_infobanner; }
}
