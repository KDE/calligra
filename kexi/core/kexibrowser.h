/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIBROWSER_H
#define KEXIBROWSER_H

#include "kexidockbase.h"

#include <qdict.h>

/**
  *@author lucijan busch
  */

class QListViewItem;

class KIconLoader;
class KexiBrowserItem;
class KexiView;
class KexiMainWindow;
class KPopupMenu;
class KListView;

namespace KexiPart
{
	class Info;
	class Item;
}

typedef QDict<KexiBrowserItem> BaseItemList;


class KEXICORE_EXPORT KexiBrowser : public KexiDockBase
{
	Q_OBJECT

	public:
		KexiBrowser(KexiMainWindow *parent);

		void installEventFilter ( const QObject * filterObj );

	public slots:
		void		addGroup(KexiPart::Info *info);
		void		addItem(KexiPart::Item *item);

		void clear();

	signals: 
		void openItem( KexiPart::Item*, bool designMode );
		void newItem( KexiPart::Info* );
		void removeItem( KexiPart::Item* );

//		void actionAvailable(const char *name, bool avail);

	protected slots:
		void		slotContextMenu(KListView*, QListViewItem *i, const QPoint &point);
//		void		slotItemListChanged(KexiPart::Info *);
		void		slotExecuteItem(QListViewItem *item);
		void slotSelectionChanged(QListViewItem* i);
//		void slotClicked(QListViewItem* i);

		void slotNewObject();
		void slotOpenObject();
		void slotDesignObject();
		//! remove current item
		void slotRemove();
		void slotCut();
		void slotCopy();
		void slotPaste();

	private:
//		KexiView	*m_view;
//		KexiPart::Info	*m_part;
//		QString		m_mime;
		BaseItemList	m_baseItems;
		KPopupMenu *m_itemPopup, *m_partPopup;
		KAction *m_newObjectAction;

		KListView *m_list;
};

#endif
