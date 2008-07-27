Ext.onReady(function() {

    var treeTables = new Ext.tree.TreePanel ({
        title: 'Objects',
        region: 'west',
        width: 200,
        margins: '5 0 5 5',
        animate: true,
        lines: false,
        rootVisible: false,
        loader: new Ext.tree.TreeLoader({
            dataUrl: '/objects'
        }),
        
        root: new Ext.tree.AsyncTreeNode()
    });

    var mainPanel = new Ext.Panel ({
        region: 'center',
        margins: '5 5 5 5'
    });
    
    var loginWindow = new Ext.Window ({
        title: 'Login',
        layout: 'border',
        plain: true,
        modal: true,
        closable: false,
        animEl: 'Viewport',
        items: new Ext.form.FormPanel ({
            region: 'center',
            url: '/login',
            defaultType: 'textfield',
            items: [{
                fieldLabel: 'User name',
                name: 'user',
                anchor: '100%'
            }, {
                fieldLabel: 'Password',
                name: 'password',
                anchor: '100%'
            }]
        }),
        buttons: [{
            text: 'Login',
            handler: function() {
                // fake process login
                loginWindow.hide();
                Ext.MessageBox.show({
                    closable: false,
                    msg: 'Authenticating',
                    icon: 'ext-mb-wait',
                    animEl: 'Viewport'
                });
                setTimeout(function(){
                    //This simulates a long-running operation like a database save or XHR call.
                    //In real code, this would be in a callback function.
                    Ext.MessageBox.hide();
                    Ext.MessageBox.show({
                        msg: 'Login failed',
                        buttons: Ext.MessageBox.OK,
                        icon: Ext.MessageBox.ERROR
                    });
                    //loginWindow.show();
                }, 2000);
            }
        },{
            text: 'Reset'
        }]
    });

    var Viewport = new Ext.Viewport ({
        layout: 'border',
        title: 'Kexi Web Forms',
        split: true,
        margins: '2 0 5 5',
        items: [
            treeTables,
            mainPanel
        ],
        renderTo: Ext.getBody()
    });

    
    //loginWindow.show();
    
});
