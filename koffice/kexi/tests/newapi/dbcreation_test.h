#ifndef DBCREATION_TEST_H 
#define DBCREATION_TEST_H 

int dbCreationTest()
{
	if (conn->databaseExists( db_name )) {
		if (!conn->dropDatabase( db_name )) {
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
	if (!conn->useDatabase( db_name )) {
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
	return 0;
}

#endif

