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
class KexiProjectPart;
class KexiProjectPartItem;

typedef QDict<KexiBrowserItem> BaseItemList;

class KexiBrowser : public KListView
{
	Q_OBJECT

	public:
		KexiBrowser(QWidget *parent, QString mime, KexiProjectPart *part, const char *name=0);
		~KexiBrowser();

	private:
		KexiView	*m_view;
		KexiProjectPart	*m_part;
		QString		m_mime;

		BaseItemList	m_baseItems;

	public slots:
		void		addGroup(KexiProjectPart *part);
		void		addItem(KexiProjectPartItem *item);

	protected slots:
		void		slotContextMenu(KListView*, QListViewItem *i, const QPoint &point);
		void		slotItemListChanged(KexiProjectPart *);
};

#endif
