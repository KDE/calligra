/* This file is part of the KDE project
   Copyright (C) 2002 Peter Simonsson <psn@linux.se>
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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

#include <kexidataiteminterface.h>

#include <qvariant.h>
#include <qscrollview.h>

#include "kexitableviewdata.h"

namespace KexiDB {
	class Field;
}

/*! @short Abstract class for a cell editor.
*/
class KEXIDATATABLE_EXPORT KexiTableEdit : public QWidget, public KexiDataItemInterface
{
	Q_OBJECT

	public:
		KexiTableEdit(KexiTableViewColumn &column, QScrollView* parent = 0, const char* name = 0);
//		KexiTableEdit(KexiDB::Field &f, QScrollView* parent = 0, const char* name = 0);

		virtual ~KexiTableEdit();

/* moved to KexiDataItemInterface*/
		/*! Just initializes \a value, and calls init(const QString& add, bool removeOld). 
		 If \a removeOld is true, current value is set up as \a add.
		 If \a removeOld if false, current value is set up as \a value + \a add.
		 \a value is stored as 'old value' -it'd be usable in the future
		 (e.g. Combo Box editor can use old value if current value does not 
		 match any item on the list).
		 Called by KexiTableView and others. */
/*		void init(const QVariant& value, const QString& add, bool removeOld = false);

		//! \return true if editor's value is changed (compared to original value)
		virtual bool valueChanged();

		//! \return true if editor's value is null (not empty)
		virtual bool valueIsNull() = 0;

		//! \return true if editor's value is empty (not null). 
		//! Only few field types can accept "EMPTY" property 
		//! (check this with KexiDB::Field::hasEmptyProperty()), 
		virtual bool valueIsEmpty() = 0;
*/

/* moved to KexiDataItemInterface
		virtual QVariant value(bool &ok) = 0;
*/

		//! Implemented for KexiDataItemInterface.
		//! \return field information for this item
		virtual KexiDB::Field *field() const { return m_column->field(); }

		//! Implemented for KexiDataItemInterface.
		//! Does nothing because instead KexiTableViewColumn is used to get field's schema.
		virtual void setField(KexiDB::Field *field) { }

		//! \return column information for this item 
		//! (extended information, comparing to field()).
		inline KexiTableViewColumn *column() const { return m_column; }

//moved to KexiDataItemInterface
		/*! \return true if internal editor's cursor (whatever that means, eg. line edit cursor)
		 is at the beginning of editor's contents. This can inform table view that 
		 after pressing "left arrow" key should stop editing and move to cell at the left 
		 hand of the current cell. */
//		virtual bool cursorAtStart() = 0;

//moved to KexiDataItemInterface
		/*! \return true if internal editor's cursor (whatever that means, eg. line edit cursor)
		 is at the end of editor's contents. This can inform table view that 
		 after pressing "right arrow" key should stop editing and move to cell at the right 
		 hand of the current cell. */
//		virtual bool cursorAtEnd() = 0;

		/*! Reimplemented: resizes a view(). */
		virtual void resize(int w, int h);

		/*! \return the view widget of this editor, e.g. line edit widget. */
		virtual QWidget* widget() { return m_view; }

		/*! Hides item's widget, if available. */
		inline virtual void hideWidget() { hide(); }

		/*! Shows item's widget, if available. */
		inline virtual void showWidget() { show(); }

//moved to KexiDataItemInterface
		//! clears editor's data, so the data will contain NULL data
//		virtual void clear() = 0;

//moved to KexiDataItemInterface
#if 0
		/*! Displays additional elements that are needed for indicating that the current cell
		 is selected. For example, combobox editor (KexiComboBoxTableEdit) moves and shows
		 dropdown button. \a r is the rectangle for the cell. 
		 For reimplementation. By default does nothing. */
		virtual void showFocus( const QRect& r );

		/*! Hides additional elements that are needed for indicating that the current cell
		 is selected. 
		 For reimplementation. By default does nothing. */
		virtual void hideFocus();
#endif

		/*! Paints a border for the cell described by \a x, \a y, \a w, \a h on \a p painter.
		 The cell's value is \a val (may be usefull if you want to reimplement this method).
		*/
		virtual void paintFocusBorders( QPainter *p, QVariant &cal, int x, int y, int w, int h );

		/*! For reimplementation.
		 Sets up anmd paints cell's contents using context of \a val value. 
		 \a focused is true if the cell is focused. \a align is set using Qt::AlignmentFlags.
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
		virtual bool handleKeyPress( QKeyEvent * /*ke*/, bool /*editorActive*/ ) { return false; }

		/*! \return width of \a value. For the default implementation \a val is converted to a string 
		 and width of this string is returned. */
		virtual int widthForValue( QVariant &val, QFontMetrics &fm );

		/*! \return total size of this editor, including any buttons, etc. (if present). 
		 Reimpelment this if you want to return more appropriate size. This impelmentation just
		 returns QWidget::size(). */
		virtual QSize totalSize() { return QWidget::size(); }

//moved to KexiDataItemInterface
		/*! \return true if this editor offers a widget (e.g. line edit) that we can move focus to.
		 Editor for boolean values has this set to false (see KexiBoolTableEdit). 
		 You can override this flag by changing m_hasFocusableWidget in your subclass' constructor. */
//		inline bool hasFocusableWidget() const { return m_hasFocusableWidget; }

//moved to KexiDataItemInterface
		/*! Allows to define reaction for clicking on cell's contents. 
		 Currently it's used for editor of type boolean, where we want to toggle true/false
		 on single mouse click. \sa hasFocusableWidget(), KexiBoolTableEdit. 
		 Default implementation does nothing. */
//		virtual void clickedOnContents() {};

//moved to KexiDataItemInterface
		/*! \return true if editing should be accepted immediately after
		 deleting contents for the cell (usually using Delete key).
		 This flag is false by default, and is true e.g. for date, time and datetime types. */
//		bool acceptEditorAfterDeleteContents() const { return m_acceptEditorAfterDeleteContents; }

	signals:
		void editRequested();
		void cancelRequested();
		void acceptRequested();

	protected:
#if 0
//moved to KexiDataItemInterface
		/*! Initializes this editor with \a add value, which should be somewhat added to the current
		 value (already storted in m_origValue). 
		 If \a removeOld is true, a value should be set to \a add, otherwise 
		 -it should be set to current \a m_origValue + \a add, if possible.
		 Implement this. */
		virtual void init(const QVariant& add, bool removeOld) = 0;
#endif
		virtual bool eventFilter(QObject* watched, QEvent* e);

		/*! Sets \a v as view widget for this editor. The view will be assigned as focus proxy
		 for the editor, its events will be filtered, it will be resized when neede, and so on. */
		void setViewWidget(QWidget *v);

		/*! Moves child widget within the viewport. Use this for child widgets that 
		 are outside of this editor widget, instead of calling QWidget::move(). */
		void moveChild( QWidget * child, int x, int y ) {
			m_scrollView->moveChild(child, x, y); }

	//		virtual void paintEvent( QPaintEvent *pe );
//moved to KexiDataItemInterface: QVariant m_origValue;
		KexiTableViewColumn *m_column;
//		KexiDB::Field *m_field;
//		int m_type; //! one of KexiDB::Field
		int m_leftMargin;
		int m_rightMargin;

		QScrollView* m_scrollView;

//moved to KexiDataItemInterface: 		bool m_hasFocusableWidget : 1;
//moved to KexiDataItemInterface:		bool m_acceptEditorAfterDeleteContents : 1;
	private:
		QWidget* m_view;
};

#endif
