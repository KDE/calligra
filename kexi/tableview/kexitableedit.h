/* This file is part of the KDE project
   Copyright (C) 2002 Peter Simonsson <psn@linux.se>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef _KEXITABLEEDIT_H_
#define _KEXITABLEEDIT_H_

#include <qvariant.h>
#include <qwidget.h>

namespace KexiDB {
	class Field;
}

class KEXIDATATABLE_EXPORT KexiTableEdit : public QWidget
{
	public:
		KexiTableEdit(KexiDB::Field &f, QWidget* parent = 0, const char* name = 0);

		virtual ~KexiTableEdit();

		//! Just initializes \a value, and calls init(const QString& add). Called by KexiTableView
		void init(QVariant value, const QString& add);

		//! @return true if editor's value is changed (compared to original value)
		virtual bool valueChanged();

		//! \return true is editor's value is null (not empty)
		virtual bool valueIsNull() = 0;

		//! \return true if editor's value is empty (not null). 
		//! Only few field types can accept "EMPTY" property 
		//! (check this with KexiDB::Field::hasEmptyProperty()), 
		virtual bool valueIsEmpty() = 0;

		virtual QVariant value(bool &ok) = 0;

		inline KexiDB::Field *field() const { return m_field; }

		/*! \return true if internal editor's cursor (whatever that means, eg. line edit cursor)
		 is at the beginning of editor's contents. This can inform table view that 
		 after pressing "left arrow" key should stop editing and move to cell at the left 
		 hand of the current cell. */
		virtual bool cursorAtStart() = 0;

		/*! \return true if internal editor's cursor (whatever that means, eg. line edit cursor)
		 is at the end of editor's contents. This can inform table view that 
		 after pressing "right arrow" key should stop editing and move to cell at the right 
		 hand of the current cell. */
		virtual bool cursorAtEnd() = 0;

		/*! Reimplemented: resizes a view(). */
		virtual void resize(int w, int h);

		QWidget* view() const { return m_view; }

		//! clears editor's data, so the data now contains NULL data
		virtual void clear() = 0;

	protected:
		/*! Initializes this editor with \a add value, which should be somewhat added to the current
		 value (already storted in m_origValue). Implement this. */
		virtual void init(const QString& add) = 0;

		virtual bool eventFilter(QObject* watched, QEvent* e);
		void setView(QWidget *v);

	//		virtual void paintEvent( QPaintEvent *pe );
		QVariant m_origValue;
		KexiDB::Field *m_field;
//		int m_type; //! one of KexiDB::Field

		QWidget* m_view;
};

#endif
