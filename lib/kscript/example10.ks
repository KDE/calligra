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
		println("Constructor");
	}

	delete( in this )
	{
		println("Destructor");
	}
};

main
{
	println( "---- start -----" );
	{
		println( "1---");
		m = MyClass();
		println( "2---");
	}
	println( "---- exceptions -----" );
	try
	{
		err = MyError();
		err.text = "Ein Fehler :-)";
		raise MyError, err;
	}
	catch( MyError, exc )
	{
		println( "------ catched MyError -------" );
		println( exc );
	}
	default( type, exc )
	{
		println( "------ catched default -------", type );
		println( exc );
	}
	println( "---- end -----" );
}
