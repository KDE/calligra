int parserTest(const char *st)
{
	int r = 0;

	if (!conn->useDatabase( db_name )) {
		conn->debugError();
		return 1;
	}
	
	KexiDB::Parser parser(conn);

	const bool ok = parser.parse(QString::fromLocal8Bit( st ));
	KexiDB::QuerySchema *q = parser.query();
	if (ok && q) {
		cout << q->debugString().latin1() << '\n';
		cout << "STATEMENT:\n" << conn->selectStatement( *q ).latin1() << '\n';
	}
	else {
		KexiDB::ParserError	err = parser.error();
		kdDebug() << QString("Error = %1\ntype = %2\nat = %3").arg(err.error())
			.arg(err.type()).arg(err.at()) << endl;
		r = 1;
	}
	delete q;
	q=0;

	
	if (!conn->closeDatabase()) {
		conn->debugError();
		return 1;
	}
	
	return r;
}

