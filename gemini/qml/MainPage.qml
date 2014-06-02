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
import "components"
import org.calligra 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

Page {
    id: base;
    property string pageName: "MainPage";
    Connections {
        target: Settings;
        onCurrentFileChanged: {
            if(Settings.currentFileClass === WORDS_MIME_TYPE) {
                viewLoader.sourceComponent = wordsView;
            } else if(Settings.currentFileClass === STAGE_MIME_TYPE) {
                viewLoader.sourceComponent = stageView;
            } else {
                console.debug("BANG!");
            }
            viewLoader.item.source = Settings.currentFile;
        }
    }
    ToolManager {
        id: toolManager;
        onCurrentToolChanged: console.debug("Current tool is now " + currentTool.toolId());
    }
    Loader {
        id: viewLoader;
        anchors {
            top: toolbar.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
    }
    Component { id: stageView; StageDocumentPage {} }
    Component { id: wordsView; WordsDocumentPage {} }
    Rectangle {
        id: toolbar;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: Constants.GridHeight * 2 / 3;
        color: "whitesmoke";
        Row {
            anchors.left: parent.left;
            height: parent.height;
            spacing: Constants.DefaultMargin;
            Button {
                height: parent.height;
                width: height * 1.5;
                color: "dodgerblue";
                image: Settings.theme.icon("krita_sketch");
            }
            Button {
                height: parent.height;
                width: height * 2;
                text: "Undo";
                textColor: "black";
            }
            Button {
                height: parent.height;
                width: height * 2;
                text: "Redo";
                textColor: "black";
            }
        }
        Row {
            id: toolbarTextTool
            anchors.centerIn: parent;
            height: parent.height;
            opacity: (toolManager.currentTool !== null && toolManager.currentTool.toolId() === "TextToolFactory_ID") ? 1 : 0;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
            Rectangle { height: parent.height; width: 1; }
            Button {
                text: "Text Style"
                height: parent.height; width: Constants.GridWidth;
                textColor: "black";
            }
            Rectangle { height: parent.height; width: 1; }
            Button {
                text: "(font name)"
                height: parent.height; width: Constants.GridWidth;
                textColor: "black";
            }
            Rectangle { height: parent.height; width: 1; }
            Button {
                text: "(size)"
                height: parent.height; width: Constants.GridWidth / 2;
                textColor: "black";
            }
            Rectangle { height: parent.height; width: 1; }
            Button {
                text: "(colour)"
                height: parent.height; width: Constants.GridWidth / 2;
                textColor: "black";
            }
            Rectangle { height: parent.height; width: 1; }
            Button {
                text: "(B)"
                height: parent.height; width: Constants.GridWidth / 2;
                textColor: "black";
            }
            Button {
                text: "(I)"
                height: parent.height; width: Constants.GridWidth / 2;
                textColor: "black";
            }
            Button {
                text: "(U)"
                height: parent.height; width: Constants.GridWidth / 2;
                textColor: "black";
            }
            Rectangle { height: parent.height; width: 1; }
            Button {
                text: "(list)"
                height: parent.height; width: Constants.GridWidth / 2;
                textColor: "black";
            }
            Rectangle { height: parent.height; width: 1; }
            Button {
                text: "(align)"
                height: parent.height; width: Constants.GridWidth / 2;
                textColor: "black";
            }
            Rectangle { height: parent.height; width: 1; }
        }
        Row {
            id: toolbarImageTool
            anchors.centerIn: parent;
            height: parent.height;
            opacity: (toolManager.currentTool !== null && toolManager.currentTool.toolId() === "InteractionTool") ? 1 : 0;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
            Rectangle { height: parent.height; width: 1; }
            Button {
                text: "(img)"
                height: parent.height; width: Constants.GridWidth;
                textColor: "black";
            }
            Rectangle { height: parent.height; width: 1; }
            Button {
                text: "(pencol)"
                height: parent.height; width: Constants.GridWidth;
                textColor: "black";
            }
            Rectangle { height: parent.height; width: 1; }
            Button {
                text: "(bgcol)"
                height: parent.height; width: Constants.GridWidth;
                textColor: "black";
            }
            Rectangle { height: parent.height; width: 1; }
            Button {
                text: "(line)"
                height: parent.height; width: Constants.GridWidth;
                textColor: "black";
            }
            Rectangle { height: parent.height; width: 1; }
            Button {
                text: "(shadow)"
                height: parent.height; width: Constants.GridWidth;
                textColor: "black";
            }
            Rectangle { height: parent.height; width: 1; }
            Button {
                text: "(?)"
                height: parent.height; width: Constants.GridWidth;
                textColor: "black";
            }
            Rectangle { height: parent.height; width: 1; }
            Button {
                text: "(cfg)"
                height: parent.height; width: Constants.GridWidth;
                textColor: "black";
            }
        }
        Row {
            anchors.right: parent.right;
            height: parent.height;
            spacing: Constants.DefaultMargin;
            Button {
                height: parent.height; width: height;
                image: Settings.theme.icon("add");
                color: "gray";
            }
            Button {
                height: parent.height; width: height;
                image: Settings.theme.icon("paint");
                color: "gray";
            }
            Button {
                opacity: switchToDesktopAction.enabled ? 1 : 0.2;
                Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
                height: parent.height;
                width: height;
                image: Settings.theme.icon("switch");
                color: "gray";
                onClicked: switchToDesktopAction.trigger();
            }
        }
    }
}
