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

#include "kexitablelist.h"

unsigned short KexiTableList::charTable[]=
{
	#include "chartable.txt"
};

/* in the Kexitablelist.h
KexiTableList::KexiTableList() {};
*/

/* in the Kexitablelist.h
KexiTableList::~KexiTableList() {};
*/

void KexiTableList::setSorting(int key, bool order=true, short type=1)
{
	m_key = key;
	m_order = (order ? 1 : -1);
	m_type = type;
	switch(m_type)
	{
		case QVariant::Bool:
		case QVariant::Int:
			cmpFunc = &KexiTableList::cmpInt;
			break;
		default:
			cmpFunc = &KexiTableList::cmpStr;
	}
}

int KexiTableList::compareItems(Item item1, Item item2)
{
	return (KexiTableList::cmpFunc)(item1, item2);
}

int KexiTableList::cmpInt(Item item1, Item item2)
{
	return m_order* (((KexiTableItem *)item1)->getInt(m_key) - ((KexiTableItem *)item2)->getInt(m_key));
}


int KexiTableList::cmpStr(Item item1, Item item2)
{
	const QString &as =((KexiTableItem *)item1)->getText(m_key);
	const QString &bs =((KexiTableItem *)item2)->getText(m_key);

	const QChar *a = as.unicode();
	const QChar *b = bs.unicode();

	if ( a == b )
		return 0;
	if ( a == 0 )
		return 1;
	if ( b == 0 )
		return -1;

	unsigned short au;
	unsigned short bu;

	int l=QMIN(as.length(),bs.length());

	au = a->unicode();
	bu = b->unicode();
	au = (au <= 0x17e ? charTable[au] : 0xffff);
	bu = (bu <= 0x17e ? charTable[bu] : 0xffff);

	while (l-- && au == bu)
	{
		a++,b++;
		au = a->unicode();
		bu = b->unicode();
		au = (au <= 0x17e ? charTable[au] : 0xffff);
		bu = (bu <= 0x17e ? charTable[bu] : 0xffff);
	}

	if ( l==-1 )
		return m_order*(as.length()-bs.length());

	return m_order*(au-bu);
}


