/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004, 2006 Jaroslaw Staniek <js@iidea.pl>

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
*/

#ifndef KEXIQUERYVIEW_H
#define KEXIQUERYVIEW_H

#include <kexidatatable.h>

namespace KexiDB
{
	class QuerySchema;
}

class KexiQueryView : public KexiDataTable
{
	Q_OBJECT

	public:
		KexiQueryView(QWidget *parent);
		virtual ~KexiQueryView();

	protected:
		virtual tristate afterSwitchFrom(Kexi::ViewMode mode);

		virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);

		virtual tristate storeData(bool dontAsk = false);

		/*! Executes query \a query, filling the table view with query results.
		 \return true on success, false on failure and cancelled when user has 
		 cancelled execution (for example when she pressed the Cancel button 
		 of the "Enter Query Parameter" input dialog. */
		tristate executeQuery(KexiDB::QuerySchema *query);

		class Private;
		Private * const d;
};

#endif

