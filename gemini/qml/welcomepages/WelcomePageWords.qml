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
    objectName: "WelcomePageWords"
    title: i18n("Create a Text Document")
    actions: Kirigami.Action {
        text: i18n("Create Custom Document...")
        icon.name: "configure"
        onTriggered: pageStack.push(customDocWords);
    }
    Component { id: customDocWords; CustomDocWords { } }
    function activateTemplate(templateFile) {
        console.debug(templateFile);
        if(templateFile.slice(-1) === "/" || templateFile === "") {
            return;
        }
        openFile("template://" + templateFile);
    }
    TemplatesModel {
        id: wordsTemplates;
        templateType: WORDS_MIME_TYPE;
    }

    GridView {
        id: docList;
        cellWidth: Math.floor(width/Math.floor(width/(Kirigami.Units.gridUnit * 10 + Kirigami.Units.largeSpacing * 2)))
        cellHeight: cellWidth + Kirigami.Theme.defaultFont.pixelSize
        model: wordsTemplates

        delegate: QQC2.ItemDelegate {
            implicitWidth: GridView.view.cellWidth;
            implicitHeight: GridView.view.cellHeight
            padding: Kirigami.Units.largeSpacing

            onClicked: {
                if(model.variantCount === 0) {
                    activateTemplate(model.url);
                } else {
                    // then there are variants to choose between, let the user see!
                    wordsVariantSelector.model = model.variants;
                    wordsVariantSelector.opacity = 1;
                }
            }

            contentItem: ColumnLayout {
                Image {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    source: model.thumbnail ? model.thumbnail : "";
                    fillMode: Image.PreserveAspectFit;
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
                    spacing: Kirigami.Units.largeSpacing;
                    property QtObject colorModel: model.variants;
                    Layout.alignment: Qt.AlignHCenter
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
