/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

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

#ifndef KEXICSVIMPORTFILTER_H
#define KEXICSVIMPORTFILTER_H

#include "core/filters/kexifilter.h"
#include <qmap.h>

class Preview;
class KexiDB;
//class KexiCSVImport;
typedef QMap<int, int> DataTypes;

class KexiCSVImport : public KexiFilter
{
	Q_OBJECT

	enum DataType
	{
		TypeVarchar = 0,
		TypeInt = 1,
		TypeDouble = 2
	};

	public:
		KexiCSVImport(QObject *parent, const char *name, const QStringList &);
		~KexiCSVImport();

		virtual QString name();
//		virtual bool	open(KexiDB *);

		bool		parseFile(const QString &file, ImportDlg *p);

		bool import(const KURL& url,unsigned long allowedTypes);

	public slots:
		void		reparse();
		void		reparse(const QString &);

		void		colChanged(int, int);
		void		headerChanged(const QString &);
		void		typeChanged(int type);

		void		import();

	private:
		ImportDlg	*m_dlg;
		QString		m_file;
		DataTypes	m_dataTypes;
		KexiDB		*m_db;
};

#endif
