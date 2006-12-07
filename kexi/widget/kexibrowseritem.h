/* This file is part of the KDE project
   Copyright (C) 2002-2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIBROWSERITEM_H
#define KEXIBROWSERITEM_H

#include <k3listview.h>
#include <qstring.h>

#include <core/kexipartitem.h>

namespace KexiPart
{
	class Info;
}

//! @internal
class KexiBrowserItem : public K3ListViewItem
{
	public:
		KexiBrowserItem(K3ListView *parent, KexiPart::Info *i);
		KexiBrowserItem(K3ListViewItem *parent, KexiPart::Info *i, KexiPart::Item *item);

		void clearChildren();

		KexiPart::Info *info() const { return m_info; }

		//! \return part item. Can be null if the browser item is a "folder", i.e. a parent node.
		KexiPart::Item* item() const { return m_item; }

		void updateItemName();
	protected:
		void initItem();
		virtual QString key( int column, bool ascending ) const;
		
		KexiPart::Info *m_info;
		KexiPart::Item *m_item;

		QString m_sortKey;
		bool m_fifoSorting : 1;
};

#endif
