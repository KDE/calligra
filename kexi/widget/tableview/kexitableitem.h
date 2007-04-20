/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#ifndef KEXITABLEITEM_H
#define KEXITABLEITEM_H

#include <kexi_export.h>
#include <qstring.h>
#include <qdatetime.h>
#include <q3valuevector.h>
#include <qvariant.h>

#include <kexidb/connection.h>

typedef KexiDB::RowData KexiTableItemBase;

class KEXIDATATABLE_EXPORT KexiTableItem : public KexiTableItemBase
{
	public:
		~KexiTableItem();

		/*! Clears existing column values and inits new \a numCols 
		 columns with empty values. ist of values is resized to \a numCols. */
		void init(int numCols);

		/*! Clears existing column values, current number of columns is preserved. */
		void clearValues();

		/*! Prints debug string for this item. */
		void debug() const;

	protected:
		KexiTableItem(int numCols);

	friend class KexiTableViewData;
};

#endif
