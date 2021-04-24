/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
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
