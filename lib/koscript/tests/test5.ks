// Test 5 - Some primitive constructs

main
{
	println("Hallo");
	for( a = 0; a != 12; a = a+1 )
	{
		x = a * 2;
		print(a, " - ", x, "\n");
	}
	print("Ende: ",a,"\n");
	if ( a == 10 )
	{
		println("a==10");
	}
	else if ( a == 11 )
	{
		println("a==11");
	}
	else
	{
		println("a!=10 && a!=11");
	}
	do
	{
		a = a+1;
		println(a);
	} while( a != 17 );
	println("------- while --------");
	while( a != 20 )
	{
		a = a+1;
		println(a);
	}
	a = "Hallo";
	b = a[3];
	c = 'x';
	println( a, b, c );

	list = [ "Torben", "Claudia", "Lars", "Test" ];
	list[3] = "Arnt";
	foreach( y, list )
	{
		println(y);
		if ( y == "Claudia" )
		{
			y = ":-)";
		}
	}
	println("---------------------");
	foreach( y, list )
	{
		println(y);
	}
	println("---------------------");
	foreach( y, [ 100, 101, 102 ] )
	{
		println( y );
	}
	println("---------------------");
	map = { ( "Torben", 3600 ), ( "Matthias", 6000 ) };
	foreach( key, data, map )
	{
		println( key,data );
		if ( key == "Torben" )
		{
			data = 3700;
		}
	}
	println("---------------------");
	foreach( key, data, map )
	{
		println( key,data );
	}
	println("---------------------");
	println( "Hallo".length() );
	println( list.length() );
	println( map.length() );
	println( 5.toFloat() + 0.3 );
	println( "100".toInt() + 8 );
	println( "100.2".toFloat() + 0.2 );
	if ( TRUE || FALSE )
	{
		println("Ok");
	}
	if ( FALSE || FALSE )
	{
		println("Not ok");
	}
	if ( TRUE && TRUE )
	{
		println("Ok");
	}
	if ( FALSE && TRUE )
	{
		println("Not ok");
	}
	println( 256 << 1 );
	println( 256 >> 1 );
	println( 5-8, -9, -0.6, 12.0-3.5 );
	println( 8 / 3, 8.0 / 3.0, 8 % 5 );
	println( 8 <= 9 );
	println( 5.4 <= 3.2 );
	println( "Hallo" <= "Hello" );
	println( 8 >= 9 );
	println( 5.4 >= 3.2 );
	println( "Hallo" >= "Hello" );
	str = "Test";
	println( str[0] == 'T' );
	println( str[0] < 'X' );
	a = 500;
	println( ++a );
	println( a++ );
	println( a );
	x = a++;
	println( a, x );
	println("--------");
	println( a, ++a, a, a++, a );  // Not sure how to "fix" that
	a--;
	println( a );
	println( !FALSE, !TRUE );
	println( 8/3, 8.0/3, 8/3.0, 8.0/3.0 );
	println( 8*3, 8.1*3, 8*3.1, 8.1*3.1 );
	println( 8+3, 8.1+3, 8+3.1, 8.1+3.1 );
	println( 8-3, 8.1-3, 8-3.1, 8.1-3.1 );
}
