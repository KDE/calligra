/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXIPROJECTPART_H
#define KEXIPROJECTPART_H

#include <qobject.h>
#include <qpopupmenu.h>
#include <qdict.h>
#include <qguardedptr.h>

#include "kexiproject.h"
#include "kexidialogbase.h"
#include "kexihandlerpopupmenu.h"
#include "kexiprojecthandleritem.h"
#include "kexiview.h"

#define KEXIPROJECT(obj) (static_cast<KexiProject*>(obj->qt_cast("KexiProject")))


class KexiProjectHandlerItem;
class KexiProjectHandlerProxy;
class KexiEventHandler;
class KexiDataProvider;

/*!
 *  this is a baseclass for project parts like:
 *  queries, tables, forms, reports
 */

class KexiProjectHandler : public QObject
{
	Q_OBJECT

	public:
		typedef QDict<KexiProjectHandlerItem> ItemList;
		typedef QDictIterator<KexiProjectHandlerItem> ItemIterator;

		KexiProjectHandler(KexiProject *project);
		virtual ~KexiProjectHandler() {};

		//general information about parthandler
		virtual QString			name() = 0;
		virtual QString			mime() = 0;
		virtual bool			visible() = 0;

		KexiProjectHandlerProxy		*proxy(KexiView *);
		virtual void hookIntoView(KexiView*)=0;
		virtual void unhookFromView(KexiView*);

		virtual QWidget *embeddReadOnly(QWidget *, KexiView *v);

		virtual QPixmap			groupPixmap() = 0;
		virtual QPixmap			itemPixmap() = 0;

		virtual void saveXML(QDomDocument&);
		virtual void loadXML(const QDomDocument&, const QDomElement&);
		virtual void store (KoStore*);
		virtual void load (KoStore*);
		virtual ItemList		*items();

		virtual KexiEventHandler	*eventHandler() { return 0; }

		KexiProject *kexiProject()const;

		static QString localIdentifier(const QString &globalIdentifier);
		static QString handlerNameFromGlobalIdentifier(const QString &globalIdentifier);
		QString globalIdentifier(const QString &localIdentifier);

		virtual KexiDataProvider	*provider() { return 0; }
	signals:
		void itemListChanged(KexiProjectHandler*);

	protected:
		void insertIntoViewProxyMap(KexiView*,KexiProjectHandlerProxy*);
		void deleteFromViewProxyMap(KexiView*);
	private:
		typedef QMap<KexiView*,KexiProjectHandlerProxy*> ViewProxyMap;
		ViewProxyMap m_viewProxyMap;
		QGuardedPtr<KexiProject> m_project;
		ItemList* m_items;
};

#endif
