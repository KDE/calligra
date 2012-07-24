import QtQuick 1.1

Item {
    id: base;

    Row {
        id: layout;
        height: Constants.GridHeight / 2;
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
    }

    QtObject {
        id: d;

        property variant panels: [ presetsPanel, layersPanel, filterPanel, selectPanel, toolPanel ];
    }

    function panelStateChanged( panel, state ) {
        if( state == "full" ) {
            for ( var i in d.panels ) {
                if( d.panels[i].objectName != panel.objectName )
                    d.panels[i].state = "collapsed";
            }

            panel.parent = base;
            panel.anchors.top = layout.bottom;
            panel.anchors.left = base.left;
            panel.anchors.bottom = base.bottom;
        }
    }

    function setCollapsedParent( panel ) {
        panel.parent = layout;
        panel.anchors.top = layout.top;
        panel.anchors.left = undefined;
        panel.anchors.bottom = undefined;
    }
}