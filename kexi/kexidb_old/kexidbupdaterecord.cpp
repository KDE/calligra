/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexidbupdaterecord.h"


//BEGIN KexiDBUpdateRecord

KexiDBUpdateRecord::KexiDBUpdateRecord(bool insert, bool wantNotification)
	: m_insertRecord(insert),update_it(0),m_wantNotification(wantNotification) {

	m_fieldsByName.setAutoDelete(true);
}

bool KexiDBUpdateRecord::wantNotification() {
	return m_wantNotification;
}

void KexiDBUpdateRecord::reset() {
	for(QDictIterator<KexiUpdateField> it(m_fieldsByName); it.current(); ++it) {
		KexiUpdateField *kuf =it.current();
		kuf->reset();
	}
}

QVariant KexiDBUpdateRecord::value(int id) {
	KexiUpdateField *kuf=m_fields.at(id);
	if (!kuf) return QVariant();
	return kuf->value();
}

bool KexiDBUpdateRecord::setValue(QString name, QVariant value) {
	KexiUpdateField *kuf=m_fieldsByName[name.upper()];
	if (!kuf) return false;
	kuf->setValue(value);
	return true;
}

bool KexiDBUpdateRecord::setValue(int id, QVariant value) {
	KexiUpdateField *kuf=m_fields.at(id);
	if (!kuf) return false;
	kuf->setValue(value);
	return true;
}

bool KexiDBUpdateRecord::resetValue(QString name) {
	KexiUpdateField *kuf=m_fieldsByName[name.upper()];
	if (!kuf) return false;
	kuf->reset();
	return true;
}

bool KexiDBUpdateRecord::isInsert() {
	return m_insertRecord;
}

QStringList KexiDBUpdateRecord::fieldNames() {
	QStringList tmp;
	for(QDictIterator<KexiUpdateField> it(m_fieldsByName); it.current(); ++it) {
		tmp<<it.current()->m_name;        	
        }
	return tmp;
}

bool KexiDBUpdateRecord::addField(QString name, QVariant resetvalue) {
	if (m_fieldsByName[name.upper()]) return false;
	KexiUpdateField  *uf=new KexiUpdateField(name,resetvalue);
	m_fieldsByName.insert(name.upper(), uf);
	m_fields.append(uf);
	return true;
}

bool KexiDBUpdateRecord::addPrimaryKey(QString name, QVariant value) {
	if (m_primaryKeys.contains(name)) return false;
	m_primaryKeys.insert(name,value);
	return true;
}

QMap<QString,QVariant> KexiDBUpdateRecord::primaryKeys() {
	return m_primaryKeys;
}

bool KexiDBUpdateRecord::firstUpdateField( QString &retName, QVariant &retValue) {
	if (update_it) delete update_it;

	update_it=new QDictIterator<KexiUpdateField>(m_fieldsByName);

	for (;update_it->current() && (!update_it->current()->markedForUpdate());++(*update_it));
	if (update_it->current())
	{
		retName=update_it->current()->name();
		retValue=update_it->current()->value();		
		return true;
	}
	return false;

}

bool KexiDBUpdateRecord::nextUpdateField( QString &retName, QVariant &retValue)
{
	if (!update_it) return false;
	for (++(*update_it);update_it->current() && (!update_it->current()->markedForUpdate());++(*update_it));
	if (update_it->current())
	{
		retName=update_it->current()->name();
		retValue=update_it->current()->value();		
		return true;
	}
	return false;
}


KexiDBUpdateRecord::~KexiDBUpdateRecord() {
	delete update_it;
}

//END KexiDBUpdateRecord

//BEGIN KexiDBUpdateRecord::KexiUpdateField

KexiDBUpdateRecord::KexiUpdateField::KexiUpdateField(const QString &name, QVariant defaultResetValue)
	:m_name(name),m_update(false),m_resetValue(defaultResetValue) {
}

void KexiDBUpdateRecord::KexiUpdateField::reset() {
	m_update=false;
}

void KexiDBUpdateRecord::KexiUpdateField::setValue(QVariant value) {
	m_update=true;
	m_value=value;
}

bool KexiDBUpdateRecord::KexiUpdateField::markedForUpdate() {
	return m_update;
}

QString KexiDBUpdateRecord::KexiUpdateField::name() {
	return m_name;
}

QVariant KexiDBUpdateRecord::KexiUpdateField::value() {
	if (m_update)	return m_value;
	else return m_resetValue;
}


KexiDBUpdateRecord::KexiUpdateField::~KexiUpdateField() {
}


//END KexiDBUpdateRecord::KexiUpdateField

