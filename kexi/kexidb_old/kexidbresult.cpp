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

#include "kexidbresult.h"
#include "kexidbfield.h"

#include <qvariant.h>

KexiDBResult::KexiDBResult(QObject *parent)
{
}

KexiDBField*
KexiDBResult::fieldInfo(unsigned int)
{
	return 0;
}

KexiDBField*
KexiDBResult::fiedlInfo(QString)
{
	return 0;
}


QVariant
KexiDBResult::value(unsigned int)
{
	return QVariant();
}

QVariant
KexiDBResult::value(QString)
{
	return QVariant();
}

bool
KexiDBResult::next()
{
	return false;
}

unsigned int
KexiDBResult::numRows()
{
	return 0;
}

KexiDBResult::~KexiDBResult()
{
}
