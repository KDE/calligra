/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDATAITEMINTERFACE_H
#define KEXIDATAITEMINTERFACE_H

#include <qstring.h>
#include <qvariant.h>

//! An interface for declaring widgets to be data-aware.
class KEXICORE_EXPORT KexiDataItemInterface
{
	public:
		KexiDataItemInterface();
		~KexiDataItemInterface();

		//! \return the name of the data source for this widget
		//! Data source usually means here a table or query or field name name.
		QString dataSource() const { return m_dataSource; }

		//! Sets the name of the data source for this widget
		//! Data source usually means here a table or query or field name name.
		void setDataSource(const QString &ds) { m_dataSource = ds; }

		virtual void setValue(const QVariant& value) = 0;

	protected:
		QString m_dataSource;
};

#endif
