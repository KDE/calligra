class MyClass
{
	MyClass( in this )
	{
		println("Constructor");
		this.a = 100;
	}
	println( in this )
	{
		println("Output", this.a );
	}
	delete( in this )
	{
		println("MyClass destructor");
	}
	const answer = 42;
};

main
{
	println("Hallo Welt");
	m = MyClass();
	println("Hallo Welt 2");	
	m.print();
	println("Hallo Welt 3", m.a );
	MyClass().print();
	println( MyClass() );
}