const hallo = "Hallo Torben!";

main
{
	b = a = 5;
	b = 6;
	c = a * b;
	d = [ 100, 200, 300 ];
	e = { ( "Weis", 8 ), ( "Linzbach", 10 ) };
	x = d[ 1 ];
	d[ 5 ] = "Index";
	n = e{ "Weis" };
	m = e{ "Sorg" };
	e{ "Weis" } = 9;
	e{ "Sorg" } = "Claudia";
	f = 'A';
	name = "Torben";
	g = name[ 2 ];
	name[0] = 't';
	print("Hallo Welt!", "Wie gehst es Dir", 42, e );
	print( hallo );
}
