/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

// A quick hack on the button to make it work more akin to how Cohere demands text-only buttons must work

import QtQuick 2.0
import org.calligra 1.0

Item {
    id: base;

    signal clicked();

    property color color: Settings.theme.color("components/button/base");
    property alias border: fill.border;
    property alias radius: fill.radius;
    property alias text: label.text;
    property color textColor: Settings.theme.color("components/button/text");
    property alias font: label.font;
    property alias textSize: label.font.pixelSize;
    property alias bold: label.font.bold;
    property bool shadow: false;
    property bool enabled: true; // XXX: visualize disabledness

    property bool highlight: false;
    property color highlightColor: Settings.theme.color("components/button/highlight");

    property bool checkable: false;
    property bool checked: false;
    property color checkedColor: Settings.theme.color("components/button/checked");
    property double checkedOpacity: 1;

    property bool hasFocus: false;

    property string tooltip: "";

    width: label.width + Settings.theme.adjustedPixel(40);
    height: label.height + Constants.DefaultMargin * 4;

    Rectangle {
        id: fill;
        anchors.fill: parent;
        anchors.margins: 0;
        color: base.highlight && mouse.pressed && base.enabled ? base.highlightColor : base.color;
        visible: true
        radius: 4;

        Rectangle {
            anchors {
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
                margins: fill.radius / 2;
            }
            height: fill.radius / 2;
            radius: fill.radius / 4;
            color: base.textColor;
            visible: base.hasFocus;
            opacity: 0.3
        }

        Rectangle {
            id: checkedVisualiser;
            opacity: base.checked ? base.checkedOpacity : 0;
            Behavior on opacity { NumberAnimation { duration: Constants.AnimationDuration; } }
            anchors.fill: parent;
            anchors.margins: 2;
            color: base.checkedColor;
            radius: fill.radius;//base.height === base.width ? base.height / 2 - 1 : base.radius;
        }

        Text {
            id: label;
            anchors.centerIn: parent;
            height: paintedHeight;
            width: paintedWidth;
            opacity: base.enabled ? 1 : 0.7;
            color: base.textColor;
            font: Settings.theme.font("applicationSemi");
        }
//         Rectangle {
//             id: enabledVisualiser;
//             opacity: base.enabled ? 0 : 0.7;
//             anchors.fill: parent;
//             color: "black";
//         }
    }

    SimpleTouchArea {
        anchors.fill: parent;
        onTouched: {
            if (base.enabled) {
                base.clicked();
                if ( base.checkable ) {
                    base.checked = !base.checked;
                }
            }
        }
    }
    MouseArea {
        id: mouse;
        anchors.fill: parent;
        hoverEnabled: true;
        acceptedButtons: Qt.LeftButton | Qt.RightButton;
        
        onClicked: {
            if(mouse.button == Qt.LeftButton && base.enabled) {
                base.clicked();
                if ( base.checkable ) {
                    base.checked = !base.checked;
                }
            } else if(mouse.button == Qt.RightButton && base.tooltip != "") {
                tooltip.show(base.width / 2, 0);
            }
        }
        onEntered: {
            hoverDelayTimer.start();
        }
        onPositionChanged: {
            if(hoverDelayTimer.running) {
                hoverDelayTimer.restart();
            }
        }
        onExited: {
            hoverDelayTimer.stop();
            tooltip.hide();
        }
    }

    Timer {
        id: hoverDelayTimer;
        interval: 1000;
        onTriggered: { if(base.tooltip != "") tooltip.show(base.width / 2, 0) }
    }

    Tooltip {
        id: tooltip;
        text: base.tooltip;
    }

    states: State {
        name: "pressed";
        when: (mouse.pressed || base.checked) && enabled;

        PropertyChanges {
            target: mouse
            anchors.topMargin: 0
        }
    }

    transitions: Transition {
        ParallelAnimation {
            NumberAnimation { properties: "size"; duration: 50; }
            ColorAnimation { duration: 50; }
        }
    }
}
