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
import org.krita.draganddrop 1.0 as DnD
import org.krita.sketch 1.0

Item {
    id: base;

    function collapse() {
        if(d.peeking) {
            d.peeking.state = "collapsed";
            d.peeking.z = 0;
            d.peeking = null;
        }
    }

    PresetsPanel {
        id: presetsPanel;
        objectName: "presets";
        width: Constants.GridWidth * 2;
        height: base.height;
        page: base.parent;
        onPeek: beginPeek( presetsPanel );
        onCollapsed: endPeek( presetsPanel );
        onFull: endPeek( presetsPanel );
        onDragStarted: beginDrag( presetsPanel );
        onDrop: endDrag( presetsPanel, action );
    }
    LayersPanel {
        id: layersPanel;
        objectName: "layers";
        width: Constants.GridWidth * 2;
        height: base.height;
        page: base.parent;
        onPeek: beginPeek( layersPanel );
        onCollapsed: endPeek( layersPanel );
        onFull: endPeek( layersPanel );
        onDragStarted: beginDrag( layersPanel );
        onDrop: endDrag( layersPanel, action );
    }
    FilterPanel {
        id: filterPanel;
        objectName: "filter";
        width: Constants.GridWidth * 2;
        height: base.height;
        page: base.parent;
        onPeek: beginPeek( filterPanel );
        onCollapsed: endPeek( filterPanel );
        onFull: endPeek( filterPanel );
        onDragStarted: beginDrag( filterPanel );
        onDrop: endDrag( filterPanel, action );
    }
    SelectPanel {
        id: selectPanel;
        objectName: "select";
        width: Constants.GridWidth * 2;
        height: base.height;
        page: base.parent;
        onPeek: beginPeek( selectPanel );
        onCollapsed: endPeek( selectPanel );
        onFull: endPeek( selectPanel );
        onDragStarted: beginDrag( selectPanel );
        onDrop: endDrag( selectPanel, action );
    }
    ToolPanel {
        id: toolPanel;
        objectName: "tool";
        width: Constants.GridWidth * 2;
        height: base.height;
        page: base.parent;
        onPeek: beginPeek( toolPanel );
        onCollapsed: endPeek( toolPanel );
        onFull: endPeek( toolPanel );
        onDragStarted: beginDrag( toolPanel );
        onDrop: endDrag( toolPanel, action );
    }
    ColorPanel {
        id: colorPanel;
        objectName: "color";
        width: Constants.GridWidth * 2;
        height: base.height;
        page: base.parent;
        onPeek: beginPeek( colorPanel );
        onCollapsed: endPeek( colorPanel );
        onFull: endPeek( colorPanel );
        onDragStarted: beginDrag( colorPanel );
        onDrop: endDrag( colorPanel, action );
    }

    Component.onCompleted: {
        panelConfiguration.restore();
    }

    PanelDropArea {
        id: leftArea;

        objectName: "leftFull";

        anchors.bottom: parent.bottom;
        anchors.left: parent.left;

        height: parent.height;
        onHeightChanged: if( children.length > 0 ) children[0].height = height;
        Behavior on height { NumberAnimation { duration: 100; } }
        width: Constants.GridWidth * 2;
        state: "full";
    }

    PanelDropArea {
        id: rightArea;

        objectName: "rightFull";

        anchors.bottom: parent.bottom;
        anchors.right: parent.right;

        height: parent.height;
        onHeightChanged: if( children.length > 0 ) children[0].height = height;
        Behavior on height { NumberAnimation { duration: 100; } }
        width: Constants.GridWidth * 2;
        state: "full";
    }

    Row {
        id: topRow;
        PanelDropArea {
            id: leftTopArea1;

            objectName: "leftTop1";

            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;

            onChildrenChanged: leftArea.height = (children.length == 0 && leftTopArea2.children.length == 0) ? base.height : base.height - Constants.GridHeight / 2;
        }

        PanelDropArea {
            id: leftTopArea2;

            objectName: "leftTop2";

            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;

            onChildrenChanged: leftArea.height = (children.length == 0 && leftTopArea1.children.length == 0) ? base.height : base.height - Constants.GridHeight / 2;
        }

        PanelDropArea {
            id: centerTopArea1;
            objectName: "centerTop1"

            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;
        }
        PanelDropArea {
            id: centerTopArea2;
            objectName: "centerTop2"

            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;
        }
        PanelDropArea {
            id: centerTopArea3;
            objectName: "centerTop3"

            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;
        }
        PanelDropArea {
            id: centerTopArea4;
            objectName: "centerTop4"

            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;
        }
        PanelDropArea {
            id: centerTopArea5;
            objectName: "centerTop5"

            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;
        }
        PanelDropArea {
            id: centerTopArea6;
            objectName: "centerTop6"

            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;
        }
        PanelDropArea {
            id: centerTopArea7;
            objectName: "centerTop7"

            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;
        }
        PanelDropArea {
            id: centerTopArea8;
            objectName: "centerTop8"

            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;
        }

        PanelDropArea {
            id: rightTopArea1;

            objectName: "rightTop1";

            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;

            onChildrenChanged: rightArea.height = (children.length == 0 && rightTopArea2.children.length == 0) ? base.height : base.height - Constants.GridHeight / 2;
        }

        PanelDropArea {
            id: rightTopArea2;

            objectName: "rightTop2";

            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;

            onChildrenChanged: rightArea.height = (children.length == 0 && rightTopArea1.children.length == 0) ? base.height : base.height - Constants.GridHeight / 2;
        }
    }

    Item {
        id: dropOverlay;
        anchors.fill: parent;

        opacity: 0;
        Behavior on opacity { NumberAnimation { } }

        Repeater {
            model: 12;

            delegate: Rectangle {
                x: Constants.GridWidth * index;
                y: 0;
                width: Constants.GridWidth;
                height: Constants.GridHeight / 2;

                color: "transparent";

                border.color: "white";
                border.width: 2;
            }
        }

        Rectangle {
            x: 0;
            y: Constants.GridHeight / 2;
            width: Constants.GridWidth * 2;
            height: parent.height - Constants.GridHeight / 2;

            color: "transparent";

            border.color: "white";
            border.width: 2;
        }

        Rectangle {
            x: Constants.GridWidth * 10;
            y: Constants.GridHeight / 2;
            width: Constants.GridWidth * 2;
            height: parent.height - Constants.GridHeight / 2;

            color: "transparent";

            border.color: "white";
            border.width: 2;
        }
    }

    QtObject {
        id: d;

        property variant panels: [ presetsPanel, layersPanel, filterPanel, selectPanel, toolPanel, colorPanel ];
        property variant panelAreas: [
            leftTopArea1,
            leftTopArea2,
            centerTopArea1,
            centerTopArea2,
            centerTopArea3,
            centerTopArea4,
            centerTopArea5,
            centerTopArea6,
            centerTopArea7,
            centerTopArea8,
            rightTopArea1,
            rightTopArea2,
            leftArea,
            rightArea ];
        property Item peeking: null;
        property Item dragParent: null;
    }

    PanelConfiguration {
        id: panelConfiguration;

        panels: d.panels;
        panelAreas: d.panelAreas;
    }

    function beginPeek( item ) {
        for( var i in d.panels ) {
            var obj = d.panels[i];
            if( obj.state == "peek" && obj.objectName != item.objectName ) {
                obj.state = "collapsed";
            }
        }

        d.peeking = item;
        item.parent.z = 11;
    }

    function endPeek( item ) {
        if(d.peeking == item)
        {
            d.peeking = null;
        }
        item.parent.z = 0;
    }

    function beginDrag( item ) {
        dropOverlay.opacity = 1;
        item.parent.z = 0;
        d.dragParent = item.parent;
        item.parent = null;
        item.opacity = 0;
    }

    function endDrag( item, action ) {
        dropOverlay.opacity = 0;
        item.opacity = 1;
        if(action == Qt.IgnoreAction) {
            item.parent = d.dragParent;
        }
    }
}
