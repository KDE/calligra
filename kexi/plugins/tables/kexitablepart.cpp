/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexitablepart.h"

#include <kdebug.h>
#include <kgenericfactory.h>
#include <kxmlguiclient.h>

#include "keximainwindow.h"
#include "kexiproject.h"
#include "widget/kexidatatable.h"
#include "widget/kexidatatableview.h"

#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexipartinfo.h>


namespace KexiPart {

class GUIClient : public QObject, public KXMLGUIClient
{
public:
	GUIClient(KexiMainWindow *win, Part* part, const QString &i18nInstanceName);
	virtual ~GUIClient() {};

	inline Part *part() { return static_cast<Part*>(parent()); }
};

GUIClient::GUIClient(KexiMainWindow *win, Part* part, const QString &i18nInstanceName)
 : QObject(win, part->info()->objectName().latin1()), KXMLGUIClient(win)
{
	new KAction(i18nInstanceName+"...", part->info()->itemIcon(), 0, this, 
		SLOT(create()), actionCollection(), part->info()->objectName()+"part_create");
		setXMLFile(QString("kexi")+part->info()->objectName()+"partui.rc");
		win->guiFactory()->addClient(this);
}

}

/*class KexiTablePartGUIClient: public KexiPart::GUIClient {
public:
	KexiTablePartGUIClient(KexiMainWindow *win, KexiPart::Part *part)
		: KexiPart::GUIClient(part, i18n("Table"))
	{
//	new KAction(i18n("Table..."), info()->itemIcon(), 0, this, SLOT(create()), actionCollection(), "tablepart_create");
//	new KAction(i18n("Table..."), "createTable", 0, this, SLOT(create()), actionCollection(), "tablepart_create");
		setXMLFile("kexitablepartui.rc");
		win->guiFactory()->addClient(this);
	}
	virtual ~KexiTablePartGUIClient(){}

};*/

KexiTablePart::KexiTablePart(QObject *parent, const char *name, const QStringList &l)
 : KexiPart::Part(parent, name, l)
{
	kdDebug() << "KexiTablePart::KexiTablePart()" << endl;
}

KexiTablePart::~KexiTablePart()
{
}

#if 0
void
KexiTablePart::execute(KexiMainWindow *win, const KexiPart::Item &item)
{
	KexiDB::TableSchema *sch = win->project()->dbConnection()->tableSchema(item.name());
	kdDebug() << "KexiTablePart::execute(): schema is " << sch << endl;
	if(!sch)
		return;

	KexiDB::Cursor *c = win->project()->dbConnection()->prepareQuery(*sch);

	kdDebug() << "KexiTablePart::execute(): cursor is " << c << endl;
	if(!c)
		return;

	KexiDataTable *t = new KexiDataTable(win, QString("%1 - %2").arg(item.name()).arg(instanceName()), c);
	t->resize(t->sizeHint());
	t->setMinimumSize(t->minimumSizeHint().width(),t->minimumSizeHint().height());

	t->setIcon( DesktopIcon( info()->itemIcon() ) );
	t->setDocID(item.identifier());
}
#endif

KexiDialogBase*
KexiTablePart::createInstance(KexiMainWindow *win, const KexiPart::Item &item)
{
	KexiDB::TableSchema *sch = win->project()->dbConnection()->tableSchema(item.name());
	kdDebug() << "KexiTablePart::execute(): schema is " << sch << endl;
	if(!sch)
		return 0;

	KexiDB::Cursor *c = win->project()->dbConnection()->prepareQuery(*sch);

	kdDebug() << "KexiTablePart::execute(): cursor is " << c << endl;
	if(!c)
		return 0;

	KexiDataTable *t = new KexiDataTable(win, QString("%1 - %2").arg(item.name()).arg(instanceName()), c);
	return t;
}


QString KexiTablePart::instanceName() const
{
	return i18n("Table");
}

void
KexiTablePart::createGUIClient(KexiMainWindow *win) {
//	KXMLGUIClient *guiclient =new KexiTablePartGUIClient(win);
	(void)new KexiPart::GUIClient(win, this, instanceName());
}


K_EXPORT_COMPONENT_FACTORY( kexihandler_table, KGenericFactory<KexiTablePart> )

#include "kexitablepart.moc"

