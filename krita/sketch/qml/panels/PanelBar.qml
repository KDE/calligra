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

    MouseArea {
        id: peekCapture;
        anchors.fill: parent;
        enabled: false;

        onClicked: d.peeking.state = "collapsed";
    }
    SimpleTouchArea {
        anchors.fill: parent;
        enabled: peekCapture.enabled;
        onTouched: d.peeking.state = "collapsed";
    }

    PresetsPanel {
        id: presetsPanel;
        objectName: "presets";
        width: Constants.GridWidth * 2;
        height: base.height;
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
        onPeek: beginPeek( colorPanel );
        onCollapsed: endPeek( colorPanel );
        onFull: endPeek( colorPanel );
        onDragStarted: beginDrag( colorPanel );
        onDrop: endDrag( colorPanel, action );
    }

    Component.onCompleted: {
        for( var i in d.panels ) {
            d.panels[i].parent = centerTopArea.children[i];
        }
    }


    PanelDropArea {
        id: leftArea;

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

        anchors.bottom: parent.bottom;
        anchors.right: parent.right;

        height: parent.height;
        onHeightChanged: if( children.length > 0 ) children[0].height = height;
        Behavior on height { NumberAnimation { duration: 100; } }
        width: Constants.GridWidth * 2;
        state: "full";
    }

    PanelDropArea {
        id: leftTopArea1;

        width: Constants.GridWidth;
        height: Constants.GridHeight / 2;

        onChildrenChanged: leftArea.height = (children.length == 0 && leftTopArea2.children.length == 0) ? parent.height : parent.height - Constants.GridHeight / 2;
    }

    PanelDropArea {
        id: leftTopArea2;

        anchors.left: leftTopArea1.right;
        anchors.leftMargin: 1;

        width: Constants.GridWidth;
        height: Constants.GridHeight / 2;

        onChildrenChanged: leftArea.height = (children.length == 0 && leftTopArea1.children.length == 0) ? parent.height : parent.height - Constants.GridHeight / 2;
    }

    Row {
        id: centerTopArea;
        anchors.left: leftTopArea2.right;
        anchors.leftMargin: 1;

        spacing: 1;
        Repeater {
            model: Constants.GridColumns - 4;

            delegate: PanelDropArea {
                width: Constants.GridWidth;
                height: Constants.GridHeight / 2;
            }
        }
    }

    PanelDropArea {
        id: rightTopArea1;

        anchors.right: rightTopArea2.left;
        anchors.rightMargin: 1;

        width: Constants.GridWidth;
        height: Constants.GridHeight / 2;

        onChildrenChanged: rightArea.height = (children.length == 0 && rightTopArea2.children.length == 0) ? parent.height : parent.height - Constants.GridHeight / 2;
    }

    PanelDropArea {
        id: rightTopArea2;

        anchors.right: parent.right;

        width: Constants.GridWidth;
        height: Constants.GridHeight / 2;

        onChildrenChanged: rightArea.height = (children.length == 0 && rightTopArea1.children.length == 0) ? parent.height : parent.height - Constants.GridHeight / 2;
    }

    Item {
        id: dropOverlay;
        anchors.fill: parent;

        opacity: 0;
        Behavior on opacity { NumberAnimation { } }

        Row {
            id: dropRow;
            spacing: 1;
            Repeater {
                model: Constants.GridColumns;

                delegate: Rectangle {
                    width: Constants.GridWidth;
                    height: Constants.GridHeight / 2;

                    color: "transparent";

                    border.color: "white";
                    border.width: 5;
                }
            }
        }

        Rectangle {
            width: Constants.GridWidth * 2;
            anchors.top: dropRow.bottom;
            anchors.bottom: parent.bottom;

            color: "transparent";

            border.color: "white";
            border.width: 5;
        }

        Rectangle {
            width: Constants.GridWidth * 2;
            anchors.top: dropRow.bottom;
            anchors.bottom: parent.bottom;
            anchors.right: parent.right;

            color: "transparent";

            border.color: "white";
            border.width: 5;
        }
    }

    QtObject {
        id: d;

        property variant panels: [ presetsPanel, layersPanel, filterPanel, selectPanel, toolPanel, colorPanel ];
        property Item peeking: null;
        property Item dragParent: null;
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
        peekCapture.enabled = true;
    }

    function endPeek( item ) {
        if(d.peeking == item)
        {
            d.peeking = null;
            peekCapture.enabled = false;
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
