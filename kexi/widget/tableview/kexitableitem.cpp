/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#include "kexitableitem.h"
#include <kexi_global.h>

#include <kdebug.h>

KexiTableItem::KexiTableItem(int numCols)
: KexiTableItemBase(numCols)
{
//js	m_userData=0;
//js	m_columns.resize(numCols);
//js	m_insertItem = false;
//js	m_pTable = 0;
}

KexiTableItem::~KexiTableItem()
{
}

void
KexiTableItem::init(int numCols)
{
	clear();
	resize(numCols);
}

void
KexiTableItem::clearValues()
{
	init(count());
}

void
KexiTableItem::debug() const
{
	QString s = QString("KexiTableItem (%1 items)").arg(size());
	for (int i = 0; i < size(); i++)
		s.append( QString::number(i)+":"+at(i).toString()+" " );
	kexidbg << s << endl;
}
