#!/opt/kde2/bin/kscript

main
{
	print("Hallo");
	for( a = 0; a != 12; a = a+1 )
	{
		x = a * 2;
		print( a );
	}
	print("Ende",a);
	if ( a == 10 )
	{
		print("a==10");
	}
	else if ( a == 11 )
	{
		print("a==11");
	}
	else
	{
		print("a!=10");
	}
	do
	{
		a = a+1;
		print(a);
	} while( a != 17 );
	print("------- while --------");
	while( a != 20 )
	{
		a = a+1;
		print(a);
	}
	a = "Hallo";
	b = a[3];
	b = 'x';
	print( a, b );

	list = [ "Torben", "Claudia", "Lars", "Test" ];
	list[3] = "Arnt";
	foreach( y, list )
	{
		print(y);
		if ( y == "Claudia" )
		{
			y = ":-)";
		}
	}
	print("---------------------");
	foreach( y, list )
	{
		print(y);
	}
	print("---------------------");
	foreach( y, [ 100, 101, 102 ] )
	{
		print( y );
	}
	print("---------------------");
	map = { ( "Torben", 3600 ), ( "Matthias", 6000 ) };
	foreach( key, data, map )
	{
		print( key,data );
		if ( key == "Torben" )
		{
			data = 3700;
		}
	}
	print("---------------------");
	foreach( key, data, map )
	{
		print( key,data );
	}
	print( "Hallo".length() );
	print( list.length() );
	print( map.length() );
	print( 5.toFloat() + 0.3 );
	print( "100".toInt() + 8 );
	print( "100.2".toFloat() + 0.2 );
	if ( TRUE | FALSE )
	{
		print("Ok");
	}
	if ( FALSE | FALSE )
	{
		print("Not ok");
	}
	if ( TRUE & TRUE )
	{
		print("Ok");
	}
	if ( FALSE & TRUE )
	{
		print("Not ok");
	}
	if ( TRUE ^ FALSE )
	{
		print("Ok");
	}
	if ( TRUE ^ TRUE )
	{
		print("Not ok");
	}
	print( 256 << 1 );
	print( 256 >> 1 );
	print( 5-8, -9, -0.6, 12.0-3.5 );
	print( 8 / 3, 8.0 / 3.0, 8 % 5 );
	print( 8 <= 9 );
	print( 5.4 <= 3.2 );
	print( "Hallo" <= "Hello" );
	print( 8 >= 9 );
	print( 5.4 >= 3.2 );
	print( "Hallo" >= "Hello" );
	str = "Test";
	print( str[0] == 'T' );
	print( str[0] < 'X' );
	a = 500;
	print( ++a );
	print( a++ );
	print( a );
	x = a++;
	print( a,x);
	print("--------");
	print( a, ++a, a, a++, a );
	a--;
	print( a );
	print( !FALSE, !TRUE );
}
