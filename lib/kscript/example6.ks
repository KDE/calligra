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
};

main
{
	m = MyWidget();
	r = MyWidget();
	a = MyWidget();
}