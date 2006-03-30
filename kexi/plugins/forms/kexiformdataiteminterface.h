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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXIFORMDATAITEMINTERFACE_H
#define KEXIFORMDATAITEMINTERFACE_H

#include <kexidataiteminterface.h>
#include <qwidget.h>
//Added by qt3to4:
#include <Q3CString>

namespace KexiDB {
	class Field;
}

//! An interface for declaring form widgets to be data-aware.
class KEXIFORMUTILS_EXPORT KexiFormDataItemInterface : public KexiDataItemInterface
{
	public:
		KexiFormDataItemInterface();
		virtual ~KexiFormDataItemInterface();

		//! \return the name of the data source for this widget.
		//! Data source usually means here a table or query, a field name or an expression.
		inline QString dataSource() const { return m_dataSource; }

		//! Sets the name of the data source for this widget.
		//! Data source usually means here a table or query or field name name.
		inline void setDataSource(const QString &ds) { m_dataSource = ds; }

		/*! \return the mime type of the data source for this widget.
		 Data source mime type means here types like "kexi/table" or "kexi/query"
		 in.the data source is set to object (as within form or subform) or is empty
		 if the data source is set to table field or query column. */
		inline Q3CString dataSourceMimeType() const { return m_dataSourceMimeType; }

		/*! Sets the mime type of the data source for this widget.
		 Data source usually means here a "kexi/table" or "kexi/query".
		 @see dataSourceMimeType() */
		inline void setDataSourceMimeType(const Q3CString &ds) { m_dataSourceMimeType = ds; }

		/*! Convenience function: casts this item to a QWidget.
		 Can return 0 if the item is not a QWidget-derived object. */
		virtual QWidget* widget() { return dynamic_cast<QWidget*>(this); }

		/*! Sets 'invalid' state, e.g. a text editor widget should display
		 text \a displayText and become read only to prevent entering data,
		 because updating at the database backend is not available.
		 \a displayText is usually set to something i18n'd like "#NAME?".
		 Note: that even widgets that usualy do not display texts (e.g. pixmaps)
		 should display \a displayText too.
		*/
		virtual void setInvalidState( const QString& displayText ) = 0;

		//! \return database column information for this item
		virtual KexiDB::Field* field() const;

		//! \return database column information for this item
		virtual KexiDB::QueryColumnInfo* columnInfo() const { return m_columnInfo; }

		/*! Used internally to set database column information.
		 Reimplement if you need to do additional actions,
		 e.g. set data validator based on field type. Don't forget about
		 calling superclass implementation. */
		virtual void setColumnInfo(KexiDB::QueryColumnInfo* cinfo) { m_columnInfo = cinfo; }

		/*! Does nothing, because within forms, widgets are always visible. */
		virtual void hideWidget() { }

		/*! Does nothing, because within forms, widgets are always visible. */
		virtual void showWidget() { }

		/*! Undoes changes made to this item - just resets to original value. */
		void undoChanges();

	protected:
		QString m_dataSource;
		Q3CString m_dataSourceMimeType;
		KexiDB::QueryColumnInfo* m_columnInfo;

	friend class KexiDBAutoField;
};

#endif
