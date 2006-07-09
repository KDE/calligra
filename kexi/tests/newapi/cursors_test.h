int tablesTest();

int cursorsTest()
{
	if (!conn->databaseExists( db_name )) {
		if (tablesTest()!=0)
			return 1;
		kdDebug() << "DB created & filled"<< endl;
	}

	if (!conn->useDatabase( db_name )) {
		conn->debugError();
		return 1;
	}
	
	KexiDB::Cursor *cursor = conn->executeQuery( "select * from persons", cursor_options );//KexiDB::Cursor::Buffered );
	kdDebug()<<"executeQuery() = "<<!!cursor<<endl;
	if (!cursor)
		return 1;

	kdDebug()<<"Cursor::moveLast() ---------------------" << endl;
	kdDebug()<<"-- Cursor::moveLast() == " << cursor->moveLast() << endl;
	cursor->moveLast();
	kdDebug()<<"Cursor::moveFirst() ---------------------" << endl;
	kdDebug()<<"-- Cursor::moveFirst() == " << cursor->moveFirst() << endl;

/*		kdDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
	kdDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
	kdDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
	kdDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
	kdDebug()<<"Cursor::eof() == "<<cursor->eof()<<endl;*/
	conn->deleteCursor(cursor);

	return 0;
}

