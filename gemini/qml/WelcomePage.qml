/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.calligra 1.0
import org.kde.kirigami 2.1 as Kirigami
import "welcomepages"
import "components"

Kirigami.ApplicationItem {
    id: base;
    onWidthChanged: {
        Constants.setGridWidth( width / Constants.GridColumns );
        drawer.modal = width < Kirigami.Units.gridUnit * 20;
    }
    onHeightChanged: Constants.setGridHeight( height / Constants.GridRows );
    DocumentListModel { id: allDocumentsModel; }
    DocumentListModel { id: textDocumentsModel; filter: DocumentListModel.TextDocumentType; }
    DocumentListModel { id: presentationDocumentsModel; filter: DocumentListModel.PresentationType; }

    pageStack.initialPage: welcomePageFilebrowser;
    pageStack.defaultColumnWidth: pageStack.width
    pageStack.layers.onCurrentItemChanged: pageStack.layers.currentItem !== null ? mainWindow.currentTouchPage = (pageStack.layers.currentItem.pageName !== undefined) ? pageStack.layers.currentItem.pageName : pageStack.layers.currentItem.toString() : ""
    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.Auto
    Component.onCompleted: {
        if(RecentFileManager.size() > 0) {
            pageStack.replace(welcomePageRecent);
        }
    }

    function openFile(fileName, alternativeSaveAction) {
        baseLoadingDialog.visible = true;
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

    Connections {
        target: pageStack.layers
        onDepthChanged: {
            if (pageStack.layers.depth === 1) {
                base.globalDrawer.drawerOpen = true
                applicationWindow().controlsVisible = true
            } else {
                base,globalDrawer.drawerOpen = false
                base.contextDrawer.drawerOpen = false
                applicationWindow().controlsVisible = false
            }
        }
    }

    globalDrawer: Kirigami.GlobalDrawer {
        id: drawer
        // Autohiding behavior
        onModalChanged: drawerOpen = !modal

        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        header: Kirigami.AbstractApplicationHeader {
            topPadding: Kirigami.Units.smallSpacing;
            bottomPadding: Kirigami.Units.smallSpacing;
            leftPadding: Kirigami.Units.largeSpacing
            rightPadding: Kirigami.Units.largeSpacing
            Kirigami.Heading {
                level: 1
                text: i18n("Calligra Gemini")
            }
        }

        component PlaceHeading : Kirigami.Heading {
            topPadding: Kirigami.Units.largeSpacing
            leftPadding: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            level: 6
            opacity: 0.7
        }

        component PlaceItem : QQC2.ItemDelegate {
            id: item
            checkable: true
            Layout.fillWidth: true
            Keys.onDownPressed: nextItemInFocusChain().forceActiveFocus(Qt.TabFocusReason)
            Keys.onUpPressed: nextItemInFocusChain(false).forceActiveFocus(Qt.TabFocusReason)
            Accessible.role: Accessible.MenuItem
            highlighted: checked
            contentItem: Row {
                Kirigami.Icon {
                    source: item.icon.name
                    width: height
                    height: Kirigami.Units.iconSizes.small
                }
                QQC2.Label {
                    leftPadding: Kirigami.Units.smallSpacing
                    text: item.text
                }
            }
        }

        // Place
        QQC2.ScrollView {
            id: scrollView
            Layout.topMargin: -Kirigami.Units.smallSpacing;
            Layout.bottomMargin: -Kirigami.Units.smallSpacing;
            Layout.fillHeight: true
            Layout.fillWidth: true

            Accessible.role: Accessible.MenuBar

            ColumnLayout {
                id: placeColumn
                spacing: 1
                width: scrollView.width

                QQC2.ButtonGroup {
                    id: placeGroup
                }

                PlaceHeading {
                    text: i18n("Open")
                }
                PlaceItem {
                    id: recentDocumentsItem
                    text: i18n("Recent Documents")
                    icon.name: "document-open-recent"
                    onToggled: if(checked) { pageStack.replace(welcomePageRecent) }
                    QQC2.ButtonGroup.group: placeGroup
                    checked: true
                }
                PlaceItem {
                    id: libraryItem
                    text: i18n("Library")
                    icon.name: "folder-documents"
                    onToggled: if(checked) { pageStack.replace(welcomePageFilebrowser) }
                    QQC2.ButtonGroup.group: placeGroup
                }
                PlaceItem {
                    id: cloudItem
                    text: i18n("Cloud")
                    icon.name: "folder-cloud"
                    onToggled: if(checked) { pageStack.replace(welcomePageCloud) }
                    QQC2.ButtonGroup.group: placeGroup
                }
                PlaceHeading {
                    text: i18n("Create new")
                }
                PlaceItem {
                    id: documentItem
                    text: i18n("Document")
                    icon.name: "x-office-document"
                    onToggled: if(checked) { pageStack.replace(welcomePageWords) }
                    QQC2.ButtonGroup.group: placeGroup
                }
                PlaceItem {
                    id: presentationItem
                    text: i18n("Presentation")
                    icon.name: "x-office-presentation"
                    onToggled: if(checked) { pageStack.replace(welcomePageStage) }
                    QQC2.ButtonGroup.group: placeGroup
                }
            }
        }
    }

    Component { id: mainPage; MainPage { } }
    Component { id: welcomePageFilebrowser; WelcomePageFilebrowser { } }
    Component { id: welcomePageRecent; WelcomePageRecent { } }
    Component { id: welcomePageStage; WelcomePageStage { } }
    Component { id: welcomePageWords; WelcomePageWords { } }
    Component { id: welcomePageCloud; WelcomePageCloud { } }

    VariantSelector { id: variantSelector; }
    VariantSelector { id: wordsVariantSelector; selectorType: "words"; }

    // This component is used to get around the fact that MainPage takes a very long time to initialise in some cases
    Dialog {
        id: baseLoadingDialog;
        title: i18n("Loading");
        message: i18n("Please wait...");
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
