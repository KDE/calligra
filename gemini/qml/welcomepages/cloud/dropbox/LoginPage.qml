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

Page {
    anchors.fill: parent

    property string urlString : "https://www.dropbox.com/1/oauth/authorize?display=mobile&oauth_token="+ controllerMIT.oauth_token

    Component.onCompleted: {
        controllerMIT.oauth_v1_request_token()
    }

    DropboxWebView { id: webView }

    InfoBanner { id: i_infobanner; }

    Connections {
        target: controllerMIT
        onNetwork_error : {
            i_infobanner.show(error);
            console.debug("Network error while loading DropBox:\n" + error);
        }
        onAuthenticate_finished : {
            pageStack.pop();
        }
        onOpen_oauth_authorize_page : {
            webView.url = "https://www.dropbox.com/1/oauth/authorize?display=mobile&oauth_token="+oauth_token
        }
    }
}
// TODO Handle error() code 3 (dns resolution error) and other no-network situations
