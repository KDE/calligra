/* This file is part of the KDE project
   Copyright (C) 2003   Joseph Wenninger <jowenn@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIKSpreadIMPORTFILTER_H
#define KEXIKSpreadIMPORTFILTER_H

#include "core/filters/kexifilter.h"
#include "core/filters/kexifiltermanager.h"
#include "core/filters/kexitableimportsourceiface.h"
#include <qmap.h>
#include <kexiDB/kexidbtable.h>

class KexiDB;
class KexiKSpreadSource;
typedef QMap<int, int> DataTypes;

class KexiKSpreadImport : public KexiFilter, public KexiTableImportSourceIface
{
	Q_OBJECT

	enum DataType
	{
		TypeVarchar = 0,
		TypeInt = 1,
		TypeDouble = 2
	};

	public:
		KexiKSpreadImport(QObject *parent, const char *name, const QStringList &);
		~KexiKSpreadImport();

		virtual QString name();

		virtual bool prepareImport(unsigned long type, const KURL& url=KURL());

	        virtual KexiDBTable tableStructure();

	        virtual bool firstTableRow();
        	virtual bool nextTableRow();
	        virtual QVariant tableValue(int field);
	
        	virtual QPtrList<QWidget> sourceWidgets(QWidget *parent);

		virtual unsigned long supportedTypes() {return KexiFilterManager::Data;}

		virtual bool pageChanging(QWidget *from, QWidget *to);

	private:
		QString		m_file;
		QString		m_data;
		DataTypes	m_dataTypes;
		KexiKSpreadSource   *m_srcWidget;
};

#endif
