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
