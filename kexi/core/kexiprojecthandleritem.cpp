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

#include "kexiprojectpartitem.h"

KexiProjectPartItem::KexiProjectPartItem(KexiProjectPart *parent, QString name, QString mime, QString identifier)
 : QObject(parent, identifier.latin1())
{
	m_parent = parent;
	m_name = name;
	m_mime = mime;
	m_identifier = identifier;
}

KexiProjectPart *
KexiProjectPartItem::projectPart()
{
	return m_parent;
}

QString
KexiProjectPartItem::name()
{
	return m_name;
}

QString
KexiProjectPartItem::mime()
{
	return m_mime;
}

QString
KexiProjectPartItem::identifier()
{
	return m_identifier;
}

KexiProjectPartItem::~KexiProjectPartItem()
{
}

#include "kexiprojectpartitem.moc"
