/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2012 Sujith Haridasan <sujith.haridasan@kdemail.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents

Item
{
    id: root
    property QtObject documentController

    onDocumentControllerChanged: {
      initializePageIcons();
    }

    ListModel{
      id:viewModel
    }

    function initializePageIcons() {
        for ( i=1; i <= root.documentController.documentHandler().totalPages; i++ ) {
              displayText = "Page " + i;
              viewModel.append({"name": displayText});
        }
    }

    Component {
       id: customText
       Item {
         width: 100; height: 50
         Column {
             Text {
                text: name
                MouseArea {
                   anchors.fill: parent
                   onClicked: root.documentController.documentHandler().gotoPage(index+1)
                }
             }
         }
       }
    }

    ListView {
       anchors.fill: parent
       model: viewModel
       delegate: customText
       focus: true
    }
}
