/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>

#include <kexidb.h>
#include <kexidberror.h>
#include <kexidbwatcher.h>

#include <qsqlrecord.h>
#include <qsqlindex.h>
#include <qregexp.h>

#include "qtsqldb.h"
#include "qtsqlresult.h"
#include "qtsqlrecord.h"

#define _ME "#######qtsql-mysql: "


QtSqlRecord::QtSqlRecord(/*QTSQL_RES*/void *result, QtSqlDB *db, const char *name, bool buffer, QtSqlRecord *parent)
 : KexiDBRecordSet(db, name)//, QtSqlResult(result, db)
{
// FIXME holger
	kdDebug() << _ME  << "QtSqlRecord()::QtSqlRecord(void *result,...)" << endl;
//	m_db = db;
	m_lastItem = 0;
	m_readOnly = false;
	if ( ! findKey() )
		m_readOnly = true;
	if (m_record.field( 0 ) && m_record.field( 0 )->isReadOnly() )
		m_readOnly = true;
}

QtSqlRecord::QtSqlRecord(QSqlDatabase *realdb, KexiDB *db, const char *name, const QString& querystatement)
 : KexiDBRecordSet(db, name)
{
// FIXME holger
	kdDebug() << _ME  << "QtSqlRecord()::QtSqlRecord(kexidb, const char*) qs: _" << querystatement << "_" << endl;
	m_db = realdb;
	m_queryString = querystatement;
	m_query = new QSqlQuery();
	if ( m_query->exec( m_queryString ) )
	{
		kdDebug() << "query was successful." << endl;
	}
	else
	{
		kdDebug() << "query failed." << endl;
	}
	m_lastItem = 0;


	// try to get the table from the querystatement...
	// TODO: make this foolproof`
	QRegExp rx("(.*) from \`(.*)\`");
	kdDebug() << "trying to get m_table" << endl;
	if ( rx.search( m_queryString ) != -1 )
	{
		kdDebug() << _ME  << "QtSqlRecord()::QtSqlRecord() before: " << rx.cap(1) << endl;
		kdDebug() << _ME  << "QtSqlRecord()::QtSqlRecord() after: " << rx.cap(2) << endl;
		m_table = rx.cap( 2 );
/*		company = rx.cap( 1 );
		web = rx.cap( 2 );
		country = rx.cap( 3 );*/
	}
	m_record = m_db->record( m_table );
	m_readOnly = false;
	if ( ! findKey() )
		m_readOnly = true;
	if (m_record.field( 0 ) && m_record.field( 0 )->isReadOnly() )
		m_readOnly = true;
	//m_table =
}


bool
QtSqlRecord::findKey()
{
	kdDebug() << _ME  << "QtSqlRecord()::findKey()" << endl;
	kdDebug() << "mytable : " << m_table << endl;
	QString myname;
	myname = m_db->primaryIndex( m_table ).name();
	kdDebug() << "myindex : " << myname << endl; 

	//and now for some braindamage...
//	QSqlIndex myindex;
//	myindex = m_db->primaryIndex( m_table );
//	for (int i = 0 ; i < myindex.count() ; i++ )
//	{
//	    kdDebug() << "columnname: " << myindex.field( i )->name() << endl;
//	    
//	}
	
	if ( myname.isEmpty() )
	        return false;
	return true;	
}

bool
QtSqlRecord::readOnly()
{
	kdDebug() << _ME  << "QtSqlRecord()::readOnly()" << endl;
	return m_readOnly;
}

void
QtSqlRecord::reset()
{
// FIXME holger
	kdDebug() << _ME  << "QtSqlRecord()::reset()" << endl;
	for(UpdateBuffer::Iterator it = m_updateBuffer.begin(); it != m_updateBuffer.end(); it++)
	{
		m_updateBuffer.remove(it);
	}
}

////

KexiDBUpdateRecord *
QtSqlRecord::writeOut()
{
// FIXME holger
	kdDebug() << _ME  << "QtSqlRecord()::writeOut()" << endl;
}

bool
QtSqlRecord::writeOut(KexiDBUpdateRecord*)
{
// FIXME holger
	kdDebug() << _ME  << "QtSqlRecord()::writeOut(kexidbupdaterecord)" << endl;
}

KexiDBUpdateRecord *
QtSqlRecord::insert(bool wantNotification)
{
// FIXME holger
	kdDebug() << _ME  << "QtSqlRecord()::insert()" << endl;
}

KexiDBUpdateRecord *
QtSqlRecord::updateCurrent()
{
// FIXME holger
	kdDebug() << _ME  << "QtSqlRecord()::updateCurrent()" << endl;
}

KexiDBUpdateRecord *
QtSqlRecord::update(unsigned long record)
{
// FIXME holger
	kdDebug() << _ME  << "QtSqlRecord()::update(ulong)" << endl;
}

KexiDBUpdateRecord *
QtSqlRecord::update(QMap<QString,QVariant> fieldNameValueMap)
{
// FIXME holger

	kdDebug() << _ME  << "QtSqlRecord()::update(qmap)" << endl;
}

////
bool
QtSqlRecord::commit(unsigned int record, bool insertBuffer)
{
// FIXME holger: check this
	kdDebug() << _ME  << "QtSqlRecord()::commit()" << endl;
	kdDebug() << "QtSqlRecord::commit()" << endl;
/*	for(UpdateBuffer::Iterator it = m_updateBuffer.begin(); it != m_updateBuffer.end(); it++)
	{
		if((*it).record == record && (*it).done == false)
		{
			QString value;

			if((*it).value.type() == QVariant::ByteArray)
			{
				value = m_db->escape((*it).value.toByteArray());
			}
			else
			{
				value = m_db->escape((*it).value.toString());
			}

			QString key = m_db->escape(m_keyBuffer.find(record).data().toString());
			int index = m_insertList.findIndex(record);

			if(!insertBuffer)
			{
				kdDebug() << "QtSqlRecord::commit: committing update" << endl;

				QString statement;
				if(fieldInfo((*it).field)->table() == m_table)
				{
					statement = "update " + m_table + " set " + (*it).field + "='" +
					 m_db->encode(value) + "' where " + m_keyField + "='" + m_db->encode(key) + "'";
				}
				else
				{
					statement = forignUpdate((*it).field, value, key, false);
				}
				kdDebug() << "QtSqlRecord::commit(): query: " << statement << endl;
				m_db->query(statement);
				(*it).done = true;
			}
			else if(insertBuffer && index != -1)
			{
				kdDebug() << "QtSqlRecord::commit: committing suicide" << endl;

				QString statement;
				if(fieldInfo((*it).field)->table() == m_table)
				{
					statement = "insert into " + m_table + " set " + (*it).field +
					 " = '" + m_db->encode(value) + "'";
				}
				else
				{
					statement = forignUpdate((*it).field, value, "", true);
				}
				kdDebug() << "QtSqlRecord::commit(insert): " << statement << endl;
				m_db->query(statement);
				m_insertList.remove(m_insertList.at(index));
				(*it).done = true;

				// hopefully asign magic data (no metter how :)
				if(!m_keyBuffer.contains(record))
				{
					if((*it).field == m_keyField)
					{
						m_keyBuffer.insert(record, QVariant(value));
						m_db->watcher()->update(0, m_table, m_keyField, record, QVariant(value));
					}
					if(fieldInfo(m_keyField)->auto_increment())
					{
						m_keyBuffer.insert(record, QVariant((unsigned int)m_db->lastAuto()));
						uint autoID = m_db->lastAuto();
						m_db->watcher()->update(0, m_table, m_keyField, record, QVariant(autoID));
					}
					// else, maybe we should wait, and don't insert it by now...

					m_lastID = (unsigned int)m_db->lastAuto();
				}
			}
		}
	}*/
	return true;
}

QString
QtSqlRecord::forignUpdate(const QString &field, const QString &value, const QString &key, bool n)
{
// FIXME holger: check this, rename it
	kdDebug() << _ME  << "QtSqlRecord()::forignUpdate()" << endl;
	kdDebug() << "QtSqlRecord::commit(): forign manipulation!" << endl;
/*	//working out forign key field
	QString ftable = fieldInfo(field)->table();
	QString ffield = fieldInfo(field)->name();

	QString updateq;
	bool create = true;

	QString fkeyq("SELECT * FROM " + ftable + " WHERE " +
	 ffield + " = " + "'" + value + "'");
	kdDebug() << "QtSqlRecord::forignUpdate(): fm: " << fkeyq << endl;
	KexiDBRecordSet *r = m_db->queryRecord(fkeyq, false);
	if(r)
	{
		if(r->next())
			create = false;
		else
			create = true;

		for(uint i=0; i < r->fieldCount(); i++)
		{
			if(r->fieldInfo(i)->primary_key() || r->fieldInfo(i)->unique_key())
			{
 				RelationList rl = m_db->relations();
				for(RelationList::Iterator it = rl.begin(); it != rl.end(); ++it)
				{
					QString fkey = m_db->escape(r->value(i).toString());

					if((*it).rcvTable == ftable && (*it).srcTable == m_table)
					{
						QString local = (*it).srcField;

						if(create)
						{
							kdDebug() << "QtSqlRecord::forignUpdate(): creating..." << endl;
							uint buffer = r->insert();
// FIXME holgi							r->update(buffer, ffield, QVariant(value));
// FIXME holgi							r->commit(buffer, true);
							delete r;
							return forignUpdate(field, value, key, n);
						}

						if(!n)
						{
							kdDebug() << "QtSqlRecord::forignUpdate(): update" << endl;
							updateq = "update " + m_table + " set " + local + "='" +
							 m_db->encode(fkey) + "' where " + m_keyField + "='" + m_db->encode(key) + "'";
						}
						else
						{
							updateq = "insert into " + m_table + " set " + local +
							 " = '" + m_db->encode(fkey) + "'";

						}

						kdDebug() << "QtSqlRecord::forignUpdate(): u: " << updateq << endl;
//						delete r;

//						break;
					}
				}

//				break;

			}
		}

	}
	kdDebug() << "QtSqlRecord::forignUpdate(): done 0!" << endl;
	delete r;
	kdDebug() << "QtSqlRecord::forignUpdate(): done 1!" << endl;
	return updateq;*/
}

QVariant
QtSqlRecord::value(unsigned int column)
{
//	kdDebug() << _ME  << "QtSqlRecord()::value(uint)" << endl;
//	kdDebug() << _ME  << "QtSqlRecord()::value(uint) column: "<< column << endl;
	QVariant res;
	res = m_query->value( column );
	kdDebug() << _ME  << "QtSqlRecord()::value(uint) column: " << 
	    column << " result:" << res.toString() << endl;
	return res;
}

QVariant
QtSqlRecord::value(QString column)
{
// FIXME holger: implement
	kdDebug() << _ME  << "QtSqlRecord()::value(qstring)" << endl;
	return QVariant( "lala" );
	if(m_contentBuffer.count() > 0)
	{
		ContentBuffer::Iterator it = m_contentBuffer.find(column);
		return it.data();
	}
	else
	{
// 		return QtSqlResult::value(column);
	}
}

QVariant::Type
QtSqlRecord::type(unsigned int column)
{
// FIXME holger: implement
	kdDebug() << _ME  << "QtSqlRecord()::type(uint)" << endl;
// 	return QtSqlResult::fieldInfo(column)->type();
}

QVariant::Type
QtSqlRecord::type(QString column)
{
// FIXME holger: implement
	kdDebug() << _ME  << "QtSqlRecord()::type(qstring)" << endl;
// 	return QtSqlResult::fieldInfo(column)->type();
}

KexiDBField::ColumnType
QtSqlRecord::sqlType(unsigned int)
{
// FIXME holger: what does it do?
	kdDebug() << _ME  << "QtSqlRecord()::sqlType(uint)" << endl;
	return KexiDBField::SQLInvalid;
}

KexiDBField::ColumnType
QtSqlRecord::sqlType(QString)
{
// FIXME holger: what does it do?
	kdDebug() << _ME  << "QtSqlRecord()::sqlType(qstring)" << endl;
	return KexiDBField::SQLInvalid;
}

KexiDBField*
QtSqlRecord::fieldInfo(unsigned int column)
{
// FIXME holger: set all the attributes of the field
	kdDebug() << _ME  << "QtSqlRecord()::fieldInfo(uint)" << endl;
	QSqlField *qf;

	KexiDBField *kf;
	kf = new KexiDBField;
	return kf;
//	kf->setName();
// 	return QtSqlResult::fieldInfo(column);
}

KexiDBField*
QtSqlRecord::fieldInfo(QString column)
{
// FIXME holger
	kdDebug() << _ME  << "QtSqlRecord()::fieldInfo(qstring)" << endl;
// 	return QtSqlResult::fieldInfo(column);
}

bool
QtSqlRecord::update(unsigned int record, unsigned int field, QVariant value)
{
// FIXME holger
	kdDebug() << _ME  << "QtSqlRecord()::update(unsigned int field)" << endl;
	kdDebug() << "QtSqlRecord::update(uint): holding field '" << fieldInfo(field)->name() << "' for update" << endl;
// 	return update(record, QtSqlResult::fieldInfo(field)->name(), value);
}

bool
QtSqlRecord::update(unsigned int record, QString field, QVariant value)
{
// FIXME holger: implement
	kdDebug() << _ME  << "QtSqlRecord()::update(qstring field)" << endl;
	if(readOnly())
	{
		kdDebug() << "QtSqlRecord::update(): record is read only, abroating..." << endl;
		return false;
	}

	UpdateItem i;
	i.record = record;
	i.field = field;
	i.value = value;
	i.done = false;

	m_updateBuffer.append(i);
	kdDebug() << "QtSqlRecord::update(): we have now " << m_updateBuffer.count() << " items" << endl;
	return true;
}

int
QtSqlRecord::insert()
{
// FIXME holger: implement
	kdDebug() << _ME  << "QtSqlRecord()::insert()" << endl;
//	return 0;
	if(readOnly())
		return -1;

	m_insertList.append(m_lastItem);
	int record = m_lastItem;
	m_lastItem++;
	return record;
}

bool
QtSqlRecord::deleteRecord(uint record)
{
// FIXME holger: implement
	kdDebug() << _ME  << "QtSqlRecord()::deleteRecord()" << endl;
/*	m_error.setup(0);
	kdDebug() << "QtSqlRecord::deleteRecord()" << endl;
	if(readOnly())
		return false;

	QString key = m_db->escape(m_keyBuffer.find(record).data().toString());
	QString statement("delete from " + m_table + " where " + m_keyField + " = '" + key + "'");
	kdDebug() << "QtSqlRecord::deleteRecord():" << statement << endl;
	if (!m_db->query(statement))
	{
		m_error=KexiDBError(*(m_db->latestError()));
		return false;
	}*/
	return true;
}

void
QtSqlRecord::gotoRecord(unsigned long r)
{
// FIXME holger: implement it
	kdDebug() << _ME  << "QtSqlRecord()::gotoRecord()" << endl;
// 	QtSqlResult::gotoRecord(r);
//	return;
}

unsigned int
QtSqlRecord::fieldCount()
{
	kdDebug() << _ME  << "QtSqlRecord()::fieldCount()" << endl;
	int i;
///	i = m_db->record( m_queryString ).count(); // this doesn't work...

//	i = m_db->record( m_table ).count();
	i = m_record.count();
	kdDebug() << _ME  << "QtSqlRecord()::fieldCount() lala m_queryString: " << m_queryString << endl;
	kdDebug() << _ME  << "QtSqlRecord()::fieldCount() result: " << i << endl;

// 	return QtSqlResult::numFields();
	return i;
}

QString
QtSqlRecord::fieldName(unsigned int field)
{
	kdDebug() << _ME  << "QtSqlRecord()::fieldName(uint field )" << endl;
	QString tmp;
	tmp = m_record.fieldName( field );
	kdDebug() << _ME  << "QtSqlRecord()::fieldName() result: " << tmp << endl;
	return tmp;
// 	return QtSqlResult::fieldInfo(field)->name();
}

bool
QtSqlRecord::next()
{
	kdDebug() << _ME  << "QtSqlRecord()::next()" << endl;
//	m_query->next();
	if( m_query->next())
	{
//		m_keyContent = value(m_keyField);
		if(readOnly())
			return true;

//		m_keyBuffer.insert(QtSqlResult::currentRecord() - 1, value(m_keyField));
//		m_lastItem++;
		return true;
	}
	return false;
}

bool
QtSqlRecord::prev()
{
	kdDebug() << _ME  << "QtSqlRecord()::prev()" << endl;
	return m_query->prev();
// 	return QtSqlResult::prev();
}


unsigned long
QtSqlRecord::last_id()
{
// FIXME holger: what does it do?
	kdDebug() << _ME  << "QtSqlRecord()::last_id()" << endl;
	return m_lastID;
}

bool
QtSqlRecord::isForignField(uint f)
{
// FIXME holger: rename to isForeignField
	kdDebug() << _ME  << "QtSqlRecord()::isForignField()" << endl;
	if(fieldInfo(f)->table() == m_table)
		return false;
	else
		return true;
}

QtSqlRecord::~QtSqlRecord()
{
// FIXME holger: check that all memory is freed
	kdDebug() << _ME  << "QtSqlRecord()::~QtSqlRecord()" << endl;
	kdDebug()<<"QtSqlRecord::~QtSqlRecord()"<<endl;
	if (m_query)
	{
		delete m_query;
		m_query = 0L;
	}
}

KexiDBError *QtSqlRecord::latestError() {
// FIXME holger
	kdDebug() << _ME  << "QtSqlRecord()::latestError()" << endl;
	return &m_error;
}

unsigned int	QtSqlRecord::numRows() {
	kdDebug() << _ME  << "QtSqlRecord()::numRows()" << endl;
	int i = m_query->size();
	kdDebug() << _ME  << "QtSqlRecord()::numRows(): returning: " << i << endl;
	return i;
// 	return QtSqlResult::numRows();
}

#include "qtsqlrecord.moc"
