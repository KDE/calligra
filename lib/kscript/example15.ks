struct QRect
{
	var x;
	var y;
	var width;
	var height;
};

main
{
	print("-------1");
	doc = findApplication( "kspread", "Document" );
	print("-------2");
	map = doc.map();
	//	count = app.documentCount();
	// 	print( count );
	// 	doc = app.document( 0 );
	print("-------3");
	tcount = map.tableCount();
	print( tcount );
	table = map.table( 0 );
	print("--------4");
	name = table.name();
	print( name );
	print( map.tableNames() );
	r = QRect();
	r.x = 2;
	r.y = 2;
	r.width = 3;
	r.height = 4;
	table.setSelection( r );
	cell = table.cell( 1, 1 );
	cell.setText( "Hallo" );

	for( y = 2; y < 10; ++y )
	{
		cell = table.cell( 1, y );
		cell.setValue( y );
	}

	map.table( "Table1" ).cell("B1").setText( "Linux" );
	map.Table1().cell("B2").setText( "Torben" );
	map.Table1().B3().setText( "KDE" );
	map.Table1.B4.setText( "KOffice");
	t = map.insertTable( "NewTable" );
	t.A1().setText( "Wow" );
}
