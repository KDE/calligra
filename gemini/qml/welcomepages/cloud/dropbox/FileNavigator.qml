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
import org.calligra 1.0
import Calligra.Gemini.Dropbox 1.0
import "../../../components"

Item {
    id: page;
    property QtObject model: folderListModel;
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

    InfoBanner { id: i_infobanner; }
    Component {
        id: itemDelegate;
        Button {
            width: folderListView.cellWidth;
            height: folderListView.cellHeight;
            enabled: model.is_dir ? true : Settings.mimeTypeToDocumentClass(model.mime_type) !== DocumentListModel.UnknownType;
            opacity: enabled ? 1 : 0.3

            Image {
                id: icon;
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    margins: Constants.DefaultMargin / 2;
                }
                height: parent.width;
                fillMode: Image.PreserveAspectFit;
                smooth: true;
                asynchronous: true;
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
                source: model.is_dir ? Settings.theme.icon("layer_group-black") : iconFromDocumentClass(Settings.mimeTypeToDocumentClass(model.mime_type));
                sourceSize.width: width;
                sourceSize.height: height;
            }
            Label {
                id: lblName;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.bottom;
                    margins: Constants.DefaultMargin;
                    bottomMargin: Constants.DefaultMargin * 2;
                }
                height: font.pixelSize + Constants.DefaultMargin * 2;
                horizontalAlignment: Text.AlignHCenter;
                verticalAlignment: Text.AlignVCenter;
                text:{
                    var nameof = model.path.split("/")
                    var nameof1 = nameof[nameof.length - 1]
                    if(model.is_dir) {
                        return nameof1;
                    }
                    return "%1 (%2)".arg(nameof1).arg(model.size);
                }
                elide: Text.ElideMiddle;
                font: Settings.theme.font("templateLabel");
                color: "#5b6573";
            }
            onClicked: {
                if(model.is_dir)
                {
                    controllerMIT.setFolderRoot(model.path)
                    controllerMIT.getlistoffolder()
                }
                else
                {
                    controllerMIT.setCheck(index, true);
                    controllerMIT.downloadSelectedFiles();
                    var nameof = model.path.split("/");
                    page.downloadWasRequested = true;
                    page.fileName = nameof[nameof.length - 1];
                    page.fileMimetype = model.mime_type;
                    pageStack.push(downloadStatus);
                    controllerMIT.start_transfer_process();
                }
            }
        }
    }

    Page {
        id: downloadStatus;
        ListView {
            id: filestransferListView
            anchors {
                fill: parent
                topMargin: Settings.theme.adjustedPixel(64);
            }
            model: filesTransferModel
            delegate: Item {
                width: downloadStatus.width
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

                    Label {
                        id: l_name
                        anchors.horizontalCenter: parent.horizontalCenter;
                        height: font.pixelSize;
                        text: {
                            var filename1 = model.filename.split("/");
                            var filename2 = filename1[filename1.length-1];
                            return filename2;
                        }
                        color: "black"
                    }

                    ProgressBar {
                        id: pb_updown
                        anchors.horizontalCenter: parent.horizontalCenter;
                        visible: !is_finished && !in_queue
                        width: parent.width - 120
                        height: 18
                        progress: progressing
                    }

                    Label {
                        id: lb_updown_total
                        anchors.horizontalCenter: parent.horizontalCenter;
                        height: visible ? font.pixelSize : 0;
                        visible: !is_finished && !in_queue
                        font: Settings.theme.font("templateLabel");
                        color: "#5b6573";
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

    GridView {
        id: folderListView
        visible:false
        clip: true;
        contentWidth: width;
        anchors.fill: parent;
        cellWidth: width / 4 - Constants.DefaultMargin;
        cellHeight: cellWidth + Settings.theme.font("templateLabel").pixelSize + Constants.DefaultMargin * 4;
        model: folderListModel;
        delegate: itemDelegate;
        //section.property: "section"
        cacheBuffer: 1000
        header: Item {
            id: pageHeader;
            height: currentDir !== "/" ? backToRootButton.height : 0;
            width: page.width;
            Button {
                id: backToRootButton;
                anchors {
                    top: parent.top;
                    margins: fakeTheme.paddingSmall
                }
                width: page.width;
                height: Settings.theme.adjustedPixel(64);
                opacity: currentDir !== "/" ? 1 : 0;
                Behavior on opacity{ PropertyAnimation{ duration: Constants.AnimationDuration; } }
                image: Settings.theme.icon("SVG-Icon-MenuBack-1");
                imageMargin: Settings.theme.adjustedPixel(8);
                text: "Back to parent folder"
                textColor: "#5B6573";
                onClicked: {
                    controllerMIT.backtoRootDir()
                    controllerMIT.getlistoffolder()
                }
            }
        }
        ScrollDecorator { flickableItem: folderListView; }
    }

    Item {
        id:this_folder_is_empty
        visible: false
        anchors.fill: parent
        Label {
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
        Label {
            id: r_label_x
            anchors {
                bottom: r_button_x.top;
                left: parent.left;
                right: parent.right;
                margins: fakeTheme.paddingLarge;
            }
            height: paintedHeight;
            text: "There was a problem loading your Dropbox. It could be lost connection or a slow network. Check your connection or try again later. The reported error was: " + errorText;
            wrapMode: Text.Wrap;
        }
        Button {
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

    BusyIndicator {
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
            folderListView.visible = true
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            //tb_back.enabled = !controllerMIT.isRootDir()
            //f_fastscroll.test()
            //top_banner.havemenu = true

            currentDir = controllerMIT.getcurrentdir()
            changeCurrentDir()

            this_folder_is_empty.visible = folderListModel.count ? false : true

            //folderListView.visible = folderListModel.count ? true : false
            folderListView.visible = folderListModel.count ? true : true
            folderListView.positionViewAtBeginning();
        }

        onNetwork_error : { //error
            //top_banner.havemenu = false
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            folderListView.visible = false
            errorText = error;
            r_networkerror.visible = true
        }

        onNotification : {
            i_infobanner.show(notification);
            console.log("notification:"+ notification)
        }

        onDelete_selected_items_finished: {
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            i_infobanner.show(result);
            console.log("onDelete_selected_items_finished:" + result);
        }

        onEnable_download_and_delete_button: {
            console.log("onEnable_download_and_delete_button")
            //top_banner.deleteVisible = have_checked
            //top_banner.addDownloadVisible = have_checked
            //top_banner.moveVisible = have_checked

            page.have_checked = have_checked
        }

        onStopTransfer: {
            if (!controllerMIT.is_push_notification()){
                i_infobanner.show("Files transfer completed");
            }

            if(page.downloadWasRequested) {
                page.downloadWasRequested = false;
                var filePath = controllerMIT.dropboxFolder() + "/" + page.fileName;
                var docClass = Settings.mimeTypeToDocumentClass(page.fileMimetype);
                if(docClass !== DocumentListModel.UnknownType) {
                    pageStack.pop();
                    openFile(filePath, controllerMIT.uploadMostRecentAction());
                }
                else {
                    console.log("Unknown file format " + docClass + " for file " + filePath + " with stated mimetype " + page.fileMimetype);
                }
            }

            refreshDir();
//             switch(docClass) {
//                 case DocumentListModel.TextDocument:
//                     pageStack.push(pages.textDocument, { title: page.fileName, path: filePath, mimeType: page.fileMimetype });
//                     break;
//                 case DocumentListModel.SpreadSheetDocument:
//                     pageStack.push(pages.spreadsheet, { title: page.fileName, path: filePath, mimeType: page.fileMimetype });
//                     break;
//                 case DocumentListModel.PresentationDocument:
//                     pageStack.push(pages.presentation, { title: page.fileName, path: filePath, mimeType: page.fileMimetype });
//                     break;
//                 case DocumentListModel.PDFDocument:
//                     pageStack.push(pages.pdf, { title: page.fileName, path: filePath, mimeType: page.fileMimetype });
//                     break;
//                 default:
//                     console.log("Unknown file format " + docClass + " for file " + filePath + " with stated mimetype " + page.fileMimetype);
//                     break;
//             }
        }

        onCreate_folder_finished: {
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            i_infobanner.show(result);
            console.log("onCreate_folder_finished:" + result)
        }

        onRename_folder_finished: {
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            i_infobanner.show(result);
        }

        onMove_files_folders_finished: {
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            m_is_move = false
            m_is_copy = false
            toolicon_show(true);
            i_infobanner.show(result);
        }

        onShares_finished:{ //result
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            if (!result){
                i_infobanner.show("Could not create share link, try again later.");
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
//             bottom: page.bottom;
//             right: page.right;
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
         if (page.width > page.height)
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
