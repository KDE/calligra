import module1;

func( in a )
{
	print( a );
}

class my
{
	my( in this )
	{
		this.name = "Sorg";
		this.call = "Claudia";
	}

	print( in this )
	{
		print("-----------------");
		print( this );
		print("-----------------");
	}
};

class my2 : my
{
	my2( in this )
	{
		this.my();
		this.title = "Student";
	}

	print( in this )
	{
		print("=================");
		print( this );
		print("=================");
	}
};

func2( in this )
{
	print("*****************");
	print( this );
	print("*****************");
}

func3()
{
	print("*****************");
	y = module1.my();
	print("*****************");
	y.func();
}

main
{
	print("Hello");
	a = 100;
	func( "Torben" );
	print( a );
	m = my();
	m.print();
	m2 = my2();
	m2.print();
	m2.x = func2;
	m2.x();
	func2( m2 );
	module1.func();
	func3();
}
