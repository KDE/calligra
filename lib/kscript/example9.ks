struct Struct
{
	var a,b,c;
	func( inout this )
	{
		println( this.a, this.b, this.c );
		this.a = 333;
	}
	fun()
	{
		println("FUN");
	}
};

class my2
{
	func( inout this )
	{
		println("Hello");
	}	
};

class my
{
	func( inout this )
	{
		println("Hello stupid!");
		this = my2();
	}
};
	
class super
{
	super( in this )
	{
		this.a = 555;
	}

	over( in this )
	{
		println( this.a, "Super");
	}
};

class derived : super
{
	derived( in this )
	{
		this.super();
	}

	over( in this )
	{
		println( this.a, "Derived" );
		super.over( this );
	}

	const x = "Torben";
};

main
{
	m = my();
	m.func();
	m.func();
	println("-------------1");
	s = Struct();
	println("-------------2");
	s.a = 100;
	println( s.a, "-------------3");
	s.b = 200;
	println( s.a, "-------------4");
	s.c = 300;
	println( s.func, "-------------5");
	println( s );
	println("-------------6");
	s.func();
	println( s.a, "-------------7");
	x = s;
	x.a = 400;
	println( s, x );
	x.a = x.b = x.c = 111;
	println( x );
	m.call = x.func;
	m.call();
	Struct.fun();
	d = derived();
	d.over();
	println( derived.x );
}
