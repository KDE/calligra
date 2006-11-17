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

//Added by qt3to4:
#include <QKeyEvent>
#include <QEvent>

class KPushButton;
class KLineEdit;
class KexiComboBoxPopup;
class KexiTableItem;
class KexiTableViewColumn;

/*! @short Drop-down cell editor.
*/
class KexiComboBoxTableEdit : public KexiInputTableEdit, public KexiComboBoxBase
{
	Q_OBJECT

	public:
		KexiComboBoxTableEdit(KexiTableViewColumn &column, QWidget *parent=0);
		virtual ~KexiComboBoxTableEdit();

		//! Implemented for KexiComboBoxBase
		virtual KexiTableViewColumn *column() const { return m_column; }

		//! Implemented for KexiComboBoxBase
		virtual KexiDB::Field *field() const { return m_column->field(); }

		//! Implemented for KexiComboBoxBase
		virtual QVariant origValue() const { return m_origValue; }

		virtual void setValueInternal(const QVariant& add, bool removeOld)
			{ KexiComboBoxBase::setValueInternal(add, removeOld); }

		virtual QVariant value() { return KexiComboBoxBase::value(); }

		virtual void clear();

		virtual bool valueChanged();

		virtual QVariant visibleValue();

		/*! Reimplemented: resizes a view(). */
		virtual void resize(int w, int h);

		virtual void showFocus( const QRect& r, bool readOnly );

		virtual void hideFocus();

		virtual void paintFocusBorders( QPainter *p, QVariant &cal, int x, int y, int w, int h );

		/*! Setups contents of the cell. As a special case, if there is lookup field schema 
		 defined, \a val already contains the visible value (usually the text)
		 set by \ref KexiTableView::paintcell(), so there is noo need to lookup the value 
		 in the combo box's popup. */
		virtual void setupContents( QPainter *p, bool focused, const QVariant& val, 
			QString &txt, int &align, int &x, int &y_offset, int &w, int &h );

		/*! Used to handle key press events for the item. */
		virtual bool handleKeyPress( QKeyEvent *ke, bool editorActive );

		virtual int widthForValue( QVariant &val, const QFontMetrics &fm );
	
		virtual void hide();
		virtual void show();

		/*! \return total size of this editor, including popup button. */
		virtual QSize totalSize() const;

		virtual void createInternalEditor(KexiDB::QuerySchema& schema);

		/*! Reimplemented after KexiInputTableEdit. */
		virtual void handleAction(const QString& actionName);

		/*! Reimplemented after KexiInputTableEdit. 
		 For a special case (combo box), \a visibleValue can be provided,
		 so it can be copied to the clipboard instead of unreadable \a value. */
		virtual void handleCopyAction(const QVariant& value, const QVariant& visibleValue);

	public slots:
		//! Implemented for KexiDataItemInterface
		virtual void moveCursorToEnd();

		//! Implemented for KexiDataItemInterface
		virtual void moveCursorToStart();

		//! Implemented for KexiDataItemInterface
		virtual void selectAll();

	protected slots:
		void slotButtonClicked();
		void slotRowAccepted(KexiTableItem *item, int row) { KexiComboBoxBase::slotRowAccepted(item, row); }
		void slotItemSelected(KexiTableItem* item) { KexiComboBoxBase::slotItemSelected(item); }
		void slotInternalEditorValueChanged(const QVariant& v)
			{ KexiComboBoxBase::slotInternalEditorValueChanged(v); }
		void slotLineEditTextChanged(const QString& s);
		void slotPopupHidden();

	protected:
		//! internal
		void updateFocus( const QRect& r );

		virtual bool eventFilter( QObject *o, QEvent *e );

		//! Implemented for KexiComboBoxBase
		virtual QWidget *internalEditor() const;

		//! Implemented for KexiComboBoxBase
		virtual void moveCursorToEndInInternalEditor();

		//! Implemented for KexiComboBoxBase
		virtual void selectAllInInternalEditor();

		//! Implemented for KexiComboBoxBase
		virtual void setValueInInternalEditor(const QVariant& value);

		//! Implemented for KexiComboBoxBase
		virtual QVariant valueFromInternalEditor();

		//! Implemented for KexiComboBoxBase
		virtual void editRequested() { KexiInputTableEdit::editRequested(); }

		//! Implemented for KexiComboBoxBase
		virtual void acceptRequested() { KexiInputTableEdit::acceptRequested(); }

		//! Implemented for KexiComboBoxBase
		virtual QPoint mapFromParentToGlobal(const QPoint& pos) const;

		//! Implemented for KexiComboBoxBase
		virtual int popupWidthHint() const;

		//! Implemented this to update button state. 
		virtual void updateButton();

		virtual KexiComboBoxPopup *popup() const;
		virtual void setPopup(KexiComboBoxPopup *popup);

		class Private;
		Private *d;
};

KEXI_DECLARE_CELLEDITOR_FACTORY_ITEM(KexiComboBoxEditorFactoryItem)

#endif
