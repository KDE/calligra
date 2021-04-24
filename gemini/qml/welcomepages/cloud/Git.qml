/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import org.calligra 1.0
import org.kde.kirigami 2.7 as Kirigami
import "../../components"

Kirigami.Page {
    id: base;
    property string pageName: "accountsPageGit";
    title: accountDetails === null ? "" : accountDetails.readProperty("text");
    property QtObject accountDetails: null;
    Loader {
        anchors.fill: parent;
        source: "git/repositoryContentContainer.qml"
    }
}
