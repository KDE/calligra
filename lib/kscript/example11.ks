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
		println("Constructor", this.text );
	}

	delete( in this )
	{
		println("Destructor", this.text );
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

	println( "---- exceptions -----" );
	m = MyClass( "IN_EXCEPTION" );
	err = MyError2();
	err.text = "Ein Fehler :-)";
//	raise "Hallo", err;
	raise MyError, err;
//	raise MyError2, err;
	println( "---- No no no -----" );
}

func3()
{
	// Testing stack unwinding
	m = MyClass( "IN_FUNC_3" );
	{
		m = MyClass( "IN_FUNC_3 B" );	
		return;
		println( "1 ---- No no no -----" );
	}
	println( "2 ---- No no no -----" );
}

main
{
	println("---------- start -------" );
	try
	{
		func1();
	}
	catch( MyError, exc )
	{
		println( "------ catched MyError -------" );
		println( exc );
		return;
	}
	default( type, exc )
	{
		println( "------ catched default ------", type, exc );
		return;
	}

	func3();

	println( "---- end -----" );
}
