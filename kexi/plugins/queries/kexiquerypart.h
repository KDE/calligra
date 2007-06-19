/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004,2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIQUERYPART_H
#define KEXIQUERYPART_H

#include <QMap>
#include <Q3CString>

#include <kexipart.h>
#include <kexipartitem.h>
#include <KexiWindowData.h>
#include <kexidb/queryschema.h>
#include <kexidb/connection.h>

namespace KexiDB
{
	class QuerySchema;
	class Connection;
}


//! @short Kexi Query Designer Plugin.
class KexiQueryPart : public KexiPart::Part
{
	Q_OBJECT

	public:
		KexiQueryPart(QObject *parent, const QStringList &);
		virtual ~KexiQueryPart();

		virtual bool remove(KexiPart::Item &item);

		//! @short Temporary data kept in memory while switching between Query Window's views
		class TempData : public KexiWindowData, 
		                 public KexiDB::Connection::TableSchemaChangeListenerInterface
		{
			public:
				TempData(KexiWindow* parent, KexiDB::Connection *conn);
				virtual ~TempData();
				virtual tristate closeListener();
				void clearQuery();
				void unregisterForTablesSchemaChanges();
				void registerTableSchemaChanges(KexiDB::QuerySchema *q);

				/*! Assigns query \a query for this data.
				 Existing query (available using query()) is deleted but only 
				 if it is not owned by parent window (i.e. != KexiWindow::schemaData()).
				 \a query can be 0. 
				 If \a query is equal to existing query, nothing is performed.
				*/
				void setQuery(KexiDB::QuerySchema *query);

				//! \return query associated with this data
				KexiDB::QuerySchema *query() const { return m_query; }

				//! Takes query associated with this data (without deleting) and returns it.
				//! After this call query() == 0
				KexiDB::QuerySchema *takeQuery();

				//! Connection used for retrieving definition of the query
				KexiDB::Connection *conn;

				/*! true, if \a query member has changed in previous view. 
				 Used on view switching. We're checking this flag to see if we should 
				 rebuild internal structure for DesignViewMode of regenerated sql text 
				 in TextViewMode after switch from other view. */
				bool queryChangedInPreviousView : 1;

			protected:
				KexiDB::QuerySchema *m_query;
		};

		virtual KLocalizedString i18nMessage(const QString& englishMessage, 
			KexiWindow* window) const;

		/*! Renames stored data pointed by \a item to \a newName. 
		 Reimplemented to mark the query obsolete by using KexiDB::Connection::setQuerySchemaObsolete(). */
		virtual tristate rename(KexiPart::Item & item, const QString& newName);

	protected:
		virtual KexiWindowData* createWindowData(KexiWindow* window);

		virtual KexiView* createView(QWidget *parent, KexiWindow* window, 
			KexiPart::Item &item, Kexi::ViewMode viewMode = Kexi::DataViewMode,
			QMap<QString,QString>* staticObjectArgs = 0);

//		virtual void initPartActions( KActionCollection *col );
//		virtual void initInstanceActions( int mode, KActionCollection *col );

		virtual void initPartActions();
		virtual void initInstanceActions();

		virtual KexiDB::SchemaData* loadSchemaData(KexiWindow *window, 
			const KexiDB::SchemaData& sdata, Kexi::ViewMode viewMode);
};

#endif

