main
{
	r = QRect();
	r.left = 10;
	r.right = 180;
	r.top = 50;
	r.bottom = 110;
	a = QApplication();
	w = QRadioButton();
	w.checked = TRUE;
	w2 = QPushButton();
	w2.text = "Hallo";
	w3 = QLabel();
	w3.text = "Hallo <a href=\"\">Torben</a>";
	w3.geometry = r;
	w4 = QCheckBox();
	w4.checked = TRUE;
	w5 = QFrame();
	r.top = 500;
	r.bottom = 550;
	w5.geometry = r;
	w5.frameshape = "Panel";
	w5.frameshadow = "Sunken";
	c1 = QPushButton( w5 );
	c1.text = "Click 1";
	c2 = QPushButton( w5 );
	c2.text = "Click 2";
	l1 = QVBoxLayout( w5, 6, 6, "xx" );
	w.show();
	w2.show();
	w3.show();
	w4.show();
	w5.show();
	a.exec();
}
