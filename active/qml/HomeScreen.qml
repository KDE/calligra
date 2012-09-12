/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
 * Copyright (C) 2012 Sujith H <sujith.h@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

import QtQuick 1.0
import CalligraActive 1.0
import org.kde.plasma.components 0.1 as PlasmaComponents

Image {
    id: homeScreen

    function openDocument(path) {
        doc.openDocument(path);
    }

    source: "qrc:///images/fabrictexture.png"
    fillMode: Image.Tile

    DocumentTypeSelector {
        id: docTypeSelector

        visible: metadataInternalModel ? true : false
        buttonWidth: homeScreen.width/2.1; buttonHeight: 100;
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.horizontalCenter
        anchors.bottom: progressBar.top
        anchors.margins: 10
    }

    RecentFiles {
        id: recentFiles

        visible: metadataInternalModel ? true : false
        buttonWidth: homeScreen.width/2.1; buttonHeight: 100;
        anchors.left: parent.horizontalCenter
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: openFileDialogButton.top
        anchors.margins: 10
    }

    PlasmaComponents.Button {
        id: openFileDialogButton

        iconSource: "document-open"
	text: "Open File"
        width: homeScreen.width/2.1;
	height: 100;

        anchors.left: parent.left
        anchors.right: parent.horizontalCenter
        anchors.bottom: aboutCalligraButton.top
        anchors.margins: 10

        onClicked: mainwindow.openFileDialog()
    }

    PlasmaComponents.Button {
        id: aboutCalligraButton

        iconSource: "active-about"
        text: "About Calligra Active"
        width: homeScreen.width/2.1;
        height: 100;

        anchors.left: parent.left
        anchors.right: parent.horizontalCenter
        anchors.bottom: progressBar.top
        anchors.margins: 10

        onClicked: homeScreen.state = "showing-about-screen"
    }

    Rectangle {
        id: progressBar

        color: "blue"
        //DISABLED
        //width: parent.width/100*doc.loadProgress; height: 32;
        anchors.bottom: parent.bottom
    }

    Doc {
        id: doc

        width: parent.width; height: parent.height;
        anchors.left: parent.right
        anchors.verticalCenter: parent.verticalCenter

        onDocumentLoaded: {
            homeScreen.state = "doc"
        }
    }

    AboutCalligraActive {
        id: aboutScreen

        width: parent.width; height: parent.height;
        anchors.left: parent.right
        anchors.verticalCenter: parent.verticalCenter
    }

    transitions: Transition {
         // smoothly reanchor myRect and move into new position
         AnchorAnimation { duration: 500 }
     }

    states : [
        State {
            name: "doc"
            AnchorChanges {
                target: docTypeSelector
                anchors.left: undefined
                anchors.right: parent.left
            }
            AnchorChanges {
                target: recentFiles
                anchors.left: undefined
                anchors.right: parent.left
            }
            AnchorChanges {
                target: doc
                anchors.left: parent.left
            }
            AnchorChanges {
                target: openFileDialogButton
                anchors.left: undefined
                anchors.right: parent.left
            }
            AnchorChanges {
                target: aboutCalligraButton
                anchors.left: undefined
                anchors.right: parent.left
            }
        },
        State {
            name: "showTextDocs"
            PropertyChanges { target: recentFiles; typeFilter: "PaginatedTextDocument"; }
        },
        State {
            name: "showSpreadsheets"
            PropertyChanges { target: recentFiles; typeFilter: "Spreadsheet"; }
        },
        State {
            name: "showPresentations"
            PropertyChanges { target: recentFiles; typeFilter: "Presentation"; }
        },
        State {
            name: "showing-about-screen"
            AnchorChanges {
                target: docTypeSelector
                anchors.left: undefined
                anchors.right: parent.left
            }
            AnchorChanges {
                target: recentFiles
                anchors.left: undefined
                anchors.right: parent.left
            }
            AnchorChanges {
                target: aboutScreen
                anchors.left: parent.left
            }
            AnchorChanges {
                target: openFileDialogButton
                anchors.left: undefined
                anchors.right: parent.left
            }
            AnchorChanges {
                target: aboutCalligraButton
                anchors.left: undefined
                anchors.right: parent.left
            }
        }
    ]
}
