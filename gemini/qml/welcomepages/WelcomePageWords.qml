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
import "../components"

Page {
    id: base;
    Label {
        id: docTypeSelectorRow;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: Constants.GridHeight * 1.5;
        verticalAlignment: Text.AlignVCenter;
        horizontalAlignment: Text.AlignHCenter;
        font: Settings.theme.font("pageHeader");
        text: "Create a Text Document";
        color: "#22282f";
        Row {
            anchors {
                right: parent.right;
                rightMargin: 20;
                verticalCenter: parent.verticalCenter;
            }
            height: parent.height - Constants.DefaultMargin * 2;
            spacing: 4;
            CohereButton {
                anchors.verticalCenter: parent.verticalCenter;
                text: "Custom Document";
                textColor: "#5b6573";
                textSize: Settings.theme.adjustedPixel(18);
                checkedColor: "#D2D4D5";
                onClicked: { if(!checked) { wordsDocStack.replace(customDocWords); } }
                checked: wordsDocStack.currentPage.pageName === "CustomDocWords";
            }
            CohereButton {
                anchors.verticalCenter: parent.verticalCenter;
                text: "Templates";
                textColor: "#5b6573";
                textSize: Settings.theme.adjustedPixel(18);
                checkedColor: "#D2D4D5";
                onClicked: { if(!checked) { wordsDocStack.replace(templatesWords); } }
                checked: wordsDocStack.currentPage.pageName === "TemplatesWords";
            }
        }
    }
    PageStack {
        id: wordsDocStack;
        anchors {
            margins: Constants.DefaultMargin;
            top: docTypeSelectorRow.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
            bottomMargin: 0;
        }
        initialPage: templatesWords;
    }
    Component { id: customDocWords; CustomDocWords { } }
    Component { id: templatesWords; TemplatesWords { } }
}
