/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QtControls
import org.kde.kirigami as Kirigami
import org.calligra
import "git"

Kirigami.Dialog {
    id: component

    property string serviceName: "";

    title: i18nc("@title:dialog", "Add Git Account")

    contentItem: CreateCheckout {}
}
