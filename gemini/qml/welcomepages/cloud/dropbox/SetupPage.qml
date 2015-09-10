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
import Calligra.Gemini.Dropbox 1.0
import "../../../components"

Item {
    anchors.fill: parent;
    clip: true;
    Rectangle {
        anchors.fill: parent;
        opacity: 0.6;
    }
    Item {
        id: header;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: Settings.theme.adjustedPixel(64);
    }
    Item {
        anchors {
            left: parent.left;
            right: parent.right;
            top: header.bottom;
        }
        height: Constants.GridHeight;

        CohereButton {
            anchors.centerIn: parent;
            textColor: "#5b6573";
            textSize: Settings.theme.adjustedPixel(18);
            color: "#D2D4D5";
            text: controllerMIT.needAuthenticate ? "Log in to DropBox" : "Log out of DropBox";
            onClicked: {
                if(controllerMIT.needAuthenticate) {
                    pageStack.push( loginPage );
                }
                else {
                    signOutNow();
                }
            }
            Text {
                anchors {
                    top: parent.bottom;
                    topMargin: Settings.theme.adjustedPixel(16);
                    horizontalCenter: parent.horizontalCenter;
                }
                visible: !controllerMIT.needAuthenticate;
                font: Settings.theme.font("application");
                width: parent.parent.width / 2;
                height: font.pixelSize * 6;
                horizontalAlignment: Text.AlignHCenter;
                verticalAlignment: Text.AlignTop;
                wrapMode: Text.Wrap;
                text: "You are already signed into DropBox, and we only support one DropBox account. To sign in as another user, please click on the button above to sign out of your current account first.";
            }
        }
    }
    Component {
        id: loginPage;
        LoginPage { }
    }
    Page {
        id: signoutconfirmationDlg
        //PageHeader { title: "Log Out" }
        Rectangle {
            anchors.fill: parent;
            anchors.margins: Settings.theme.adjustedPixel(16);
            radius: Settings.theme.adjustedPixel(8);
            color: "white";
            Text {
                anchors {
                    bottom: signoutButtonsRow.top;
                    left: parent.left;
                    right: parent.right;
                    margins: Constants.DefaultMargin;
                }
                color: "black"
                font: Settings.theme.font("application");
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                text: "Do you really want to log out of DropBox?"
            }
            Row {
                id: signoutButtonsRow;
                anchors.centerIn: parent;
                width: childrenRect.width;
                spacing: Settings.theme.adjustedPixel(8);
                CohereButton {
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    color: "#D2D4D5";
                    text: "Yes"
                    onClicked: {
                        controllerMIT.logout()
                        pageStack.pop()
                    }
                }
                CohereButton {
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    color: "#D2D4D5";
                    text: "No"
                    onClicked: {
                        pageStack.pop()
                    }
                }
            }
        }
    }

    InfoBanner { id: i_infobanner; }
    function signOutNow(){
        if (!controllerMIT.is_transfer()){
            pageStack.push(signoutconfirmationDlg)
        }else{
            i_infobanner.show("Please complete the upload/download tasks in Files Transfer before Sign out.");
        }
    }
}
