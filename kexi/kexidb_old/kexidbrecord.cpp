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

#include <qvariant.h>

#include "kexidbrecord.h"

KexiDBRecord::KexiDBRecord(KexiDBResult*, unsigned int)
{
}

void
KexiDBRecord::reset()
{
}

bool
KexiDBRecord::commit(bool)
{
	return false;
}

QVariant
KexiDBRecord::value(unsigned int)
{
	return QVariant();
}

QVariant
KexiDBRecord::value(QString)
{
	return QVariant();
}

QVariant::Type
KexiDBRecord::type(unsigned int)
{
	return QVariant::Invalid;
}

QVariant::Type
KexiDBRecord::type(QString)
{
	return QVariant::Invalid;
}

KexiDBField::ColumnType
KexiDBRecord::sqlType(unsigned int)
{
	return KexiDBField::SQLInvalid;
}

KexiDBField::ColumnType
KexiDBRecord::sqlType(QString)
{
	return KexiDBField::SQLInvalid;
}

bool
KexiDBRecord::update(unsigned int, QVariant)
{
	return false;
}

bool
KexiDBRecord::update(QString, QVariant)
{
	return false;
}

bool
KexiDBRecord::deleteRecord()
{
	return false;
}

KexiDBRecord*
KexiDBRecord::gotoRecord(unsigned int)
{
	return 0;
}

KexiDBRecord*
KexiDBRecord::insert()
{
	return 0;
}

KexiDBRecord*
KexiDBRecord::operator++()
{
	return 0;
}

KexiDBRecord*
KexiDBRecord::operator--()
{
	return 0;
}

KexiDBRecord::~KexiDBRecord()
{
}
