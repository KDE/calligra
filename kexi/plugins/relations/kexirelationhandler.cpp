/* This file is part of the KDE project
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

#include <qpixmap.h>

#include <koApplication.h>
#include <kgenericfactory.h>
#include <kdebug.h>

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
	return QString(i18n("Relation"));
}

QString
KexiRelationHandler::mime()
{
	return QString("kexi/relation");
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
	return QPixmap();
}

void KexiRelationHandler::hookIntoView(KexiView *view)
{
        KexiRelationHandlerProxy *prx=new KexiRelationHandlerProxy(this,view);
        insertIntoViewProxyMap(view,prx);
}



K_EXPORT_COMPONENT_FACTORY( kexihandler_relation, KGenericFactory<KexiRelationHandler> )


#include "kexirelationhandler.moc"
