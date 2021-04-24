/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0

Text {
    verticalAlignment: Text.AlignVCenter;

//     font.pixelSize: Constants.DefaultFontSize;

    color: Settings.theme.color("components/label");
//     font.family: "Source Sans Pro"

    font: Settings.theme.font("application");

    elide: Text.ElideRight;
}
