class MyWidget
{
	MyWidget( in this )
	{
		print("MyWidget");
	}

	delete( in this )
	{
		print("Destruct MyWidget");
	}

	signal hello( in str );
};

class Receiver
{
	Receiver( in this )
	{
		print("Receiver");
	}

	talk( in this, in str )
	{
		print( str );
	}

	delete( in this )
	{
		print("Destruct Receiver");
	}
};

class Derived : Receiver, MyWidget
{
	delete( in this )
	{
		print("Destruct Derived");
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
	print("Hallo Welt");
	m = MyWidget();
	testme( m );
	print("---------------");
	r = Receiver();
	connect( m.hello, r.talk );
	print("Emit");
	m.hello( "Torben" );
	m.disconnect( "hello", r.talk );
	print("---------------");
	d = Derived();
	d.talk("Ok");
	print( d.isA() );
	print( d.inherits( "Derived" ) );
	print( d.inherits( "MyWidget" ) );
	print( d.inherits( "Receiver" ) );
	print( d.inherits( "Dummy" ) );
	print("Bye");
}
