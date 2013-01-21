/* This file is part of the KDE project
 * Copyright (C) 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
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
import QtWebKit 1.0
import "../../components"

SharePage {
    pluginName: "ImageShare"
    function submitArt() {
        console.debug("Submit using " + sharingHandler);
    }
    Connections {
        target: sharingHandler;
        onOpenBrowser: {
            shareStack.push(webPage);
            showUrl = url;
        }
        onCloseBrowser: shareStack.pop();
    }
    property string showUrl: "";
    property QtObject stash: null;
    onSharingHandlerChanged: {
        if(sharingHandler !== null) {
            stash = sharingHandler.stash();
        }
    }

    PageStack {
        id: shareStack;
        anchors.fill: parent;
        clip: true;
        initialPage: detailsPage;
    }

    Component {
        id: detailsPage;
        Item {
            anchors.fill: parent;
            anchors.margins: Constants.DefaultMargin;

            Column {
                id: content;
                width: parent.width;
                spacing: Constants.DefaultMargin;

                DropShadow {
                    anchors.horizontalCenter: parent.horizontalCenter;
                    width: Constants.GridWidth * 4;
                    height: Constants.GridHeight * 3;

    //                         Image {
    //                             anchors.fill: parent;
    //                             source:
    //                         }
                }

                TextField { placeholder: "Title"; }
                TextField { placeholder: "Tags"; }
                TextFieldMultiline { height: Constants.GridHeight * 4; placeholder: "Description"; }
            }
        }
    }
    Component {
        id: webPage;
        Item {
            anchors.fill: parent;
            anchors.margins: Constants.DefaultMargin;
            Flickable {
                width: parent.width
                contentWidth: Math.max(parent.width, webView.width)
                contentHeight: Math.max(parent.height, webView.height)
                anchors.fill: parent;
                pressDelay: 200
                onWidthChanged : {
                    // Expand (but not above 1:1) if otherwise would be smaller that available width.
                    if (width > webView.width*webView.contentsScale && webView.contentsScale < 1.0)
                        webView.contentsScale = width / webView.width * webView.contentsScale;
                }
                WebView {
                    id: webView;
                    preferredWidth: shareStack.width;
                    preferredHeight: shareStack.height;
                    url: showUrl;
                }
            }
        }
    }
}