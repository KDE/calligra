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

#ifndef _KEXIUPDATERECORD_H_
#define _KEXIUPDATERECORD_H_

#include <qstring.h>
#include <qvariant.h>
#include <qmap.h>
#include <qdict.h>
#include <qstringlist.h>

class KEXIDB_EXPORT KexiDBUpdateRecord {
public:

	void reset();
	bool setValue(QString name, QVariant value);
	bool setValue(int id, QVariant value);
	bool resetValue(QString name);
	bool value(QString name);
	QStringList fieldNames();
	QMap<QString,QVariant> primaryKeys();

/** these should only be used by classes inheriting  KexiDBRecordSet
**/
	bool isInsert();
	bool addField(QString name, QVariant resetValue);
	bool addPrimaryKey(QString name, QVariant value);
	KexiDBUpdateRecord(bool insert);
	~KexiDBUpdateRecord();
	bool firstUpdateField( QString &retName, QVariant &retValue);
	bool nextUpdateField( QString &retName, QVariant &retValue);

private:

	class KexiUpdateField {
	public:
		KexiUpdateField(const QString &name, QVariant defaultResetValue);
		~KexiUpdateField();
	protected:
		friend class KexiDBUpdateRecord;
		void reset();
		void setValue(QVariant value);
		bool markedForUpdate();
		QString name();
		QVariant value();
	private:
		QVariant m_value;
		bool m_update;
		QString m_name;
		QVariant m_resetValue;
	};

	QDict<KexiUpdateField> m_fieldsByName;
	QPtrList<KexiUpdateField> m_fields;
	bool m_insertRecord;
	QMap<QString,QVariant> m_primaryKeys;
	QDictIterator<KexiUpdateField> *update_it;

};

#endif
