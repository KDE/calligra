/***************************************************************************
                          kexibrowser.h  -  description
                             -------------------
    begin                : Sun Jun 9 2002
    copyright            : (C) 2002 by lucijan busch
    email                : lucijan@gmx.at
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
