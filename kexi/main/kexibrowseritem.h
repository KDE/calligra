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

#include "kexipartitem.h"

namespace KexiPart
{
	class Info;
};

class KEXIMAIN_EXPORT KexiBrowserItem : public KListViewItem
{
	public:
		/*constructor for listviews in seperated lists */
/*		KexiBrowserItem(KListView *parent, KexiProjectHandlerProxy  *proxy);
		KexiBrowserItem(KListView *parent, KexiProjectHandlerItem *item);*/
		/*constructor for main db listing */

//		KexiBrowserItem(KListView *parent, QString mime, QString name, int identifier, KexiPart::Info *i);
//		KexiBrowserItem(KListViewItem *parent, QString mime, QString name, int identifier);

		KexiBrowserItem(KListView *parent, KexiPart::Info *i);
		KexiBrowserItem(KListViewItem *parent, KexiPart::Info *i, KexiPart::Item *item);
//		KexiBrowserItem(KListViewItem *parent, KexiProjectHandlerItem *item);

		~KexiBrowserItem() {};

//js		QString			mime();
//js		QString			name();
//js		int			identifier() { return m_identifier; }

		void			clearChildren();

		KexiPart::Info	*info() { return m_info; }

		//! can be null
		KexiPart::Item* item() { return m_item; }

	protected:
		void initItem();
		virtual QString key( int column, bool ascending ) const;
		
//		KexiProjectHandlerItem	*m_item;
//		KexiProjectHandlerProxy	*m_proxy;
//js		QString			m_mime;
//js		QString			m_name;
//js		int			m_identifier;
		KexiPart::Info *m_info;
		KexiPart::Item *m_item;

		QString m_sortKey;
		bool m_fifoSorting : 1;
};

#endif

