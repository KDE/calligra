main
{
	a = QApplication();
	println( "---- Application ----" );
	x = QWidget();
	x.show();
	println( "--------" );
	y = QLabel( x );
	println( "--------" );
	y = 0;
	println( "---- Visible ----" );
	x.destroy();
	println( "---- Destroy ----" );
	x = 0;
	a.exec();
}
