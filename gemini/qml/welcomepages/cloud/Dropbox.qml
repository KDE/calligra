/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import org.calligra 1.0
import org.kde.kirigami 2.7 as Kirigami
import "dropbox" as Dropbox
import "../../components"

Kirigami.ScrollablePage {
    id: base;
    property string pageName: "accountsPageDropbox";
    property QtObject accountDetails;
    title: "DropBox";
    Dropbox.FileNavigator {
    }
}
