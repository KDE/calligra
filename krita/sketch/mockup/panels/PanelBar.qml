/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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

Item {
    id: base;
    width: 0
    smooth: false
    visible: true

    Row {
        id: layout;
        width: 0
        height: Constants.GridHeight / 2;
        spacing: 8
        z: 2;

        PresetsPanel {
            id: presetsPanel;
            objectName: "presets";
            width: Constants.GridWidth * 2;
            height: base.height;
            onStateChanged: panelStateChanged( presetsPanel, state );
            onCollapsed: setCollapsedParent( presetsPanel );
        }
        LayersPanel {
            id: layersPanel;
            objectName: "layers";
            width: Constants.GridWidth * 2;
            height: base.height;
            onStateChanged: panelStateChanged( layersPanel, state );
            onCollapsed: setCollapsedParent( layersPanel );
        }
        FilterPanel {
            id: filterPanel;
            objectName: "filter";
            width: Constants.GridWidth * 2;
            height: base.height;
            onStateChanged: panelStateChanged( filterPanel, state );
            onCollapsed: setCollapsedParent( filterPanel );
        }
        SelectPanel {
            id: selectPanel;
            objectName: "select";
            width: Constants.GridWidth * 2;
            height: base.height;
            onStateChanged: panelStateChanged( selectPanel, state );
            onCollapsed: setCollapsedParent( selectPanel );
        }
        ToolPanel {
            id: toolPanel;
            objectName: "tool";
            width: Constants.GridWidth * 2;
            height: base.height;
            onStateChanged: panelStateChanged( toolPanel, state );
            onCollapsed: setCollapsedParent( toolPanel );
        }
        ColorPanel {
            id: colorPanel;
            objectName: "color";
            width: Constants.GridWidth * 2;
            height: base.height;
            onStateChanged: panelStateChanged( colorPanel, state );
            onCollapsed: setCollapsedParent( colorPanel );
        }
    }

    MouseArea {
        id: peekCapture;
        anchors.fill: parent;
        z: 3;
        enabled: false;

        onClicked: if( d.peeking != null ) {
            d.peeking.state = "collapsed";
            d.peeking.z = 0;
            enabled = false;
            d.peeking = null;
        }
    }

    QtObject {
        id: d;

        property variant panels: [ presetsPanel, layersPanel, filterPanel, selectPanel, toolPanel, colorPanel ];
        property Item peeking: null;
    }

    function panelStateChanged( panel, state ) {
        if( state == "full" ) {
            setPanelsState( "collapsed", panel );

            panel.parent = base;
            panel.anchors.top = layout.bottom;
            panel.anchors.left = base.left;
            panel.anchors.bottom = base.bottom;
        }

        if( state == "peek" ) {
            setPanelsState( "collapsed", panel );

            d.peeking = panel;
            layout.z = 4;
            peekCapture.enabled = true;
        }
    }

    function setCollapsedParent( panel ) {
        panel.parent = layout;
        panel.anchors.top = layout.top;
        panel.anchors.left = undefined;
        panel.anchors.bottom = undefined;
    }

    function setPanelsState( state, exclude ) {
        for ( var i in d.panels ) {
            if( d.panels[i].objectName != exclude.objectName )
                d.panels[i].state = state;
        }
    }
}
