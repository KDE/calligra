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

#include <kexidb/connection.h>
#include <kexidb/cursor.h>

#include "widget/kexidatatable.h"
#include "keximainwindow.h"
#include "kexiproject.h"
#include <kxmlguiclient.h>


class KexiTablePartGUIClient: public QObject, public KXMLGUIClient {
public:
	KexiTablePartGUIClient(KexiMainWindow *parent): QObject(parent), KXMLGUIClient(parent) {
	        new KAction(i18n("Table..."), "createTable", 0, this, SLOT(create()), actionCollection(), "tablepart_create");
        	setXMLFile("kexitablepartui.rc");
		parent->guiFactory()->addClient(this);
	}
	virtual ~KexiTablePartGUIClient(){}

};

KexiTablePart::KexiTablePart(QObject *parent, const char *name, const QStringList &l)
 : KexiPart::Part(parent, name, l)
{
	kdDebug() << "KexiTablePart::KexiTablePart()" << endl;
}

void
KexiTablePart::execute(KexiMainWindow *win, const QString &table)
{
	KexiDB::TableSchema *sch = win->project()->dbConnection()->tableSchema(table);
	kdDebug() << "KexiTablePart::execute(): schema is " << sch << endl;
	if(!sch)
		return;

	KexiDB::Cursor *c = win->project()->dbConnection()->prepareQuery(*sch);

	kdDebug() << "KexiTablePart::execute(): cursor is " << c << endl;
	if(!c)
		return;

	KexiDataTable *t = new KexiDataTable(win, i18n("%1 - Table").arg(table), c);
}

void
KexiTablePart::createGUIClient(KexiMainWindow *win) {
	KXMLGUIClient *guiclient =new KexiTablePartGUIClient(win);

}


KexiTablePart::~KexiTablePart()
{
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_table, KGenericFactory<KexiTablePart> )

#include "kexitablepart.moc"

