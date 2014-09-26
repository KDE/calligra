import QtQuick 1.1
import QtWebKit 1.0
import "../../../components"

Page {
    anchors.fill: parent

    property string urlString : "https://www.dropbox.com/1/oauth/authorize?display=mobile&oauth_token="+ controllerMIT.oauth_token

    Component.onCompleted: controllerMIT.oauth_v1_request_token()

    DropboxWebView { id: webView }

    InfoBanner { id: i_infobanner; }

    Connections {
        target: controllerMIT
        onNetwork_error : {
            i_infobanner.text = error
            i_infobanner.visible = true
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
