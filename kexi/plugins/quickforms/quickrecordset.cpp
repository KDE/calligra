/*
 * Copyright 2014 Adam Pigg <email>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "quickrecordset.h"
#include <kdebug.h>
#include <db/connection.h>
#include <db/cursor.h>
#include <db/utils.h>

class QuickRecordSet::Private
{
public:
    Private(KexiDB::Connection *pDb)
      : cursor(0), connection(pDb), originalSchema(0), copySchema(0)
    {
    }
    ~Private()
    {
        delete copySchema;
        delete originalSchema;
        delete cursor;
    }


    QString qstrQuery;

    KexiDB::Cursor *cursor;
    KexiDB::Connection *connection;
    KexiDB::QuerySchema *originalSchema;
    KexiDB::QuerySchema *copySchema;
};

QuickRecordSet::QuickRecordSet(const QString& source, KexiDB::Connection* pConnection, QObject* parent): QObject(parent), d(new Private(pConnection))
{
    d->qstrQuery = source;
    getSchema();
    open();
}

QuickRecordSet::~QuickRecordSet()
{
  close();
  delete d;
}

qint64 QuickRecordSet::at() const
{
  if (d->cursor) {
    return d->cursor->at();
  }
  return -1;
}

qint64 QuickRecordSet::recordCount() const
{
    if ( d->copySchema )
    {
        return KexiDB::rowCount ( *d->copySchema );
    }
    else
    {
        return 1;
    }
}

bool QuickRecordSet::moveFirst()
{
    if ( d->cursor ) {
      if ( d->cursor->moveFirst() ) {
	emit positionChanged(at());
	return true;
      } else {
	return false;
      }
    }

    return false;
}

bool QuickRecordSet::movePrevious()
{
    if ( d->cursor ){
      if ( d->cursor->movePrev() ) {
	emit positionChanged(at());
	return true;
      } else {
	return false;
      }
    }
    return false;
}

bool QuickRecordSet::moveNext()
{
    if ( d->cursor ){
      if ( d->cursor->moveNext() ) {
	emit positionChanged(at());
	return true;
      } else {
	return false;
      }
    }
    return false;
}

bool QuickRecordSet::moveLast()
{
    if ( d->cursor ){
      if ( d->cursor->moveLast() ) {
	emit positionChanged(at());
	return true;
      } else {
	return false;
      }
    }
    return false;
}

bool QuickRecordSet::open()
{
    if ( d->connection && d->cursor == 0 )
    {
        if ( d->qstrQuery.isEmpty() )
        {
            d->cursor = d->connection->executeQuery ( "SELECT '' AS expr1 FROM kexi__db WHERE kexi__db.db_property = 'kexidb_major_ver'" );
        }
        else if ( d->copySchema)
        {
            kDebug() << "Opening cursor.." << d->copySchema->debugString();
            d->cursor = d->connection->executeQuery ( *d->copySchema, 1 );
        }


        if ( d->cursor )
        {
            kDebug() << "Moving to first record..";
            return d->cursor->moveFirst();
        }
        else
            return false;
    }
    return false;
}

bool QuickRecordSet::close()
{
    if ( d->cursor )
    {
        d->cursor->close();
        delete d->cursor;
        d->cursor = 0;
    }

    return true;
}

bool QuickRecordSet::getSchema()
{
    if ( d->connection )
    {
        delete d->originalSchema;
        d->originalSchema = 0;
        delete d->copySchema;
        d->copySchema = 0;

        if ( d->connection->tableSchema ( d->qstrQuery ) )
        {
            kDebug() << d->qstrQuery <<  " is a table..";
            d->originalSchema = new KexiDB::QuerySchema ( *(d->connection->tableSchema ( d->qstrQuery )) );
        }
        else if ( d->connection->querySchema ( d->qstrQuery ) )
        {
            kDebug() << d->qstrQuery <<  " is a query..";
            d->connection->querySchema(d->qstrQuery)->debug();
            d->originalSchema = new KexiDB::QuerySchema(*(d->connection->querySchema ( d->qstrQuery )));
        }

        if (d->originalSchema) {
            kDebug() << "Original:" << d->connection->selectStatement(*d->originalSchema);
            d->originalSchema->debug();

            d->copySchema = new KexiDB::QuerySchema(*d->originalSchema);
            d->copySchema->debug();
            kDebug() << "Copy:" << d->connection->selectStatement(*d->copySchema);
        }

        return true;
    }
    return false;
}
#include "quickrecordset.moc"
