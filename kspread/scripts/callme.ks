main( in a, in b )
{
	print("Someone told me to call");
	print( a );
	print( b );

	doc = findApplication( a, b );
	table = doc.table();
	table.A1.setText("You called me");
}
