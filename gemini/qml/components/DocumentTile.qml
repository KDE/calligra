// This file is part of the KDE project
// SPDX-FileCopyrightText: 2019 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.11
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.7 as Kirigami
import org.calligra 1.0

QQC2.ItemDelegate {
    id: component;
    property string filePath;
    property string imageUrl: "image://recentimage/" + component.filePath;
    property alias title: lblName.text;
    implicitWidth: GridView.view.cellWidth;
    implicitHeight: GridView.view.cellHeight
    contentItem: ColumnLayout {
        Image {
            id: documentImage;
            source: component.imageUrl;
            fillMode: Image.PreserveAspectFit;
            Layout.fillHeight: true
            Layout.fillWidth: true
            smooth: true;
            asynchronous: true;
            QQC2.BusyIndicator {
                anchors.centerIn: parent
                width: docList.cellWidth / 3
                height: width
                running: parent.status === Image.Loading
            }
        }
        QQC2.Label {
            id: lblName;
            horizontalAlignment: Text.AlignHCenter;
            verticalAlignment: Text.AlignVCenter;
            elide: Text.ElideRight
            Layout.fillWidth: true
        }
    }
}
