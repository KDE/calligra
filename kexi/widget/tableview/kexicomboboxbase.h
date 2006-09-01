/* This file is part of the KDE project
   Copyright (C) 2002 Peter Simonsson <psn@linux.se>
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

#ifndef _KEXICOMBOBOXBASE_H_
#define _KEXICOMBOBOXBASE_H_

#include "kexidb/field.h"
#include "kexiinputtableedit.h"
#include <kexidb/lookupfieldschema.h>

class KPushButton;
class KLineEdit;
class KexiComboBoxPopup;
class KexiTableItem;
class KexiTableViewColumn;

/*! @short A base class for handling data-aware combo boxes.
 This class is used by KexiComboBoxTableEdit and KexiDBComboBox.
*/
class KEXIDATATABLE_EXPORT KexiComboBoxBase
{
	public:
		KexiComboBoxBase();
		virtual ~KexiComboBoxBase();

		//! \return column related to this combo; for KexiComboBoxTableEdit 0 is returned here
		virtual KexiTableViewColumn *column() const = 0;

		//! \return database field related to this combo
		virtual KexiDB::Field *field() const = 0;

		//! \return the original value
		virtual QVariant origValue() const = 0;

		//! Note: Generally in current implementation this is integer > 0; may be null if no value is set
		virtual QVariant value();

		//! Reimplement this and call this impl.: used to clear internal editor
		virtual void clear();

//		virtual bool cursorAtStart();
//		virtual bool cursorAtEnd();

		virtual tristate valueChangedInternal();
		virtual bool valueIsNull();
		virtual bool valueIsEmpty();
		virtual QVariant visibleValueForLookupField();

		/*! Reimplemented: resizes a view(). */
//moved		virtual void resize(int w, int h);

//moved		virtual void showFocus( const QRect& r, bool readOnly );

//moved		virtual void hideFocus();

//moved		virtual void paintFocusBorders( QPainter *p, QVariant &cal, int x, int y, int w, int h );

//moved		virtual void setupContents( QPainter *p, bool focused, const QVariant& val, 
//			QString &txt, int &align, int &x, int &y_offset, int &w, int &h );

//moved		virtual bool handleKeyPress( QKeyEvent *ke, bool editorActive );

//moved		virtual int widthForValue( QVariant &val, const QFontMetrics &fm );

	public:
		virtual void hide();
//moved		virtual void show();

		/*! \return total size of this editor, including popup button. */
//moved		virtual QSize totalSize() const;

//	protected slots:
//moved		void slotButtonClicked();

		void createPopup(bool show);

		void showPopup();
		
		//! Call this from slot
		virtual void slotRowAccepted(KexiTableItem *item, int row);
		
		//! Call this from slot
		virtual void slotItemSelected(KexiTableItem*);
		
		void slotLineEditTextChanged(const QString &newtext);
//moved		void slotPopupHidden();

	protected:
		//! internal
//moved		void updateFocus( const QRect& r );

		virtual void setValueInternal(const QVariant& add, bool removeOld);

//moved		virtual bool eventFilter( QObject *o, QEvent *e );

//moved		void updateTextForHighlightedRow();

		//! Used to select row item for an user-entered text \a str.
		//! Only for "lookup table" mode.
		KexiTableItem* selectItemForStringInLookupTable(const QString& str);

//		//! \return value (col #1 of related data) - only reasonable for 'related table data' model
//		QString valueForID(const QVariant& val);

		/*! \return value from \a returnFromColumn related to \a str value from column \a lookInColumn.
		 If \a allowNulls is true, NULL is returend if no matched column found, else: 
		 \a str is returned.
		 Example: lookInColumn=0, returnFromColumn=1 --returns user-visible string 
		 for column #1 for id-column #0 */
		QString valueForString(const QString& str, int* row, uint lookInColumn, 
			uint returnFromColumn, bool allowNulls = false);

		//! sets \a value for the line edit without setting a flag (m_userEnteredText) that indicates that 
		//! the text has been entered by hand (by a user)
		void setValueOrTextInInternalEditor(const QVariant& value); //QString& text);

		KexiDB::LookupFieldSchema* lookupFieldSchema() const;

		int rowToHighlightForLookupTable() const;

		//! Implement this to return the internal editor
		virtual QWidget *internalEditor() const = 0;

		//! Implement this to perform "move cursor to end" in the internal editor
		virtual void moveCursorToEndInInternalEditor() = 0;

		//! Implement this to perform "select all" in the internal editor
		virtual void selectAllInInternalEditor() = 0;

		//! Implement this to perform "set value" in the internal editor
		virtual void setValueInInternalEditor(const QVariant& value) = 0;

//		//! Implement this to perform "set value or text" in the internal editor (not entered by user)
//		void setValueOrTextInInternalEditor(const QVariane& value) = 0;

		//! Implement this to return value from the internal editor
		virtual QVariant valueFromInternalEditor() const = 0;

		//! Implement this as signal
		virtual void editRequested() = 0;

		//! Implement this as signal
		virtual void acceptRequested() = 0;

		//! Implement this to return a position \a pos mapped from parent (e.g. viewport) 
		//! to global coordinates. QPoint(-1, -1) should be returned if this cannot be computed.
		virtual QPoint mapFromParentToGlobal(const QPoint& pos) const = 0;

		//! Implement this to return a hint for popup width.
		virtual int popupWidthHint() const = 0;

		QString m_userEnteredText; //!< text entered by hand (by user)
	
		bool m_internalEditorValueChanged : 1; //!< true if user has text or other value inside editor
		bool m_slotLineEditTextChanged_enabled : 1;
		bool m_mouseBtnPressedWhenPopupVisible : 1; //!< Used only by KexiComboBoxTableEdit
		KexiComboBoxPopup *m_popup;

//		class Private;
//		Private *d;
};

#endif
