/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIBROWSERITEM_H
#define KEXIBROWSERITEM_H

#include <klistview.h>
#include <qstring.h>

class KexiProjectPart;
class KexiProjectPartItem;

class KexiBrowserItem : public KListViewItem
{
	public:
		/*constructor for listviews in seperated lists */
		KexiBrowserItem(KListView *parent, QString mime, QString name, QString identifier);
		KexiBrowserItem(KListView *parent, KexiProjectPart *part);
		KexiBrowserItem(KListView *parent, KexiProjectPartItem *item);
		/*constructor for main db listing */
		KexiBrowserItem(KListViewItem *parent, QString mime, QString name, QString identifier);
		KexiBrowserItem(KListViewItem *parent, KexiProjectPartItem *item);

		~KexiBrowserItem() {};

		KexiProjectPart		*part();
		KexiProjectPartItem	*item();
		QString			mime();
		QString			name();
		QString			identifier();
		
		void			clearChildren();

	protected:
		KexiProjectPart		*m_part;
		KexiProjectPartItem	*m_item;
		QString			m_mime;
		QString			m_name;
		QString			m_identifier;
};

#endif
