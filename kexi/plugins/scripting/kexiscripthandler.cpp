/* This file is part of the KDE project
Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

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

#include <qpixmap.h>

#include <kgenericfactory.h>
#include <kdebug.h>
#include <kparts/genericfactory.h>
#include <kiconloader.h>
#include <klineeditdlg.h>

#include <koApplication.h>
#include <koStore.h>

#include "kexiqsahost.h"
#include "kexiqsaeventhandler.h"
#include "kexiscripthandlerproxy.h"
#include "kexiscripthandler.h"

KexiScriptHandler::KexiScriptHandler(QObject *project, const char *, const QStringList &)
 : KexiProjectHandler(KEXIPROJECT(project))
{
	kdDebug() << "KexiScriptHandler::KexiScriptHandler()" << endl;
	kexiProject()->addFileReference(FileReference("scripts", "qsa", "/script/project.qsa"));
	m_host = new KexiQSAHost(this, "qsa-host");
	m_eventHandler = new KexiQSAEventHandler(this, m_host);
}

QString
KexiScriptHandler::name()
{
	return i18n("Scripts");
}

QString
KexiScriptHandler::mime()
{
	return QString("kexi/script");
}

bool
KexiScriptHandler::visible()
{
	return true;
}

QPixmap
KexiScriptHandler::groupPixmap()
{
	return SmallIcon("scripting");
}

QPixmap
KexiScriptHandler::itemPixmap()
{
	return SmallIcon("moc_src");
}

void
KexiScriptHandler::store(KoStore *store)
{
	kdDebug() << "KexiScriptHandler::store()" << endl;

		kdDebug() << "KexiScriptHandler::store(): changes in scripts" << endl;

		store->open("/script/project.qsa");
		store->write(m_host->getProjectData());
		store->close();
}

void
KexiScriptHandler::load(KoStore *store)
{
	kdDebug() << "KexiScriptHandler::load()" << endl;

	if(store->open("/script/project.qsa"))
	{
		QByteArray buffer = store->device()->readAll();
		m_host->setProjectData(buffer);
		store->close();

		QStringList files(m_host->scriptFiles());
		for(QStringList::Iterator it = files.begin(); it != files.end(); ++it)
		{
			kdDebug() << "KexiScriptHandler::load(): subfiles: " << *it << endl;
			KexiProjectHandlerItem *i = new KexiProjectHandlerItem(this, (*it), "kexi/script",(*it));
			items()->insert("kexi/script/" + (*it), i);
			emit itemListChanged(this);
		}
	}
	else
	{
		kdDebug() << "KexiScriptHandler::load(): scriptfile doesn't exist" << endl;
		m_host->setProjectData(QByteArray());
	}
}

void
KexiScriptHandler::hookIntoView(KexiView *view)
{
	KexiScriptHandlerProxy *prx=new KexiScriptHandlerProxy(this, view);
	insertIntoViewProxyMap(view, prx);
}

K_EXPORT_COMPONENT_FACTORY(kexihandler_script, KGenericFactory<KexiScriptHandler>)

#include "kexiscripthandler.moc"
