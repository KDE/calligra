pot( in x, inout o )
{
	println( o );
	o = x * 2;
}

pot2( in x, inout o )
{
	println( o );
	o = x * 2;
}

mul( in x, in y = 10 )
{
	x = 4;
	a = 7;
	println( x, y, x*y );
	return 5*a;
}

main
{
	println("Hallo Welt!" );
	mul( 5, 6 );
	a = 5;
	x = mul( a, 3*3 );
	b = 8;
	println( a, b, a*b );

	a = 1;
	while( a != 16 )
	{
		println( a );
		a = a * 2;
	}
	println( "Ende", a );
	mul( 8 );
	o = 0;
	pot( 23, o );
	println( "out:", o );
	pot2( 27, o );
	println( "inout:", o );
}