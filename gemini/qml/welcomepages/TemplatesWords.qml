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

import QtQuick 1.1
import org.calligra 1.0
import "../components"

Page {
    id: base;
    property string pageName: "TemplatesWords";
    function activateTemplate(templateFile) {
        var file = Settings.wordsTemplateLocation(templateFile);
        console.debug(file);
        if(file.slice(-1) === "/" || file === "") {
            return;
        }
        baseLoadingDialog.visible = true;
        openFile("template://" + file);
    }
    ListModel {
        id: wordsTemplates;
        ListElement { text: "Blank A4 Document"; thumbnail: "template_a4"; color: "white"; templateFile: "Normal/.source/A4.odt"; variants: [ ] }
        ListElement { text: "Colorful Document"; thumbnail: "template_colorful"; color: "white"; templateFile: ""; variants: [
            ListElement { paperformat: "A4"; thumbnail: ""; swatch: "ColorPalettes-Red-1"; templateFile: "Normal/.source/ColorfulA4.odt"; color: "red" },
            ListElement { paperformat: "Letter"; thumbnail: ""; swatch: "ColorPalettes-Red-1"; templateFile: "Normal/.source/ColorfulLetter.odt"; color: "red" }
        ] }
        ListElement { text: "Fax Template"; thumbnail: "template_fax"; color: "#E8EFF1"; templateFile: ""; variants: [
            ListElement { paperformat: "A4"; thumbnail: ""; swatch: "ColorPalettes-Magenta-1"; templateFile: "Normal/.source/FaxA4.odt"; color: "black" },
            ListElement { paperformat: "Letter"; thumbnail: ""; swatch: "ColorPalettes-gold-1"; templateFile: "Normal/.source/FaxLetter.odt"; color: "black" }
        ] }
        ListElement { text: "Professional Letter"; thumbnail: "template_letter"; color: "#A9B3B6"; templateFile: ""; variants: [
            ListElement { paperformat: "A4"; thumbnail: ""; swatch: "ColorPalettes-Magenta-1"; templateFile: "Normal/.source/ProfessionalA4.odt"; color: "black" },
            ListElement { paperformat: "Letter"; thumbnail: ""; swatch: "ColorPalettes-gold-1"; templateFile: "Normal/.source/ProfessionalLetter.odt"; color: "black" }
        ] }
    }
    Flickable {
        id: wordsFlickable;
        anchors {
            fill: parent;
            margins: Constants.DefaultMargin;
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
                id: wordsTemplatesRepeater;
                model: wordsTemplates;
                Column {
                    width: (templatesFlow.width / 5) - Constants.DefaultMargin * 3;
                    height: width * 1.4;
                    spacing: Constants.DefaultMargin;
                    Item {
                        width: parent.width;
                        height: parent.height - templateName.height - Constants.DefaultMargin;
                        Image {
                            anchors.centerIn: parent;
                            height: parent.height - Constants.DefaultMargin * 2;
                            width: height;
                            source: model.thumbnail ? Settings.theme.icon(model.thumbnail, true) : "";
                            smooth: true;
                        }
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
                            property ListModel colorModel: model.variants;
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
                                if(model.variants.count === 0) {
                                    activateTemplate(model.templateFile);
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
    ScrollDecorator { flickableItem: wordsFlickable; anchors.fill: wordsFlickable; }
}
