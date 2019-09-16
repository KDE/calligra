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
import QtQuick.Layouts 1.11 as QtLayouts
import QtQuick.Controls 2.2 as QtControls
import org.kde.kirigami 2.7 as Kirigami
import Calligra.Gemini.Dropbox 1.0
import org.calligra 1.0
import "../components"
import "cloud"

Kirigami.ScrollablePage {
    id: base;
    objectName: "WelcomePageCloud";
    title: "Open from the Cloud";
/*    ListModel {
        id: cloudAccounts;
        ListElement { text: "Dropbox"; selected: false; accountType: "DropBox"; stackComponent: "accountsPageDropbox"; accountDetails: [ ListElement { userkey: "asfdoijfdshaloiuhs" } ] }
        ListElement { text: "ownCloud"; selected: false; accountType: "WebDav"; stackComponent: "accountsPageWebdav"; accountDetails: [ ListElement { username: "lala" } ] }
        ListElement { text: "bitbucket"; selected: false; accountType: "Git"; stackComponent: "accountsPageGit"; accountDetails: [ ListElement { localrepo: "C:\\Users\\danjensen\\Documents\\nohnas" } ] }
        ListElement { text: "github"; selected: false; accountType: "Git"; stackComponent: "accountsPageGit"; accountDetails: [ ListElement { localrepo: "C:\\dev\\documentation" } ] }
    }*/
    function showAccount(index, stackComponent, accountDetails) {
        cloudAccounts.selectIndex(index);
        pageStack.push(elementFromName(stackComponent));
        pageStack.currentItem.accountDetails = accountDetails;
    }

    CloudAccountsModel {
        id: cloudAccounts;
    }
    actions {
        main: Kirigami.Action {
        }
    }
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
        model: cloudAccounts;
        footer: Item {
            anchors {
                left: parent.left;
                right: parent.right;
                margins: Kirigami.Units.largeSpacing;
            }
            height: bottomButtonRow.height + Kirigami.Units.largeSpacing * 2;
            QtControls.Label {
                anchors.fill: parent;
                text: "Add new account:";
                horizontalAlignment: Text.AlignLeft;
                verticalAlignment: Text.AlignVCenter;
            }
            Row {
                id: bottomButtonRow;
                anchors {
                    verticalCenter: parent.verticalCenter;
                    right: parent.right;
                }
                height: childrenRect.height;
                spacing: Kirigami.Units.largeSpacing;
                Repeater {
                    model: ListModel {
                        ListElement { text: "Dropbox"; accountType: "DropBox"; serviceName: "dropbox"; }
                        ListElement { text: "WebDav"; accountType: "WebDav"; serviceName: ""; }
                        //ListElement { text: "ownCloud"; accountType: "WebDav"; serviceName: "ownCloud"; }
                        ListElement { text: "Git"; accountType: "Git"; serviceName: ""; }
                        //ListElement { text: "GitHub"; accountType: "Git"; serviceName: "github"; }
                        //ListElement { text: "bitbucket (git)"; accountType: "Git"; serviceName: "bitbucket"; }
                    }
                    QtControls.Button {
                        text: model.text
                        onClicked: {
                            dlgStack.replace(base.addComponentFromName(model.accountType));
                            dlgStack.currentPage.serviceName = model.serviceName;
                        }
                    }
                }
            }
        }
        delegate: Kirigami.SwipeListItem {
            QtLayouts.RowLayout {
                Item {
                    QtLayouts.Layout.preferredHeight: Kirigami.Units.iconSizes.huge - Kirigami.Units.smallSpacing * 2;
                    QtLayouts.Layout.minimumWidth: Kirigami.Units.iconSizes.huge;
                    QtLayouts.Layout.maximumWidth: Kirigami.Units.iconSizes.huge;
                    Kirigami.Icon {
                        anchors {
                            fill: parent;
                            margins: Kirigami.Units.smallSpacing;
                        }
                        source: "document";
                    }
                }
                QtLayouts.ColumnLayout {
                    QtLayouts.Layout.fillWidth: true
                    QtLayouts.Layout.fillHeight: true
                    Item {
                        QtLayouts.Layout.fillWidth: true
                        QtLayouts.Layout.fillHeight: true
                    }
                    Kirigami.Heading {
                        id: nameText
                        QtLayouts.Layout.fillWidth: true
                        text: model.text
                    }
                    QtControls.Label {
                        id: descriptionText
                        QtLayouts.Layout.fillWidth: true
                        text: model.accountType === "DropBox" ? "" : model.accountType
                        elide: Text.ElideRight
                        wrapMode: Text.Wrap
                    }
                    Item {
                        QtLayouts.Layout.fillWidth: true
                        QtLayouts.Layout.fillHeight: true
                    }
                }
            }
            onClicked: base.showAccount(model.index, model.stackComponent, model.accountDetails);
            actions: [
                Kirigami.Action {
                    text: (model.accountType === "DropBox") ? "Sign Out" : "Edit Account";
                    icon.name: (model.accountType === "DropBox") ? "leave" : "document-edit"
                    onTriggered: {
                        dlgStack.replace(base.editComponentFromName(model.accountType));
                        if(dlgStack.currentPage.accountIndex !== undefined) {
                            dlgStack.currentPage.accountIndex = index;
                            dlgStack.currentPage.text = model.text;
                        }
                    }
                },
                Kirigami.Action {
                    text: "Remove";
                    icon.name: "remove"
                    visible: model.accountType !== "DropBox";
                    onTriggered: {
                        dlgStack.replace(removeAccountDlg);
                        if(dlgStack.currentPage.accountIndex !== undefined) {
                            dlgStack.currentPage.accountIndex = index;
                            dlgStack.currentPage.text = model.text;
                        }
                    }
                }
            ]
        }
        QtControls.Label {
            anchors.fill: accountsView;
            text: "You have no cloud accounts defined. Please add one by clicking on your service of choice below.";
            horizontalAlignment: Text.AlignHCenter;
            verticalAlignment: Text.AlignVCenter;
            opacity: cloudAccounts.count === 0 ? 1 : 0;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
        }
        PageStack {
            id: dlgStack;
            anchors {
                fill: parent;
                margins: -Settings.theme.adjustedPixel(8);
            }
            initialPage: addEmptyComp;
        }
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

    function elementFromName(name) {
        var elements = {
            "accountsPageDropbox": accountsPageDropbox,
            "accountsPageWebdav": accountsPageWebdav,
            "accountsPageGit": accountsPageGit
        };
        return elements[name];
    }
    Component { id: accountsPageDropbox; Dropbox { } }
    Component { id: accountsPageWebdav; Webdav { } }
    Component { id: accountsPageGit; Git { } }
}
