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
import "../components"
import "cloud"

Page {
    id: base;
/*    ListModel {
        id: cloudAccounts;
        ListElement { text: "Dropbox"; selected: false; accountType: "DropBox"; stackComponent: "accountsPageDropbox"; accountDetails: [ ListElement { userkey: "asfdoijfdshaloiuhs" } ] }
        ListElement { text: "ownCloud"; selected: false; accountType: "WebDav"; stackComponent: "accountsPageWebdav"; accountDetails: [ ListElement { username: "lala" } ] }
        ListElement { text: "bitbucket"; selected: false; accountType: "Git"; stackComponent: "accountsPageGit"; accountDetails: [ ListElement { localrepo: "C:\\Users\\danjensen\\Documents\\nohnas" } ] }
        ListElement { text: "github"; selected: false; accountType: "Git"; stackComponent: "accountsPageGit"; accountDetails: [ ListElement { localrepo: "C:\\dev\\documentation" } ] }
    }*/
    CloudAccountsModel {
        id: cloudAccounts;
    }
    Label {
        id: docTypeSelectorRow;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: Constants.GridHeight * 1.5;
        verticalAlignment: Text.AlignVCenter;
        horizontalAlignment: Text.AlignHCenter;
        font: Settings.theme.font("pageHeader");
        text: "Open from The Cloud";
        color: "#22282f";
        CohereButton {
            id: accountsButton;
            anchors {
                left: parent.left;
                leftMargin: 20;
                verticalCenter: parent.verticalCenter;
            }
            text: "Accounts";
            textColor: "#5b6573";
            textSize: Settings.theme.adjustedPixel(18);
            checkedColor: "#D2D4D5";
            onClicked: {
                cloudAccounts.selectIndex(-1);
                cloudStack.replace(accountsPage);
            }
            checked: cloudStack.currentPage.pageName === "accountsPage";
        }
        Row {
            id: accountsRow;
            anchors {
                right: parent.right;
                rightMargin: 20;
                verticalCenter: parent.verticalCenter;
            }
            height: accountsButton.height; // yeah, i know... cleaner than hacking in and finding the first child button, though ;)
            spacing: 4;
            function elementFromName(name) {
                var elements = {
                    "accountsPageDropbox": accountsPageDropbox,
                    "accountsPageWebdav": accountsPageWebdav,
                    "accountsPageGit": accountsPageGit
                };
                return elements[name];
            }
            Repeater {
                model: cloudAccounts;
                delegate: CohereButton {
                    text: model.text
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    checkedColor: "#D2D4D5";
                    onClicked: {
                        if(model.selected) {
                            return;
                        }
                        cloudAccounts.selectIndex(index);
                        cloudStack.replace(accountsRow.elementFromName(model.stackComponent));
                        cloudStack.currentPage.accountDetails = model.accountDetails;
                    }
                    checked: model.selected;
                }
            }
        }
    }
    PageStack {
        id: cloudStack;
        initialPage: accountsPage;
        anchors {
            margins: Settings.theme.adjustedPixel(8);
            top: docTypeSelectorRow.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
            bottomMargin: 0;
        }
    }
    Component { id: accountsPage; CloudAccounts { accountsList: cloudAccounts; } }
    Component { id: accountsPageDropbox; Dropbox { } }
    Component { id: accountsPageWebdav; Webdav { } }
    Component { id: accountsPageGit; Git { } }
}
