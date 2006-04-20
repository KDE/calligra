#ifndef DBCREATION_TEST_H 
#define DBCREATION_TEST_H 

int dbCreationTest()
{
	if (conn->databaseExists( db_name )) {
		if (!conn->dropDatabase( db_name )) {
			conn->debugError();
			return 1;
		}
		kDebug() << "DB '" << db_name << "' dropped"<< endl;
	}
	if (!conn->createDatabase( db_name )) {
		conn->debugError();
		return 1;
	}
	kDebug() << "DB '" << db_name << "' created"<< endl;
	if (!conn->useDatabase( db_name )) {
		conn->debugError();
		return 1;
	}
/*	KexiDB::Cursor *cursor = conn->executeQuery( "select * from osoby", KexiDB::Cursor::Buffered );
	kDebug()<<"executeQuery() = "<<!!cursor<<endl;
	if (cursor) {
		kDebug()<<"Cursor::moveLast() ---------------------" << endl;
		kDebug()<<"-- Cursor::moveLast() == " << cursor->moveLast() << endl;
		cursor->moveLast();
		kDebug()<<"Cursor::moveFirst() ---------------------" << endl;
		kDebug()<<"-- Cursor::moveFirst() == " << cursor->moveFirst() << endl;
*/
/*		kDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
		kDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
		kDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
		kDebug()<<"Cursor::moveNext() == "<<cursor->moveNext()<<endl;
		kDebug()<<"Cursor::eof() == "<<cursor->eof()<<endl;*/
//		conn->deleteCursor(cursor);
//	}
	return 0;
}

#endif

