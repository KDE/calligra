/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
Copyright (C) 2003   Joseph Wenninger<jowenn@kde.org>

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
 : KexiDBRecordSet(db, name), MySqlResult(result, db)
{
	m_db = db;
	m_lastItem = 0;
	m_readOnly = findKey();
	m_insertList.setAutoDelete(true);
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
MySqlRecord::writeOut(KexiDBUpdateRecord * ur)
{
	QString fieldList;
	QString valueList;

	QVariant tmpValue;
	QString tmpField;

#warning " \" should be escaped within values";

	if (ur->isInsert()) {
		for (bool valid=ur->firstUpdateField(tmpField, tmpValue);
			valid; valid=ur->nextUpdateField(tmpField,tmpValue)) {

			if (!fieldList.isEmpty()) {
				fieldList+=",";
				valueList+=",";
			}

			fieldList+=tmpField;
			valueList+="\""+tmpValue.asString()+"\"";
		}
		if (!fieldList.isEmpty())
		{
			QString statement="insert into "+m_table+" ("+fieldList+") VALUES ("+valueList+");";
			kdDebug()<<"INSERT STATEMENT:"<<statement<<endl;
			m_db->query(statement);

		}
	}
	else
	{

		QString statement;
		for (bool valid=ur->firstUpdateField(tmpField, tmpValue);
			valid; valid=ur->nextUpdateField(tmpField,tmpValue)) {

			if (!statement.isEmpty()) statement+=",";
			statement+=tmpField+"=\""+tmpValue.asString()+"\"";
		}

		if (!statement.isEmpty())
		{
			QMap<QString,QVariant> pk=ur->primaryKeys();
			QString primkeys;

			QMap<QString,QVariant>::Iterator it;
			for ( it = pk.begin(); it != pk.end(); ++it ) {
				if (!primkeys.isEmpty()) primkeys+=" AND ";
				primkeys+=(it.key()+"=\""+it.data().asString()+"\"");
			}
			statement="update "+m_table+" set "+statement + " where " +
			primkeys;
			kdDebug()<<"UPDATE STATEMENT:"<<statement<<endl;
			m_db->query(statement);
		}
	}

	m_insertList.remove(ur);
	return true; //ERROR HANDLING NEEDED
}


KexiDBUpdateRecord*
MySqlRecord::writeOut()
{

	kdDebug()<<"bool MySqlRecord::commit(unsigned int record, bool insertBuffer) entered"<<endl;

	for (KexiDBUpdateRecord *record=m_insertList.first();record;record=m_insertList.first()) {
		if (!writeOut(record)) return record;
	}




//	} else {
#if 0
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
					 m_db->encode(value) + "' where " + m_keyField + "='" + m_db->encode(key) + "'";
				}
				else
				{
					statement = foreignUpdate((*it).field, value, key, false);
				}
				kdDebug() << "MySqlRecord::commit(): query: " << statement << endl;
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
					 " = '" + m_db->encode(value) + "'";
				}
				else
				{
					statement = foreignUpdate((*it).field, value, "", true);
				}
				kdDebug() << "MySqlRecord::commit(insert): " << statement << endl;
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
#endif
//}
	return 0;
}

QString
MySqlRecord::foreignUpdate(const QString &field, const QString &value, const QString &key, bool n)
{
#if 0
	kdDebug() << "MySqlRecord::commit(): foreign manipulation!" << endl;
	//working out foreign key field
	QString ftable = fieldInfo(field)->table();
	QString ffield = fieldInfo(field)->name();

	QString updateq;
	bool create = true;

	QString fkeyq("SELECT * FROM " + ftable + " WHERE " +
	 ffield + " = " + "'" + value + "'");
	kdDebug() << "MySqlRecord::foreignUpdate(): fm: " << fkeyq << endl;
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
							kdDebug() << "MySqlRecord::foreignUpdate(): creating..." << endl;
							uint buffer = r->insert();
							r->update(buffer, ffield, QVariant(value));
							r->commit(buffer, true);
							delete r;
							return foreignUpdate(field, value, key, n);
						}

						if(!n)
						{
							kdDebug() << "MySqlRecord::foreignUpdate(): update" << endl;
							updateq = "update " + m_table + " set " + local + "='" +
							 m_db->encode(fkey) + "' where " + m_keyField + "='" + m_db->encode(key) + "'";
						}
						else
						{
							updateq = "insert into " + m_table + " set " + local +
							 " = '" + m_db->encode(fkey) + "'";

						}

						kdDebug() << "MySqlRecord::foreignUpdate(): u: " << updateq << endl;
//						delete r;

//						break;
					}
				}

//				break;

			}
		}

	}
	kdDebug() << "MySqlRecord::foreignUpdate(): done 0!" << endl;
	delete r;
	kdDebug() << "MySqlRecord::foreignUpdate(): done 1!" << endl;
	return updateq;
#endif
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

KexiDBUpdateRecord *
MySqlRecord::updateCurrent()
{
	kdDebug()<<"KexiDBUpdateRecord * MySqlRecord::updateCurrent() entered "<<endl;
	if (readOnly()) return 0; //perhaps a kexidberror should be set here too
	KexiDBUpdateRecord *rec=new KexiDBUpdateRecord(false);
	for (int i=0;i<fieldCount();i++) {
		KexiDBField *f=fieldInfo(i);
		rec->addField(f->name(),f->defaultValue());
		if (f->primary_key()) {
			rec->addPrimaryKey(f->name(),value(f->name()));
		}
	}
	m_insertList.append(rec);

//	int record = m_lastItem;
	m_lastItem++;

	return rec;
}


KexiDBUpdateRecord *
MySqlRecord::update(QMap<QString,QVariant> fieldNameValueMap)
{
	QString tableName=fieldInfo(0)->table();
	QStringList primaryKeys=m_db->table(tableName)->primaryKeys();
	KexiDBUpdateRecord *rec=new KexiDBUpdateRecord(false);
	for (int i=0;i<fieldCount();i++) {
		KexiDBField *f=fieldInfo(i);
		rec->addField(f->name(),f->defaultValue());
		if (f->primary_key()) {
			if (fieldNameValueMap.contains(f->name())) {
				rec->addPrimaryKey(f->name(),fieldNameValueMap[f->name()]);
				primaryKeys.remove(f->name());
			}
		}
	}
	if (primaryKeys.count()!=0) {
		delete rec;
		kdDebug()<<"UNIQUENESS COULDN'T BE ENSURED"<<endl;
#warning generate some error here
		return 0;
	}
	m_insertList.append(rec);
	return  rec;

}

KexiDBUpdateRecord *
MySqlRecord::insert()
{
	kdDebug()<<"KexiDBUpdateRecord * MySqlRecord::insert() entered "<<endl;
	if (readOnly()) return 0; //perhaps a kexidberror should be set here too
	KexiDBUpdateRecord *rec=new KexiDBUpdateRecord(true);
	for (int i=0;i<fieldCount();i++) {
		KexiDBField *f=fieldInfo(i);
		rec->addField(f->name(),f->defaultValue());
	}
	m_insertList.append(rec);
//	int record = m_lastItem;
	m_lastItem++;

	return rec;
}

bool
MySqlRecord::deleteRecord(uint record)
{
	m_error.setup(0);
	kdDebug() << "MySqlRecord::deleteRecord()" << endl;
	if(readOnly())
		return false;

	QString key = m_db->escape(m_keyBuffer.find(record).data().toString());
	QString statement("delete from " + m_table + " where " + m_keyField + " = '" + key + "'");
	kdDebug() << "MySqlRecord::deleteRecord():" << statement << endl;
	if (!m_db->query(statement))
	{
		m_error=KexiDBError(*(m_db->latestError()));
		return false;
	}
	return true;
}

void
MySqlRecord::gotoRecord(unsigned long r)
{
	MySqlResult::gotoRecord(r);
//	return;
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
MySqlRecord::isForeignField(uint f)
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

KexiDBError *MySqlRecord::latestError() {
	return &m_error;
}

unsigned int	MySqlRecord::numRows() {
	return MySqlResult::numRows();
}

#include "mysqlrecord.moc"
