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

/*! @short Abstract class for a cell editor.
*/
class KEXIDATATABLE_EXPORT KexiTableEdit : public QWidget
{
	Q_OBJECT

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

		/*! \return the view widget of this editor, e.g. line edit widget. */
		QWidget* view() const { return m_view; }

		//! clears editor's data, so the data now contains NULL data
		virtual void clear() = 0;

		/*! Displays additional elements that are needed for indicating that the current cell
		 is selected. For example, combobox editor (KexiComboBoxTableEdit) moves and shows
		 dropdown button. \a r is the rectangle for the cell. 
		 For reimplementation. By default does nothing. */
		virtual void showFocus( const QRect& r );

		/*! Hides additional elements that are needed for indicating that the current cell
		 is selected. 
		 For reimplementation. By default does nothing. */
		virtual void hideFocus();

		/*! Paints a border for the cell described by \a x, \a y, \a w, \a h on \a p painter.
		 The cell's value is \a val (may be usefull if you want to reimplement this method).
		*/
		virtual void paintFocusBorders( QPainter *p, QVariant &cal, int x, int y, int w, int h );

		/*! For reimplementation.
		 Sets up cell's contents using context of \a val value. 
		 \a focusd is true if the cell is focused. \a align is set using Qt::AlignmentFlags.
		 Some additional things may be painted using \a p,
		 it's not needed to paint the text (this is done automatically outside.

		 Before calling, \a x, \a y_offset, \a w, \a h parameters are initialized,
		 but you can tune these values depending on the context. 
		 You should set \a txt to a text representation of \a val, 
		 otherwise no text will be painted. */
		virtual void setupContents( QPainter *p, bool focused, QVariant val, 
			QString &txt, int &align, int &x, int &y_offset, int &w, int &h );

		/*! For reimplementation.
		 Paints selection's background using \a p. Most parameters are similar to these from 
		 setupContents().
		*/
		virtual void paintSelectionBackground( QPainter *p, bool focused, const QString& txt, 
			int align, int x, int y_offset, int w, int h, const QColor& fillColor,
			bool readOnly, bool fullRowSelection );

		/*! Sometimes, editor can contain non-standard margin, for example combobox editor contains
		 dropdown button at the right side. \returns left margin's size; 
		 0 by default. For reimplementation.  */
		int leftMargin() const { return m_leftMargin; }

		/*! Sometimes, editor can contain non-standard margin, for example combobox editor contains
		 dropdown button at the right side. \returns right margin;s size; 
		 0 by default. For reimplementation.  */
		int rightMargin() const { return m_rightMargin; }

		/*! Handles \a ke key event that came over the column that is bound to this editor.
		 For implementation: true should be returned if \a ke should be accepted.
		 If \a editorActive is true, this editor is currently active, i.e. the table view is in edit mode.
		 By default false is returned. */
		virtual bool handleKeyPress( QKeyEvent *ke, bool editorActive ) { return false; }

		/*! \return width of \a value. For the default implementation \a val is converted to a string 
		 and width of this string is returned. */
		virtual int widthForValue( QVariant &val, QFontMetrics &fm );

	signals:
		void editRequested();
		void cancelRequested();
		void acceptRequested();

	protected:
		/*! Initializes this editor with \a add value, which should be somewhat added to the current
		 value (already storted in m_origValue). Implement this. */
		virtual void init(const QString& add) = 0;

		virtual bool eventFilter(QObject* watched, QEvent* e);

		/*! Sets \a v as view widget for this editor. The view will be assigned as focus proxy
		 for the editor, its events will be filtered, it will be resized when neede, and so on. */
		void setView(QWidget *v);

	//		virtual void paintEvent( QPaintEvent *pe );
		QVariant m_origValue;
		KexiDB::Field *m_field;
//		int m_type; //! one of KexiDB::Field
		int m_leftMargin;
		int m_rightMargin;

	private:
		QWidget* m_view;
};

#endif
