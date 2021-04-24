/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0
import org.kde.calligra 1.0 as Calligra

ApplicationWindow {
    id: window;
    width: 1024;
    height: 600;

    title: (doc.source != "" ? doc.source + " - " : "") + "Calligra Viewer";

    menuBar: MenuBar {
        Menu {
            title: "File";

            MenuItem { action: openAction; }
            MenuSeparator { }
            MenuItem { action: quitAction; }
        }

        Menu {
            title: "View";
            MenuItem { action: zoomInAction; }
            MenuItem { action: zoomOutAction; }
        }
    }

    toolBar: ToolBar {
        id: toolbar;

        RowLayout {
            ToolButton { action: openAction; }
            ToolButton { action: zoomInAction; }
            ToolButton { action: zoomOutAction; }
        }
    }

    SplitView {
        id: centralWidget;
        anchors.fill: parent;

        ScrollView {
            ListView {
                id: contents;

                property int thumbnailWidth: 120;
                property int thumbnailHeight: doc.documentType == Calligra.DocumentType.TextDocument ? 180 : 90;

                model: Calligra.ContentsModel {
                    document: doc;
                    thumbnailSize.width: contents.thumbnailWidth;
                    thumbnailSize.height: contents.thumbnailHeight;
                }

                delegate: Rectangle {
                    width: ListView.view.width;
                    height: ListView.view.thumbnailHeight + 30;
                    color: doc.currentIndex == model.contentIndex ? "lightBlue" : index % 2 == 0 ? "white" : "lightGrey";

                    Calligra.ImageDataItem {
                        id: thumbnail;
                        anchors.top: parent.top;
                        anchors.horizontalCenter: parent.horizontalCenter;

                        width: parent.ListView.view.thumbnailWidth;
                        height: parent.ListView.view.thumbnailHeight;

                        data: model.thumbnail;
                    }

                    Label {
                        anchors {
                            bottom: parent.bottom;
                            left: parent.left;
                            right: parent.right;
                        }

                        text: model.title;
                        elide: Text.ElideRight;
                        horizontalAlignment: Text.AlignHCenter;
                    }

                    MouseArea {
                        anchors.fill: parent;
                        onClicked: doc.currentIndex = model.contentIndex;
                    }
                }
            }
        }

        Item {
            Calligra.View {
                id: v;
                anchors.fill: parent;
                document: doc;
            }

            ScrollView {
                id: f;
                anchors.fill: parent;

                Calligra.ViewController {
                    id: controller;
                    view: v;
                    flickable: f.flickableItem;
                    minimumZoomFitsWidth: true;
                    Calligra.LinkArea {
                        anchors.fill: parent;
                        document: doc;
                        onClicked: console.debug("clicked somewhere without a link");
                        onLinkClicked: console.debug("clicked on the link: " + linkTarget);
                        controllerZoom: controller.zoom;
                    }
                }
            }
        }
    }

    Calligra.Document {
        id: doc;
    }

    FileDialog {
        id: fileDialog;
        title: "Open a Document";
        nameFilters: [
            "OpenDocument Document (*.odt *.ods *.odp)",
            "Microsoft Office Document (*.doc *.docx *.xls *.xlsx *.ppt *.pptx)",
            "All Files (*)"
        ];

        onAccepted: doc.source = fileDialog.fileUrl;
    }

    Action {
        id: openAction;

        iconName: "document-open";
        text: "Open";
        tooltip: "Open a document.";
        shortcut: "Ctrl+o";

        onTriggered: fileDialog.open();
    }

    Action {
        id: quitAction;

        iconName: "application-exit"
        text: "Quit";
        tooltip: "Quit the application.";
        shortcut: "Ctrl+q";

        onTriggered: Qt.quit();
    }

    Action {
        id: zoomInAction;

        iconName: "zoom-in";
        text: "Zoom In";
        tooltip: "Zoom In";
        shortcut: "Ctrl+=";

        onTriggered: controller.zoomAroundPoint(0.1, v.width / 2, v.height / 2);
    }

    Action {
        id: zoomOutAction;

        iconName: "zoom-out";
        text: "Zoom Out";
        tooltip: "Zoom Out";
        shortcut: "Ctrl+-";

        onTriggered: controller.zoomAroundPoint(-0.1, v.width / 2, v.height / 2);
    }
}
