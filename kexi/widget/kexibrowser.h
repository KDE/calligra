/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIBROWSER_H
#define KEXIBROWSER_H

#include <klistview.h>
#include <qasciidict.h>
#include <qintdict.h>

class QListViewItem;
class KIconLoader;
class KPopupMenu;
class KAction;
class KActionMenu;
class KActionCollection;
class KListView;
class KToolBar;
class KexiBrowserItem;
class KexiView;
class KexiMainWindow;
class KexiSmallToolButton;
class KexiBrowserListView;

namespace KexiPart
{
	class Info;
	class Item;
	class Part;
}

//! @short Main Kexi Navigator Widget
class KEXIEXTWIDGETS_EXPORT KexiBrowser : public QWidget
{
	Q_OBJECT

	public:
		KexiBrowser(KexiMainWindow *mainWin);
		virtual ~KexiBrowser(); 

		KexiPart::Item* selectedPartItem() const;

		void installEventFilter ( const QObject * filterObj );
		virtual bool eventFilter ( QObject *o, QEvent * e );

		bool actionEnabled(const QCString& actionName) const;
//		virtual bool isExecuteArea( const QPoint& point );

	public slots:
		void addGroup(KexiPart::Info& info);
		void addItem(KexiPart::Item& item);
		void slotRemoveItem(const KexiPart::Item &item);
		virtual void setFocus();
		void updateItemName(KexiPart::Item& item, bool dirty);
		void highlightItem(KexiPart::Item& item);
//		virtual void rename(QListViewItem *item, int c);
		void clear();

		//! Sets by main window to disable actions that may try to modify the project.
		//! Does not disable actions like opening objects.
		void setReadOnly(bool set);

		bool isReadOnly() const;

	signals: 
		void openItem( KexiPart::Item*, int viewMode );

		/*! this signal is emmited when user double clicked (or single -depending on settings)
		 or pressed return ky on the part item.
		 This signal differs from openItem() signal in that if the object is already opened
		 in view mode other than \a viewMode, the mode is not changed. */
		void openOrActivateItem( KexiPart::Item*, int viewMode );

		void newItem( KexiPart::Info* );

		void removeItem( KexiPart::Item* );

		void renameItem( KexiPart::Item *item, const QString& _newName, bool &succes );

		void selectionChanged( KexiPart::Item* item );

		void executeItem( KexiPart::Item* );

		void exportItemAsDataTable( KexiPart::Item* );

		void printItem( KexiPart::Item* );

		void pageSetupForItem( KexiPart::Item* );

//		void actionAvailable(const char *name, bool avail);

	protected slots:
		void slotContextMenu(KListView*, QListViewItem *i, const QPoint &point);
		void slotExecuteItem(QListViewItem *item);
		void slotSelectionChanged(QListViewItem* i);
		void slotSettingsChanged(int);
		void slotNewObjectPopupAboutToShow();

		void slotNewObject();
		void slotOpenObject();
		void slotDesignObject();
		void slotEditTextObject();
		//! remove current item
		void slotRemove();
		void slotCut();
		void slotCopy();
		void slotPaste();
		void slotRename();
		void slotExecuteObject();
		void slotExportAsDataTable();
		void slotPrintItem();
		void slotPageSetupForItem();

	protected:
		void itemRenameDone();

		KexiMainWindow *m_mainWin;
		KexiBrowserListView *m_list;
		KActionCollection *m_actions;
		QAsciiDict<KexiBrowserItem> m_baseItems;
		QIntDict<KexiBrowserItem> m_normalItems;
		KPopupMenu *m_itemPopup, *m_partPopup;
		KAction *m_deleteAction, *m_renameAction, 
			*m_newObjectAction, // *m_newObjectToolbarAction,
			*m_openAction, *m_designAction, *m_editTextAction,
			*m_executeAction,
			*m_dataExportAction, *m_printAction, *m_pageSetupAction;
		KActionMenu* m_exportActionMenu;
		KPopupMenu* m_newObjectPopup;
		int m_itemPopupTitle_id, m_partPopupTitle_id, 
			m_openAction_id, m_designAction_id, m_editTextAction_id,
			m_executeAction_id,
			m_exportActionMenu_id, m_exportActionMenu_id_sep,
			m_printAction_id, m_pageSetupAction_id, m_pageSetupAction_id_sep;

		KexiPart::Part *m_prevSelectedPart;
		KToolBar *m_toolbar;
		KexiSmallToolButton *m_newObjectToolButton, *m_deleteObjectToolButton;
		bool m_singleClick : 1;
		bool m_readOnly : 1;
};

#endif
