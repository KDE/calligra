/* This file is part of the KDE project
   Copyright (C) 2002   Peter Simonsson <psn@linux.se>
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

#ifndef _KEXICOMBOBOXTABLEEDIT_H_
#define _KEXICOMBOBOXTABLEEDIT_H_

#include "kexidb/field.h"
//#include "kexitableedit.h"
#include "kexiinputtableedit.h"
//#include "kexicelleditorfactory.h"

class KPushButton;
class KLineEdit;
class KexiComboBoxPopup;
class KexiTableItem;
class KexiTableViewColumn;

/**
 * 
 **/
class KexiComboBoxTableEdit : public KexiInputTableEdit
{
	Q_OBJECT

	public:
		KexiComboBoxTableEdit(KexiTableViewColumn &column, QScrollView *parent=0);
		virtual ~KexiComboBoxTableEdit();

		//! Note: Generally in current implementation this is integer > 0; may be null if no value is set
		virtual QVariant value(bool &ok);

		virtual void clear();
//		virtual bool cursorAtStart();
//		virtual bool cursorAtEnd();

		virtual bool valueChanged();
		virtual bool valueIsNull();
		virtual bool valueIsEmpty();

		/*! Reimplemented: resizes a view(). */
		virtual void resize(int w, int h);

		virtual void showFocus( const QRect& r );

		virtual void hideFocus();

		virtual void paintFocusBorders( QPainter *p, QVariant &cal, int x, int y, int w, int h );

		virtual void setupContents( QPainter *p, bool focused, QVariant val, 
			QString &txt, int &align, int &x, int &y_offset, int &w, int &h );

//		virtual int rightMargin();

		virtual bool handleKeyPress( QKeyEvent *ke, bool editorActive );

		virtual int widthForValue( QVariant &val, QFontMetrics &fm );

	public:
		virtual void hide();
		virtual void show();

		/*! \return total size of this editor, including popup button. */
		virtual QSize totalSize() const;

	protected slots:
		void slotButtonClicked();
		void showPopup();
		void slotRowAccepted(KexiTableItem *item, int row);
		void slotItemSelected(KexiTableItem*);
		void slotLineEditTextChanged(const QString &newtext);
		void slotPopupHidden();

	protected:
		//! internal
		void updateFocus( const QRect& r );

		virtual void init(const QString& add, bool removeOld);

		virtual bool eventFilter( QObject *o, QEvent *e );

		class Private;
		Private *d;
};

class KexiComboBoxEditorFactoryItem : public KexiCellEditorFactoryItem
{
	public:
		KexiComboBoxEditorFactoryItem();
		virtual ~KexiComboBoxEditorFactoryItem();

	protected:
		virtual KexiTableEdit* createEditor(KexiTableViewColumn &column, QScrollView* parent = 0);
};

#endif
