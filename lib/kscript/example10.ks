struct MyError
{
	var text;
};

struct MyError2
{
	var text;
};

class MyClass
{
	MyClass( in this )
	{
		print("Constructor");
	}

	delete( in this )
	{
		print("Destructor");
	}
};

main
{
	print( "---- start -----" );
	{
		print( "1---");
		m = MyClass();
		print( "2---");
	}
	print( "---- exceptions -----" );
	try
	{
		err = MyError();
		err.text = "Ein Fehler :-)";
		raise MyError, err;
	}
	catch( MyError, exc )
	{
		print( "------ catched MyError -------" );
		print( exc );
	}
	default( type, exc )
	{
		print( "------ catched default -------", type );
		print( exc );
	}
	print( "---- end -----" );
}
