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
import org.krita.sketch 1.0
import "../../components"

SharePage {
    id: root;
    pluginName: "ImageShare"
    function submitArt() {
        if(stash !== null) {
            stash.submit(sketchView.view, sketchView.fileTitle, title, description, tags, "");
        }
    }
    canShare: false
    Connections {
        target: sharingHandler;
        onOpenBrowser: {
            shareStack.push(webPage);
            showUrl = url;
        }
        onCloseBrowser: shareStack.pop();
        onLinkingSucceeded: {
            if(!sharingHandler.isAuthenticated()) {
                sharingHandler.authenticate();
            }
        }
    }
    property string showUrl: "";
    onSharingHandlerChanged: if(sharingHandler !== null) { sharingHandler.authenticate(); }
    property QtObject stash: sharingHandler ? sharingHandler.stash : null;
    onStashChanged: if(stash !== null) { stash.testCall(); }
    property string title: "";
    property string tags: "";
    property string description: "";

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

                Item {
                    height: thumbImg.height;
                    width: thumbImg.width;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    DropShadow {
                        anchors.horizontalCenter: parent.horizontalCenter;
                        anchors.fill: thumbImg;
                    }
                    Image {
                        id: thumbImg;
                        anchors.horizontalCenter: parent.horizontalCenter;
                        height: Constants.GridHeight * 3;
                        width: height * (sourceSize.width / sourceSize.height);
                        fillMode: Image.PreserveAspectFit
                        source: layerModel.fullImageThumbUrl;
                    }
                }

                function updateCanShare() {
                    root.canShare = (txtTitle.text !== "" && txtTags.text !== "" && txtDescription.text !== "");
                    root.title = txtTitle.text;
                    root.tags = txtTags.text;
                    root.description = txtDescription.text;
                }
                TextField { id: txtTitle; placeholder: "Title"; text: sketchView.fileTitle.substring(0, sketchView.fileTitle.indexOf(".")); onTextChanged: content.updateCanShare(); }
                TextField { id: txtTags; placeholder: "Tags"; onTextChanged: content.updateCanShare(); }
                TextFieldMultiline { id: txtDescription; height: Constants.GridHeight * 4; placeholder: "Description"; onTextChanged: content.updateCanShare(); }
            }

            Button {
                anchors {
                    top: parent.top;
                    right: parent.right;
                    margins: Constants.DefaultMargin;
                }
                width: Constants.GridWidth * 2;
                height: textSize + Constants.DefaultMargin;
                color: "transparent";
                text: "Open Your Sta.sh...";
                onClicked: Qt.openUrlExternally("http://sta.sh/");
            }
            Button {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    margins: Constants.DefaultMargin;
                }
                width: Constants.GridWidth * 2;
                height: textSize + Constants.DefaultMargin;
                color: "transparent";
                text: "Log Out of Sta.sh";
                onClicked: { sharingManager.clearCookies(QMLEngine); sharingHandler.deauthenticate(); }
            }

            Item {
                anchors.fill: parent;
                anchors.margins: -Constants.DefaultMargin;
                opacity: (root.stash !== null && root.stash.ready) ?  0 : 1;
                Behavior on opacity { PropertyAnimation { duration: 200; } }
                Rectangle {
                    anchors.fill: parent;
                    color: "white";
                    opacity: 0.9;
                }
                MouseArea {
                    // This mouse area blocks any mouse click from passing through the panel. We need this to ensure we don't accidentally pass
                    // any clicks to the collapsing area, or to the canvas, by accident.
                    anchors.fill: parent;
                    // This will always work, and never do anything - but we need some kind of processed thing in here to activate the mouse area
                    onClicked: parent.focus = true;
                }
                SimpleTouchArea {
                    // As above, but for touch events
                    anchors.fill: parent;
                    onTouched: parent.focus = true;
                }
                Label {
                    anchors.centerIn: parent;
                    text: "Sta.sh is not ready..."
                }
            }
        }
    }
    Component {
        id: webPage;
        Item {
            id: webPageContainer
            anchors.fill: parent;
            anchors.margins: Constants.DefaultMargin;
            Component {
                id: webViewNewWindowComponent
                Item {
                    // This container item is a massive hack, caused by newWindowComponent being created from C++ and /not/ set to QML ownership.
                    // That means we have to leave objects around, and Items are fairly cheap
                    width: webPageContainer.width
                    height: webPageContainer.height
                    Rectangle {
                        id: childViewContainer
                        width: parent.width
                        height: parent.height
                        color: "silver";
                        Flickable {
                            clip: true;
                            width: parent.width
                            contentWidth: Math.max(parent.width, childWebView.width)
                            contentHeight: Math.max(parent.height, childWebView.height)
                            anchors.fill: parent;
                            anchors.margins: Constants.DefaultMargin;
                            anchors.topMargin: Constants.GridHeight + Constants.DefaultMargin;
                            pressDelay: 200
                            onWidthChanged : {
                                // Expand (but not above 1:1) if otherwise would be smaller that available width.
                                if (width > childWebView.width*childWebView.contentsScale && childWebView.contentsScale < 1.0)
                                    childWebView.contentsScale = width / childWebView.width * childWebView.contentsScale;
                            }
                            WebView {
                                id: childWebView
                                preferredWidth: childViewContainer ? childViewContainer.width - Constants.DefaultMargin * 2 : parent.width;
                                newWindowComponent: webViewNewWindowComponent
                                newWindowParent: childViewContainer ? childViewContainer : null;
                            }
                        }
                        Button {
                            anchors {
                                top: parent.top;
                                left: parent.left;
                                margins: Constants.DefaultMargin;
                            }
                            color: "gray";
                            text: "Back";
                            onClicked: parent.destroy();
                        }
                    }
                }
            }
            Flickable {
                clip: true;
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
                    newWindowComponent: webViewNewWindowComponent
                    newWindowParent: webPageContainer;
                    url: showUrl;
                }
            }
        }
    }
}