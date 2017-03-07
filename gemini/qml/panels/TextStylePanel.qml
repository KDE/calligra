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
import "../components"
import org.calligra 1.0

Item {
    property alias zoomLevel: paragraphStyles.zoomLevel;
    property alias document: paragraphStyles.document;
    property alias textEditor: paragraphStyles.textEditor;
    property alias cursorFont: paragraphStyles.cursorFont;
    opacity: parent.checked ? 1 : 0;
    Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
    anchors {
        top: parent.bottom;
        left: parent.left;
        topMargin: 4;
    }
    height: Constants.GridHeight * 6;
    width: Constants.GridWidth * 2;
    Rectangle {
        anchors.fill: parent;
        color: "white";
        border.color: "#e8e9ea";
        border.width: 1;
        opacity: 0.96;
    }
    BorderImage {
        anchors {
            fill: parent;
            topMargin: -28;
            leftMargin: -36;
            rightMargin: -36;
            bottomMargin: -44;
        }
        border { left: 36; top: 28; right: 36; bottom: 44; }
        horizontalTileMode: BorderImage.Stretch;
        verticalTileMode: BorderImage.Stretch;
        source: Settings.theme.image("drop-shadows.png");
    }
    Rectangle {
        id: titleBar;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: Constants.SmallFontSize + 2 * Constants.DefaultMargin;
        color: "#e8e9ea";
        Label {
            anchors {
                left: parent.left;
                leftMargin: Constants.DefaultMargin;
                verticalCenter: parent.verticalCenter;
            }
            text: "PARAGRAPH STYLES";
            color: "#5b6573";
            font: Settings.theme.font("applicationSemi");
        }
        Rectangle {
            anchors {
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            height: 1;
            color: "black";
            opacity: 0.5;
        }
    }
    ParagraphStylesModel {
        id: paragraphStyles;
        onCurrentStyleChanged: stylesList.positionViewAtIndex(currentStyle, ListView.Contain);
    }
    ListView {
        id: stylesList;
        anchors {
            top: titleBar.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
        clip: true;
        model: paragraphStyles;
        delegate: Item {
            width: ListView.view.width;
            height: Constants.GridHeight;
            clip: true;
            Rectangle {
                anchors.fill: parent;
                color: "#00adf5";
                opacity: model.current ? 0.6 : 0;
                Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
            }
            Text {
                anchors {
                    fill: parent;
                    leftMargin: Constants.DefaultMargin;
                }
                text: model.name;
                verticalAlignment: Text.AlignVCenter;
                font: model.font;
            }
            Rectangle {
                anchors.bottom: parent.bottom;
                width: parent.width;
                height: 1;
                color: "#e8e9ea";
                opacity: 0.7;
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: paragraphStyles.activate(index);
            }
        }
    }
}