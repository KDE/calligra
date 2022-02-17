/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 2.2 as QQC2
import org.kde.kirigami 2.7 as Kirigami
import org.calligra 1.0
import "../../components"

Kirigami.Page {
    id: base;
    property string pageName: "accountsPageWebdav";
    property QtObject accountDetails;
    QQC2.Label {
        anchors.fill: parent;
        text: accountDetails ? "webdav stuffs for account " + accountDetails.username : "werbderb";
        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;
        font: Settings.theme.font("templateLabel");
        color: "#5b6573";
    }
}
