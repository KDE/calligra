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

#include "kexiprojecthandleritem.h"

KexiProjectHandlerItem::KexiProjectHandlerItem(KexiProjectHandler *parent, const QString& name, const QString& mime, 
	const QString& shortIdentifier)
 : QObject(parent, shortIdentifier.latin1())
{
	m_parent = parent;
	m_name = name;
	m_mime = mime;
	m_shortIdentifier = shortIdentifier;
}

KexiProjectHandler *
KexiProjectHandlerItem::projectPart()
{
	return m_parent;
}

QString
KexiProjectHandlerItem::name()
{
	return m_name;
}

QString
KexiProjectHandlerItem::mime()
{
	return m_mime;
}

QString
KexiProjectHandlerItem::identifier()
{
	return m_mime+"/"+m_shortIdentifier;
}

QString
KexiProjectHandlerItem::shortIdentifier()
{
	return m_shortIdentifier;
}


KexiProjectHandlerItem::~KexiProjectHandlerItem()
{
}

#include "kexiprojecthandleritem.moc"
