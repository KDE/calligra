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
import QtQuick.Controls 1.3
import org.calligra 1.0
import org.kde.kirigami 2.1 as Kirigami
import "welcomepages"
import "components"

Kirigami.ApplicationItem {
    id: base;
    onWidthChanged: Constants.setGridWidth( width / Constants.GridColumns );
    onHeightChanged: Constants.setGridHeight( height / Constants.GridRows );
    DocumentListModel { id: allDocumentsModel; }
    DocumentListModel { id: textDocumentsModel; filter: DocumentListModel.TextDocumentType; }
    DocumentListModel { id: presentationDocumentsModel; filter: DocumentListModel.PresentationType; }

    pageStack.initialPage: welcomePageFilebrowser;
    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.Auto
    pageStack.layers.onCurrentItemChanged: pageStack.layers.currentItem !== null ? mainWindow.currentTouchPage = (pageStack.layers.currentItem.pageName !== undefined) ? pageStack.layers.currentItem.pageName : pageStack.layers.currentItem.toString() : ""
    Component.onCompleted: {
        if(RecentFileManager.size() > 0) {
            pageStack.replace(welcomePageRecent);
        }
    }

    function openFile(fileName, alternativeSaveAction) {
        loadInitiator.fileName = fileName;
        loadInitiator.alternativeSaveAction = alternativeSaveAction;
        loadInitiator.start();
    }
    Timer {
        id: loadInitiator
        running: false; repeat: false; interval: 1
        property string fileName
        property var alternativeSaveAction
        onTriggered: {
            pageStack.layers.push(mainPage);
            Settings.currentFile = "";
            Settings.currentFile = fileName;
            pageStack.layers.currentItem.openFileReal(fileName);
            RecentFileManager.addRecent(fileName);
            mainWindow.setAlternativeSaveAction(alternativeSaveAction);
        }
    }

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }
    globalDrawer: Kirigami.GlobalDrawer {
        title: "Calligra Gemini"
        titleIcon: Settings.theme.iconActual("Calligra-MockIcon-1");
        drawerOpen: true;
        modal: false;
        actions: [
            Kirigami.Action  {
                text: "OPEN"
            },
            Kirigami.Action  {
                text: "Recent Documents"
                iconName: "document-open-recent"
                onTriggered: if(pageStack.currentItem.objectName != "welcomePageRecent") pageStack.replace(welcomePageRecent);
                checked: pageStack.currentItem !== null && pageStack.currentItem.objectName == "WelcomePageRecent";
            },
            Kirigami.Action  {
                text: "Library"
                iconName: "folder-documents"
                onTriggered: if(pageStack.currentItem.objectName != "WelcomePageFilebrowser") pageStack.replace(welcomePageFilebrowser);
                checked: pageStack.currentItem !== null && pageStack.currentItem.objectName == "WelcomePageFilebrowser";
            },
            Kirigami.Action  {
                text: "Cloud"
                iconName: "folder-cloud"
                onTriggered: if(pageStack.currentItem.objectName != "WelcomePageCloud") pageStack.replace(welcomePageCloud);
                checked: pageStack.currentItem !== null && pageStack.currentItem.objectName == "WelcomePageCloud";
            },
            Kirigami.Action  {
                text: "CREATE NEW"
            },
            Kirigami.Action  {
                text: "Document"
                iconName: "x-office-document"
                onTriggered: if(pageStack.currentItem.objectName != "WelcomePageWords") pageStack.replace(welcomePageWords);
                checked: pageStack.currentItem !== null && pageStack.currentItem.objectName == "WelcomePageWords";
            },
            Kirigami.Action  {
                text: "Presentation"
                iconName: "x-office-presentation"
                onTriggered: if(pageStack.currentItem.objectName != "WelcomePageStage") pageStack.replace(welcomePageStage);
                checked: pageStack.currentItem !== null && pageStack.currentItem.objectName == "WelcomePageStage";
            }
        ]
    }
    Component { id: mainPage; MainPage { } }
    Component { id: welcomePageFilebrowser; WelcomePageFilebrowser { } }
    Component { id: welcomePageRecent; WelcomePageRecent { } }
    Component { id: welcomePageStage; WelcomePageStage { } }
    Component { id: welcomePageWords; WelcomePageWords { } }
    Component { id: welcomePageCustom; WelcomePageCustom { } }
    Component { id: welcomePageCloud; WelcomePageCloud { } }

    VariantSelector { id: variantSelector; }
    VariantSelector { id: wordsVariantSelector; selectorType: "words"; }

    // This component is used to get around the fact that MainPage takes a very long time to initialise in some cases
    Dialog {
        id: baseLoadingDialog;
        title: "Loading";
        message: "Please wait...";
        textAlign: Text.AlignHCenter;
        modalBackgroundColor: "#ffffff";
        opacity: 1;
        progress: 0;
        visible: false;
        function hideMe() { timer.start(); }
        Timer {
            id: timer;
            interval: 500; running: false; repeat: false;
            onTriggered: { parent.visible = false; baseLoadingDialog.progress = -1; }
        }
    }
}
