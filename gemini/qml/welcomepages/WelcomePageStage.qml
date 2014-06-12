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
        id: stageTemplates;
        ListElement { text: "Standard"; thumbnail: ""; templateFile: ""; variants: [
            ListElement { thumbnail: ""; templateFile: "Screen/.source/emptyLandscape.otp"; color: "white" }, // wide
            ListElement { thumbnail: ""; templateFile: "Screen/.source/emptyLandscapeWide.otp"; color: "white" }  // non-wide
            ] }
        ListElement { text: "White"; thumbnail: ""; templateFile: ""; variants: [ ] }
        ListElement { text: "Black"; thumbnail: ""; templateFile: ""; variants: [
            ListElement { thumbnail: ""; templateFile: ""; color: "black" },
            ListElement { thumbnail: ""; templateFile: ""; color: "lightblue" },
            ListElement { thumbnail: ""; templateFile: ""; color: "yellow" },
            ListElement { thumbnail: ""; templateFile: ""; color: "red" },
            ListElement { thumbnail: ""; templateFile: ""; color: "pink" },
            ListElement { thumbnail: ""; templateFile: ""; color: "purple" }
            ] }
        ListElement { text: "Metropolitan"; thumbnail: ""; templateFile: ""; variants: [ ] }
        ListElement { text: "Elegant"; thumbnail: ""; templateFile: ""; variants: [ ] }
        ListElement { text: "Vintage"; thumbnail: ""; templateFile: ""; variants: [ ] }
        ListElement { text: "Modernist"; thumbnail: ""; templateFile: ""; variants: [ ] }
        ListElement { text: "Distraught"; thumbnail: ""; templateFile: ""; variants: [ ] }
        ListElement { text: "Fabric"; thumbnail: ""; templateFile: ""; variants: [ ] }
        ListElement { text: "Metallic"; thumbnail: ""; templateFile: ""; variants: [ ] }
        ListElement { text: "Luna Rising"; thumbnail: ""; templateFile: ""; variants: [ ] }
        ListElement { text: "Galactic Voyage"; thumbnail: ""; templateFile: ""; variants: [ ] }
        ListElement { text: "Blue Orange Vector"; thumbnail: ""; templateFile: "odf/.source/blue_orange_vector.otp"; variants: [ ] }
        ListElement { text: "Burning Desire"; thumbnail: ""; templateFile: "odf/.source/burning_desire.otp"; variants: [ ] }
        ListElement { text: "Business"; thumbnail: ""; templateFile: "odf/.source/business.otp"; variants: [ ] }
        ListElement { text: "Flood Light"; thumbnail: ""; templateFile: "odf/.source/flood_light.otp"; variants: [ ] }
        ListElement { text: "Rounded Square"; thumbnail: ""; templateFile: "odf/.source/rounded_square.otp"; variants: [ ] }
        ListElement { text: "Simple Waves"; thumbnail: ""; templateFile: "odf/.source/simple_waves.otp"; variants: [ ] }
        ListElement { text: "Skyline Monotone"; thumbnail: ""; templateFile: "odf/.source/skyline_monotone.otp"; variants: [ ] }
        ListElement { text: "Strange Far Hills"; thumbnail: ""; templateFile: "odf/.source/strange_far_hills.otp"; variants: [ ] }
    }
    Flickable {
        id: stageFlickable;
        anchors {
            fill: parent;
            margins: Constants.DefaultMargin;
        }
        contentWidth: templatesFlow.width;
        contentHeight: templatesFlow.height;
        Flow {
            id: templatesFlow;
            width: base.width - Constants.DefaultMargin * 2;
            spacing: Constants.DefaultMargin * 3;
            Repeater {
                model: stageTemplates;
                Column {
                    width: (templatesFlow.width / 4) - Constants.DefaultMargin * 3;
                    height: width * 0.7;
                    spacing: Constants.DefaultMargin;
                    Image {
                        source: model.thumbnail;
                        width: parent.width;
                        height: parent.height - templateName.height - Constants.DefaultMargin;
                        Rectangle {
                            anchors.fill: parent;
                            color: "transparent";
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
                    }
                }
            }
        }
    }
    ScrollDecorator { flickableItem: stageFlickable; anchors.fill: stageFlickable; }
    Item {
        id: variantSelector;
        property alias model: variantView.model;
        anchors {
            fill: parent;
            margins: Constants.DefaultMargin * 3;
        }
        opacity: 0;
        Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
        MouseArea { anchors.fill: parent; onClicked: {} }
        SimpleTouchArea { anchors.fill: parent; onTouched: {} }
        Rectangle {
            anchors.fill: parent;
            color: "#22282f";
            opacity: 0.7;
        }
        Label {
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            height: Constants.GridHeight;
            horizontalAlignment: Text.AlignHCenter;
            verticalAlignment: Text.AlignVCenter;
            color: "white";
            font: Settings.theme.font("title");
            text: "Select a Style Option";
        }
        Button {
            anchors {
                top: parent.top;
                right: parent.right;
                margins: Constants.DefaultMargin;
            }
            height: Constants.GridHeight - Constants.DefaultMargin * 2;
            width: height;
            textColor: "white";
            text: "X";
            onClicked: variantSelector.opacity = 0;
        }
        Flickable {
            id: variantFlickable;
            anchors {
                fill: parent;
                topMargin: Constants.GridHeight;
            }
            clip: true;
            contentHeight: variantFlow.height;
            contentWidth: variantFlow.width;
            Flow {
                id: variantFlow;
                width: variantFlickable.width;
                Repeater {
                    id: variantView;
                    delegate: Item {
                        height: variantFlickable.height;
                        width: variantFlickable.width / 4;
                        Rectangle {
                            anchors.centerIn: parent;
                            height: Constants.GridHeight;
                            width: Constants.GridWidth;
                            color: model.color;
                        }
                        MouseArea {
                            anchors.fill: parent;
                            onClicked: {
                                variantSelector.opacity = 0;
                                activateTemplate(model.templateFile);
                            }
                        }
                    }
                }
            }
        }
        ScrollDecorator { flickableItem: variantFlickable; anchors.fill: variantFlickable; }
    }
}
