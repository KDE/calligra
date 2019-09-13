/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

import QtQuick 2.0
import org.kde.kirigami 2.7 as Kirigami
import org.calligra 1.0
import "../components"

Kirigami.ScrollablePage {
    id: base;
    objectName: "WelcomePageStage";
    title: "Pick a Presentation Template"
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

    actions {
        contextualActions: [
            Kirigami.Action {
                text: "Classic";
                onTriggered: { if(!checked) { stageTemplatesRepeater.model = stageTemplatesClassic; } }
                checked: stageTemplatesRepeater.model === stageTemplatesClassic;
            },
            Kirigami.Action {
                text: "Wide";
                onTriggered: { if(!checked) { stageTemplatesRepeater.model = stageTemplates; } }
                checked: stageTemplatesRepeater.model === stageTemplates;
            }
        ]
    }
    Flickable {
        id: stageFlickable;
        anchors {
            fill: parent;
            margins: Constants.DefaultMargin;
            topMargin: Constants.GridHeight * 1.5;
            bottomMargin: 0;
        }
        contentWidth: templatesFlow.width;
        contentHeight: templatesFlow.height;
        clip: true;
        Flow {
            id: templatesFlow;
            width: base.width - Constants.DefaultMargin * 2;
            spacing: Constants.DefaultMargin * 3;
            Repeater {
                id: stageTemplatesRepeater;
                model: stageTemplates;
                Column {
                    width: (templatesFlow.width / 4) - Constants.DefaultMargin * 3;
                    height: width * 0.7;
                    spacing: Constants.DefaultMargin;
                    Image {
                        source: model.thumbnail ? model.thumbnail : "";
                        width: parent.width;
                        height: parent.height - templateName.height - Constants.DefaultMargin;
                        fillMode: Image.PreserveAspectFit;
                        smooth: true;
                        Rectangle {
                            anchors.fill: parent;
                            color: model.thumbnail ? "transparent" : model.color;
                            border {
                                width: 1;
                                color: "#c1cdd1";
                            }
                            opacity: 0.6;
                        }
                        Row {
                            anchors {
                                bottom: parent.bottom;
                                right: parent.right;
                                margins: Constants.DefaultMargin;
                            }
                            height: Constants.DefaultMargin * 2;
                            spacing: Constants.DefaultMargin;
                            property QtObject colorModel: model.variants;
                            Repeater {
                                model: parent.colorModel;
                                Rectangle {
                                    height: Constants.DefaultMargin * 2;
                                    width: height;
                                    radius: Constants.DefaultMargin;
                                    color: model.color;
                                }
                            }
                        }
                        MouseArea {
                            anchors.fill: parent;
                            onClicked: {
                                if(model.variantCount === 0) {
                                    activateTemplate(model.url);
                                }
                                else {
                                    // then there are variants to choose between, let the user see!
                                    variantSelector.model = model.variants;
                                    variantSelector.opacity = 1;
                                }
                            }
                        }
                    }
                    Label {
                        id: templateName;
                        width: parent.width;
                        horizontalAlignment: Text.AlignHCenter;
                        text: model.text;
                        font: Settings.theme.font("templateLabel");
                        color: "#5b6573";
                    }
                }
            }
        }
    }
}
