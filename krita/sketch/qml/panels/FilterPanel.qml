/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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
import org.krita.sketch 1.0
import "../components"

Panel {
    name: "Filter";
    panelColor: Constants.Theme.TertiaryColor;

    actions: [
        Button {
            id: undoButton;
            width: height;
            height: Constants.GridHeight
            color: "transparent";
            image: "../images/svg/icon-undo.svg"
            textColor: "white";
            shadow: false;
            highlight: false;
        },
        Button {
            id: applyButton;
            width: height;
            height: Constants.GridHeight
            color: "transparent";
            image: "../images/svg/icon-apply.svg"
            textColor: "white";
            shadow: false;
            highlight: false;
        },
        Item {
            width: (Constants.GridWidth * 2) - Constants.DefaultMargin - (Constants.GridHeight * 3)
            height: Constants.GridHeight;
        },
        Button {
            id: toggleShowPreviewButton;
            property bool showPreview: true;
            width: height;
            height: Constants.GridHeight
            color: "transparent";
            image: showPreview ? "../images/svg/icon-visible_on.svg" : "../images/svg/icon-visible_off.svg";
            textColor: "white";
            shadow: false;
            highlight: false;
            onClicked: showPreview = !showPreview;
        }
    ]

    FiltersCategoryModel {
        id: filtersCategoryModel;
    }

    peekContents: Column {
        anchors {
            fill: parent;
            margins: Constants.DefaultMargin;
        }
        spacing: Constants.DefaultMargin;
        ExpandingListView {
            id: categoryList;
            width: parent.width;
            model: filtersCategoryModel;
            onCurrentIndexChanged: {
                fullCategoryList.currentIndex = currentIndex;
                model.activateItem(currentIndex)
            }
        }
        ExpandingListView {
            width: parent.width;
            model: filtersCategoryModel.filterModel;
        }
    }

    fullContents: Column {
        anchors {
            fill: parent;
            margins: Constants.DefaultMargin;
        }
        spacing: Constants.DefaultMargin;
        ExpandingListView {
            id: fullCategoryList;
            width: parent.width;
            model: filtersCategoryModel;
            onCurrentIndexChanged: {
                if(categoryList.currentIndex !== currentIndex) {
                    categoryList.currentIndex = currentIndex;
                }
            }
        }
        ExpandingListView {
            width: parent.width;
            model: filtersCategoryModel.filterModel;
            onCurrentIndexChanged: {
                if(model.filterRequiresConfiguration(currentIndex)) {
                    noConfigNeeded.visible = false;
                    configNeeded.visible = true;
                    configLoader.source = "filterconfigpages/" + model.filterID(currentIndex) + ".qml";
                }
                else {
                    noConfigNeeded.visible = true;
                    configNeeded.visible = false;
                }
            }
        }
        Label {
            id: noConfigNeeded;
            width: parent.width;
            text: "This filter requires no configuration";
        }
        Item {
            id: configNeeded;
            width: parent.width;
            height: childrenRect.height > 0 ? childrenRect.height : 1;
            MouseArea {
                anchors.fill: parent;
                hoverEnabled: true;
                onContainsMouseChanged: configLoader.focus = containsMouse;
            }
            Loader {
                id: configLoader;
                width: parent.width;
                height: item.height;
            }
        }
    }
}