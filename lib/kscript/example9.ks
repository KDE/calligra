struct Struct
{
	var a,b,c;
	func( inout this )
	{
		print( this.a, this.b, this.c );
		this.a = 333;
	}
	fun()
	{
		print("FUN");
	}
};

class my2
{
	func( inout this )
	{
		print("Hello");
	}	
};

class my
{
	func( inout this )
	{
		print("Hello stupid!");
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
		print( this.a, "Super");
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
		print( this.a, "Derived" );
		super.over( this );
	}

	const x = "Torben";
};

main
{
	m = my();
	m.func();
	m.func();
	print("-------------1");
	s = Struct();
	print("-------------2");
	s.a = 100;
	print( s.a, "-------------3");
	s.b = 200;
	print( s.a, "-------------4");
	s.c = 300;
	print( s.func, "-------------5");
	print( s );
	print("-------------6");
	s.func();
	print( s.a, "-------------7");
	x = s;
	x.a = 400;
	print( s, x );
	x.a = x.b = x.c = 111;
	print( x );
	m.call = x.func;
	m.call();
	Struct.fun();
	d = derived();
	d.over();
	print( derived.x );
}
