main
{
	print("1------");
	r = QRect();
	print("2------");
	r.left = 100;
	r.right = 156;
	r.top = 20;
	r.bottom = 40;
	print( r.width, r.height );
	print("3------");
	print( r.left );
	print("4------");
	r2 = r;
	print("5------");
	r2.left = 55;
	print("6------");
	print( r.left, r2.left );
	print("7------");
	r3 = QRect();
	print( r.isNull(), r3.isNull() );
	print("8------");
	r2.left = 10;
	r2.right = 256;
	r2.top = 10;
	r2.bottom = 50;	
	print( r.contains( r2 ), r2.contains( r ) );
	print("9------");
	r2.left = 300;
	print("9a------");
	print( r2.normalize() );
	print("9b------");
	r4 = r2.normalize();
	print("9c------");
	print( r2 );
	print("10------");
	print( r4 );
	print( r4.left, r4.right );
}
