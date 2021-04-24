/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 2.1 as QQC2
import org.kde.kirigami 2.4 as Kirigami
import QtQuick.Layouts 1.2
import org.calligra 1.0

Item {
    id: base;
    enabled: (visible && opacity > 0.0);

    property alias title: dialog.title;
    property alias message: dialogLabel.text;
    property variant buttons: null;
    property alias textAlign: dialogLabel.horizontalAlignment;
    property alias modalBackgroundColor: modalFill.color;
    property int progress: -1;

    property int defaultButton: 0;
    property int currentButton: defaultButton;

    signal buttonClicked(int button);
    signal canceled();

    function show(message) {
        if (message) {
            base.message = message;
        }
        base.opacity = 1;
    }

    function hide(message) {
        if (message) {
            base.message = message;
        }
        base.opacity = 0;
        base.currentButton = base.defaultButton; 
    }

    onEnabledChanged: {
        if (base.enabled) {
            dialog.open();
        } else {
            dialog.close();
        }
    }

    anchors.fill: parent;
    z: 99; //Just to make sure we're always on top.
    opacity: 0;
    Behavior on opacity { NumberAnimation { duration: Constants.AnimationDuration; } }

    MouseArea {
        anchors.fill: parent;
        onClicked: {
            // Don't allow people to click away a progress bar...
            // horrible things could happen if they do so (inconsistent states and what not)
            if (progress !== -1)
                return;
            base.canceled();
            base.hide();
        }
    }
    SimpleTouchArea {
        anchors.fill: parent;
        onTouched: {
            // Don't allow people to click away a progress bar...
            // horrible things could happen if they do so (inconsistent states and what not)
            if (progress !== -1)
                return;
            base.canceled();
            base.hide();
        }
    }

    Rectangle {
        id: modalFill;

        anchors.fill: parent;
        color: Settings.theme.color("components/dialog/modalOverlay");

        Keys.enabled: base.visible && base.opacity === 1;
        Keys.onEscapePressed: {
                // Don't allow people to escape from a progress bar...
                // horrible things could happen if they do so (inconsistent states and what not)
                if (progress !== -1)
                    return;
                base.canceled();
                base.hide();
            }
        Keys.onEnterPressed: { base.buttonClicked(base.currentButton); base.hide(); }
        Keys.onReturnPressed: { base.buttonClicked(base.currentButton); base.hide(); }
        focus: Keys.enabled;
        Keys.onTabPressed: {
            base.currentButton += 1;
            if (base.currentButton >= base.buttons.length) {
                base.currentButton = 0;
            }
        }
    }

    QQC2.Dialog {
        id: dialog
        modal: true
        focus: true
        x: (base.width - width) / 2
        y: base.height / 2 - height
        width: Math.min(base.width - Kirigami.Units.gridUnit * 4, Kirigami.Units.gridUnit * 20)
        height: 200

        contentItem: ColumnLayout {
            QQC2.Label {
                id: dialogLabel
                width: dialog.availableWidth
                wrapMode: Label.Wrap
            }

            QQC2.BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                running: base.enabled
            }
        }
    }
}
