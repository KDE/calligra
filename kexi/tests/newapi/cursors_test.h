#ifndef CURSORS_TEST_H
#define CURSORS_TEST_H

int tablesTest();

int cursorsTest()
{
	if (!conn->databaseExists( db_name )) {
		if (tablesTest()!=0)
			return 1;
		kDebug() << "DB created & filled"<< endl;
	}

	if (!conn->useDatabase( db_name )) {
		conn->debugError();
		return 1;
	}
	
	KexiDB::Cursor *cursor = conn->executeQuery( "select * from persons", cursor_options );//KexiDB::Cursor::Buffered );
	kDebug()<<"executeQuery() = "<<!!cursor<<endl;
	if (!cursor)
		return 1;

	kDebug()<<"Cursor::moveLast() ---------------------" << endl;
	kDebug()<<"-- Cursor::moveLast() == " << cursor->moveLast() << endl;
	cursor->moveLast();
	kDebug()<<"Cursor::moveFirst() ---------------------" << endl;
	kDebug()<<"-- Cursor::moveFirst() == " << cursor->moveFirst() << endl;

/*		kDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
	kDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
	kDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
	kDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
	kDebug()<<"Cursor::eof() == "<<cursor->eof()<<endl;*/
	conn->deleteCursor(cursor);

	return 0;
}

#endif

