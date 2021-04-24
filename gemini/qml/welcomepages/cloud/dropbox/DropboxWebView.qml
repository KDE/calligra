/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.11
import QtQuick.Controls 2.5 as QtControls
import QtWebEngine 1.5

WebEngineView {
    id: webView
    anchors.fill: parent

//    experimental.userAgent: "Mozilla/5.0 (Linux; U; like Android 4.0.3; ko-kr; Jolla Build/Alpha) AppleWebkit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30"
    onLoadingChanged: {
        if(loadRequest.status == WebEngineView.LoadSucceededStatus) {
            tryingText.visible = false;
            if(loadRequest.url.toString().length === 48) {  controllerMIT.oauth_v1_access_token() }
    //        if(loadRequest.url.toString().length === 140) experimental.test.touchTap(webView, 100, 150)
        }
    }

    QtControls.Label {
        id: tryingText
        anchors.centerIn: parent;
        text: "Trying to load login page..."
        color: "Grey"
        z: 10
        QtControls.BusyIndicator {
            id: b_indicator
            anchors {
                top: parent.verticalCenter
                topMargin: parent.font.pixelSize;
                horizontalCenter: parent.horizontalCenter
            }
            running: parent.visible;
        }
    }
}


