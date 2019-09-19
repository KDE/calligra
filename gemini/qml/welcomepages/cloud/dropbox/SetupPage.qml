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
