int cursorsTest()
{
	if (!conn->databaseExists( "mydb" )) {
		if (!conn->createDatabase( "mydb" )) {
			conn->debugError();
			return 1;
		}
		kdDebug() << "DB created"<< endl;
	}
	if (!conn->useDatabase( "mydb" )) {
		conn->debugError();
		return 1;
	}
	KexiDB::Cursor *cursor = conn->executeQuery( "select * from osoby", KexiDB::Cursor::Buffered );
	kdDebug()<<"executeQuery() = "<<!!cursor<<endl;
	if (cursor) {
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
	}
	return 0;
}
