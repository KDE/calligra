	if (argc<=3) {
		kdDebug() << prgname << ": name for new db?" << endl;
		usage();
		return 0;
	}
	QCString db_name = QCString(argv[3]);
	conn_data.setFileName( db_name );

	conn = driver->createConnection(conn_data);
	if (driver->error()) {
		driver->debugError();
		return 1;
	}
	if (!conn->connect()) {
		conn->debugError();
		return 1;
	}
	if (conn->databaseExists( db_name )) {
		if (!conn->dropDatabase(  )) {
			conn->debugError();
			return 1;
		}
		kdDebug() << "DB '" << db_name << "' dropped"<< endl;
	}
	if (!conn->createDatabase( db_name )) {
		conn->debugError();
		return 1;
	}
	kdDebug() << "DB '" << db_name << "' created"<< endl;
	if (!conn->useDatabase( /*default*/ )) {
		conn->debugError();
		return 1;
	}
/*	KexiDB::Cursor *cursor = conn->executeQuery( "select * from osoby", KexiDB::Cursor::Buffered );
	kdDebug()<<"executeQuery() = "<<!!cursor<<endl;
	if (cursor) {
		kdDebug()<<"Cursor::moveLast() ---------------------" << endl;
		kdDebug()<<"-- Cursor::moveLast() == " << cursor->moveLast() << endl;
		cursor->moveLast();
		kdDebug()<<"Cursor::moveFirst() ---------------------" << endl;
		kdDebug()<<"-- Cursor::moveFirst() == " << cursor->moveFirst() << endl;
*/
/*		kdDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
		kdDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
		kdDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
		kdDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
		kdDebug()<<"Cursor::eof() == "<<cursor->eof()<<endl;*/
//		conn->deleteCursor(cursor);
//	}
