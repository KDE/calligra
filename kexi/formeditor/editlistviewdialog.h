/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef EDITLISTVIEW_DIALOG_H
#define EDITLISTVIEW_DIALOG_H

#include <qintdict.h>
#include <qtoolbutton.h>
#include <kdialogbase.h>

class QFrame;
class QListView;
class QListViewItem;
class KListViewItem;
class KListView;
class KListBox;
class QListBoxItem;

class KexiProperty;
class KexiPropertyBuffer;
class KexiPropertyEditor;

namespace KFormDesigner {

class KFORMEDITOR_EXPORT EditListViewDialog : public KDialogBase
{
	Q_OBJECT

	public:
		EditListViewDialog(QWidget *parent);
		~EditListViewDialog() {;}

		int exec(QListView *listview);

	public slots:
		// Columns page
		void updateItemProperties(QListBoxItem*);
		void newItem();
		void removeItem();
		void MoveItemUp();
		void MoveItemDown();
		void changeProperty(KexiPropertyBuffer&, KexiProperty&);

		// Contents page
		void updateButtons(QListViewItem*);
		void newRow();
		void newChildRow();
		void removeRow();
		void MoveRowUp();
		void MoveRowDown();

	protected:
		void loadChildNodes(QListView *listview, QListViewItem *item, QListViewItem *parent);

	protected:
		enum { BNewRow = 10, BNewChild, BRemRow, BRowUp, BRowDown , BColAdd = 20, BColRem, BColUp, BColDown };
		KexiPropertyEditor  *m_editor;
		KexiPropertyBuffer  *m_buffer;
		QFrame   *m_contents, *m_column;
		KListBox  *m_listbox;
		KListView  *m_listview;
		QIntDict<QToolButton>  m_buttons;
};

}

#endif
