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

#ifndef KEXIPROJECTPARTITEM_H
#define KEXIPROJECTPARTITEM_H

#include <qobject.h>
#include <qguardedptr.h>

#include "kexiprojecthandler.h"

/*! Project Handler Item stores 
	- KexiProjectHandler
	- identifier ident (low-level name, for example: table name)
	- mime type name
	- title (visible hight leve name, eg. table or query title)
*/
class KEXICORE_EXPORT KexiProjectHandlerItem : public QObject
{
	Q_OBJECT

	public:
		KexiProjectHandlerItem(KexiProjectHandler *item_handler, const QString& item_ident, 
		 const QString& item_mime, const QString& item_title);
		~KexiProjectHandlerItem();

		KexiProjectHandler	*handler();
		QString		identifier();
		QString		fullIdentifier();
		QString		mime();
		QString		title();

	private:
		QGuardedPtr<KexiProjectHandler> m_handler;
		QString 	m_ident;
		QString 	m_mime;
		QString 	m_title;
};

#endif
