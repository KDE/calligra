 
/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
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
