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
}
