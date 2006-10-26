/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef PARSER_TEST_H
#define PARSER_TEST_H

int parserTest(const char *st, const QStringList& params)
{
	int r = 0;

	if (!conn->useDatabase( db_name )) {
		conn->debugError();
		return 1;
	}
	
	KexiDB::Parser parser(conn);

	const bool ok = parser.parse(QString::fromLocal8Bit( st ));
	KexiDB::QuerySchema *q = parser.query();
	QValueList<QVariant> variantParams;
	foreach( QStringList::ConstIterator, it, params )
		variantParams.append(*it);
	if (ok && q) {
		cout << q->debugString().latin1() << '\n';
		cout << "-STATEMENT:\n" << conn->selectStatement( *q, variantParams ).latin1() << '\n';
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

#endif

