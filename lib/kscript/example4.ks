class MyClass
{
	MyClass( in this )
	{
		print("Constructor");
		this.a = 100;
	}
	print( in this )
	{
		print("Output", this.a );
	}
	delete( in this )
	{
		print("MyClass destructor");
	}
	const answer = 42;
};

main
{
	print("Hallo Welt");
	m = MyClass();
	print("Hallo Welt 2");	
	m.print();
	print("Hallo Welt 3", m.a );
	MyClass().print();
	print( MyClass() );
}