main
{
	print("-------1");
	app = findApplication( "kspread", "Application" );
	print("-------2");
	docs = app.documents();
	print( docs );
}
