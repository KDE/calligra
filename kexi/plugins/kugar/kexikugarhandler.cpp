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

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <klineeditdlg.h>
#include <kmessagebox.h>
#include <kgenericfactory.h>
#include <kstandarddirs.h>
#include "kexikugarhandler.h"
#include "kexikugarhandleritem.h"
#include "kexikugarhandlerproxy.h"
#include "kexikugarwrapper.h"
#include "kexikugardesignerwrapper.h"
#include "kexitempdir.h"

KexiKugarHandler::KexiKugarHandler(QObject *project,const char *,const QStringList &)
 : KexiProjectHandler(KEXIPROJECT(project))
{
	m_tempDir=new KexiTempDir(locateLocal("tmp", KGlobal::instance()->instanceName()+"reports"));
	m_tempDir->setAutoDelete(true);
	nextFreeID="A";
	kdDebug() << "KexiKugarHandler::KexiQueryPart()" << endl;
}

KexiKugarHandler::~KexiKugarHandler()
{
	delete m_tempDir;
}

QString
KexiKugarHandler::name()
{
	return QString(i18n("Reports"));
}

QString
KexiKugarHandler::mime()
{
	return QString("kexi/reports");
}

bool
KexiKugarHandler::visible()
{
	return true;
}

QPixmap
KexiKugarHandler::groupPixmap()
{
	return kapp->iconLoader()->loadIcon(QString("reports"), KIcon::Small);
}

QPixmap
KexiKugarHandler::itemPixmap()
{
	return kapp->iconLoader()->loadIcon(QString("reports"), KIcon::Small);
}

void KexiKugarHandler::hookIntoView(KexiView *view)
{
        KexiKugarHandlerProxy *prx=new KexiKugarHandlerProxy(this,view);
        insertIntoViewProxyMap(view,prx);
}


QString KexiKugarHandler::nextID()
{
	QString retval=nextFreeID;
	char c;
	uint i=0;
	for (;(i<nextFreeID.length()) && (nextFreeID.at(i)=='Z');i++);
	if (i<nextFreeID.length())
		nextFreeID=nextFreeID.replace(i,1,nextFreeID.at(i).latin1()+1);
	else
	{
		uint l=nextFreeID.length()+1;
		nextFreeID="";
		nextFreeID=nextFreeID.fill('A',l);
	}
	return retval;
}

void KexiKugarHandler::createReport(KexiView *view)
{
        bool ok = false;
        QString name = KLineEditDlg::getText(i18n("New Report"), i18n("Report Name:"), "", &ok, view);

        if(ok && name.length() > 0)
        {
		QString id=nextID();
		KexiKugarHandlerItem *item;
                items()->insert(id,item=new KexiKugarHandlerItem(this, name, "kexi/reports", id));
                emit itemListChanged(this);
//              project()->addFileReference("/query/" + name + ".query");
		KexiKugarDesignerWrapper *kw=new KexiKugarDesignerWrapper(view,0,"kugar_editview",item,true);
                kexiProject()->setModified(true);
        }


//        KexiKugarWrapper *kw = new KexiKugarWrapper(kexiView(), 0, "identifier", "kugar_view");
//      kw->show();

}

void KexiKugarHandler::view(KexiView *view, const QString &identifier)
{
        KexiKugarWrapper *kw = new KexiKugarWrapper(view, 0, "identifier", "kugar_view");

}

QString KexiKugarHandler::tempPath()
{
	return m_tempDir->name();
}


K_EXPORT_COMPONENT_FACTORY( kexihandler_kugar, KGenericFactory<KexiKugarHandler> )


#include "kexikugarhandler.moc"
