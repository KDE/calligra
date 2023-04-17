/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 2.2 as QtControls
import org.kde.kirigami 2.1 as Kirigami
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
        property string progressMessage;
        onPullCompleted: {
            logModel.refreshLog();
            updatedLabel.opacity = 1;
            if(DocumentManager.doc()) {
                DocumentManager.doc().clearStatusBarMessage();
            }
            progressMessage = "";
        }
        onTransferProgress: {
            if(DocumentManager.doc()) {
                DocumentManager.doc().sigProgress(progress);
            }
            progressMessage = "Transfer progress: %1%".arg(progress);
            console.log("Transfer progress: " + progress);
        }
        onOperationBegun: {
            console.log(message);
            if(DocumentManager.doc()) {
                DocumentManager.doc().statusBarMessage(message);
            }
            progressMessage = message;
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
        QtControls.Button {
            id: pullButton;
            anchors {
                top: parent.top;
                horizontalCenter: parent.horizontalCenter;
                margins: Settings.theme.adjustedPixel(8);
            }
            text: "Pull from upstream";
            onClicked: {
                enabled = false;
                pullInProgress.running = true;
                gitController.pull();
            }
        }
        QtControls.Label {
            id: updatedLabel;
            opacity: 0;
            Behavior on opacity {
                SequentialAnimation {
                    ScriptAction { script: pullInProgress.running = false; }
                    NumberAnimation { duration: Kirigami.Units.shortDuration; easing.type: Easing.InOutQuad; }
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
            header: QtControls.Label {
                width: logListView.width;
                height: Constants.GridHeight / 2;
                text: "Recent Changes";
                verticalAlignment: Text.AlignVCenter;
                horizontalAlignment: Text.AlignHCenter;
            }
            delegate: Column {
                width: logListView.width;
                height: childrenRect.height;
                QtControls.Label {
                    id: messageText;
                    width: parent.width;
                    height: paintedHeight;
                    text: model.shortMessage;
                    wrapMode: Text.Wrap;
                }
                QtControls.Label {
                    id: timeText;
                    width: parent.width;
                    height: paintedHeight;
                    opacity: 0.7;
                    text: "on " + model.time;
                }
                QtControls.Label {
                    id: nameText;
                    width: parent.width;
                    height: paintedHeight;
                    opacity: 0.7
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
        ScrollDecorator { flickableItem: logListView; anchors.fill: logListView; }
    }
    GridView {
        id: docList;
        clip: true;
        anchors {
            margins: Constants.DefaultMargin;
            top: parent.top;
            topMargin: Constants.DefaultMargin * 2;
            left: parent.left;
            right: logSidebar.left;
            bottom: parent.bottom;
            bottomMargin: 0;
        }
        cellWidth: width / 3;
        cellHeight: cellWidth + Settings.theme.font("templateLabel").pixelSize;
        model: gitController.documents;
        delegate: documentTile;
        ScrollDecorator { flickableItem: docList; }
    }
    QtControls.Label {
        anchors.fill: parent;
        text: "No Documents\n\nPlease add some documents to your repository.\n(%1)".arg(docList.model.documentsFolder);
        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;
        visible: docList.count === 0;
    }
    Component {
        id: documentTile;
        DocumentTile {
            filePath: model.filePath;
            title: model.fileName != "" ? model.fileName : "";
            onClicked: openFile(model.filePath, gitController.commitAndPushCurrentFileAction());
        }
    }
    Item {
        anchors.fill: parent;
        opacity: pullInProgress.running ? 1 : 0;
        Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
        Rectangle {
            anchors.fill: parent;
            opacity: 0.7;
            color: Kirigami.Theme.backgroundColor;
        }
        MouseArea {
            enabled: pullInProgress.running;
            anchors.fill: parent;
            onClicked: {}
        }
        QtControls.BusyIndicator {
            id: pullInProgress;
            running: false;
            height: Kirigami.Units.iconSizes.huge;
            width: height;
            anchors.centerIn: parent;
            QtControls.Label {
                anchors {
                    top: parent.bottom;
                    left: parent.left;
                    right: parent.right;
                    margins: Kirigami.Units.largeSpacing;
                }
                horizontalAlignment: Text.AlignHCenter;
                text: gitController.progressMessage;
            }
        }
    }
}
