/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.11
import org.kde.kirigami 2.7 as Kirigami
import "../../../components"

Kirigami.Page {
    title: "DropBox Login"

    property string urlString : "https://www.dropbox.com/1/oauth/authorize?display=mobile&oauth_token="+ controllerMIT.oauth_token

    Component.onCompleted: {
        controllerMIT.oauth_v1_request_token()
    }

    Loader {
        id: webView
        anchors.fill: parent
        source: "DropboxWebView.qml"
        onStatusChanged: {
            if(status === Loader.Error) {
                applicationWindow().showPassiveNotification("Failed to load Web View component.");
            }
        }
    }

    Connections {
        target: controllerMIT
        onNetwork_error : {
            applicationWindow().showPassiveNotification("Network error while loading DropBox:\n" + error);
            console.debug("Network error while loading DropBox:\n" + error);
        }
        onAuthenticate_finished : {
            pageStack.pop();
        }
        onOpen_oauth_authorize_page : {
            webView.item.url = "https://www.dropbox.com/1/oauth/authorize?display=mobile&oauth_token="+oauth_token
        }
    }
}
// TODO Handle error() code 3 (dns resolution error) and other no-network situations
