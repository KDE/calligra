/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import "../components"

Item {
    id: base;
    signal interactionStarted();
    opacity: parent.checked ? 1 : 0;
    Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
    anchors {
        top: parent.bottom;
        right: parent.right;
        rightMargin: -Settings.theme.adjustedPixel(10);
        topMargin: 4;
    }
    height: Settings.theme.adjustedPixel(70) * 4;
    width: Constants.GridWidth * 2;
    Rectangle {
        anchors.fill: parent;
        color: "#4e5359";
        opacity: 0.96;
        border.color: "#22282f";
        border.width: 1;
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
    Column {
        anchors.fill: parent;
        Button {
            opacity: switchToDesktopAction.enabled ? 1 : 0.2;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
            image: Settings.theme.icon("SVG-Icon-DesktopMode-1");
            imageMargin: Settings.theme.adjustedPixel(11);
            text: "Switch to Desktop Mode";
            font: Settings.theme.font("optionsMenu");
            height: Settings.theme.adjustedPixel(70);
            width: parent.width;
            onClicked: {
                base.interactionStarted();
                switchToDesktopAction.trigger();
            }
            highlight: true; highlightOpacity: 0.6;
        }
        Button {
            image: Settings.theme.icon("SVG-Icon-Help-1");
            imageMargin: Settings.theme.adjustedPixel(11);
            text: "Help";
            font: Settings.theme.font("optionsMenu");
            height: Settings.theme.adjustedPixel(70);
            width: parent.width;
            highlight: true; highlightOpacity: 0.6;
        }
        Button {
            image: Settings.theme.icon("SVG-Icon-Find-1");
            imageMargin: Settings.theme.adjustedPixel(11);
            text: "Find";
            font: Settings.theme.font("optionsMenu");
            height: Settings.theme.adjustedPixel(70);
            width: parent.width;
            highlight: true; highlightOpacity: 0.6;
        }
        Button {
            image: Settings.theme.icon("SVG-Icon-SpellCheck-1");
            imageMargin: Settings.theme.adjustedPixel(11);
            text: "Check Spelling";
            font: Settings.theme.font("optionsMenu");
            height: Settings.theme.adjustedPixel(70);
            width: parent.width;
            highlight: true; highlightOpacity: 0.6;
        }
    }
}
