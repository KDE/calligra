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

#include "mysqldb.h"
#include "mysqlresult.h"
#include "mysqlrecord.h"

MySqlRecord::MySqlRecord(MYSQL_RES *result, MySqlDB *db, const char *name, bool buffer, MySqlRecord *parent)
 : KexiDBRecord(db, name), MySqlResult(result, db)
{
	m_db = db;
	m_lastItem = 0;
	m_readOnly = findKey();
}

bool
MySqlRecord::findKey()
{
	for(uint i=0; i < fieldCount(); i++)
	{
		if(MySqlResult::fieldInfo(i)->primary_key() || MySqlResult::fieldInfo(i)->unique_key())
		{
			m_keyField = MySqlResult::fieldInfo(i)->name();
			m_table = MySqlResult::fieldInfo(i)->table();
			return false;
		}
	}

	kdDebug() << "MySqlRecrod::findKey(): name is: " << m_keyField << endl;

	return true;
}

bool
MySqlRecord::readOnly()
{
	return m_readOnly;
}

void
MySqlRecord::reset()
{
	for(UpdateBuffer::Iterator it = m_updateBuffer.begin(); it != m_updateBuffer.end(); it++)
	{
		m_updateBuffer.remove(it);
	}
}

bool
MySqlRecord::commit(unsigned int record, bool insertBuffer)
{
	kdDebug() << "MySqlRecord::commit()" << endl;
	for(UpdateBuffer::Iterator it = m_updateBuffer.begin(); it != m_updateBuffer.end(); it++)
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
				kdDebug() << "MySqlRecord::commit: committing update" << endl;

				QString statement;
				if(fieldInfo((*it).field)->table() == m_table)
				{
					statement = "update " + m_table + " set " + (*it).field + "='" +
					 value + "' where " + m_keyField + "='" + key + "'";
				}
				else
				{
					statement = forignUpdate((*it).field, value, key, false);
				}
//				kdDebug() << "MySqlRecord::commit(): query: " << statement << endl;
				m_db->query(statement);
				(*it).done = true;
			}
			else if(insertBuffer && index != -1)
			{
				kdDebug() << "MySqlRecord::commit: committing suicide" << endl;

				QString statement;
				if(fieldInfo((*it).field)->table() == m_table)
				{
					statement = "insert into " + m_table + " set " + (*it).field +
					 " = '" + value + "'";
				}
				else
				{
					statement = forignUpdate((*it).field, value, "", true);
				}
//				kdDebug() << "MySqlRecord::commit(insert): " << statement << endl;
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
	}
	return true;
}

QString
MySqlRecord::forignUpdate(const QString &field, const QString &value, const QString &key, bool n)
{
	kdDebug() << "MySqlRecord::commit(): forign manipulation!" << endl;
	//working out forign key field
	QString ftable = fieldInfo(field)->table();
	QString ffield = fieldInfo(field)->name();

	QString updateq;
	bool create = true;

	QString fkeyq("SELECT * FROM " + ftable + " WHERE " +
	 ffield + " = " + "'" + value + "'");
	kdDebug() << "MySqlRecord::forignUpdate(): fm: " << fkeyq << endl;
	KexiDBRecord *r = m_db->queryRecord(fkeyq, false);
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
							kdDebug() << "MySqlRecord::forignUpdate(): creating..." << endl;
							uint buffer = r->insert();
							r->update(buffer, ffield, QVariant(value));
							r->commit(buffer, true);
							delete r;
							return forignUpdate(field, value, key, n);
						}

						if(!n)
						{
							kdDebug() << "MySqlRecord::forignUpdate(): update" << endl;
							updateq = "update " + m_table + " set " + local + "='" +
							 fkey + "' where " + m_keyField + "='" + key + "'";
						}
						else
						{
							updateq = "insert into " + m_table + " set " + local +
							 " = '" + fkey + "'";

						}

						kdDebug() << "MySqlRecord::forignUpdate(): u: " << updateq << endl;
//						delete r;

//						break;
					}
				}

//				break;

			}
		}

	}
	kdDebug() << "MySqlRecord::forignUpdate(): done 0!" << endl;
	delete r;
	kdDebug() << "MySqlRecord::forignUpdate(): done 1!" << endl;
	return updateq;
}

QVariant
MySqlRecord::value(unsigned int column)
{
	return MySqlResult::value(column);
}

QVariant
MySqlRecord::value(QString column)
{
	if(m_contentBuffer.count() > 0)
	{
		ContentBuffer::Iterator it = m_contentBuffer.find(column);
		return it.data();
	}
	else
	{
		return MySqlResult::value(column);
	}
}

QVariant::Type
MySqlRecord::type(unsigned int column)
{
	return MySqlResult::fieldInfo(column)->type();
}

QVariant::Type
MySqlRecord::type(QString column)
{
	return MySqlResult::fieldInfo(column)->type();
}

KexiDBField::ColumnType
MySqlRecord::sqlType(unsigned int)
{
	return KexiDBField::SQLInvalid;
}

KexiDBField::ColumnType
MySqlRecord::sqlType(QString)
{
	return KexiDBField::SQLInvalid;
}

KexiDBField*
MySqlRecord::fieldInfo(unsigned int column)
{
	return MySqlResult::fieldInfo(column);
}

KexiDBField*
MySqlRecord::fieldInfo(QString column)
{
	return MySqlResult::fieldInfo(column);
}

bool
MySqlRecord::update(unsigned int record, unsigned int field, QVariant value)
{
	kdDebug() << "MySqlRecord::update(uint): holding field '" << fieldInfo(field)->name() << "' for update" << endl;
	return update(record, MySqlResult::fieldInfo(field)->name(), value);
}

bool
MySqlRecord::update(unsigned int record, QString field, QVariant value)
{
	if(readOnly())
	{
		kdDebug() << "MySqlRecord::update(): record is read only, abroating..." << endl;
		return false;
	}

	UpdateItem i;
	i.record = record;
	i.field = field;
	i.value = value;
	i.done = false;

	m_updateBuffer.append(i);
	kdDebug() << "MySqlRecord::update(): we have now " << m_updateBuffer.count() << " items" << endl;
	return true;
}

int
MySqlRecord::insert()
{
//	return 0;
	if(readOnly())
		return -1;

	m_insertList.append(m_lastItem);
	int record = m_lastItem;
	m_lastItem++;
	return record;
}

bool
MySqlRecord::deleteRecord(uint record)
{
	kdDebug() << "MySqlRecord::deleteRecord()" << endl;
	if(readOnly())
		return false;

	QString key = m_db->escape(m_keyBuffer.find(record).data().toString());
	QString statement("delete from " + m_table + " where " + m_keyField + " = '" + key + "'");
	kdDebug() << "MySqlRecord::deleteRecord():" << statement << endl;
	try
	{
		m_db->query(statement);
	}
	catch(KexiDBError &err)
	{
		throw err;
	}
	return true;
}

void
MySqlRecord::gotoRecord(unsigned int)
{
	return;
}

unsigned int
MySqlRecord::fieldCount()
{
	return MySqlResult::numFields();
}

QString
MySqlRecord::fieldName(unsigned int field)
{
	return MySqlResult::fieldInfo(field)->name();
}

bool
MySqlRecord::next()
{
	if(MySqlResult::next())
	{
//		m_keyContent = value(m_keyField);
		if(readOnly())
			return true;

		m_keyBuffer.insert(MySqlResult::currentRecord() - 1, value(m_keyField));
		m_lastItem++;
		return true;
	}
	return false;
}

bool
MySqlRecord::prev()
{
	return MySqlResult::prev();
}


unsigned long
MySqlRecord::last_id()
{
	return m_lastID;
}

bool
MySqlRecord::isForignField(uint f)
{
	if(fieldInfo(f)->table() == m_table)
		return false;
	else
		return true;
}

MySqlRecord::~MySqlRecord()
{
	kdDebug()<<"MySqlRecord::~MySqlRecord()"<<endl;
}

#include "mysqlrecord.moc"
