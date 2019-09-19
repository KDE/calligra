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

Kirigami.OverlaySheet {
    id: component;
    property int accountIndex: -1;
    property string text;
    onTextChanged: {
        nameField.text = text;
    }
    property alias headerTitle: headerItem.text

    header: Kirigami.Heading {
        id: headerItem
        text: "Edit Account"
        width: component.width / 2
    }

    Kirigami.FormLayout {
        QtControls.TextField {
            id: nameField;
            anchors {
                verticalCenter: parent.verticalCenter;
                left: parent.left;
                right: parent.right;
                margins: Settings.theme.adjustedPixel(16);
            }
            Kirigami.FormData.label: "Account Name";
        }

        Kirigami.Separator {
        }

        QtControls.Button {
            text: "Save";
            onClicked: {
                cloudAccounts.renameAccount(base.accountIndex, nameField.text);
                component.close();
            }
        }
    }
}
