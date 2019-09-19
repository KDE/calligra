 
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
import QtQuick.Layouts 1.5 as QtLayouts
import org.kde.kirigami 2.7 as Kirigami
import org.calligra 1.0

Kirigami.OverlaySheet {
    id: component;
    property int accountIndex: -1;
    property string text;

    header: Kirigami.Heading {
        id: headerItem
        text: "Edit Account"
        width: component.width / 2
    }

    Kirigami.FormLayout {
        QtControls.Label {
            verticalAlignment: Text.AlignVCenter;
            horizontalAlignment: Text.AlignHCenter;
            text: "Do you wish to remove this account?\n\n" + component.text + "\n\n(removing the account will leave all files in place, this only removes it from Calligra Gemini's list)";
        }

        Kirigami.Separator {
        }

        QtControls.Button {
            id: okButton;
            text: "Remove Account";
            QtLayouts.Layout.alignment: Qt.AlignHCenter
            onClicked: {
                cloudAccounts.removeAccount(component.accountIndex);
                component.close();
            }
        }
    }
}
