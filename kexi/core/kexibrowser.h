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
class KexiProjectHandler;
class KexiProjectHandlerItem;

typedef QDict<KexiBrowserItem> BaseItemList;

class KexiBrowser : public KListView
{
	Q_OBJECT

	public:
		KexiBrowser(QWidget *parent, QString mime, KexiProjectHandler *part, const char *name=0);

	private:
		KexiView	*m_view;
		KexiProjectHandler	*m_part;
		QString		m_mime;

		BaseItemList	m_baseItems;

	public slots:
		void		addGroup(KexiProjectHandler *part);
		void		addItem(KexiProjectHandlerItem *item);

	protected slots:
		void		slotContextMenu(KListView*, QListViewItem *i, const QPoint &point);
		void		slotItemListChanged(KexiProjectHandler *);
		void		slotExecuteItem(QListViewItem *item);
};

#endif
