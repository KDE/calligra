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
import "qml"
import "qml/components"
import org.kde.calligra 1.0 as Calligra

Item {
    id: base;
//     width: 1280;
//     height: 768;
    onWidthChanged: Constants.setGridWidth( width / Constants.GridColumns );
    onHeightChanged: Constants.setGridHeight( height / Constants.GridRows );
    property QtObject window: mainWindow;
    function openFile(fileName, alternativeSaveAction) {
        mainPageStack.push(mainPage);
        Settings.currentFile = "";
        Settings.currentFile = fileName;
        RecentFileManager.addRecent(fileName);
        mainWindow.setAlternativeSaveAction(alternativeSaveAction);
    }
    StackView {
        id: mainPageStack;
        anchors.fill: parent;
        onCurrentItemChanged: currentItem !== null ? window.currentTouchPage = (currentItem.pageName !== undefined) ? currentItem.pageName : currentItem.toString() : "";
        initialItem: welcomePage;
    }
    Component { id: welcomePage; WelcomePage { } }
    Component { id: mainPage; MainPage { } }

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
