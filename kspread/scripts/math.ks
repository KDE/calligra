import kspread;

mul( in a, in b )
{
	return a*b;
}

main
{
	print("!!!!!!! math.ks !!!!!!!!!!");
	kspread.mul = mul;
	print( kspread.mul( 3, 4 ) );
	print("!!!!!!! exported");
}
