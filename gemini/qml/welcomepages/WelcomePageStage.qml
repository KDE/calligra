// This file is part of the KDE project
// SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.14
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.calligra 1.0

Kirigami.ScrollablePage {
    id: base;
    objectName: "WelcomePageStage";
    title: i18n("Pick a Presentation Template")
    function activateTemplate(templateFile) {
        console.debug(templateFile);
        if(templateFile.slice(-1) === "/" || templateFile === "") {
            return;
        }
        openFile("template://" + templateFile);
    }
    TemplatesModel {
        id: stageTemplates;
        templateType: STAGE_MIME_TYPE;
        showWide: true;
    }
    TemplatesModel {
        id: stageTemplatesClassic;
        templateType: STAGE_MIME_TYPE;
        showWide: false;
    }

// templates to make:
//         ListElement { text: "Metropolitan"; thumbnail: ""; color: "#E8EFF1"; templateFile: ""; variants: [ ] }
//         ListElement { text: "Elegant"; thumbnail: ""; color: "#A9B3B6"; templateFile: ""; variants: [ ] }
//         ListElement { text: "Vintage"; thumbnail: ""; color: "#828C8F"; templateFile: ""; variants: [ ] }
//         ListElement { text: "Modernist"; thumbnail: ""; color: "#A9B3B6"; templateFile: ""; variants: [ ] }
//         ListElement { text: "Distraught"; thumbnail: ""; color: "#828C8F"; templateFile: ""; variants: [ ] }
//         ListElement { text: "Fabric"; thumbnail: ""; color: "#E8EFF1"; templateFile: ""; variants: [ ] }
//         ListElement { text: "Metallic"; thumbnail: ""; color: "#A9B3B6"; templateFile: ""; variants: [ ] }
//         ListElement { text: "Luna Rising"; thumbnail: ""; color: "#828C8F"; templateFile: ""; variants: [ ] }
//         ListElement { text: "Galactic Voyage"; thumbnail: ""; color: "#E8EFF1"; templateFile: ""; variants: [] }

    property bool showClassicModel: true
    actions: [
        Kirigami.Action {
            text: i18n("Classic");
            onToggled: base.showClassicModel = checked
            checked: showClassicModel
            checkable: true
        },
        Kirigami.Action {
            text: i18n("Wide");
            onToggled: base.showClassicModel = !checked
            checked: !showClassicModel
            checkable: true
        }
    ]

    GridView {
        id: docList;
        cellWidth: Math.floor(width/Math.floor(width/(Kirigami.Units.gridUnit * 14 + Kirigami.Units.largeSpacing * 2)))
        cellHeight: cellWidth - Kirigami.Units.gridUnit * 6 + Kirigami.Theme.defaultFont.pixelSize
        model: showClassicModel ? stageTemplatesClassic : stageTemplates

        delegate: QQC2.ItemDelegate {
            implicitWidth: GridView.view.cellWidth;
            implicitHeight: GridView.view.cellHeight
            padding: Kirigami.Units.largeSpacing
            onClicked: {
                if (model.variantCount === 0) {
                    activateTemplate(model.url);
                } else {
                    // then there are variants to choose between, let the user see!
                    variantSelector.model = model.variants;
                    variantSelector.opacity = 1;
                }
            }
            contentItem: ColumnLayout {
                Image {
                    source: model.thumbnail ? model.thumbnail : "";
                    fillMode: Image.PreserveAspectFit;
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    smooth: true;
                    Rectangle {
                        anchors.fill: parent;
                        visible: !model.thumbnail
                        color: model.thumbnail ? "transparent" : model.color;
                        border {
                            width: 1;
                            color: Kirigami.Theme.textColor
                        }
                        opacity: 0.4;
                    }
                }
                Row {
                    Layout.preferredHeight: Kirigami.Units.largeSpacing * 2;
                    Layout.alignment: Qt.AlignHCenter
                    spacing: Kirigami.Units.largeSpacing;
                    property QtObject colorModel: model.variants;
                    Repeater {
                        model: parent.colorModel;
                        Rectangle {
                            height: Kirigami.Units.largeSpacing * 2;
                            width: height;
                            radius: Kirigami.Units.largeSpacing;
                            color: model.color;
                        }
                    }
                }
                QQC2.Label {
                    id: templateName;
                    horizontalAlignment: Text.AlignHCenter;
                    verticalAlignment: Text.AlignVCenter;
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                    text: model.text;
                }
            }
        }
    }
}
