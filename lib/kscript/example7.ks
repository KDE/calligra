class MyClass
{
	MyClass( in this )
	{
	}

	clicked( in this )
	{
		println( "Yeah, click me, faster, harder ....." );
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
	println( r.width );
	println( r.intersects( r ) );

	r2 = QRect();
	r2.left = 60;
	r2.right = 180;
	r2.top = 25;
	r2.bottom = 300;

	println( r.unite( r2 ).width );
	println( "------ Ende ------" );

	a = QApplication();
	println( "---- Application ----" );
	w = QWidget();
	l = QLineEdit( w );
	l.text = "Edit me";
	p = QPushButton( w );
	p.text = "PushMe";
	w.show();
	w.caption = "KScript Test";
	println( w.caption );
	w.name = "Hello";
	println( w.name );
	println( w.x, w.y, w.width, w.height );
	println( w.geometry.width );
	w.geometry = r2;
	r3 = QRect();
	r3.left = 10;
	r3.top = 30;
	r3.bottom = 60;
	r3.right = 120;
	p.geometry = r3;
	println( w.geometry.width );
	connect( l.textChanged, p.setText );
	connect( p.clicked, m.clicked );
	a.exec();

}
