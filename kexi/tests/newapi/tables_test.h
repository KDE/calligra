int tablesTest()
{
	if (dbCreationTest()!=0)
		return 1;

	if (!conn->useDatabase( /*default*/ )) {
		conn->debugError();
		return 1;
	}

	//now: lets create table:
	
	KexiDB::TableSchema *t_persons = new KexiDB::TableSchema("persons");
	t_persons->setCaption("Persons group in our factory");
	t_persons->addField( new KexiDB::Field("id", KexiDB::Field::Integer, KexiDB::Field::PrimaryKey | KexiDB::Field::AutoInc, KexiDB::Field::Unsigned) )
	.addField( new KexiDB::Field("age", KexiDB::Field::Integer, 0, KexiDB::Field::Unsigned) )
	.addField( new KexiDB::Field("name", KexiDB::Field::Text) )
	.addField( new KexiDB::Field("surname", KexiDB::Field::Text) );
	if (!conn->createTable( t_persons )) {
		conn->debugError();
		return 1;
	}
	kdDebug() << "-- PERSONS created --" << endl;
	
	KexiDB::TableSchema *t_cars = new KexiDB::TableSchema("cars");
	t_cars->setCaption("Cars owned by persons");
	t_cars->addField( new KexiDB::Field("owner", KexiDB::Field::Integer, 0, KexiDB::Field::Unsigned) )
	.addField( new KexiDB::Field("model", KexiDB::Field::Text) );
	if (!conn->createTable( t_cars )) {
		conn->debugError();
		return 1;
	}
	kdDebug() << "-- CARS created --" << endl;
	

	return 0;
}