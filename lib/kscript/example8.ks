import module1;

func( in a )
{
	println( a );
}

class my
{
	my( in this )
	{
		this.name = "Sorg";
		this.call = "Claudia";
	}

	println( in this )
	{
		println("-----------------");
		println( this );
		println("-----------------");
	}
};

class my2 : my
{
	my2( in this )
	{
		this.my();
		this.title = "Student";
	}

	println( in this )
	{
		println("=================");
		println( this );
		println("=================");
	}
};

func2( in this )
{
	println("*****************");
	println( this );
	println("*****************");
}

func3()
{
	println("*****************");
	y = module1.my();
	println("*****************");
	y.func();
}

main
{
	println("Hello");
	a = 100;
	func( "Torben" );
	println( a );
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
