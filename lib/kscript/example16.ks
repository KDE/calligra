main
{
	a = QApplication();
	print( "---- Application ----" );
	x = QWidget();
	x.show();
	print( "--------" );
	y = QLabel( x );
	print( "--------" );
	y = 0;
	print( "---- Visible ----" );
	x.destroy();
	print( "---- Destroy ----" );
	x = 0;
	a.exec();
}
