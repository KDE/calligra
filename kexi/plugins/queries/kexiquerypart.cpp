/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <klineeditdlg.h>
#include <kmessagebox.h>
#include <kgenericfactory.h>
#include "kexiquerydesigner.h"
#include "kexiquerypart.h"
#include "kexiquerypartproxy.h"
#include "kexiquerypartitem.h"

KexiQueryPart::KexiQueryPart(QObject *project,const char *,const QStringList &)
 : KexiProjectHandler(KEXIPROJECT(project)),KexiDataProvider()
{
	kdDebug() << "KexiQueryPart::KexiQueryPart()" << endl;
}

QString
KexiQueryPart::name()
{
	return i18n("Queries");
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
	return kapp->iconLoader()->loadIcon("queries", KIcon::Small);
}

QPixmap
KexiQueryPart::itemPixmap()
{
	return kapp->iconLoader()->loadIcon("query", KIcon::Small);
}


void KexiQueryPart::store (KoStore *ks)
{
	kdDebug() << "KexiQueryPart::store(KoStore*)" << endl;

	for(KexiProjectHandler::ItemIterator it(*items());it.current();++it)
	{
		KexiQueryPartItem *qpi=static_cast<KexiQueryPartItem*>(it.current());
		qpi->store(ks);
	}
}

void KexiQueryPart::load (KoStore *ks)
{
	References fileRefs = kexiProject()->fileReferences("Queries");
	ItemList *list=items();
	list->clear();

	for(References::Iterator it = fileRefs.begin(); it != fileRefs.end(); it++)
	{
		kdDebug() << "KexiQueryPart::getQueries() added " << (*it).name << endl;
		KexiQueryPartItem *qpi;
		qpi=new KexiQueryPartItem(this, (*it).name, "kexi/query", (*it).name);
    list->insert( qpi->fullIdentifier(), qpi );
//		list->insert( (*it).name,qpi=new KexiQueryPartItem(this, (*it).name, "kexi/query", (*it).name));
		qpi->load(ks);
	}

	emit itemListChanged(this);

}

void KexiQueryPart::hookIntoView(KexiView *view)
{
    KexiQueryPartProxy *prx=new KexiQueryPartProxy(this,view);
    insertIntoViewProxyMap(view,prx);
}

#if 0
	virtual QStringList datasets() {return QStringList();}
	virtual QStringList datasetNames() { return QStringList();}
	virtual QStringList fields(const QString& identifier) {return QStringList();}
	virtual ParameterList parameters(const QString &identifier) { return ParameterList();}
#endif

QStringList KexiQueryPart::datasets(QWidget*) {
	QStringList list;

	for(KexiProjectHandler::ItemIterator it(*items());it.current();++it) {
		list <<it.current()->identifier();
	}

	return list;
}

KexiDBRecordSet *KexiQueryPart::records(QWidget* dpar,const QString& identifier,Parameters params) {
	QString shortID=localIdentifier(identifier);

	KexiProjectHandlerItem *it=(*items())[shortID];
	if (it) {
		return (dynamic_cast<KexiQueryPartItem*>(it))->records(dpar,params);
	}
	return 0;
}

QStringList KexiQueryPart::fields(QWidget*,const QString& identifier) {
	QString shortID=localIdentifier(identifier);
	KexiProjectHandlerItem *it=(*items())[shortID];
	if (it) {
		return (dynamic_cast<KexiQueryPartItem*>(it))->m_fields;
	}
	kdDebug()<<"KexiQueryPart::fields(): couldn't find shortID "<<shortID<<endl;
	QStringList list;
	return list;
}

QStringList KexiQueryPart::datasetNames(QWidget*) {
	QStringList list;

	for(KexiProjectHandler::ItemIterator it(*items());it.current();++it) {
		list <<it.current()->name();
	}

	return list;

}


K_EXPORT_COMPONENT_FACTORY( kexihandler_query, KGenericFactory<KexiQueryPart> )


#include "kexiquerypart.moc"
