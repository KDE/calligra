import QtQuick 1.1
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
        }
    }
    Component {
        id: loginPage;
        LoginPage { }
    }
    Page {
        id: signoutconfirmationDlg
        //PageHeader { title: "Log Out" }
        Item {
            anchors.fill: parent;

            Text {
                anchors {
                    bottom: signoutButtonsRow.top;
                    left: parent.left;
                    right: parent.right;
                    margins: Constants.DefaultMargin;
                }
                color: "white"
                font: Settings.theme.font("application");
                wrapMode: Text.Wrap
                text: "Do you really want to log out of DropBox?"
            }
            Row {
                id: signoutButtonsRow;
                anchors.centerIn: parent;
                width: childrenRect.width;
                CohereButton {
                    text: "Yes"
                    onClicked: {
                        controllerMIT.logout()
                        pageStack.pop()
                    }
                }
                CohereButton {
                    text: "No"
                    onClicked: {
                        pageStack.pop()
                    }
                }
            }
        }
    }

    function signOutNow(){
        if (!controllerMIT.is_transfer()){
            pageStack.push(signoutconfirmationDlg)
        }else{
            i_infobanner.text = "Please complete the upload/download tasks in Files Transfer before Sign out."
            i_infobanner.visible = true
        }
    }
}
