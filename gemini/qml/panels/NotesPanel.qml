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
import "notespanelpages"
import org.calligra 1.0

Item {
    id: base;
    property QtObject canvas: null;
    Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
    height: Constants.GridHeight * 8;
    width: Constants.GridWidth * 3;
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

    PageStack {
        id: notesPageStack;
        property string customNoteTitleText;
        anchors.fill: parent;
        initialPage: mainView;
    }
    Component { id: mainView; NotesPanelMain { canvas: base.canvas; } }
    Component { id: summaryView; NotesPanelSummary { canvas: base.canvas; } }
    Component { id: customNoteView; NotesPanelAddCustom { canvas: base.canvas; titleText: notesPageStack.customNoteTitleText; } }
}
