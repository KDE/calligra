	conn_data.setFileName( "db" );

	KexiDB::Connection *conn = driver->createConnection(conn_data);
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

	KexiDB::Table *t = conn->tableSchema( "persons" );
	if (t)
		t->debug();
	t = conn->tableSchema( "cars" );
	if (t)
		t->debug();

