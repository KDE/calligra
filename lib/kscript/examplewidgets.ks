class MyClass
{
	MyClass( in this )
	{
	}
};

main
{
	r = QRect();
	r.left = 10;
	r.right = 80;
	r.top = 5;
	r.bottom = 10;
	a = QApplication();
	w = QRadioButton();
	w2 = QPushButton();
	w3 = QLabel();
	w4 = QCheckBox();
	w.geometry = r;
	w.text = "KScript Test";
	print( w.text );
	w.show();
	w2.show();
	w3.show();
	w4.show();
//	w.text = "KScript Test";
//	w.geometry = r;
	a.exec();
}
