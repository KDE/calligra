/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>

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
 
   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#ifndef KEXITABLELIST_H
#define KEXITABLELIST_H

#include <qptrlist.h>

#include "kexitableitem.h"
/**re-implements QPtrList to allow sorting
  *@author till busch
*/

typedef QPtrList<KexiTableItem>		KexiTableListBase;

class KexiTableList : public KexiTableListBase
{
public: 
	KexiTableList() : m_key(0), m_order(true), m_type(1) {}
	~KexiTableList() {/* qDebug("~List"); */}
	void setSorting(int key, bool order=true, short type=1);
protected:
	virtual int compareItems(Item item1, Item item2);

	int cmpStr(Item item1, Item item2);
	int cmpInt(Item item1, Item item2);

	int			m_key;
	short		m_order;
	short		m_type;
	static unsigned short charTable[];
	
	int (KexiTableList::*cmpFunc)(void *, void *);
};

#endif
