class MyClass
{
	MyClass( in this )
	{
	}

	clicked( in this )
	{
		print( "Yeah, click me, faster, harder ....." );
	}
};

main
{
	m = MyClass();

	r = QRect();
	r.left = 10;
	r.right = 80;
	r.top = 5;
	r.bottom = 10;
	print( r.width );
	print( r.intersects( r ) );

	r2 = QRect();
	r2.left = 60;
	r2.right = 180;
	r2.top = 25;
	r2.bottom = 300;

	print( r.unite( r2 ).width );
	print( "------ Ende ------" );

	a = QApplication();
	print( "---- Application ----" );
	w = QWidget();
	l = QLineEdit( w );
	l.text = "Edit me";
	p = QPushButton( w );
	p.text = "PushMe";
	w.show();
	w.caption = "KScript Test";
	print( w.caption );
	w.name = "Hello";
	print( w.name );
	print( w.x, w.y, w.width, w.height );
	print( w.geometry.width );
	w.geometry = r2;
	r3 = QRect();
	r3.left = 10;
	r3.top = 30;
	r3.bottom = 60;
	r3.right = 120;
	p.geometry = r3;
	print( w.geometry.width );
	connect( l.textChanged, p.setText );
	connect( p.clicked, m.clicked );
	a.exec();

}
