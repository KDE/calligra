main
{
	println("-------1");
	app = findApplication( "kspread", "Application" );
	println("-------2");
	docs = app.documents();
	println( docs );
}
