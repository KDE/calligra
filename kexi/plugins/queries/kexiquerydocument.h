/* This file is part of the KDE project
   Copyright (C) 2004   Lucijan Busch <lucijan@kde.org>

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

#ifndef KEXIQUERYDOCUMENT_H
#define KEXIQUERYDOCUMENT_H

#include <qobject.h>

namespace KexiDB
{
	class QuerySchema;
}

/**
 * this class stores the current data for queries (to be shared between the different views)
 * and can save/load queries
 */
class KexiQueryDocument
{
	public:
		KexiQueryDocument(KexiDB::QuerySchema *s);
		~KexiQueryDocument();

		void			setSchema(KexiDB::QuerySchema *s) { m_schema = s; }
		KexiDB::QuerySchema	*schema() { return m_schema; }

		void			addHistoryItem(const QString &time, const QString &query, const QString &error);

	private:
		KexiDB::QuerySchema	*m_schema;
};

#endif

