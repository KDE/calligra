/***************************************************************************
                          kexibrowser.cpp  -  description
                             -------------------
    begin                : Sun Jun 9 2002
    copyright            : (C) 2002 by lucijan busch
    email                : lucijan@gmx.at
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <klocale.h>
#include <kpushbutton.h>
#include <klistview.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kpopupmenu.h>
#include <kdebug.h>
#include <klineeditdlg.h>
#include <kmessagebox.h>

#include <qlayout.h>
#include <qheader.h>
#include <qpoint.h>
#include <qlistview.h>

#include "kexiDB/kexidb.h"

#include "keximainwindow.h"
#include "kexiapplication.h"
#include "kexibrowser.h"
#include "kexiformbase.h"
#include "kexiworkspace.h"
#include "kexibrowseritem.h"
#include "kexidatatable.h"
#include "kexialtertable.h"
#include "kexiquerydesigner.h"

KexiBrowser::KexiBrowser(QWidget *parent, Section s, const char *name ) : KListView(parent,name)
{
	m_parent = parent;
	m_section = s;

	iconLoader = KGlobal::iconLoader();

	header()->hide();
//	setRootIsDecorated(true);
	
	addColumn(i18n("Database Content"));
	setResizeMode(QListView::LastColumn);

	connect(this, SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)),
		SLOT(slotContextMenu(KListView*, QListViewItem *, const QPoint&)));
	
	connect(this, SIGNAL(executed(QListViewItem *)), SLOT(slotCreate(QListViewItem *)));
}

void KexiBrowser::clearView()
{
	clear();
	generateView();
}

void KexiBrowser::generateView()
{
	kdDebug() << "KexiBrowser::generateView()" << endl;
	clear();

	m_database = new KexiBrowserItem(KexiBrowserItem::Parent, KexiBrowserItem::Table, this, i18n("Database"));
	m_tables = new KexiBrowserItem(KexiBrowserItem::Parent, KexiBrowserItem::Table, m_database, i18n("Tables"));
	m_queries = new KexiBrowserItem(KexiBrowserItem::Parent, KexiBrowserItem::Query, m_database, i18n("Queries"));
	m_forms = new KexiBrowserItem(KexiBrowserItem::Parent, KexiBrowserItem::Form, m_database, i18n("Forms"));
	m_reports = new KexiBrowserItem(KexiBrowserItem::Parent, KexiBrowserItem::Report, m_database, i18n("Reports"));

	addTables(m_tables);
	
	m_database->setPixmap(0, iconLoader->loadIcon("db", KIcon::Small));
	m_tables->setPixmap(0, iconLoader->loadIcon("tables", KIcon::Small));
	m_queries->setPixmap(0, iconLoader->loadIcon("queries", KIcon::Small));
	m_forms->setPixmap(0, iconLoader->loadIcon("forms", KIcon::Small));
	m_reports->setPixmap(0, iconLoader->loadIcon("reports", KIcon::Small));

	setOpen(m_database, true);
	setOpen(m_tables, true);
	setOpen(m_queries, true);
}


void KexiBrowser::addTables(KexiBrowserItem *parent)
{
	kdDebug() << "KexiBrowser::addTables()" << endl;

	QStringList tables = kexi->project()->db()->tables();

	for ( QStringList::Iterator it = tables.begin(); it != tables.end(); ++it )
	{
		KexiBrowserItem *item = new KexiBrowserItem(KexiBrowserItem::Child, KexiBrowserItem::Table, parent, (*it) );
		item->setPixmap(0, iconLoader->loadIcon("table", KIcon::Small));
	}
}

void KexiBrowser::slotContextMenu(KListView* , QListViewItem *i, const QPoint &p)
{
	// TODO: look up the type, wich we have to create
	KexiBrowserItem *r = static_cast<KexiBrowserItem *>(i);
	kdDebug() << "context menu requested..." << endl;
		
	if(i)
	{
		KPopupMenu *m = new KPopupMenu();
		switch(r->content())
		{
			case KexiBrowserItem::Table:
			{
				m->insertItem(i18n("Create Table"), this, SLOT(slotCreateTable()));
				
				if ( r->type() == KexiBrowserItem::Child )
				{
					m->insertItem(i18n("Alter Table"), this, SLOT(slotAlterTable()));
					m->insertItem(i18n("Delete Table"), this, SLOT(slotDeleteTable()));
				}
				break;
			}
			case KexiBrowserItem::Query:
			{
				m->insertItem(i18n("Create Query"), this, SLOT(slotCreateQuery()));
				break;
			}
			case KexiBrowserItem::Form:
			{
				m->insertItem(i18n("Create Form"), this, SLOT(slotCreateNewForm()));
				if (r->type()==KexiBrowserItem::Child)
				{
					m->insertItem(i18n("Delete Form"), this, SLOT(slotDelete()));
					m->insertItem(i18n("Edit Form"), this, SLOT(slotEdit()));
				}
				break;
			}
			default:
			{
				break;
			}
		}
		m->exec(p);
	}
}


void KexiBrowser::slotCreate(QListViewItem *i)
{
	KexiBrowserItem* r = static_cast<KexiBrowserItem *>(i);

	switch (r->content())
	{
		case KexiBrowserItem::Form:
		{
			if ( r->type() == KexiBrowserItem::Child)
			{
				kexi->project()->formManager()->showForm(r->identifier(), KexiFormManager::View,
					kexi->mainWindow()->workspaceWidget()); 

//    			    KexiFormBase *fb = new KexiFormBase(kexi->mainWindow()->workspace(), "form",r->identifier());
//			    kexi->mainWindow()->workspace()->addItem(fb);
//			    fb->show();
			}
			break;
		}

		case KexiBrowserItem::Table:
		{
			if ( r->type() == KexiBrowserItem::Child )
			{
				KexiDataTable *kt = new KexiDataTable(kexi->mainWindow()->workspaceWidget(), r->text(0), "table");
				if(kt->executeQuery("select * from " + r->text(0)))
				{
					kt->show();
				}
				else
				{
					delete kt;
				} 
			}
			break;
		}

		default:
		break;
	}	
}


void KexiBrowser::slotCreateNewForm()
{
	QString name=kexi->project()->formManager()->newForm();

	KexiBrowserItem *item = new KexiBrowserItem(KexiBrowserItem::Child, KexiBrowserItem::Form, m_forms,name,name);
	item->setPixmap(0, iconLoader->loadIcon("form", KIcon::Small));
	slotCreate(item);	
	
}

void KexiBrowser::slotDelete()
{
}

void KexiBrowser::slotEdit()
{
}

void KexiBrowser::slotCreateTable()
{
	bool ok = false;
	QString name = KLineEditDlg::getText(i18n("New Table"), i18n("Table Name:"), "", &ok, this);
	
	if(ok && name.length() > 0)
	{
		if(kexi->project()->db()->query("CREATE TABLE " + name + " (id INT(10))"))
		{
			KexiAlterTable* kat = new KexiAlterTable(kexi->mainWindow()->workspaceWidget(), name, "alterTable");
			kat->show();
			KexiBrowserItem *item = new KexiBrowserItem(KexiBrowserItem::Child, KexiBrowserItem::Table, m_tables, name);
			item->setPixmap(0, iconLoader->loadIcon("table", KIcon::Small));
		}
	}
}

void KexiBrowser::slotAlterTable()
{
	KexiBrowserItem* r = static_cast<KexiBrowserItem *>(selectedItems().first());
	
	if ( r->type() == KexiBrowserItem::Child )
	{
		KexiAlterTable* kat = new KexiAlterTable(kexi->mainWindow()->workspaceWidget(), r->text(0), "alterTable");
		kat->show();
	}
}

void KexiBrowser::slotCreateQuery()
{
	bool ok = false;
	QString name = KLineEditDlg::getText(i18n("New Query"), i18n("Query Name:"), "", &ok, this);

	if(ok && name.length() > 0)
	{
		KexiQueryDesigner *kqd = new KexiQueryDesigner(kexi->mainWindow()->workspaceWidget(), name, "query");
		KexiBrowserItem *item = new KexiBrowserItem(KexiBrowserItem::Child, KexiBrowserItem::Query, m_queries, name);
		kexi->project()->addFileReference("/query/" + name + ".query");
		m_queries->setOpen(true);
		kqd->show();

		kexi->project()->setModified();

	}
}

void KexiBrowser::slotDeleteTable()
{
	KexiBrowserItem* r = static_cast<KexiBrowserItem *>(selectedItems().first());
	
	if ( r->type() == KexiBrowserItem::Child )
	{
		int ans = KMessageBox::questionYesNo(this,
			i18n("Do you realy want to delete %1?").arg(r->text(0)), i18n("Delete Table?"));
		
		if(ans == KMessageBox::Yes)
		{
			if(kexi->project()->db()->query("DROP TABLE " + r->text(0)))
			{
				delete r;
			}
		}
	}
}

KexiBrowser::~KexiBrowser(){
}

#include "kexibrowser.moc"
