/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002,2003   Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003   Jaroslaw Staniek <js@iidea.pl>

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
#include <kaction.h>

#include "kexitablepartproxy.h"
#include "kexitablepart.h"
#include "kexiprojecthandleritem.h"
#include "kexidatatable.h"
#include "kexialtertable.h"
//#include "tables/kexitablefiltermanager.h"
//#include "tables/kexitableimportfilter.h"
#include "core/filters/kexifiltermanager.h"

#include "kexi_global.h"

KexiTablePartProxy::KexiTablePartProxy(KexiTablePart *part,KexiView *view)
 : KexiProjectHandlerProxy(part,view),KXMLGUIClient()
{
	setInstance(part->kexiProject()->instance());
	m_tablePart=part;
	kdDebug() << "KexiTablePartProxy::KexiTablePartProxy()" << endl;

	(void*) new KAction(i18n("Create &Table..."), "table", "",
		this,SLOT(slotCreate()), actionCollection(), "tablepart_create");

	setXMLFile("kexitablepartui.rc");

	view->insertChildClient(this);
}

KexiPartPopupMenu*
KexiTablePartProxy::groupContext()
{
	kdDebug() << "KexiTablePart::groupContext()" << endl;
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("Create Table..."), SLOT(slotCreate()));
	m->insertAction(i18n("Import Tables/Data..."),SLOT(slotImport()));

	return m;
}

KexiPartPopupMenu*
KexiTablePartProxy::itemContext(const QString& identifier)
{
	kdDebug() << "KexiTablePart::itemContext()" << endl;
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("Open Table"), SLOT(slotOpen(const QString&)));
	m->insertAction(i18n("Alter Table"), SLOT(slotAlter(const QString&)));
	m->insertAction(i18n("Delete Table"), SLOT(slotDrop(const QString&)));
	m->insertSeparator();
	m->insertAction(i18n("Create Table..."), SLOT(slotCreate()));

	return m;
}

void
KexiTablePartProxy::slotCreate()
{
	KexiProjectHandler::ItemList *list=m_tablePart->items();
	bool ok = false;
	QString name = KLineEditDlg::getText(i18n("New Table"), i18n("Table name:"), "", &ok, 0);

	if(ok && name.length() > 0)
	{
		QString mime = "kexi/table";
		if (list->find(mime + "/" + name)) {
			KMessageBox::sorry( 0, i18n( "Table \"%1\" already exists" ).arg(name) );
			return;
		}
		KexiProjectHandlerItem *new_item = new KexiProjectHandlerItem(part(), name, "kexi/table",  name);
		KexiAlterTable* kat = new KexiAlterTable(kexiView(), 0, new_item, true);
//		KexiAlterTable* kat = new KexiAlterTable(kexiView(), 0, name, true, "alterTable");
//		kat->show();
		kat->setIcon( new_item->handler()->itemPixmap() );
		list->insert(new_item->fullIdentifier(), new_item);
		emit m_tablePart->itemListChanged(part());
	}
}

void
KexiTablePartProxy::slotOpen(const QString& identifier)
{
	KexiDialogBase *w = m_view->findWindow(identifier);
	KexiProjectHandlerItem *item = part()->items()->find(identifier);
	if (!item)
		return;
	if (w) {
		if (w->isA("KexiDataTable")) { //just activate data table window
			m_view->activateWindow(identifier);
			return;
		}
		else if (w->isA("KexiAlterTable")) {
			m_view->activateWindow(identifier);
			if (KMessageBox::questionYesNo( 0, i18n(
				"Project of \"%1\" table is altered now.\n"
				"Do you want to stop altering this table and display its data?" ).arg(item->title()) )==KMessageBox::No)
			{
				return;
			}
			//yes: close alter window and open in data mode
			w->close(true);
		}
	else
		return; //unknown window type
	}

	kdDebug() << "KexiTablePartProxy::slotOpen(): indentifier = " << identifier << endl;
	kdDebug() << "KexiTablePartProxy::slotOpen(): kexiView = " << kexiView() << endl;

	//trying to get data
	KexiDBRecordSet  *data=m_tablePart->records(kexiView(),
		identifier,KexiDataProvider::Parameters());
	if (!data) {
		 kdDebug() <<"KexitablePartProxy::slotOpen(): error while retrieving data, aborting"<<endl;
		 return;
	}

	KexiDataTable *kt = new KexiDataTable(kexiView(), 0, item);
//	KexiDataTable *kt = new KexiDataTable(kexiView(), 0, item->title(), "table");
	kt->setIcon( item->handler()->itemPixmap() );
	kdDebug() << "KexiTablePart::slotOpen(): indentifier = " << identifier << endl;
	kt->setDataSet(data);
}

void
KexiTablePartProxy::slotAlter(const QString& identifier)
{
	m_part->debug();
	KexiDialogBase *w = m_view->findWindow(identifier);
	KexiProjectHandlerItem *item = part()->items()->find(identifier);
	if (!item)
		return;
	if (w) {
		if (w->isA("KexiAlterTable")) { //just activate alter window for this table
			m_view->activateWindow(identifier);
			return;
		}
		else if (w->isA("KexiDataTable")) {
			m_view->activateWindow(identifier);
			if (KMessageBox::questionYesNo( 0, i18n(
				"\"%1\" table is opened for data editing or viewing.\n"
				"Do you want to close and alter this table?" ).arg(item->title()) )==KMessageBox::No)
			{
				return;
			}
			//yes: close window and open in alter mode
			w->close(true);
		}
	else
		return; //unknown window type
	}
	KexiAlterTable* kat = new KexiAlterTable(kexiView(), 0, item, false);
//	KexiAlterTable* kat = new KexiAlterTable(kexiView(), 0, part()->items()->find(identifier)->identifier(), false, "alterTable");
	kat->setIcon( part()->itemPixmap() );
	kat->show();
}

void
KexiTablePartProxy::slotDrop(const QString& identifier)
{
        KexiProjectHandlerItem * item = part()->items()->find(identifier);

        if (!item)
        {
          KMessageBox::sorry( 0, i18n( "Table not found" ) );
          return;
        }
	//close table window if exists
	KexiDialogBase *w = m_view->findWindow(identifier);
	if (w)
		w->close(true);

//	QString rI = item->title();
	int ans = KMessageBox::questionYesNo(kexiView(),
		i18n("Do you want to delete \"%1\" table?").arg(item->title()), KEXI_APP_NAME);

	if(ans == KMessageBox::Yes)
	{
		kdDebug() << "DROP TABLE " << item->identifier() << endl;
		if(kexiView()->project()->db()->query("DROP TABLE " + item->identifier()))
		{
			// FIXME: Please implement a less costly solution.
			m_tablePart->getTables();
		}
	}
}

void
KexiTablePartProxy::executeItem(const QString& identifier)
{
	slotOpen(identifier);
}

void
KexiTablePartProxy::slotImport(/*int filter*/)
{
	m_tablePart->kexiProject()->filterManager()->import(KexiFilterManager::Data);
}

#include "kexitablepartproxy.moc"
