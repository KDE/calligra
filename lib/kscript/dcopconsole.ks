class List
{
  List( in this )
  {
    this.internal = [ 0 ];
  }
  append( in this, in item )
  {
    l = this.internal;
    l += [ item ];
    this.internal = l;
  }
  remove( in this, in item )
  {
    l = this.internal;
    new = [ 0 ];
    newc = 1;
    for ( i = 1; i < l.length(); i++ )
    {
      if ( l[ i ] != item )
      {
        new[ newc++ ] = l[ i ];
      }
    }
    this.internal = new;
  }
  count( in this )
  {
    return this.internal.length() - 1;
  }
  print( in this )
  {
    l = this.internal;
    for ( i = 1; i < l.length(); i++ )
    {
      println( l[ i ] );
    }
  }
};

class App : QApplication
{
  App( in this )
  {
    this.QApplication();
  }
};

class MyPushButton : QPushButton
{
  MyPushButton( in this, in parent )
  {
    this.QPushButton( parent );
  }
};

class AppListBox : QComboBox
{
  update( in this )
  {
    this.clear();
    dcopserver = findApplication( "DCOPServer", "" );
    lst = dcopserver.registeredApplications();
    foreach ( app, lst )
    {
      this.insertItem( app );
    }
  }

  AppListBox( in this, in parentWidget )
  {
    this.QComboBox( parentWidget );
    this.update();
  }
};

class FunctionsListBox : QListBox
{
  update( in this )
  {
    dcopobj = findApplication( this.applst.currentText, this.objedit.text );
    this.clear();
    functions = stringListSplit( ";", dcopobj.functions() );
    foreach( f, functions )
    {
      this.insertItem( f );
    }
  }
  FunctionsListBox( in this, in parentWidget, in applst, in objedit )
  {
    this.QListBox( parentWidget );
    this.applst = applst;
    this.objedit = objedit;
  }
};

class Arg : QWidget
{
  removeMe( in this )
  {
    println( "removeme" );
    this.parent.removeArg( this );
    this.edit.destroy();
    this.button.destroy();
    this.layout.destroy();
  }
  Arg( in this, in parent )
  {
    this.QWidget( parent );
    this.parent = parent;

    this.layout = QHBoxLayout( this );

    this.edit = QLineEdit( this );

    this.button = MyPushButton( this );
    this.button.text = "Remove Me";
    connect( this.button.clicked, this.removeMe );

    this.layout.addWidget( this.edit );
    this.layout.addWidget( this.button );
  }
};

class ArgsWidget : QWidget
{
  doCall( in this )
  {
    app = this.applst.currentText;
    obj = this.objedit.text;
    func = this.funcslstbox.currentText;
    lst = this.argsList.internal;
    if ( lst.length() > 0 )
    {
      dcoparglst = [ "" ];
      for ( i = 1; i < lst.length(); i++ )
      {
        dcoparglst[ i-1 ] = lst[ i ].edit.text;
      }
      dcopCall( app, obj, func, dcoparglst );
    }
    else
    {
      dcopCall( app, obj, func );
    }
  }
  removeArg( in this, in arg )
  {
    println( "removearg" );
    this.argsList.remove( arg );
    println( this.argsList.count() );
  }
  addArg( in this )
  {
    newArg = Arg( this );
    this.layout.addWidget( newArg );
    newArg.show();
    this.argsList.append( newArg );
  }
  ArgsWidget( in this, in parentWidget, in applst, in objedit, in funcslstbox )
  {
    this.QWidget( parentWidget );
    this.applst = applst;
    this.objedit = objedit;
    this.funcslstbox = funcslstbox;

    this.layout = QVBoxLayout( this );

    this.addButton = MyPushButton( this );
    this.addButton.text = "Add Argument";
    connect( this.addButton.clicked, this.addArg );

    this.layout.addWidget( this.addButton );

    this.argsList = List();
  }
};

main
{
  a = App();

  w = QWidget();
  w.caption = "KScript DCOP Console";

  layout = QVBoxLayout( w );

  l = QLabel( w );
  l.text = "(1) Select a DCOP application";
  layout.addWidget( l );

  applst = AppListBox( w );

  layout.addWidget( applst );

  updateAppListButton = QPushButton( w );
  updateAppListButton.text = "Update Application List";
  connect( updateAppListButton.clicked, applst.update );

  layout.addWidget( updateAppListButton );

  l = QLabel( w );
  l.text = "(2) Enter the name of a DCOP Object (TODO: extend DCOP to return a list of objects)";
  layout.addWidget( l );

  objectEditLayout = QHBoxLayout();

  objectEditLabel = QLabel( w );
  objectEditLabel.text = "Object :";

  objectEditLayout.addWidget( objectEditLabel );

  objectLineEdit = QLineEdit( w );

  objectEditLayout.addWidget( objectLineEdit );

  layout.addLayout( objectEditLayout );

  l = QLabel( w );
  l.text = "(3) Select a function of the DCOP object or enter the name manually";
  layout.addWidget( l );

  funcEditLayout = QHBoxLayout();

  funcEditLabel = QLabel( w );
  funcEditLabel.text = "Function :";
  funcEditLayout.addWidget( funcEditLabel );

  funcLineEdit = QLineEdit( w );
  funcEditLayout.addWidget( funcLineEdit );

  layout.addLayout( funcEditLayout );

  funclstbox = FunctionsListBox( w, applst, objectLineEdit );

  layout.addWidget( funclstbox );

  updateFuncLstButton = QPushButton( w );
  updateFuncLstButton.text = "Update Function List";
  
  layout.addWidget( updateFuncLstButton );  
  
  connect( updateFuncLstButton.clicked, funclstbox.update );

  connect( funclstbox.selected, funcLineEdit.setText );

  argswidget = ArgsWidget( w, applst, objectLineEdit, funclstbox );
  
  layout.addWidget( argswidget );

  callButton = QPushButton( w );
  callButton.text = "Call!";
  
  layout.addWidget( callButton );

  connect( callButton.clicked, argswidget.doCall );

  exitButton = QPushButton( w );
  exitButton.text = "Exit";
  connect( exitButton.clicked, a.quit );

  layout.addWidget( exitButton );

  w.show();
  a.exec();
}
