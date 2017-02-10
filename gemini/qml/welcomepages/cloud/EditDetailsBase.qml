 
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

import QtQuick 2.0
import org.calligra 1.0
import QtQuick.Controls 1.4 as QtControls
import "../../components"

Rectangle {
    anchors.fill: parent;
    anchors.margins: Settings.theme.adjustedPixel(16);
    property int accountIndex: -1;
    property string text;
    onTextChanged: {
        nameField.text = text;
    }
    property Component addEmpty;
    radius: Settings.theme.adjustedPixel(8);
    color: "white";
    Rectangle {
        anchors {
            fill: parent;
            margins: -Settings.theme.adjustedPixel(16);
            topMargin: -(Settings.theme.adjustedPixel(8) + Constants.GridHeight * 1.5);
        }
        opacity: 0.5;
        color: "white";
        MouseArea { anchors.fill: parent; onClicked: { /*nothing */ } }
        SimpleTouchArea { anchors.fill: parent; onTouched: { /*nothing */ } }
    }
    TextField {
        id: nameField;
        anchors {
            verticalCenter: parent.verticalCenter;
            left: parent.left;
            right: parent.right;
            margins: Settings.theme.adjustedPixel(16);
        }
        placeholder: "Account Name";
    }
    QtControls.Button {
        id: okButton;
        anchors {
            bottom: parent.bottom;
            right: cancelButton.left;
            margins: Settings.theme.adjustedPixel(8);
        }
        text: "Save";
        onClicked: {
            cloudAccounts.renameAccount(base.accountIndex, nameField.text);
            dlgStack.replace(addEmpty);
        }
    }
    QtControls.Button {
        id: cancelButton;
        anchors {
            bottom: parent.bottom;
            right: parent.right;
            margins: Settings.theme.adjustedPixel(8);
        }
        text: "Cancel";
        onClicked: dlgStack.replace(addEmpty);
    }
}
