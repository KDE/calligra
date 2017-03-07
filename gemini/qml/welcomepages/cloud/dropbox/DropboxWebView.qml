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
import QtWebKit 3.0
import "../../../components"

WebView {
    id: webView
    anchors.fill: parent

//    experimental.userAgent: "Mozilla/5.0 (Linux; U; like Android 4.0.3; ko-kr; Jolla Build/Alpha) AppleWebkit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30"
    onLoadingChanged: {
        if(status == WebView.LoadSucceededStatus) {
            tryingText.visible = false;
            if(url.toString().length === 48) {  controllerMIT.oauth_v1_access_token() }
    //        if(url.toString().length === 140) experimental.test.touchTap(webView, 100, 150)
        }
    }

    Label {
        id: tryingText
        anchors.centerIn: parent;
        text: "Trying to load login page..."
        color: "Grey"
        z: 10
        BusyIndicator {
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


