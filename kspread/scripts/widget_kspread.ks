class MyClass
{
	MyClass(in this,in v,in w,in l )
	{
        	this.appli=v;
        	this.mainwindow=w;
        	this.line=l;
	}

	clicked( in this )
	{
 		doc = findApplication( this.appli, this.mainwindow );
        	table = doc.table();
        	table.B1.setText(this.line.text);
	}

};

main( in a, in b )
{

	z = QApplication();
	println( "---- Application ----" );
	w = QWidget();
	l = QLineEdit( w );
	l.text = "Change me";

        m = MyClass(a,b,l);

	p = QPushButton( w );
	p.text = "Apply";
	layout = QVBoxLayout( w );
	layout.addWidget( l );
	layout.addWidget( p );
	w.show();
	w.caption = "KSpread script Test";
	println( w.caption );


	connect ( p.clicked,m.clicked );
	z.exec();
}
