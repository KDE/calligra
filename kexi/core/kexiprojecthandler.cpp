/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002   Joseph Wenninger <jowenn@kde.org>

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

#include "kexiprojecthandler.h"
#include "kexiproject.h"
#include "kexiprojecthandlerproxy.h"

#include <kdebug.h>

KexiProjectHandler::KexiProjectHandler(KexiProject *project)
 : QObject(project)
{
	m_project=project;
	m_items = new ItemList();
	m_items->setAutoDelete(true);	

	if(project)
		project->registerProjectHandler(this);
}

KexiProjectHandlerProxy *KexiProjectHandler::proxy(KexiView *view)
{
	ViewProxyMap::iterator it=m_viewProxyMap.find(view);
	return ((it==m_viewProxyMap.end())?0:(*it));
}

void KexiProjectHandler::insertIntoViewProxyMap(KexiView* view, KexiProjectHandlerProxy* proxy)
{
	if (m_viewProxyMap.find(view)!=m_viewProxyMap.end())
	{
		kdError()<<"You must not add a proxy for a view, which already has one for this part. **** Be prepared for a crash"<<endl;
		return;
	}
	m_viewProxyMap.insert(view,proxy);
}

void KexiProjectHandler::deleteFromViewProxyMap(KexiView* view)
{
        ViewProxyMap::iterator it=m_viewProxyMap.find(view);
	if (it!=m_viewProxyMap.end())
	{
		delete (*it);
		m_viewProxyMap.remove(it);
	}
}

KexiProject *KexiProjectHandler::kexiProject()
{
	return m_project;
}

void KexiProjectHandler::unhookFromView(KexiView* view)
{
	deleteFromViewProxyMap(view);
}

QWidget *KexiProjectHandler::embeddReadOnly(QWidget *, KexiView *)
{
	kdDebug() << "KexiProjectHandler::embeddReadOnly" << endl;
	return 0;
}

KexiProjectHandler::ItemList*
KexiProjectHandler::items()
{
	return m_items;
}

void KexiProjectHandler::saveXML(QDomDocument&){;}
void KexiProjectHandler::loadXML(const QDomDocument&, const QDomElement&){;}
void KexiProjectHandler::store (KoStore*){;}
void KexiProjectHandler::load (KoStore*){;}

QString KexiProjectHandler::localIdentifier(const QString &globalIdentifier)
{
	if (!globalIdentifier.startsWith("kexi/")) return globalIdentifier;
	QString tmp=globalIdentifier;
	tmp=tmp.right(tmp.length()-tmp.find("/")-1);
	tmp=tmp.right(tmp.length()-tmp.find("/")-1);
	return tmp;
}
QString KexiProjectHandler::globalIdentifier(const QString &localIdentifier)
{
	if (localIdentifier.startsWith("kexi/")) return localIdentifier;
	return mime()+"/"+localIdentifier;
}

#include "kexiprojecthandler.moc"
