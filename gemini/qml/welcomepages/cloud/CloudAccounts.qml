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
import Calligra.Gemini.Dropbox 1.0
import "../../components"
import "dropbox" as Dropbox
import "git" as Git

Page {
    id: base;
    property string pageName: "accountsPage";
    property QtObject accountsList;

    Connections {
        target: controllerMIT;
        onNeedAuthenticateChanged: {
            if(controllerMIT.needAuthenticate) {
                cloudAccounts.removeAccountByName("Dropbox");
            }
            else {
                cloudAccounts.addAccount("Dropbox", "DropBox", "accountsPageDropbox", 0, true);
            }
        }
    }
    Component.onCompleted: {
        if(!controllerMIT.needAuthenticate) {
            cloudAccounts.addAccount("Dropbox", "DropBox", "accountsPageDropbox", 0, true);
        }
    }
    ListView {
        id: accountsView;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
            bottom: bottomButtons.top;
        }
        clip: true;
        model: accountsList;
        delegate: Item {
            height: Settings.theme.adjustedPixel(64);
            width: ListView.view.width;
            Label {
                anchors.fill: parent;
                text: model.text;
                horizontalAlignment: Text.AlignLeft;
                verticalAlignment: Text.AlignVCenter;
                font: Settings.theme.font("templateLabel");
                color: "#5b6573";
            }
            Row {
                anchors {
                    verticalCenter: parent.verticalCenter;
                    right: parent.right;
                }
                width: childrenRect.width;
                spacing: Settings.theme.adjustedPixel(8);
                Text {
                    text: model.accountType;
                    width: Settings.theme.adjustedPixel(100);
                    height: parent.height;
                    horizontalAlignment: Text.AlignRight;
                    verticalAlignment: Text.AlignVCenter;
                    font: Settings.theme.font("templateLabel");
                    color: "#5b6573";
                    visible: model.accountType !== "DropBox";
                }
                CohereButton {
                    anchors.verticalCenter: parent.verticalCenter;
                    text: (model.accountType === "DropBox") ? "Sign Out" : "Edit Account";
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    color: "#D2D4D5";
                    onClicked: {
                        dlgStack.replace(base.editComponentFromName(model.accountType));
                        if(dlgStack.currentPage.accountIndex !== undefined) {
                            dlgStack.currentPage.accountIndex = index;
                            dlgStack.currentPage.text = model.text;
                        }
                    }
                }
                CohereButton {
                    anchors.verticalCenter: parent.verticalCenter;
                    text: "Remove";
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    color: "#D2D4D5";
                    visible: model.accountType !== "DropBox";
                    onClicked: {
                        dlgStack.replace(removeAccountDlg);
                        if(dlgStack.currentPage.accountIndex !== undefined) {
                            dlgStack.currentPage.accountIndex = index;
                            dlgStack.currentPage.text = model.text;
                        }
                    }
                }
            }
        }
    }
    Label {
        anchors.fill: accountsView;
        text: "You have no cloud accounts defined. Please add one by clicking on your service of choice below.";
        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;
        font: Settings.theme.font("templateLabel");
        color: "#5b6573";
        opacity: accountsList.count === 0 ? 1 : 0;
        Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
    }
    Item {
        id: bottomButtons;
        anchors {
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
        height: bottomButtonRow.height + Settings.theme.adjustedPixel(8);
        Label {
            anchors.fill: parent;
            text: "Add new account:";
            horizontalAlignment: Text.AlignLeft;
            verticalAlignment: Text.AlignVCenter;
            font: Settings.theme.font("templateLabel");
            color: "#5b6573";
        }
        Row {
            id: bottomButtonRow;
            anchors {
                verticalCenter: parent.verticalCenter;
                right: parent.right;
            }
            height: childrenRect.height;
            spacing: Settings.theme.adjustedPixel(8);
            Repeater {
                model: ListModel {
                    ListElement { text: "Dropbox"; accountType: "DropBox"; serviceName: "dropbox"; }
                    ListElement { text: "WebDav"; accountType: "WebDav"; serviceName: ""; }
                    //ListElement { text: "ownCloud"; accountType: "WebDav"; serviceName: "ownCloud"; }
                    ListElement { text: "Git"; accountType: "Git"; serviceName: ""; }
                    //ListElement { text: "GitHub"; accountType: "Git"; serviceName: "github"; }
                    //ListElement { text: "bitbucket (git)"; accountType: "Git"; serviceName: "bitbucket"; }
                }
                CohereButton {
                    text: model.text
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    color: "#D2D4D5";
                    onClicked: {
                        dlgStack.replace(base.addComponentFromName(model.accountType));
                        dlgStack.currentPage.serviceName = model.serviceName;
                    }
                }
            }
        }
    }
    PageStack {
        id: dlgStack;
        anchors.fill: base;
        initialPage: addEmptyComp;
    }
    function addComponentFromName(name) {
        var elements = {
            "DropBox": addDropBox,
            "WebDav": addWebDav,
            "Git": addGit
        };
        return elements[name];
    }
    Component {
        id: addEmptyComp;
        Item {}
    }
    Component {
        id: addDropBox;
        AddDropbox { addEmpty: addEmptyComp; }
    }
    Component {
        id: addWebDav;
        AddWebdav { addEmpty: addEmptyComp; }
    }
    Component {
        id: addGit;
        AddGit { addEmpty: addEmptyComp; }
    }
    function editComponentFromName(name) {
        var elements = {
            "DropBox": editDropBox,
            "WebDav": editWebDav,
            "Git": editGit
        };
        return elements[name];
    }
    Component {
        id: editDropBox;
        AddDropbox { addEmpty: addEmptyComp; }
    }
    Component {
        id: editWebDav;
        EditDetailsBase { addEmpty: addEmptyComp; }
    }
    Component {
        id: editGit;
        EditGit { addEmpty: addEmptyComp; }
    }

    Component {
        id: removeAccountDlg;
        RemoveAccountDlg { addEmpty: addEmptyComp; }
    }
}
