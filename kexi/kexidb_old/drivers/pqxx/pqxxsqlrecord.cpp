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

#include "pqxxsqldb.h"
#include "pqxxsqlresult.h"
#include "pqxxsqlrecord.h"

#include <stdio.h>
#include <iostream>

pqxxSqlRecord::pqxxSqlRecord(result *result, pqxxSqlDB *db, const char *name, bool buffer, pqxxSqlRecord *parent)
        : KexiDBRecordSet(db, name), pqxxSqlResult(result, db)
{
    kdDebug() << "pqxxSqlRecord: Creating Object" << endl;
    m_db = db;
    m_lastItem = 0;
    m_readOnly = !findKey();
    m_parent = parent;
}

//Returns true if there is a key field, false otherwise
//This is the oposite to the mysql driver as it makes more sense this way :)
bool pqxxSqlRecord::findKey()
{
    kdDebug() << "pqxxSqlRecord: Finding Key" << endl;
    for(uint i=0; i < fieldCount(); i++)
    {
        if(pqxxSqlResult::fieldInfo(i)->primary_key() || pqxxSqlResult::fieldInfo(i)->unique_key())
        {
            m_keyField = pqxxSqlResult::fieldInfo(i)->name();
            m_table = pqxxSqlResult::fieldInfo(i)->table();
            kdDebug() << "pqSqlRecord::findKey(): name is: " << m_keyField << endl;
            return true;
        }
    }

    return false;
}

bool pqxxSqlRecord::readOnly()
{
    return m_readOnly;
}

void pqxxSqlRecord::reset()
{
    kdDebug() << "pqxxSqlRecord: reset" << endl;
    for(UpdateBuffer::Iterator it = m_updateBuffer.begin(); it != m_updateBuffer.end(); it++)
    {
        m_updateBuffer.remove(it);
    }
}

QString pqxxSqlRecord::forignUpdate(const QString &field, const QString &value, const QString &key, bool n)
{
    kdDebug()<<"pqxxSqlRecord::foreignupdate()"<<endl;
#if 0

    kdDebug() << "pqxxSqlRecord::commit(): forign manipulation!" << endl;
    //working out forign key field
    QString ftable = fieldInfo(field)->table();
    QString ffield = fieldInfo(field)->name();

    QString updateq;
    bool create = true;

    QString fkeyq("SELECT * FROM " + ftable + " WHERE " +
                  ffield + " = " + "'" + value + "'");
    kdDebug() << "pqxxSqlRecord::forignUpdate(): fm: " << fkeyq << endl;
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

#endif

    return QString::null;
}

QVariant pqxxSqlRecord::value(unsigned int column)
{
    return pqxxSqlResult::value(column);
}

QVariant pqxxSqlRecord::value(QString column)
{
    if(m_contentBuffer.count() > 0)
    {
        ContentBuffer::Iterator it = m_contentBuffer.find(column);
        return it.data();
    }
    else
    {
        return pqxxSqlResult::value(column);
    }
}

QVariant::Type pqxxSqlRecord::type(unsigned int column)
{
    return pqxxSqlResult::fieldInfo(column)->type();
}

QVariant::Type pqxxSqlRecord::type(QString column)
{
    return pqxxSqlResult::fieldInfo(column)->type();
}

KexiDBField::ColumnType pqxxSqlRecord::sqlType(unsigned int column)
{
    return pqxxSqlResult::fieldInfo(column)->sqlType();
}

KexiDBField::ColumnType pqxxSqlRecord::sqlType(QString column)
{
    return pqxxSqlResult::fieldInfo(column)->sqlType();
}

KexiDBField* pqxxSqlRecord::fieldInfo(unsigned int column)
{
    return pqxxSqlResult::fieldInfo(column);
}

KexiDBField* pqxxSqlRecord::fieldInfo(QString column)
{
    return pqxxSqlResult::fieldInfo(column);
}

KexiDBUpdateRecord* pqxxSqlRecord::insert(bool wantNotification)
{
    kdDebug()<<"KexiDBUpdateRecord * pqxxSqlRecord::insert(bool) "<<endl;
    if (readOnly())
        return 0; //perhaps a kexidberror should be set here too
    KexiDBUpdateRecord *rec=new KexiDBUpdateRecord(true,wantNotification);
    for (uint i=0;i<fieldCount();i++)
    {
        KexiDBField *f=fieldInfo(i);
        rec->addField(f->name(),f->defaultValue());
    }
    m_insertList.append(rec);
    //	int record = m_lastItem;
    m_lastItem++;

    kdDebug()<<"Leaving and returning an update record"<<endl;
    return rec;
}

bool pqxxSqlRecord::deleteRecord(uint record)
{
    m_error.setup(0);
    kdDebug() << "pqxxSqlRecord::deleteRecord():" << record << endl;
    if(readOnly())
        return false;

    QString key = m_db->escape(m_keyBuffer.find(record).data().toString());
    QString statement("DELETE FROM " + m_db->escapeName(m_table) + " WHERE " + m_db->escapeName(m_keyField) + " = " + m_db->escapeValue(key, fieldInfo(m_keyField)->sqlType()));

    if (!m_db->query(statement, true))
    {
        m_error=KexiDBError(*(m_db->latestError()));
        return false;
    }
    return true;
}

unsigned int pqxxSqlRecord::fieldCount()
{
    return pqxxSqlResult::numFields();
}

QString pqxxSqlRecord::fieldName(unsigned int field)
{
    return pqxxSqlResult::fieldInfo(field)->name();
}

bool pqxxSqlRecord::next()
{
    if(pqxxSqlResult::next())
    {
        //		m_keyContent = value(m_keyField);
        if(readOnly())
            return true;

        m_keyBuffer.insert(pqxxSqlResult::currentRecord() - 1, value(m_keyField));
        m_lastItem++;
        return true;
    }
    return false;
}

bool pqxxSqlRecord::prev()
{
    return pqxxSqlResult::prev();
}


unsigned long pqxxSqlRecord::last_id()
{
    kdDebug() << "pqxxSqlRecord::last_id()" << endl;
    return m_lastID;
}

bool pqxxSqlRecord::isForignField(uint f)
{
    kdDebug() << "pqxxSqlRecord::isforeignfield()" << endl;
    if(fieldInfo(f)->table() == m_table)
        return false;
    else
        return true;
}

pqxxSqlRecord::~pqxxSqlRecord()
{
    kdDebug()<<"pqxxSqlRecord::~pqxxSqlRecord()"<<endl;
}

KexiDBError *pqxxSqlRecord::latestError()
{
    kdDebug()<<"pqxxSqlRecord::latesterror()"<<endl;
    return &m_error;
}

unsigned int pqxxSqlRecord::numRows()
{
    return pqxxSqlResult::numRows();
}

KexiDBUpdateRecord* pqxxSqlRecord::updateCurrent()
{
    kdDebug()<<"pqxxSqlRecord::updatecurrent()"<<endl;
    return 0;
}

KexiDBUpdateRecord* pqxxSqlRecord::update(QMap<QString,QVariant> fieldNameValueMap)
{
    kdDebug()<<"pqxxSqlRecord::update(qmap)"<<endl;
    QString tableName=fieldInfo(0)->table();
    QStringList primaryKeys=m_db->table(tableName)->primaryKeys();
    KexiDBUpdateRecord *rec=new KexiDBUpdateRecord(false);
    for (uint i=0;i<fieldCount();i++)
    {
        KexiDBField *f=fieldInfo(i);
        rec->addField(f->name(),f->defaultValue());
        if (f->primary_key())
        {
            if (fieldNameValueMap.contains(f->name()))
            {
                rec->addPrimaryKey(f->name(),fieldNameValueMap[f->name()]);
                primaryKeys.remove(f->name());
            }
        }
    }
    if (primaryKeys.count()!=0)
    {
        delete rec;
        kdDebug()<<"UNIQUENESS COULDN'T BE ENSURED"<<endl;
#ifndef Q_WS_WIN
#warning generate some error here
#endif

        return 0;
    }
    m_insertList.append(rec);
    return  rec;
}

KexiDBUpdateRecord *pqxxSqlRecord::writeOut()
{
    kdDebug()<<"pqxxSqlRecord::writeout()"<<endl;
    for (KexiDBUpdateRecord *record=m_insertList.first();record;record=m_insertList.first())
    {
        if (!writeOut(record))
            return record;
    }
    return 0;
}

bool pqxxSqlRecord::writeOut(KexiDBUpdateRecord* ur)
{
    kdDebug()<<"pqxxSqlRecord::writeout(kexidbupdaterecord)"<<endl;
    {
        QString fieldList;
        QString valueList;

        QVariant tmpValue;
        QString tmpField;

        if (ur->isInsert())
        {
            for (bool valid=ur->firstUpdateField(tmpField, tmpValue);valid; valid=ur->nextUpdateField(tmpField,tmpValue))
            {

                if (!fieldList.isEmpty())
                {
                    fieldList+=",";
                    valueList+=",";
                }

                fieldList+=m_db->escapeName(tmpField);
                valueList+=m_db->escapeValue(tmpValue.asString(), sqlType(tmpField) );
            }
            if (!fieldList.isEmpty())
            {
                QString statement="insert into " + m_db->escapeName(m_table) + " ("+fieldList+") VALUES ("+valueList+");";
                m_db->query(statement);
                if (ur->wantNotification())
                {
                    result* lr=m_db->lastRow(m_table.latin1());
                    ;
                    if (lr!=0)
                    {
                        for (int i=0;i<int(fieldCount());i++)
                        {
                            ur->setValue(i, QVariant(lr->at(0).at(i).c_str()));
                        }
                        emit recordInserted(ur);
                        delete lr;
                        lr = 0;
                    }
                }
            }
        }
        else
        {

            QString statement;
            for (bool valid=ur->firstUpdateField(tmpField, tmpValue);valid; valid=ur->nextUpdateField(tmpField,tmpValue))
            {
                if (!statement.isEmpty())
                    statement+=",";
                statement += m_db->escapeName(tmpField) + "=" + m_db->escapeValue(tmpValue.asString(), fieldInfo(tmpField)->sqlType());
            }

            if (!statement.isEmpty())
            {
                QMap<QString,QVariant> pk=ur->primaryKeys();
                QString primkeys;

                QMap<QString,QVariant>::Iterator it;
                for ( it = pk.begin(); it != pk.end(); ++it )
                {
                    if (!primkeys.isEmpty())
                        primkeys+=" AND ";
                    primkeys += (m_db->escapeName(it.key()) + "=" + m_db->escapeValue(it.data().asString(), sqlType(it.key())));
                }
                statement="update " + m_db->escapeName(m_table) + " set " + statement + " where " + primkeys;
                m_db->query(statement);
            }
        }

        m_insertList.remove(ur);
        return true; //ERROR HANDLING NEEDED
    }
}

void pqxxSqlRecord::gotoRecord(unsigned long record)
{
    pqxxSqlRecord::gotoRecord(record);
}

#include "pqxxsqlrecord.moc"
