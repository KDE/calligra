/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#ifndef KEXITABLEITEM_H
#define KEXITABLEITEM_H

#include <qstring.h>
#include <qdatetime.h>
#include <qvaluevector.h>
#include <qvariant.h>

#include <kexidb/connection.h>

typedef KexiDB::RowData KexiTableItemBase;

class KEXIDATATABLE_EXPORT KexiTableItem : public KexiTableItemBase
{
	public:
		KexiTableItem(int numCols);
		~KexiTableItem();

		/*! Clears exisiting column values and inits new \a numCols 
		 columns with empty values. ist of values is resized to \a numCols. */
		void init(int numCols);

		/*! Clears exisiting column values, current number of columns is preserved. */
		void clearValues();

	protected:
	private:
};

#endif
