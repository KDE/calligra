main
{
  konq = findApplication( "konqueror" );
  win = konq.getWindows()[ 0 ];
  println( win );
  actionmap = win.actionMap();

  print( actionmap );
  println( "Hallo?" );
  actionmap{ "open_location" }.setText( "hiho" );
  actionmap{ "print" }.setIcon( "reload" );

  win.action( "about_kde" ).activate();

  locationbartoggle = win.action( "showlocationbar" );
  println( locationbartoggle.plainText() );
  locationbartoggle.setChecked( FALSE );
  println( konq.functions() );
  println( locationbartoggle.functions() );
  println( locationbartoggle.propertyNames( TRUE ) );
  win.action( "reload" ).setProperty( "text", "heyho" );
}

