/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

ColumnLayout {
    id: base;

    property bool enabled: true;
    property alias placeholder: textField.placeholderText;
    property real value: 0;
    property real min: 0;
    property real max: 1000;
    property int decimals: 2;

    onValueChanged: {
        if (decimals === 0) {
            if (value !== Math.round(value))
            {
                value = Math.round(value);
                return;
            }
        }
        else if (value * Math.pow(10, decimals) !== Math.round(value * Math.pow(10, decimals))) {
            value = Math.round(value * Math.pow(10, decimals)) / Math.pow(10, decimals);
            return;
        }
        if (value < min) {
            value = min;
            return;
        }
        if (value > max) {
            value = max;
            return;
        }
        if (textField.text != value) {
            textField.text = value.toFixed(decimals);
        }
        if (valueSlider.value !== (value / (max - min))) {
            valueSlider.value = ( (value - min) / (max - min) );
        }
    }

    QQC2.TextField {
        id: textField
        inputMethodHints: Qt.ImhFormattedNumbersOnly;
        onAccepted: value = (text / (base.max - base.min));
        Layout.fillWidth: true
    }

    QQC2.Slider {
        id: valueSlider;
        Layout.fillWidth: true
        onValueChanged: base.value = base.min + (value * (base.max - base.min));
    }
}
