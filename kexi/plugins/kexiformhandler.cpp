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

#include <qpixmap.h>

#include <kgenericfactory.h>
#include <kdebug.h>
#include <kparts/genericfactory.h>
#include <kiconloader.h>
#include <klineeditdlg.h>

#include <koApplication.h>

#include "kexiformhandler.h"
#include "kexiformhandlerproxy.h"

KexiFormHandler::KexiFormHandler(QObject *project,const char *,const QStringList &)
 : KexiProjectHandler(KEXIPROJECT(project))
{
	kdDebug() << "KexiFormHandler::KexiFormHandler()" << endl;
}

QString
KexiFormHandler::name()
{
	return i18n("Forms");
}

QString
KexiFormHandler::mime()
{
	return QString("kexi/form");
}

bool
KexiFormHandler::visible()
{
	return true;
}

QPixmap
KexiFormHandler::groupPixmap()
{
	return kapp->iconLoader()->loadIcon("forms", KIcon::Small);
}

QPixmap
KexiFormHandler::itemPixmap()
{
	return kapp->iconLoader()->loadIcon("form", KIcon::Small);
}

void
KexiFormHandler::hookIntoView(KexiView *view)
{
	KexiFormHandlerProxy *prx=new KexiFormHandlerProxy(this, view);
	insertIntoViewProxyMap(view, prx);
}

K_EXPORT_COMPONENT_FACTORY(kexihandler_form, KGenericFactory<KexiFormHandler>)

#include "kexiformhandler.moc"
