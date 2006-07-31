#ifndef PARSER_TEST_H
#define PARSER_TEST_H

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
		cout << q->debugString().toLatin1().constData() << '\n';
		cout << "STATEMENT:\n" << conn->selectStatement( *q ).toLatin1().constData() << '\n';
	}
	else {
		KexiDB::ParserError	err = parser.error();
		kDebug() << QString("Error = %1\ntype = %2\nat = %3").arg(err.error())
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

#endif

