/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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

import QtQuick 1.1
import org.krita.sketch 1.0

Rectangle {
    id: base;

    property bool keyboardVisible: false;

    anchors.left: parent.left;
    anchors.right: parent.right;

    y: parent.height;
    height: parent.height * 0.33;
    color: "black";

    Flow {
        anchors.fill: parent;
        anchors.topMargin: 4;
        anchors.leftMargin: Constants.GridWidth * 1.5;
        anchors.rightMargin: Constants.GridWidth * 1.5;
        anchors.bottomMargin: 4;

        Repeater {
            model: keys;
            delegate: keyDelegate;
        }
    }

    states: State {
        name: "visible";
        PropertyChanges { target: base; y: base.parent.height * 0.67; keyboardVisible: true; }
    }

    transitions: Transition {
        reversible: true;
        SequentialAnimation {
            NumberAnimation { properties: "y"; }
            PropertyAction { property: "keyboardVisible"; }
        }
    }

    Connections {
        target: Krita.VirtualKeyboardController;

        onShowKeyboard: base.state = "visible";
        onHideKeyboard: base.state = "";
    }

    ListModel {
        id: keys;

        ListElement { text: "q" }
        ListElement { text: "w" }
        ListElement { text: "e" }
        ListElement { text: "r" }
        ListElement { text: "t" }
        ListElement { text: "y" }
        ListElement { text: "u" }
        ListElement { text: "i" }
        ListElement { text: "o" }
        ListElement { text: "p" }
        ListElement { text: "⇐"; width: 2; special: true; script: "Settings.focusItem.text = Settings.focusItem.text.substring(0, Settings.focusItem.text.length - 1);" }

        ListElement { spacer: true; width: 0.5; }
        ListElement { text: "a" }
        ListElement { text: "s" }
        ListElement { text: "d" }
        ListElement { text: "f" }
        ListElement { text: "g" }
        ListElement { text: "h" }
        ListElement { text: "j" }
        ListElement { text: "k" }
        ListElement { text: "l" }
        ListElement { text: "'" }
        ListElement { text: "Enter"; width: 1.5; special: true; script: "base.state = \"\""; }

        ListElement { text: "⇑"; special: true; script: ""; }
        ListElement { text: "z" }
        ListElement { text: "x" }
        ListElement { text: "c" }
        ListElement { text: "v" }
        ListElement { text: "b" }
        ListElement { text: "n" }
        ListElement { text: "m" }
        ListElement { text: "," }
        ListElement { text: "." }
        ListElement { text: "?" }
        ListElement { text: "⇑"; special: true; script: ""; }

        ListElement { text: "&123"; special: true; script: ""; }
        ListElement { text: "Ctrl"; enabled: false; }
        ListElement { text: "Smi"; enabled: false; }
        ListElement { text: " "; width: 6; }
        ListElement { text: "←"; special: true; script: "Settings.focusItem.cursorPosition = Settings.focusItem.cursorPosition - 1;" }
        ListElement { text: "→"; special: true; script: "Settings.focusItem.cursorPosition = Settings.focusItem.cursorPosition + 1;" }
        ListElement { text: "Kbd"; enabled: false; }
    }

    Component {
        id: keyDelegate;

        Item {
            width: (Constants.GridWidth * 0.75) * (model.width !== undefined ? model.width : 1);
            height: (base.height - 8) / 4;
            property bool enabled: model.enabled !== undefined ? model.enabled : true;

            Button {
                anchors.fill: parent;
                anchors.margins: 4;

                border.width: model.spacer ? 0 : 2;
                border.color: parent.enabled ? "white" : "#333333";
                radius: 8;

                text: model.spacer ? "" : model.text;
                textColor: parent.enabled ? "white" : "#333333";

                highlight: true;
                highlightColor: "#666666";

                onClicked: {
                    if(model.spacer || !parent.enabled)
                        return;

                    if(model.special) {
                        eval(model.script);
                        return;
                    }

                    Settings.focusItem.text += model.text;
                }
            }
        }
    }
}