class MyWidget
{
	MyWidget( in this )
	{
		println("MyWidget");
	}

	delete( in this )
	{
		println("Destruct MyWidget");
	}

	signal hello( in str );
};

class Receiver
{
	Receiver( in this )
	{
		println("Receiver");
	}

	talk( in this, in str )
	{
		println( str );
	}

	delete( in this )
	{
		println("Destruct Receiver");
	}
};

class Derived : Receiver, MyWidget
{
	delete( in this )
	{
		println("Destruct Derived");
	}
};

testme( in sender )
{
	x = Receiver();
	connect( sender.hello, x.talk );
	emit sender.hello( "YoYo");
	sender.disconnect( "hello", x.talk );
	emit sender.hello( "YoYo 2");
}

main
{
	println("Hallo Welt");
	m = MyWidget();
	testme( m );
	println("---------------");
	r = Receiver();
	connect( m.hello, r.talk );
	println("Emit");
	m.hello( "Torben" );
	m.disconnect( "hello", r.talk );
	println("---------------");
	d = Derived();
	d.talk("Ok");
	println( d.isA() );
	println( d.inherits( "Derived" ) );
	println( d.inherits( "MyWidget" ) );
	println( d.inherits( "Receiver" ) );
	println( d.inherits( "Dummy" ) );
	println("Bye");
}
