class MyClass
{
	MyClass( in this )
	{
	}

	clicked( in this )
	{
		print( "Yeah, click me, faster, harder ....." );
	}
};

main
{
	m = MyClass();

	a = QApplication();
	print( "---- Application ----" );
	w = QWidget();
	l = QLineEdit( w );
	l.text = "Edit me";
	p = QPushButton( w );
	p.text = "PushMe";
	r = QRadioButton( w );
	r.text = "Radio";
	c = QCheckBox( w );
	c.text = "Check";
	c.checked = TRUE;
	la = QLabel( w );
	la.text = "<b>Hello</b> World";
	layout = QVBoxLayout( w );
	layout.addWidget( l );
	layout.addWidget( p );
	layout.addWidget( r );
	layout.addWidget( c );
	layout.addWidget( la );
	w.show();
	w.caption = "KScript Test";
	print( w.caption );
	w.name = "Hello";
	print( w.name );
	connect( l.textChanged, p.setText );
	connect( p.clicked, m.clicked );
	a.exec();

}
