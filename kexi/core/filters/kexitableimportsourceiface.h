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

#ifndef _KEXI_TABLE_IMPORTER_SOURCE_INTERFACE_
#define _KEXI_TABLE_IMPORTER_SOURCE_INTERFACE_

#include <qstringlist.h>
#include <qvariant.h>
#include <kexiDB/kexidbtable.h>
#include <qwidget.h>
#include <qptrlist.h>

#define KEXITABLEIMPORTSOURCEIFACE(obj) (obj?static_cast<KexiTableImportSourceIface*>(\
		obj->qt_cast("KexiTableImportSourceIface")):0)

class KEXIFILTER_EXPORT KexiTableImportSourceIface {
public:
	KexiTableImportSourceIface() {}
	virtual ~KexiTableImportSourceIface() {}

	virtual KexiDBTable tableStructure()=0;

	virtual bool firstTableRow()=0;
	virtual bool nextTableRow()=0;
	virtual QVariant tableValue(int field)=0;

	virtual QPtrList<QWidget> tableSourceWidgets(QWidget *parent)=0;
};
#endif
