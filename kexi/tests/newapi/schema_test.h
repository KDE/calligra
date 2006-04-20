#ifndef SCHEMA_TEST_H
#define SCHEMA_TEST_H

int schemaTest()
{
	if (!conn->useDatabase( db_name )) {
		kDebug() << conn->errorMsg() << endl;
		return 1;
	}

	KexiDB::TableSchema *t = conn->tableSchema( "persons" );
	if (t)
		t->debug();
	else
		kDebug() << "!persons" << endl;
	t = conn->tableSchema( "cars" );
	if (t)
		t->debug();
	else
		kDebug() << "!cars" << endl;
/*
// some tests	
	{
		KexiDB::Field::ListIterator iter = t->fieldsIterator();
		KexiDB::Field::List *lst = t->fields();
		lst->clear();
		for (;iter.current();++iter) {
			kDebug() << "FIELD=" << iter.current()->name() << endl;
//			iter.current()->setName("   ");
		}
	}*/
	return 0;
}

#endif

