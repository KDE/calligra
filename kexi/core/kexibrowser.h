/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>

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

#include <klistview.h>

#include <qdict.h>

/**
  *@author lucijan busch
  */

class QListViewItem;

class KIconLoader;
class KexiBrowserItem;
class KexiView;
class KexiMainWindow;

namespace KexiPart
{
	class Info;
	class Item;
}

typedef QDict<KexiBrowserItem> BaseItemList;

class KEXICORE_EXPORT KexiBrowser : public KListView
{
	Q_OBJECT

	public:
		KexiBrowser(KexiMainWindow *parent, QString mime, KexiPart::Info *part, const char *name=0);

	public slots:
		void		addGroup(KexiPart::Info *info);
		void		addItem(KexiPart::Item item);

	protected slots:
		void		slotContextMenu(KListView*, QListViewItem *i, const QPoint &point);
		void		slotItemListChanged(KexiPart::Info *);
		void		slotExecuteItem(QListViewItem *item);

	private:
//		KexiView	*m_view;
		KexiPart::Info	*m_part;
		QString		m_mime;
		KexiMainWindow	*m_parent;
		BaseItemList	m_baseItems;

};

#endif
