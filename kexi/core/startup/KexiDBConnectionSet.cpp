/* This file is part of the KDE project
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#include "KexiDBConnectionSet.h"

#include <kdebug.h>

class KexiDBConnectionSetPrivate
{
public:
	KexiDBConnectionSetPrivate()
	{
		list.setAutoDelete(true);
		maxid=-1;
	}
	KexiDB::ConnectionData::List list;
	int maxid;
};

KexiDBConnectionSet::KexiDBConnectionSet()
: d(new KexiDBConnectionSetPrivate())
{
}

KexiDBConnectionSet::~KexiDBConnectionSet()
{
	delete d;
}

void KexiDBConnectionSet::addConnectionData(KexiDB::ConnectionData *data)
{
	if (data->id<0)
		data->id = d->maxid+1;
	//TODO: 	check for id-duplicates
	
	d->maxid = QMAX(d->maxid,data->id);
	d->list.append(data);
}

KexiDB::ConnectionData::List KexiDBConnectionSet::list() const
{
	return d->list;
}

