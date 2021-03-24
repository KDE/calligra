// This file is part of the Calligra project
// SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-LicenseIdentifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami
import org.calligra 1.0
import org.kde.calligra 1.0 as Calligra

Kirigami.Page {
    property alias document: doc

    QQC2.BusyIndicator {
        anchors.centerIn: parent
        running: doc.status === Calligra.DocumentStatus.Loading
    }

    Calligra.ContentsModel {
        id: wordsContentModel;
        document: wordsDocument;
        useToC: false;
        thumbnailSize: Qt.size(Settings.theme.adjustedPixel(280), Settings.theme.adjustedPixel(360));
    }

    onNavigateModeChanged: {
        if(navigateMode === true) {
            // This means we've just changed back from having edited stuff.
            // Consequently we want to deselect all selections. Tell the canvas about that.
            wordsCanvas.deselectEverything();
            toolManager.requestToolChange("PageToolFactory_ID");
        }
    }

    Calligra.Document {
        id: doc

        readonly property bool failure: status === Calligra.DocumentStatus.Failed
        readOnly: true

        Timer {
            interval: 1
            running: doc.status === PageStatus.Active
            onTriggered: doc.source = page.source
        }

        onStatusChanged: {
            if (status === Calligra.DocumentStatus.Failed) {
                applicationWindow().showPassiveNotification(i18n("Failed to load document"))
            }
        }
    }
}
