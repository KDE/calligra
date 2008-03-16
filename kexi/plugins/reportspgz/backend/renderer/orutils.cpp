/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */

#include "orutils.h"
#include <kdebug.h>

//
// Class orQuery implementations
//
orQuery::orQuery()
{
	qryQuery = 0;
	_database = 0;
}

orQuery::orQuery ( const QString &qstrPName, const QString &qstrSQL,
                   bool doexec, KexiDB::Connection * pDb )
{
	QString qstrParsedSQL ( qstrSQL );
	QString qstrParam;
	int     intParamNum;
	int     intStartIndex = 0;

	qryQuery = 0;
	_database = pDb;

	//  Initialize some privates
	qstrName  = qstrPName;
	qstrQuery = qstrSQL;

	kDebug() << qstrName << ":" << qstrQuery << endl;

	//For now, lets assume we only support simple tables or queries
	if ( doexec )
	{
		execute();
	}

}

orQuery::~orQuery()
{
	qryQuery->close();
	delete qryQuery;
	qryQuery = 0;
}

bool orQuery::execute()
{
	if ( _database && qryQuery == 0 )
	{
		//NOTE we can use the variation of executeQuery to pass in paramters
		if ( _database->tableSchema ( qstrQuery ) )
		{
			kDebug() << qstrQuery <<  " is a table.." << endl;
			qryQuery = _database->executeQuery ( * ( _database->tableSchema ( qstrQuery ) ), 1 );
			_schema = new KexiDB::TableOrQuerySchema ( _database->tableSchema ( qstrQuery ) );
		}
		else if ( _database->querySchema ( qstrQuery ) )
		{
			kDebug() << qstrQuery <<  " is a query.." << endl;
			qryQuery = _database->executeQuery ( * ( _database->querySchema ( qstrQuery ) ), 1 );
			kDebug() << "...got test result" << endl;
			_schema = new KexiDB::TableOrQuerySchema ( _database->querySchema ( qstrQuery ) );
			kDebug() << "...got schema" << endl;
			
		}

		if ( qryQuery )
		{
			kDebug() << "Moving to first row.." << endl;
			return qryQuery->moveFirst();
		}
		else
			return false;
	}
	return false;
}

uint orQuery::fieldNumber ( const QString &fld )
{
	uint x = -1;
	if ( qryQuery->query() )
	{
		KexiDB::QueryColumnInfo::Vector flds = qryQuery->query()->fieldsExpanded();
		for ( int i = 0; i < flds.size() ; ++i )
		{
			if ( fld.toLower() == flds[i]->aliasOrName() )
			{
				x = i;
			}
		}
	}
	return x;
}

KexiDB::TableOrQuerySchema &orQuery::schema()
{
	if ( _schema )
		return *_schema;
	else
	{
		Q_ASSERT(_database);
		KexiDB::TableOrQuerySchema *tq = new KexiDB::TableOrQuerySchema ( _database, "" );
		return *tq;
	}
}

//
// Class orData
//
orData::orData()
{
	_valid = false;
	qryThis = 0;
}

void orData::setQuery ( orQuery *qryPassed )
{
	qryThis = qryPassed;
	_valid = ( qryThis != 0 && qstrField.length() );
}

void orData::setField ( const QString &qstrPPassed )
{
	qstrField = qstrPPassed;
	_valid = ( qryThis != 0 && qstrField.length() );
}

const QString &orData::getValue()
{
	if ( _valid && qryThis->getQuery() )
	{
		qstrValue = qryThis->getQuery()->value ( qryThis->fieldNumber ( qstrField ) ).toString();
	}
	else
	{
		kDebug() << "Not Valid" << endl;
	}
	return qstrValue;
}

const QByteArray &orData::getRawValue()
{
	if ( _valid && qryThis->getQuery() )
	{
		rawValue = qryThis->getQuery()->value ( qryThis->fieldNumber ( qstrField ) ).toByteArray();
	}

	return rawValue;
}

