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
import org.calligra 1.0
import Calligra.Gemini.Git 1.0
import "../../../components"

Item {
    property alias localrepo: gitController.cloneDir;
    property alias privateKeyFile: gitController.privateKeyFile;
    property alias needsPrivateKeyPassphrase: gitController.needsPrivateKeyPassphrase;
    property alias publicKeyFile: gitController.publicKeyFile;
    property alias userForRemote: gitController.userForRemote;
    GitController {
        id: gitController;
        currentFile: Settings.currentFile;
        onPullCompleted: {
            logModel.refreshLog();
            updatedLabel.opacity = 1;
            if(DocumentManager.doc()) {
                DocumentManager.doc().clearStatusBarMessage();
            }
        }
        onTransferProgress: {
            if(DocumentManager.doc()) {
                DocumentManager.doc().sigProgress(progress);
            }
        }
        onOperationBegun: {
            console.log(message);
            if(DocumentManager.doc()) {
                DocumentManager.doc().statusBarMessage(message);
            }
        }
        onPushCompleted: {
            if(DocumentManager.doc()) {
                DocumentManager.doc().clearStatusBarMessage();
            }
        }
    }
    GitLogModel {
        id: logModel;
        repoDir: gitController.cloneDir;
    }
    Item {
        id: logSidebar;
        anchors {
            margins: Constants.DefaultMargin;
            top: parent.top;
            right: parent.right;
            bottom: parent.bottom;
            bottomMargin: 0;
        }
        width: (parent.width / 4) - Constants.DefaultMargin;
        CohereButton {
            id: pullButton;
            anchors {
                top: parent.top;
                horizontalCenter: parent.horizontalCenter;
                margins: Settings.theme.adjustedPixel(8);
            }
            textColor: "#5b6573";
            textSize: Settings.theme.adjustedPixel(18);
            color: "#D2D4D5";
            text: "Pull from upstream";
            onClicked: {
                enabled = false;
                pullInProgress.opacity = 1;
                gitController.pull();
            }
            BusyIndicator {
                id: pullInProgress;
                opacity: 0;
                running: true;
                height: parent.height;
                width: height;
                anchors.centerIn: parent;
                Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
                anchors {
                    top: pullButton.bottom;
                    left: parent.left;
                    right: parent.right;
                }
            }
        }
        Label {
            id: updatedLabel;
            opacity: 0;
            Behavior on opacity {
                SequentialAnimation {
                    ScriptAction { script: pullInProgress.opacity = 0; }
                    PropertyAnimation { duration: Constants.AnimationDuration; }
                    ScriptAction { script: hideUpdate.start(); }
                }
            }
            height: opacity * Constants.GridHeight / 2;
            anchors {
                top: pullButton.bottom;
                left: parent.left;
                right: parent.right;
            }
            text: "Update Completed!";
            font: Settings.theme.font("templateLabel");
            color: "#5b6573";
            verticalAlignment: Text.AlignVCenter;
            horizontalAlignment: Text.AlignHCenter;
            Timer { id: hideUpdate; running: false; repeat: false; interval: 1000; onTriggered: { updatedLabel.opacity = 0; pullButton.enabled = true; } }
        }
        ListView {
            id: logListView;
            model: logModel;
            clip: true;
            anchors {
                margins: Constants.DefaultMargin;
                top: updatedLabel.bottom;
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            header: Label {
                width: logListView.width;
                height: Constants.GridHeight / 2;
                text: "Recent Changes";
                font: Settings.theme.font("templateLabel");
                color: "#5b6573";
                verticalAlignment: Text.AlignVCenter;
                horizontalAlignment: Text.AlignHCenter;
            }
            delegate: Column {
                width: logListView.width;
                height: childrenRect.height;
                Text {
                    id: messageText;
                    width: parent.width;
                    height: paintedHeight;
                    font: Settings.theme.font("templateLabel");
                    color: "#5b6573";
                    text: model.shortMessage;
                    wrapMode: Text.Wrap;
                }
                Text {
                    id: timeText;
                    width: parent.width;
                    height: paintedHeight;
                    font: Settings.theme.font("applicationLight");
                    color: "#5b6573";
                    text: "on " + model.time;
                }
                Text {
                    id: nameText;
                    width: parent.width;
                    height: paintedHeight;
                    font: Settings.theme.font("applicationLight");
                    color: "#5b6573";
                    text: "by " + model.authorName;
                }
                Item {
                    width: parent.width;
                    height: nameText.height;
                    Rectangle {
                        width: parent.width - (Constants.DefaultMargin * 2);
                        height: 1;
                        anchors.centerIn: parent;
                        opacity: 0.3;
                        color: "black";
                    }
                }
            }
        }
    }
    GridView {
        id: docList;
        clip: true;
        contentWidth: width;
        anchors {
            margins: Constants.DefaultMargin;
            top: parent.top;
            left: parent.left;
            right: logSidebar.left;
            bottom: parent.bottom;
            bottomMargin: 0;
        }
        cellWidth: width / 3 - Constants.DefaultMargin;
        cellHeight: cellWidth + Settings.theme.font("templateLabel").pixelSize + Constants.DefaultMargin * 4;
        model: gitController.documents;
        delegate: documentTile;
        ScrollDecorator { flickableItem: docList; }
    }
    Label {
        anchors.fill: parent;
        text: "No Documents\n\nPlease add some documents to your reporitory.\n(%1)".arg(docList.model.documentsFolder);
        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;
        font: Settings.theme.font("templateLabel");
        color: "#5b6573";
        visible: docList.count === 0;
    }
    Component {
        id: documentTile;
        Item {
            width: docList.cellWidth;
            height: docList.cellHeight
            Image {
                source: "image://recentimage/" + model.filePath;
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    margins: Constants.DefaultMargin / 2;
                }
                height: parent.width;
                fillMode: Image.PreserveAspectFit;
                smooth: true;
                asynchronous: true;
            }
            Label {
                id: lblName;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.bottom;
                    margins: Constants.DefaultMargin;
                    bottomMargin: Constants.DefaultMargin * 2;
                }
                height: font.pixelSize + Constants.DefaultMargin * 2;
                horizontalAlignment: Text.AlignHCenter;
                verticalAlignment: Text.AlignVCenter;
                text: model.fileName ? model.fileName : "";
                font: Settings.theme.font("templateLabel");
                color: "#5b6573";
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    baseLoadingDialog.visible = true;
                    openFile(model.filePath, gitController.commitAndPushCurrentFileAction());
                }
            }
        }
    }
}

