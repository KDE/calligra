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
	MyClass( in this, in text )
	{
		this.text = text;
		print("Constructor", this.text );
	}

	delete( in this )
	{
		print("Destructor", this.text );
	}
};

func1()
{
	m = MyClass( "IN_FUNC_1" );
	func2();
}

func2()
{
	m = MyClass( "IN_FUNC_2" );

	print( "---- exceptions -----" );
	m = MyClass( "IN_EXCEPTION" );
	err = MyError2();
	err.text = "Ein Fehler :-)";
//	raise "Hallo", err;
	raise MyError, err;
//	raise MyError2, err;
	print( "---- No no no -----" );
}

func3()
{
	// Testing stack unwinding
	m = MyClass( "IN_FUNC_3" );
	{
		m = MyClass( "IN_FUNC_3 B" );	
		return;
		print( "1 ---- No no no -----" );
	}
	print( "2 ---- No no no -----" );
}

main
{
	print("---------- start -------" );
	try
	{
		func1();
	}
	catch( MyError, exc )
	{
		print( "------ catched MyError -------" );
		print( exc );
		return;
	}
	default( type, exc )
	{
		print( "------ catched default ------", type, exc );
		return;
	}

	func3();

	print( "---- end -----" );
}
