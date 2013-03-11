Ext.onReady(function() {

    // Refresh objects button
    var refreshObjectsButton = new Ext.Toolbar.Button({
        text: 'Refresh',
        handler: function() {
        }
    });

    // Objects tree
    var objectsTree = new Ext.tree.TreePanel({
        autoScroll: true,
        border: false,
        containerScroll: true,
        frame: false,
        root: new Ext.tree.AsyncTreeNode(),
        rootVisible: false,
        loader: new Ext.tree.TreeLoader({
            dataUrl: '/objects'
        })
    });
    objectsTree.on('click', function(n) {
        var sn = this.selModel.selNode || {}; // selNode is null on initial selection
        if(n.leaf && n.id != sn.id){  // ignore clicks on folders and currently selected node
            
        }
    });


    // The main panel
    var mainPanel = new Ext.Panel({
        renderTo: 'main',
        layout: 'border',
        defaults: {
            split: true
        },
        width: '100%',
        height: 711,
        items: [{
            title: 'Objects',
            region: 'west',
            margins: '5 0 5 5',
            width: 200,
            items: objectsTree
        },{
            title: 'Container',
            region: 'center',
            margins: '5 5 5 0'
        }]
    });
    
});
