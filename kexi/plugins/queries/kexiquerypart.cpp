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

#include <koApplication.h>

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <klineeditdlg.h>
#include <kmessagebox.h>
#include <kgenericfactory.h>
#include "kexiquerydesigner.h"
#include "kexiquerypart.h"
#include "kexiquerypartproxy.h"

KexiQueryPart::KexiQueryPart(QObject *project,const char *,const QStringList &)
 : KexiProjectPart(KEXIPROJECT(project))
{
	kdDebug() << "KexiQueryPart::KexiQueryPart()" << endl;
}

QString
KexiQueryPart::name()
{
	return QString(i18n("Queries"));
}

QString
KexiQueryPart::mime()
{
	return QString("kexi/query");
}

bool
KexiQueryPart::visible()
{
	return true;
}

QPixmap
KexiQueryPart::groupPixmap()
{
	return kapp->iconLoader()->loadIcon(QString("queries"), KIcon::Small);
}

QPixmap
KexiQueryPart::itemPixmap()
{
	return kapp->iconLoader()->loadIcon(QString("queries"), KIcon::Small);
}

void
KexiQueryPart::getQueries()
{
	References fileRefs = kexiProject()->fileReferences("Queries");
	ItemList *list=items();
	list->clear();

	for(References::Iterator it = fileRefs.begin(); it != fileRefs.end(); it++)
	{
		kdDebug() << "KexiQueryPart::getQueries() added " << (*it).name << endl;
		list->append(new KexiProjectPartItem(this, (*it).name, "kexi/query", (*it).name));
	}
	
	emit itemListChanged(this);
}


void KexiQueryPart::hookIntoView(KexiView *view)
{
        KexiQueryPartProxy *prx=new KexiQueryPartProxy(this,view);
        insertIntoViewProxyMap(view,prx);
}


K_EXPORT_COMPONENT_FACTORY( kexihandler_query, KGenericFactory<KexiQueryPart> )


#include "kexiquerypart.moc"
