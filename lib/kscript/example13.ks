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
	bg = QButtonGroup( w );
	r1 = QRadioButton( bg );
	r1.text = "Radio Button";
	r2 = QRadioButton( bg );
	r2.text = "Television Button";
	r3 = QRadioButton( bg );
	r3.text = "Other";
	c = QCheckBox( w );
	c.text = "Check";
	c.checked = TRUE;
	combo = QComboBox( w );
	combo.insertItem("Schubidu");
	combo.insertItem("Schabada");
	combo.insertItem("Schabadai");
	la = QLabel( w );
	la.text = "<b>Hello</b> World";
	dlg = QDialog( w );
	rect = QRect();
	rect.left = 50;
	rect.top = 80;
	rect.right = 120;
	rect.bottom = 160;
	dlg.geometry = rect;
	layout = QVBoxLayout( w );
	layout.addWidget( l );
	layout.addWidget( p );
	layout.addWidget( bg );
	layout.addWidget( c );
	layout.addWidget( la );
	layout.addWidget( combo );
	w.show();
	w.caption = "KScript Test";
	print( w.caption );
	w.name = "Hello";
	print( w.name );
	connect( l.textChanged, p.setText );
	connect( combo.activated, p.setText );
	connect( p.clicked, m.clicked );
	dlg.show();
	a.exec();
}
