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

#include <qpixmap.h>

#include <koApplication.h>

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <klineeditdlg.h>
#include <kmessagebox.h>
#include <kgenericfactory.h>

#include "kexitablepart.h"
#include "kexitablepartproxy.h"
#include "kexiprojecthandleritem.h"
#include "kexidatatable.h"
#include "kexialtertable.h"

#include <kexidberror.h>

KexiTablePart::KexiTablePart(QObject *project,const char *,const QStringList &)
 : KexiProjectHandler(KEXIPROJECT(project)),KexiDataProvider()
{
	kdDebug() << "KexiTablePart::KexiTablePart()" << endl;

	getTables();
}

void KexiTablePart::hookIntoView(KexiView *view)
{
	KexiTablePartProxy *prx=new KexiTablePartProxy(this,view);
	insertIntoViewProxyMap(view,prx);
}

QString
KexiTablePart::name()
{
	return i18n("Tables");
}

QString
KexiTablePart::mime()
{
	return QString("kexi/table");
}

bool
KexiTablePart::visible()
{
	return true;
}

QPixmap
KexiTablePart::groupPixmap()
{
	return kapp->iconLoader()->loadIcon("tables", KIcon::Small);
}

QPixmap
KexiTablePart::itemPixmap()
{
	return kapp->iconLoader()->loadIcon("table", KIcon::Small);
}

void
KexiTablePart::getTables()
{
	m_tableNames.clear();
	m_tableNames = kexiProject()->db()->tables();
	kdDebug() << "KexiTablePart::getTables(): kexiProject()->db()->tables()" << endl;
	ItemList *list=items();
	list->clear();

	for ( QStringList::Iterator it = m_tableNames.begin(); it != m_tableNames.end(); ++it )
	{
		kdDebug() << "KexiTablePart::getTables() added " << (*it) << endl;
		list->insert("kexi/table/" + (*it), new KexiProjectHandlerItem(this, (*it), "kexi/table", (*it)));
	}


	emit itemListChanged(this);
}

QWidget *
KexiTablePart::embeddReadOnly(QWidget *w, KexiView *v)
{
	return 0;
}

KexiDBRecord *KexiTablePart::records(const QString& identifier,Parameters params)
{
	kdDebug()<<"KexiDBRecord *KexiTablePart::records(const QString& identifier,Parameters params)"<<endl;

	KexiDBRecord *m_record=0;
        try
        {
                m_record = kexiProject()->db()->queryRecord("select * from " + items()->find(identifier)->name(), false);
        }
        catch(KexiDBError &err)
        {
                kdDebug() << "KexiTablePart::executeQuery(): db-error" << endl;
                err.toUser(0);
                return 0;
        }
	return m_record;

}

QStringList KexiTablePart::fields(const QString& identifier)
{
	return kexiProject()->db()->getColumns(identifier);
}

QStringList KexiTablePart::datasets()
{
	return m_tableNames;
}

QStringList KexiTablePart::datasetNames()
{
	return m_tableNames;
}


K_EXPORT_COMPONENT_FACTORY( kexihandler_table, KGenericFactory<KexiTablePart> )

#include "kexitablepart.moc"
