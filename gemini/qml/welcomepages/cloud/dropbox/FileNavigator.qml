/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.11
import QtQuick.Controls 2.5 as QtControls
import org.kde.kirigami 2.7 as Kirigami
import org.calligra 1.0
import Calligra.Gemini.Dropbox 1.0
import "../../../components"

GridView {
    id: component;
    property string title: "";
    property string errorText: "";
    property bool have_checked: false;
    property string currentDir: "/";

    property bool downloadWasRequested: false;
    property string fileMimetype;
    property string fileName;

    property QtObject provider;

    QtObject {
        id: fakeTheme;
        property int itemSizeLarge: Settings.theme.adjustedPixel(64);
        property int paddingLarge: Settings.theme.adjustedPixel(8);
        property int paddingSmall: Settings.theme.adjustedPixel(8);
        property int fontSizeLarge: Settings.theme.adjustedPixel(28);
        property int fontSizeSmall: Settings.theme.adjustedPixel(18);
        property color secondaryColor: "black";
        property color highlightColor: "grey";
    }

    cellWidth: width / 4;
    cellHeight: cellWidth + Settings.theme.font("templateLabel").pixelSize;
    model: folderListModel;
    delegate: itemDelegate;
    cacheBuffer: 1000
    header: Item {
        id: pageHeader;
        height: currentDir !== "/" ? backToRootButton.height : 0;
        width: component.width;
        QtControls.Button {
            id: backToRootButton;
            anchors {
                top: parent.top;
                margins: fakeTheme.paddingSmall
            }
            width: component.width;
            height: Settings.theme.adjustedPixel(64);
            opacity: currentDir !== "/" ? 1 : 0;
            Behavior on opacity{ PropertyAnimation{ duration: Constants.AnimationDuration; } }
            icon.source: Settings.theme.icon("SVG-Icon-MenuBack-1");
            text: "Back to parent folder"
            onClicked: {
                controllerMIT.backtoRootDir()
                controllerMIT.getlistoffolder()
            }
        }
    }

    Component {
        id: itemDelegate;
        DocumentTile {
            enabled: model.is_dir ? true : Settings.mimeTypeToDocumentClass(model.mime_type) !== DocumentListModel.UnknownType;
            imageUrl: model.is_dir ? Settings.theme.icon("layer_group-black") : iconFromDocumentClass(Settings.mimeTypeToDocumentClass(model.mime_type));
            title: {
                var nameof = model.path.split("/")
                var nameof1 = nameof[nameof.length - 1]
                if(model.is_dir) {
                    return nameof1;
                }
                return "%1 (%2)".arg(nameof1).arg(model.size);
            }
            onClicked: {
                if (model.is_dir) {
                    controllerMIT.setFolderRoot(model.path)
                    controllerMIT.getlistoffolder()
                } else {
                    controllerMIT.setCheck(index, true);
                    controllerMIT.downloadSelectedFiles();
                    var nameof = model.path.split("/");
                    component.downloadWasRequested = true;
                    component.fileName = nameof[nameof.length - 1];
                    component.fileMimetype = model.mime_type;
                    applicationWindow().pageStack.push(downloadStatus);
                    controllerMIT.start_transfer_process();
                }
            }
            function iconFromDocumentClass(documentClass) {
                switch(documentClass) {
                    case DocumentListModel.PresentationType:
                        return Settings.theme.icon("SVG-Icon-NewPresentation-1");
                        break;
                    case DocumentListModel.TextDocumentType:
                        return Settings.theme.icon("SVG-Icon-NewDocument-1");
                        break;
                    case DocumentListModel.SpreadsheetType:
                        return Settings.theme.icon("SVG-Icon-NewSpreadsheet-1");
                        break;
                    default:
                        return Settings.theme.icon("SVG-Icon-Cloud-1");
                        break;
                }
            }
            Rectangle {
                anchors.fill: parent;
                opacity: parent.enabled ? 0 : 0.7
                color: Kirigami.Theme.backgroundColor
            }
        }
    }

    Component {
        id: downloadStatus;
        Kirigami.ScrollablePage {
            id: downloadStatusPage
            title: "DropBox Download Status"
            ListView {
                id: filestransferListView
                model: filesTransferModel
                delegate: Item {
                    width: ListView.view.width
                    height: is_finished ? 0 : childrenRect.height;
                    clip: true;

                    Column {
                        id:rec_main
                        spacing: Constants.DefaultMargin;
                        height: childrenRect.height;
                        width: parent.width;

                        Image {
                            id: i_fstatus
                            anchors.horizontalCenter: parent.horizontalCenter;
                            source: Settings.theme.icon("SVG-Icon-Cloud-1"); //is_download ? Settings.theme.image("SVG-Icon-Cloud-1") : "image://theme/icon-s-cloud-upload"
                        }
                        Item {
                            width: parent.width;
                            height: Constants.DefaultMargin;
                        }

                        QtControls.Label {
                            id: l_name
                            anchors.horizontalCenter: parent.horizontalCenter;
                            height: font.pixelSize;
                            text: {
                                var filename1 = model.filename.split("/");
                                var filename2 = filename1[filename1.length-1];
                                return filename2;
                            }
                        }

                        QtControls.ProgressBar {
                            id: pb_updown
                            anchors.horizontalCenter: parent.horizontalCenter;
                            visible: !is_finished && !in_queue
                            width: parent.width - 120
                            height: 18
                            value: progressing
                        }

                        QtControls.Label {
                            id: lb_updown_total
                            anchors.horizontalCenter: parent.horizontalCenter;
                            height: visible ? font.pixelSize : 0;
                            visible: !is_finished && !in_queue
                            text:""
                        }

                        Connections {
                            target: controllerMIT
                            onProgressBarChanged : {
                                pb_updown.progress = percent
                                if (sent_received || speed){
                                    lb_updown_total.text = (is_download?"Received: ":"Sent: ")+__sent_received_calculate(sent_received) + " | Speed: " +__speed_calculate(speed)
                                }else {
                                    console.debug(lb_updown_total.text);
                                }
                            }
                        }
                    }
                } //end filestransferDeligate
                onCountChanged: {
                    filestransferListView.visible = (filestransferListView.model.count !== 0)
                    //no_transfers.visible = !filestransferListView.model.count
                }
            } //end ListView
        }
    }

    Item {
        id:this_folder_is_empty
        visible: false
        anchors.fill: parent
        QtControls.Label {
            anchors.centerIn: parent
            text:"This Folder is Empty"
            color: "grey"
        }
    }

    Item {
        id:r_networkerror
        visible: false
        anchors.centerIn: parent
        width: parent.width - fakeTheme.paddingLarge
        height: r_label_x.height + r_button_x.height;
        QtControls.Label {
            id: r_label_x
            anchors {
                bottom: r_button_x.top;
                left: parent.left;
                right: parent.right;
                margins: fakeTheme.paddingLarge;
            }
            height: paintedHeight;
            text: "There was a problem loading your Dropbox. It could be lost connection or a slow network. Check your connection or try again later. The reported error was:\n" + errorText;
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap;
        }
        QtControls.Button {
            id:r_button_x
            text:"Retry"
            anchors {
                horizontalCenter: parent.horizontalCenter;
                bottom: parent.bottom;
            }
            onClicked: {
                r_networkerror.visible = false
                b_indicator.visible = true
                b_indicator_wrap.visible=true
                controllerMIT.refresh_current_folder()
            }
        }
    }

    QtControls.BusyIndicator {
        id: b_indicator
        anchors.centerIn: parent
        running: b_indicator.visible;
    }

    MouseArea {
        id: b_indicator_wrap
        anchors.fill: parent
        visible: true
    }

    Connections {
        target: controllerMIT

        onFolderfinished : {
            r_networkerror.visible = false
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            //tb_back.enabled = !controllerMIT.isRootDir()
            //f_fastscroll.test()
            //top_banner.havemenu = true

            currentDir = controllerMIT.getcurrentdir()
            changeCurrentDir()

            this_folder_is_empty.visible = folderListModel.count ? false : true

            folderListView.positionViewAtBeginning();
        }

        onNetwork_error : { //error
            //top_banner.havemenu = false
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            errorText = error;
            r_networkerror.visible = true
        }

        onNotification : {
            applicationWindow().showPassiveNotification(notification);
            console.log("notification:"+ notification)
        }

        onDelete_selected_items_finished: {
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            applicationWindow().showPassiveNotification(result);
            console.log("onDelete_selected_items_finished:" + result);
        }

        onEnable_download_and_delete_button: {
            console.log("onEnable_download_and_delete_button")
            //top_banner.deleteVisible = have_checked
            //top_banner.addDownloadVisible = have_checked
            //top_banner.moveVisible = have_checked

            component.have_checked = have_checked
        }

        onStopTransfer: {
            if (!controllerMIT.is_push_notification()){
                applicationWindow().showPassiveNotification("Files transfer completed");
            }

            if(component.downloadWasRequested) {
                component.downloadWasRequested = false;
                var filePath = controllerMIT.dropboxFolder() + "/" + component.fileName;
                var docClass = Settings.mimeTypeToDocumentClass(component.fileMimetype);
                if(docClass !== DocumentListModel.UnknownType) {
                    pageStack.pop();
                    openFile(filePath, controllerMIT.uploadMostRecentAction());
                }
                else {
                    console.log("Unknown file format " + docClass + " for file " + filePath + " with stated mimetype " + component.fileMimetype);
                }
            }

            refreshDir();
//             switch(docClass) {
//                 case DocumentListModel.TextDocument:
//                     pageStack.push(pages.textDocument, { title: component.fileName, path: filePath, mimeType: component.fileMimetype });
//                     break;
//                 case DocumentListModel.SpreadSheetDocument:
//                     pageStack.push(pages.spreadsheet, { title: component.fileName, path: filePath, mimeType: component.fileMimetype });
//                     break;
//                 case DocumentListModel.PresentationDocument:
//                     pageStack.push(pages.presentation, { title: component.fileName, path: filePath, mimeType: component.fileMimetype });
//                     break;
//                 case DocumentListModel.PDFDocument:
//                     pageStack.push(pages.pdf, { title: component.fileName, path: filePath, mimeType: component.fileMimetype });
//                     break;
//                 default:
//                     console.log("Unknown file format " + docClass + " for file " + filePath + " with stated mimetype " + component.fileMimetype);
//                     break;
//             }
        }

        onCreate_folder_finished: {
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            applicationWindow().showPassiveNotification(result);
            console.log("onCreate_folder_finished:" + result)
        }

        onRename_folder_finished: {
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            applicationWindow().showPassiveNotification(result);
        }

        onMove_files_folders_finished: {
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            m_is_move = false
            m_is_copy = false
            toolicon_show(true);
            applicationWindow().showPassiveNotification(result);
        }

        onShares_finished:{ //result
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            if (!result){
                applicationWindow().showPassiveNotification("Could not create share link, try again later.");
            }
        }

        onShares_metadata:{ //url, expire
            sharemetadatadlg.url = url
            pageStack.push(sharemetadatadlg)
        }

        onAccountinfo_finished: { //result
            b_indicator.visible = false; b_indicator_wrap.visible=false;
        }

        onAccountinfo_metadata: { //result
            var val=[]
            for(var i=0; i<6;i++){
                if (i==0)
                    val.push(result['display_name'])
                else if(i==1)
                    val.push(result['email'])
                else if(i==2)
                    val.push(result['uid'])
                else if(i==3)
                    val.push(__convertToMB(result['quota_info']['shared']))
                else if(i==4)
                    val.push(__convertToMB(result['quota_info']['quota']))
                else
                    val.push(__convertToMB(result['quota_info']['normal']))
            }
            accountinfodlg.m_data = val
            pageStack.push(accountinfodlg)
        }
    }

    Component.onCompleted: {
        controllerMIT.need_authenticate()
        controllerMIT.getlistoffolder()
    }

//     CohereButton {
//         id:tb_back
//         anchors {
//             bottom: component.bottom;
//             right: component.right;
//         }
//         enabled: !controllerMIT.isRootDir()
//         onClicked: {
//             controllerMIT.backtoRootDir()
//             controllerMIT.getlistoffolder()
//         }
//         text: "Back";
//         textColor: "#5b6573";
//         textSize: Settings.theme.adjustedPixel(18);
//         color: "#D2D4D5";
//     }

    function changeCurrentDir(){
         var maxlength = 30
         if (component.width > component.height)
               maxlength = 70

         if (!currentDir.length) currentDir = "/"
            if (currentDir.length >= maxlength)
                 currentDir = currentDir.substring(0,maxlength-10) + " ... " + currentDir.substring(currentDir.length-10,currentDir.length)
    }
    function refreshDir(){
        b_indicator.visible = true; b_indicator_wrap.visible=true;
        controllerMIT.refresh_current_folder()
    }

    function toFixed(value, precision) {
        var power = Math.pow(10, precision || 0);
        return String(Math.round(value * power) / power);
    }
    function __sent_received_calculate(bytes){
        if (bytes < 1024)
            return toFixed(bytes,2)+ " bytes"
        else
        if (bytes < 1024*1024)
            return toFixed((bytes/1024),2)+ " KB"
        else
            return toFixed(((bytes/1024)/1024),2) + " MB"
    }

    function __speed_calculate(bytes){
        if (bytes < 1024)
            return toFixed(bytes,2) + " bps"
        else
            if(bytes < 1024*1024)
                return toFixed((bytes/1024),2) + " Kbps"
        else
                return toFixed(((bytes/1024)/1024),2) + " Mbps"
    }
}
