/* This file is part of the KDE project
   Copyright (C) 2002   Joseph Wenninger <jowenn@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qpixmap.h>

#include <koApplication.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <kiconloader.h>

#include "kexirelationdialog.h"
#include "kexirelationhandler.h"
#include "kexirelationhandlerproxy.h"
#include <kparts/genericfactory.h>

KexiRelationHandler::KexiRelationHandler(QObject *project,const char *,const QStringList &)
 : KexiProjectHandler(KEXIPROJECT(project))
{
	kdDebug() << "KexiRelationHandler::KexiRelationHandler()" << endl;
}

QString
KexiRelationHandler::name()
{
	return i18n("Relations");
}

QString
KexiRelationHandler::groupName()
{
	return i18n("Relations");
}

QString
KexiRelationHandler::mime()
{
	return QString("kexi/relations");
}

bool
KexiRelationHandler::visible()
{
	return false;
}

QPixmap
KexiRelationHandler::groupPixmap()
{
	return QPixmap();
}

QPixmap
KexiRelationHandler::itemPixmap()
{
	return kapp->iconLoader()->loadIcon("relation", KIcon::Small);
}

void KexiRelationHandler::hookIntoView(KexiView *view)
{
    KexiRelationHandlerProxy *prx=new KexiRelationHandlerProxy(this,view);
    insertIntoViewProxyMap(view,prx);
}

QWidget *
KexiRelationHandler::embeddReadOnly(QWidget *parent, KexiView *v, QString identifier)
{
	KexiRelationDialog *d = new KexiRelationDialog(v, identifier, parent, "embedded-redit", true);
	d->setIcon(itemPixmap());
	kdDebug() << "KexiRelationHandler::embeddReadOnly [" << d << "]" << endl;
	return d;
}

void
KexiRelationHandler::store(KoStore *store)
{
	kdDebug() << "KexiRelationHandler::store" << endl;
	kexiProject()->relationManager()->storeRelations(store);
}

void
KexiRelationHandler::load(KoStore *store)
{
	kdDebug() << "KexiRelationHandler::load" << endl;
	kexiProject()->relationManager()->loadRelations(store);
}


K_EXPORT_COMPONENT_FACTORY( kexihandler_relation, KGenericFactory<KexiRelationHandler> )


#include "kexirelationhandler.moc"
