class MyClass
{
	MyClass( in this )
	{
	}
};

main
{
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
	r2.top = 5;
	r2.bottom = 100;

	print( r.unite( r2 ).width );
	print( "------ Ende ------" );

	a = QApplication();
	w = QWidget();
	w.show();
	w.caption = "KScript Test";
	print( w.caption );
	w.name = "Hello";
	print( w.name );
	print( w.x, w.y, w.width, w.height );
	print( w.geometry.width );
	w.geometry = r2;
	print( w.geometry.width );
	a.exec();

}
