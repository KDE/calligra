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
    ListModel {
        id: stageTemplates;
        ListElement { text: "Standard"; thumbnail: ""; colors: [ ] }
        ListElement { text: "White"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Black"; thumbnail: ""; colors: [ ListElement { color: "black" }, ListElement { color: "lightblue" }, ListElement { color: "yellow" }, ListElement { color: "red" } ] }
        ListElement { text: "Metropolitan"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Blue Orange Vector"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Burning Desire"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Business"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Flood Light"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Rounded Square"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Simple Waves"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Skyline Monotone"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Strange Far Hills"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Blue Orange Vector"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Burning Desire"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Business"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Flood Light"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Rounded Square"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Simple Waves"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Skyline Monotone"; thumbnail: ""; colors: [ ] }
        ListElement { text: "Strange Far Hills"; thumbnail: ""; colors: [ ] }
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
                            property ListModel colorModel: model.colors;
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
}
