/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIPROJECTPARTPROXY_H
#define KEXIPROJECTPARTPROXY_H

#include <qobject.h>
#include <qguardedptr.h>

#include "kexiprojecthandler.h"
#include "kexiproject.h"
#include "kexihandlerpopupmenu.h"
#include "kexiview.h"

class KexiPartPopupMenu;
class KexiPartItemAction;

class KEXICORE_EXPORT KexiProjectHandlerProxy : public QObject
{
	Q_OBJECT

	public:
		KexiProjectHandlerProxy(KexiProjectHandler *part, KexiView *view);
		virtual ~KexiProjectHandlerProxy() {};

//		virtual KexiPartPopupMenu	*groupContext() = 0;
//		virtual KexiPartPopupMenu	*itemContext(const QString &identifier) = 0;

		KexiView *kexiView()const;
		KexiProjectHandler *part()const;

		virtual KexiPartPopupMenu* groupContextMenu() { return m_group_pmenu; }
		virtual KexiPartPopupMenu* itemContextMenu(const QString& identifier) { return m_item_pmenu; }

	public slots:
		virtual bool executeItem(const QString& identifier)=0;

	protected:
		QGuardedPtr<KexiView> m_view;
		KexiProjectHandler *m_part;

		/*! popup menu for part group */
		KexiPartPopupMenu *m_group_pmenu;
		/*! popup menu for selected part item */
		KexiPartPopupMenu *m_item_pmenu;

		/*! these are most common actions that could be implemented */
		KAction *m_createAction;
		KexiPartItemAction *m_openAction, *m_editAction, *m_deleteAction;
};

#endif
