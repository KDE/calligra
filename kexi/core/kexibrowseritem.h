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

#ifndef KEXIBROWSERITEM_H
#define KEXIBROWSERITEM_H

#include <klistview.h>
#include <qstring.h>

class KexiProjectHandlerProxy;
class KexiProjectHandlerItem;

class KEXICORE_EXPORT KexiBrowserItem : public KListViewItem
{
	public:
		/*constructor for listviews in seperated lists */
		KexiBrowserItem(KListView *parent, QString mime, QString name, QString identifier);
		KexiBrowserItem(KListView *parent, KexiProjectHandlerProxy  *proxy);
		KexiBrowserItem(KListView *parent, KexiProjectHandlerItem *item);
		/*constructor for main db listing */
		KexiBrowserItem(KListViewItem *parent, QString mime, QString name, QString identifier);
		KexiBrowserItem(KListViewItem *parent, KexiProjectHandlerItem *item);

		~KexiBrowserItem() {};

		KexiProjectHandlerItem	*item();
		KexiProjectHandlerProxy	*proxy();
		QString			mime();
		QString			name();
		QString			identifier();
				
		void			clearChildren();

	protected:
		void initItem();
		virtual QString key( int column, bool ascending ) const;
		
		KexiProjectHandlerItem	*m_item;
		KexiProjectHandlerProxy	*m_proxy;
		QString			m_mime;
		QString			m_name;
		QString			m_identifier;
		
		QString m_sortKey;
		bool m_fifoSorting : 1;
};

#endif
