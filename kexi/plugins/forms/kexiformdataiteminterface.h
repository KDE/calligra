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

#ifndef KEXIFORMDATAITEMINTERFACE_H
#define KEXIFORMDATAITEMINTERFACE_H

#include <kexidataiteminterface.h>

#include <qwidget.h>

namespace KexiDB {
	class Field;
}

//! An interface for declaring form widgets to be data-aware.
class KEXIFORMUTILS_EXPORT KexiFormDataItemInterface : public KexiDataItemInterface
{
	public:
		KexiFormDataItemInterface();
		virtual ~KexiFormDataItemInterface();

		//! \return the name of the data source for this widget
		//! Data source usually means here a table or query or field name name.
		inline QString dataSource() const { return m_dataSource; }

		//! Sets the name of the data source for this widget
		//! Data source usually means here a table or query or field name name.
		inline void setDataSource(const QString &ds) { m_dataSource = ds; }

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

		//! \return field information for this item
		virtual KexiDB::Field* field() const { return m_field; }

		/*! Used internally to set field information.
		 Reimplement if you need to do additional actions, 
		 e.g. set data validator based on field type. Don't forget about 
		 calling superclass implementation. */
		virtual void setField(KexiDB::Field* field) { m_field = field; }
		
		/*! Does nothing, because within forms, widgets are always visible. */
		virtual void hideWidget() { }

		/*! Does nothing, because within forms, widgets are always visible. */
		virtual void showWidget() { }

		/*! Undoes changes made to this item - just resets to original value. */
		void undoChanges();

	protected:
		QString m_dataSource;
		KexiDB::Field *m_field;
};

#endif
