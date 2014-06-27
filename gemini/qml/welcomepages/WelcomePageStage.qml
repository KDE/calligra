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
    function activateTemplate(templateFile) {
        var file = Settings.stageTemplateLocation(templateFile);
        console.debug(file);
        if(file.slice(-1) === "/" || file === "") {
            return;
        }
        baseLoadingDialog.visible = true;
        openFile(file);
    }
    ListModel {
        id: stageTemplatesClassic
        ListElement { text: "Standard"; thumbnail: ""; color: "white"; templateFile: "Screen/.source/emptyLandscape.otp"; variants: [ ] }
    }
    ListModel {
        id: stageTemplates;
        ListElement { text: "Standard"; thumbnail: ""; color: "white"; templateFile: "Screen/.source/emptyLandscapeWide.otp"; variants: [ ] }
        ListElement { text: "White"; thumbnail: ""; color: "white"; templateFile: ""; variants: [
            ListElement { thumbnail: ""; swatch: "ColorPalettes-Blue-1"; templateFile: "Screen/.source/emptyLandscapeWide.otp"; color: "darkblue" },
            ListElement { thumbnail: ""; swatch: "ColorPalettes-Blue-1"; templateFile: "Screen/.source/emptyLandscapeWide.otp"; color: "lightblue" },
            ListElement { thumbnail: ""; swatch: "ColorPalettes-1-04"; templateFile: "Screen/.source/emptyLandscapeWide.otp"; color: "crimson" }
        ] }
        ListElement { text: "Black"; thumbnail: "Presentaton-Magenta"; color: "white"; templateFile: ""; variants: [
            ListElement { thumbnail: "Presentaton-Magenta"; swatch: "ColorPalettes-Magenta-1"; templateFile: "odf/.source/cohere-magenta.otp"; color: "magenta" },
            ListElement { thumbnail: "Presentaton-Blue"; swatch: "ColorPalettes-Blue-1"; templateFile: "odf/.source/cohere-blue.otp"; color: "blue" },
            ListElement { thumbnail: "Presentaton-Gold"; swatch: "ColorPalettes-gold-1"; templateFile: "odf/.source/cohere-gold.otp"; color: "gold" },
            ListElement { thumbnail: "Presentaton-Red"; swatch: "ColorPalettes-Red-1"; templateFile: "odf/.source/cohere-red.otp"; color: "red" }
            ] }
        ListElement { text: "Metropolitan"; thumbnail: ""; color: "#E8EFF1"; templateFile: ""; variants: [ ] }
        ListElement { text: "Elegant"; thumbnail: ""; color: "#A9B3B6"; templateFile: ""; variants: [ ] }
        ListElement { text: "Vintage"; thumbnail: ""; color: "#828C8F"; templateFile: ""; variants: [ ] }
        ListElement { text: "Modernist"; thumbnail: ""; color: "#A9B3B6"; templateFile: ""; variants: [ ] }
        ListElement { text: "Distraught"; thumbnail: ""; color: "#828C8F"; templateFile: ""; variants: [ ] }
        ListElement { text: "Fabric"; thumbnail: ""; color: "#E8EFF1"; templateFile: ""; variants: [ ] }
        ListElement { text: "Metallic"; thumbnail: ""; color: "#A9B3B6"; templateFile: ""; variants: [ ] }
        ListElement { text: "Luna Rising"; thumbnail: ""; color: "#828C8F"; templateFile: ""; variants: [ ] }
        ListElement { text: "Galactic Voyage"; thumbnail: ""; color: "#E8EFF1"; templateFile: ""; variants: [ ] }
        ListElement { text: "Blue Orange Vector"; thumbnail: ""; color: "#E8EFF1"; templateFile: "odf/.source/blue_orange_vector.otp"; variants: [ ] }
        ListElement { text: "Burning Desire"; thumbnail: ""; color: "#828C8F"; templateFile: "odf/.source/burning_desire.otp"; variants: [ ] }
        ListElement { text: "Business"; thumbnail: ""; color: "#828C8F"; templateFile: "odf/.source/business.otp"; variants: [ ] }
        ListElement { text: "Flood Light"; thumbnail: ""; color: "#E8EFF1"; templateFile: "odf/.source/flood_light.otp"; variants: [ ] }
        ListElement { text: "Rounded Square"; thumbnail: ""; color: "#A9B3B6"; templateFile: "odf/.source/rounded_square.otp"; variants: [ ] }
        ListElement { text: "Simple Waves"; thumbnail: ""; color: "white"; templateFile: "odf/.source/simple_waves.otp"; variants: [ ] }
        ListElement { text: "Skyline Monotone"; thumbnail: ""; color: "#A9B3B6"; templateFile: "odf/.source/skyline_monotone.otp"; variants: [ ] }
        ListElement { text: "Strange Far Hills"; thumbnail: ""; color: "#E8EFF1"; templateFile: "odf/.source/strange_far_hills.otp"; variants: [ ] }
    }
    Label {
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: Constants.GridHeight * 1.5;
        verticalAlignment: Text.AlignVCenter;
        horizontalAlignment: Text.AlignHCenter;
        font: Settings.theme.font("pageHeader");
        text: "Select a template";
        color: "#22282f";
        Row {
            anchors {
                right: parent.right;
                rightMargin: Settings.theme.adjustedPixel(20);
                verticalCenter: parent.verticalCenter;
            }
            height: parent.height - Constants.DefaultMargin * 2;
            spacing: Settings.theme.adjustedPixel(4);
            CohereButton {
                anchors.verticalCenter: parent.verticalCenter;
                checked: stageTemplatesRepeater.model === stageTemplatesClassic;
                text: "Classic";
                textColor: "#5b6573";
                textSize: Settings.theme.adjustedPixel(18);
                checkedColor: "#D2D4D5";
                onClicked: { if(!checked) { stageTemplatesRepeater.model = stageTemplatesClassic; } }
            }
            CohereButton {
                anchors.verticalCenter: parent.verticalCenter;
                checked: stageTemplatesRepeater.model === stageTemplates;
                text: "Wide";
                textColor: "#5b6573";
                textSize: Settings.theme.adjustedPixel(18);
                checkedColor: "#D2D4D5";
                onClicked: { if(!checked) { stageTemplatesRepeater.model = stageTemplates; } }
            }
        }
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
                        source: model.thumbnail ? Settings.theme.icon(model.thumbnail) : "";
                        width: parent.width;
                        height: parent.height - templateName.height - Constants.DefaultMargin;
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
    ScrollDecorator { flickableItem: stageFlickable; anchors.fill: stageFlickable; }
}
