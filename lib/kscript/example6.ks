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
};

main
{
	m = MyWidget();
	r = MyWidget();
	a = MyWidget();
}