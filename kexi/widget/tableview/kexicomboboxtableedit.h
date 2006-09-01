/* This file is part of the KDE project
   Copyright (C) 2002   Peter Simonsson <psn@linux.se>
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef _KEXICOMBOBOXTABLEEDIT_H_
#define _KEXICOMBOBOXTABLEEDIT_H_

#include "kexidb/field.h"
#include "kexiinputtableedit.h"
#include "kexicomboboxbase.h"
#include <kexidb/lookupfieldschema.h>

class KPushButton;
class KLineEdit;
class KexiComboBoxPopup;
class KexiTableItem;
class KexiTableViewColumn;

/*! @short Drop-down cell editor.
*/
class KexiComboBoxTableEdit : public KexiInputTableEdit, KexiComboBoxBase
{
	Q_OBJECT

	public:
		KexiComboBoxTableEdit(KexiTableViewColumn &column, QScrollView *parent=0);
		virtual ~KexiComboBoxTableEdit();

		//! Implemented for KexiComboBoxBase
		virtual KexiTableViewColumn *column() const { return m_column; }

		//! Implemented for KexiComboBoxBase
		virtual KexiDB::Field *field() const { return m_column->field(); }

		//! Implemented for KexiComboBoxBase
		virtual QVariant origValue() const { return m_origValue; }

		virtual QVariant value() { return KexiComboBoxBase::value(); }

		virtual void clear();
//		virtual bool cursorAtStart();
//		virtual bool cursorAtEnd();

		virtual bool valueChanged();
//moved		virtual bool valueIsNull();
//moved		virtual bool valueIsEmpty();
		virtual QVariant visibleValueForLookupField();

		/*! Reimplemented: resizes a view(). */
		virtual void resize(int w, int h);

		virtual void showFocus( const QRect& r, bool readOnly );

		virtual void hideFocus();

		virtual void paintFocusBorders( QPainter *p, QVariant &cal, int x, int y, int w, int h );

		virtual void setupContents( QPainter *p, bool focused, const QVariant& val, 
			QString &txt, int &align, int &x, int &y_offset, int &w, int &h );

//		virtual int rightMargin();

		virtual bool handleKeyPress( QKeyEvent *ke, bool editorActive );

		virtual int widthForValue( QVariant &val, const QFontMetrics &fm );

	public:
		virtual void hide();
		virtual void show();

		/*! \return total size of this editor, including popup button. */
		virtual QSize totalSize() const;

	protected slots:
		void slotButtonClicked();
//moved		void createPopup(bool show);
//moved		void showPopup();
		void slotRowAccepted(KexiTableItem *item, int row) { KexiComboBoxBase::slotRowAccepted(item, row); }
		void slotItemSelected(KexiTableItem* item) { KexiComboBoxBase::slotItemSelected(item); }
		void slotLineEditTextChanged(const QString &newtext) { KexiComboBoxBase::slotLineEditTextChanged(newtext); }
		void slotPopupHidden();

	protected:
		//! internal
		void updateFocus( const QRect& r );

//moved		virtual void setValueInternal(const QVariant& add, bool removeOld);

		virtual bool eventFilter( QObject *o, QEvent *e );

		void updateTextForHighlightedRow();

		//! Used to select row item for an user-entered text \a str.
		//! Only for "lookup table" mode.
//moved		KexiTableItem* selectItemForStringInLookupTable(const QString& str);

//		//! \return value (col #1 of related data) - only reasonable for 'related table data' model
//		QString valueForID(const QVariant& val);

		/*! \return value from \a returnFromColumn related to \a str value from column \a lookInColumn.
		 If \a allowNulls is true, NULL is returend if no matched column found, else: 
		 \a str is returned.
		 Example: lookInColumn=0, returnFromColumn=1 --returns user-visible string 
		 for column #1 for id-column #0 */
//moved		QString valueForString(const QString& str, int* row, uint lookInColumn, 
//moved			uint returnFromColumn, bool allowNulls = false);


		//! sets \a text for the line edit without setting a flag (d->userEnteredText) that indicates that 
		//! the text has been entered by hand (by a user)
//moved		void setLineEditText(const QString& text);

//moved		KexiDB::LookupFieldSchema* lookupFieldSchema() const;

//moved		int rowToHighlightForLookupTable() const;

		//! Implemented for KexiComboBoxBase
		virtual QWidget *internalEditor() const;

		//! Implemented for KexiComboBoxBase
		virtual void moveCursorToEndInInternalEditor();

		//! Implemented for KexiComboBoxBase
		virtual void selectAllInInternalEditor();

		//! Implemented for KexiComboBoxBase
		virtual void setValueInInternalEditor(const QVariant& value);

		//! Implemented for KexiComboBoxBase
		virtual QVariant valueFromInternalEditor() const;

		//! Implemented for KexiComboBoxBase
		virtual void editRequested() { KexiInputTableEdit::editRequested(); }

		//! Implemented for KexiComboBoxBase
		virtual void acceptRequested() { KexiInputTableEdit::acceptRequested(); }

		//! Implemented for KexiComboBoxBase
		virtual QPoint mapFromParentToGlobal(const QPoint& pos) const;

		//! Implemented for KexiComboBoxBase
		virtual int popupWidthHint() const;

		class Private;
		Private *d;
};

KEXI_DECLARE_CELLEDITOR_FACTORY_ITEM(KexiComboBoxEditorFactoryItem)

#endif
