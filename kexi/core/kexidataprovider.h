/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>

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

#ifndef _KEXI_DATA_PROVIDER_H_
#define _KEXI_DATA_PROVIDER_H_

#include <qstring.h>
#include <qmap.h>
#include <klocale.h>
#include <qstringlist.h>

class KexiDBRecord;
class QWidget;

#define KEXIDATAPROVIDER(obj) ((obj)?static_cast<KexiDataProvider*>((obj)->qt_cast("KexiDataProvider")):0)

//Interface KexiDataProvider
class KexiDataProvider
{
public:
	class Parameter{
	public:
		Parameter(const QString& name_, const int type_){name=name_;type=type_;}
		Parameter(){}
		enum Type{Unknown=0,Text=1,Float=2,Int=3,Date=4,Time=5,DateTime=6,TheLastOne=0xff};
		static const char* typeNames[];
		static const char* typeDescription[];
		static const int maxType;
		QString name;
		int type;
	};
	typedef QValueList<Parameter> ParameterList;

	typedef QMap<QString,QString> Parameters;

	KexiDataProvider();
	virtual ~KexiDataProvider();
	virtual QStringList datasets(QWidget*) {return QStringList();}
	virtual QStringList datasetNames(QWidget*) { return QStringList();}
	virtual QStringList fields(QWidget*,const QString& identifier) {return QStringList();}
	virtual KexiDBRecord *records(QWidget*,const QString& identifier,Parameters params) {return 0;}
	virtual ParameterList parameters(QWidget*,const QString &identifier) { return ParameterList();}
};

#endif
