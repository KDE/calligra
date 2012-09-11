/* This file is part of the KDE project
 * Copyright (C) 2012 Boudewijn Rempt <boud@kogmbh.com>
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

import QtQuick 1.1
import org.krita.sketch 1.0 as Krita
import "components"

Page {
    DropShadow {
        id: header;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }

        height: Constants.GridHeight;
        z: 10;

        Header {
            text: "Open Image";

            leftArea: Button {
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                highlightColor: Constants.Theme.HighlightColor;
                text: "< Back";
                textColor: "white";
                onClicked: pageStack.pop();
            }

            rightArea: Button {
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                text: "^ Up";
                textColor: "white";
                onClicked: view.model.path = view.model.parentFolder;
            }
        }
    }

    GridView {
        id: view;
        anchors {
            top: header.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }

        model: Krita.FileSystemModel { filter: "*.png *.jpg *.jpeg *.bmp *.kra *.ora" }
        delegate: delegate;

        cellWidth: Constants.GridWidth * 2;
        cellHeight: Constants.GridHeight * 2;
    }

    Component {
        id: delegate;

        Button {
            width: GridView.view.cellWidth;
            height: GridView.view.cellHeight;

            image: model.icon;
            text: model.fileName;
	    asynchronous: true;

            onClicked: {
                if( model.fileType == "inode/directory" ) {
                    GridView.view.model.path = model.path;
                } else {
                    Settings.currentFile = model.path;
                    RecentFileManager.addRecent( model.path );
                    pageStack.push( main );
                }
            }
        }
    }

    Component { id: main; MainPage { } }
}
