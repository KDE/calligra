main( in file )
{
	if ( -r( file ) )
	{
		println("File %1 is readable".arg( file ) );
		println("Owner=%1".arg( -o( file ) ) );
		println("Group=%1".arg( -g( file ) ) );
	}
	else
	{
		println("File %1 is NOT readable".arg( file ) );
	}
	println( 10 / 2 / 5 );
}
