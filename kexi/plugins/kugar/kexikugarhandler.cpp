/*  This file is part of the KDE project
    Copyright (C) 2002,2003 Joseph Wenninger <jowenn@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Library General Public License version 2 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
#include <koStore.h>
#include <koDocument.h>
#include <kparts/componentfactory.h>

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
	return i18n("Reports");
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

void KexiKugarHandler::editReport(KexiView *view,const QString &identifier)
{
	KexiKugarHandlerItem *item=(KexiKugarHandlerItem*)((*items())[localIdentifier(identifier)]);
	if (!item->designer()) {
		//some error handling
	} else {
		(void) new KexiKugarDesignerWrapper(view,0,"kugar_editview",item,false);
		kexiProject()->setModified(true);
	}
}

void KexiKugarHandler::createReport(KexiView *view)
{
    bool ok = false;
    QString name = KLineEditDlg::getText(i18n("New Report"), i18n("Report name:"), "", &ok, view);

    if(ok && name.length() > 0)
    {
		QString id=nextID();
		KexiKugarHandlerItem *item;
        items()->insert(id,item=new KexiKugarHandlerItem(this, name, "kexi/reports", id));
		if (!item->designer(true))
		{
			items()->remove(id);
			return;
		}
		KexiKugarDesignerWrapper *kw=new KexiKugarDesignerWrapper(view,0,"kugar_editview",item,true);
        kexiProject()->setModified(true);
        emit itemListChanged(this);
    }


//        KexiKugarWrapper *kw = new KexiKugarWrapper(kexiView(), 0, "identifier", "kugar_view");
//      kw->show();
}

void KexiKugarHandler::view(KexiView *view, const QString &identifier)
{
        KexiKugarHandlerItem *item=(KexiKugarHandlerItem*)((*items())[localIdentifier(identifier)]);
        if (item) {
		item->view(view);
	} else {
		kdDebug()<<"Unable to find item with identifier "<<identifier<<endl;
		return;
	}
}

QString KexiKugarHandler::tempPath()
{
	return m_tempDir->name();
}

void KexiKugarHandler::store (KoStore *ks)
{
    kdDebug() << "KexiKugarHandler::store(KoStore*)" << endl;

    for(KexiProjectHandler::ItemIterator it(*items());it.current();++it)
    {
		KexiKugarHandlerItem *khi=static_cast<KexiKugarHandlerItem*>(it.current());
		khi->store(ks);
    }
}

void KexiKugarHandler::saveXML(QDomDocument& domDoc)
{
    QDomElement reports = domDoc.createElement("reports");
    domDoc.documentElement().appendChild(reports);

    for(KexiProjectHandler::ItemIterator it(*(items()));it.current();++it)
    {
		QDomElement itemElement=domDoc.createElement("report");
		itemElement.setAttribute("name",(*it)->name());
		itemElement.setAttribute("id",(*it)->shortIdentifier());
		reports.appendChild(itemElement);
    }
}

void KexiKugarHandler::loadXML(const QDomDocument &doc, const QDomElement &elem)
{
	kdDebug()<<"KexiKugarHandler::loadXML *****"<<endl;
    ItemList *list=items();
    list->clear();

	if (elem.tagName()=="reports") {
		for (QDomElement el=elem.firstChild().toElement();!el.isNull();el=el.nextSibling().toElement()) {
            KexiKugarHandlerItem *item;
            QString id=el.attribute("id");
            list->insert(id,item=new KexiKugarHandlerItem(this, el.attribute("name"), "kexi/reports", id));
			if (id>=nextFreeID) {
				nextFreeID=id;
				nextID();
			}
		}
	}
        emit itemListChanged(this);

}

void KexiKugarHandler::load (KoStore *ks)
{
	kdDebug()<<"KexiKugarHandler::load ******"<<endl;
	QString tmp=tempPath();
	if (tmp.isEmpty()) return;
    References fileRefs = kexiProject()->fileReferences("reports");

    for(References::Iterator it = fileRefs.begin(); it != fileRefs.end(); it++)
    {
        kdDebug() << "KexiKugarHandler::load() added " << (*it).name << endl;
		ks->extractFile((*it).location,tmp+(*it).name);
    }
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_kugar, KGenericFactory<KexiKugarHandler> )


#include "kexikugarhandler.moc"
