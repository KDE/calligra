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

Kirigami.ApplicationItem {
    id: base;
    DocumentListModel { id: allDocumentsModel; }
    DocumentListModel { id: textDocumentsModel; filter: DocumentListModel.TextDocumentType; }
    DocumentListModel { id: presentationDocumentsModel; filter: DocumentListModel.PresentationType; }
    Component.onCompleted: {
        if(RecentFileManager.size() > 0) {
            pageStack.replace(welcomePageRecent);
        }
    }
    globalDrawer: Kirigami.GlobalDrawer {
        id: welcomeSidebar;
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

    pageStack.initialPage: welcomePageFilebrowser;
    Component { id: welcomePageFilebrowser; WelcomePageFilebrowser { } }
    Component { id: welcomePageRecent; WelcomePageRecent { } }
    Component { id: welcomePageStage; WelcomePageStage { } }
    Component { id: welcomePageWords; WelcomePageWords { } }
    Component { id: welcomePageCustom; WelcomePageCustom { } }
    Component { id: welcomePageCloud; WelcomePageCloud { } }

    Component { id: mainPage; MainPage { } }

    VariantSelector { id: variantSelector; }
    VariantSelector { id: wordsVariantSelector; selectorType: "words"; }
}
