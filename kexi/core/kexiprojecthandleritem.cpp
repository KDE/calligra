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

KexiProjectHandlerItem::KexiProjectHandlerItem(KexiProjectHandler *item_handler, 
 const KexiIdentifier& ident, const QString& item_title)
 : QObject(item_handler, ident.objectPath().utf8())
	, KexiIdentifier(ident)
	, m_handler(item_handler)
	, m_title(item_title)
{

}

KexiProjectHandler *
KexiProjectHandlerItem::handler()
{
	return m_handler;
}

QString
KexiProjectHandlerItem::identifier()
{
	return objectID().asString();
}

QString
KexiProjectHandlerItem::mime()
{
	return objectLocation();
}

QString
KexiProjectHandlerItem::fullIdentifier()
{
	return objectPath();
}

QString
KexiProjectHandlerItem::title()
{
	return m_title;
}

KexiProjectHandlerItem::~KexiProjectHandlerItem()
{
}

#include "kexiprojecthandleritem.moc"
