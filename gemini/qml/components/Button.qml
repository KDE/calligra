/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import org.calligra 1.0

Item {
    id: base;

    signal clicked();

    property alias image: icon.source;
    property int imageMargin: 8;
    property color color: Settings.theme.color("components/button/base");
    property alias border: fill.border;
    property alias radius: fill.radius;
    property alias text: label.text;
    property color textColor: Settings.theme.color("components/button/text");
    property alias textSize: label.font.pixelSize;
    property alias bold: label.font.bold;
    property alias font: label.font;
    property bool shadow: false;
    property bool enabled: true; // XXX: visualize disabledness
    property alias asynchronous: icon.asynchronous;

    property bool highlight: false;
    property color highlightColor: Settings.theme.color("components/button/highlight");
    property double highlightOpacity: 1;

    property bool checkable: false;
    property bool checked: false;
    property int checkedMargin: 2;
    property color checkedColor: Settings.theme.color("components/button/checked");
    property double checkedOpacity: 1;

    property bool hasFocus: false;

    property string tooltip: "";

    width: Constants.GridWidth;
    height: Constants.GridHeight;

    Rectangle {
        id: fill;
        anchors.fill: parent;
        anchors.margins: 0;
        color: "transparent";
        visible: true

        Rectangle {
            anchors.fill: parent;
            radius: parent.radius;
            opacity: base.highlightOpacity;
            color: base.highlight && mouse.pressed && base.enabled ? base.highlightColor : base.color;
        }

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
            anchors.margins: base.checkedMargin;
            color: base.checkedColor;
            radius: fill.radius;//base.height === base.width ? base.height / 2 - 1 : base.radius;
        }

        Image {
            id: icon;
            anchors.left: (label.text == "") ? undefined : parent.left;
            anchors.horizontalCenter: (label.text == "") ? parent.horizontalCenter : undefined;
            anchors.top: parent.top;
            anchors.bottom: parent.bottom;
            anchors.margins: base.imageMargin;
            width: height;
            fillMode: Image.PreserveAspectFit;
            smooth: true;
            opacity: base.enabled ? 1 : 0.7;
            Behavior on opacity { NumberAnimation { duration: Constants.AnimationDuration; } }

            sourceSize.width: width > height ? height : width;
            sourceSize.height: width > height ? height : width;
        }

        Label {
            id: label;
            anchors.verticalCenter: parent.verticalCenter;
            height: font.pixelSize;
            width: parent.width;
            horizontalAlignment: Text.AlignHCenter;
            elide: Text.ElideRight;
            opacity: base.enabled ? 1 : 0.7;
            color: base.textColor;
            visible: (icon.source == "")
        }
        Label {
            id: otherLabel;
            anchors.left: icon.right;
            anchors.leftMargin: Constants.DefaultMargin;
            anchors.verticalCenter: parent.verticalCenter;
            text: label.text;
            font: label.font;
            height: font.pixelSize;
            width: parent.width - icon.width;
            elide: label.elide;
            opacity: base.enabled ? 1 : 0.7;
            color: base.textColor;
            visible: (icon.source != "")
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
