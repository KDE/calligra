int tablesTest()
{
	if (dbCreationTest()!=0)
		return 1;

	if (!conn->useDatabase( db_name )) {
		conn->debugError();
		return 1;
	}

	conn->setAutoCommit(false);
	KexiDB::Transaction t = conn->beginTransaction();
	if (conn->error()) {
		conn->debugError();
		return 1;
	}

	//now: lets create tables:
	KexiDB::Field *f;
	KexiDB::TableSchema *t_persons = new KexiDB::TableSchema("persons");
	t_persons->setCaption("Persons in our factory");
	t_persons->addField( f=new KexiDB::Field("id", KexiDB::Field::Integer, KexiDB::Field::PrimaryKey | KexiDB::Field::AutoInc, KexiDB::Field::Unsigned) );
	f->setCaption("ID");
	t_persons->addField( f=new KexiDB::Field("age", KexiDB::Field::Integer, 0, KexiDB::Field::Unsigned) );
	f->setCaption("Age");
	t_persons->addField( f=new KexiDB::Field("name", KexiDB::Field::Text) );
	f->setCaption("Name");
	t_persons->addField( f=new KexiDB::Field("surname", KexiDB::Field::Text) );
	f->setCaption("Surname");
	if (!conn->createTable( t_persons )) {
		conn->debugError();
		return 1;
	}
	kdDebug() << "-- PERSONS created --" << endl;
	t_persons->debug();

	if (!conn->insertRecord(*t_persons, QVariant(1), QVariant(27), QVariant("Jaroslaw"), QVariant("Staniek"))
	  ||!conn->insertRecord(*t_persons, QVariant(2), QVariant(60), QVariant("Lech"), QVariant("Walesa"))
	  ||!conn->insertRecord(*t_persons, QVariant(3), QVariant(45), QVariant("Bill"), QVariant("Gates"))
	  ||!conn->insertRecord(*t_persons, QVariant(4), QVariant(35), QVariant("John"), QVariant("Smith"))
	   )
	{
		kdDebug() << "-- PERSONS data err. --" << endl;
		return 1;
	}
	kdDebug() << "-- PERSONS data created --" << endl;


	KexiDB::TableSchema *t_cars = new KexiDB::TableSchema("cars");
	t_cars->setCaption("Cars owned by persons");
	t_cars->addField( f=new KexiDB::Field("id", KexiDB::Field::Integer, KexiDB::Field::PrimaryKey | KexiDB::Field::AutoInc, KexiDB::Field::Unsigned) );
	f->setCaption("ID");
	t_cars->addField( f=new KexiDB::Field("owner", KexiDB::Field::Integer, 0, KexiDB::Field::Unsigned) );
	f->setCaption("Car owner");
	t_cars->addField( f=new KexiDB::Field("model", KexiDB::Field::Text) );
	f->setCaption("Car model");
	if (!conn->createTable( t_cars )) {
		conn->debugError();
		return 1;
	}
	kdDebug() << "-- CARS created --" << endl;
	if (!conn->insertRecord(*t_cars, QVariant(1), QVariant(1), QVariant("Fiat"))
		||!conn->insertRecord(*t_cars, QVariant(2), QVariant(2), QVariant("Syrena"))
		||!conn->insertRecord(*t_cars, QVariant(3), QVariant(3), QVariant("Chrysler"))
		||!conn->insertRecord(*t_cars, QVariant(4), QVariant(3), QVariant("BMW"))
		||!conn->insertRecord(*t_cars, QVariant(5), QVariant(4), QVariant("Volvo"))
		)
	{
		kdDebug() << "-- CARS data err. --" << endl;
		return 1;
	}
	kdDebug() << "-- CARS data created --" << endl;

	if (!conn->commitTransaction(t)) {
		conn->debugError();
		return 1;
	}

	kdDebug() << "NOW, TABLE LIST: " << endl;
	QStringList tnames = conn->tableNames();
	for (QStringList::iterator it = tnames.begin(); it!=tnames.end(); ++it) {
		kdDebug() << " - " << (*it) << endl;
	}


	if (!conn->closeDatabase()) {
		conn->debugError();
		return 1;
	}
	
	return 0;
}

