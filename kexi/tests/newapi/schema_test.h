int schemaTest()
{
	if (!conn->useDatabase( db_name )) {
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
	return 0;
}

