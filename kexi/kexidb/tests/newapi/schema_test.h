	conn_data.setFileName( "db" );

	conn = driver->createConnection(conn_data);
	if (driver->error()) {
		kdDebug() << driver->errorMsg() << endl;
		return 1;
	}
	if (!conn->connect()) {
		kdDebug() << conn->errorMsg() << endl;
		return 1;
	}
	if (!conn->useDatabase( "db" )) {
		kdDebug() << conn->errorMsg() << endl;
		return 1;
	}

	KexiDB::TableSchema *t = conn->tableSchema( "persons" );
	if (t)
		t->debug();
	else
		kdDebug() << "!persons" << endl;
	t = conn->tableSchema( "cars" );
	if (t)
		t->debug();
	else
		kdDebug() << "!cars" << endl;
/*
// some tests	
	{
		KexiDB::Field::ListIterator iter = t->fieldsIterator();
		KexiDB::Field::List *lst = t->fields();
		lst->clear();
		for (;iter.current();++iter) {
			kdDebug() << "FIELD=" << iter.current()->name() << endl;
//			iter.current()->setName("   ");
		}
	}*/
	
