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

#include <qlayout.h>
#include <qheader.h>
#include <qpoint.h>
#include <qlistview.h>
#include <qdom.h>

#include <klocale.h>
#include <kpushbutton.h>
#include <klistview.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kpopupmenu.h>
#include <kdebug.h>
#include <klineeditdlg.h>
#include <kmessagebox.h>

#include "kexiDB/kexidb.h"

#include "kexiview.h"
#include "kexibrowser.h"
#include "kexiformbase.h"
#include "kexiworkspace.h"
#include "kexibrowseritem.h"
#include "kexidatatable.h"
#include "kexialtertable.h"
#include "kexiquerydesigner.h"
#include "kexikugarwrapper.h"
#include "kexiproject.h"

KexiBrowser::KexiBrowser(KexiView *view,QWidget *parent, Section s, const char *name ) : KListView(parent,name)
{
	m_view=view;
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
				{
					m->insertItem(i18n("Create Form"), this, SLOT(slotCreate()));
					m->insertItem(i18n("Delete Form"), this, SLOT(slotDelete()));
					m->insertItem(i18n("Edit Form"), this, SLOT(slotEdit()));
				}
				break;
			}
			case KexiBrowserItem::Report:
			{
				m->insertItem(i18n("Show Report"), this, SLOT(slotShowReport()));
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
				m_view->project()->formManager()->showForm(r->identifier(), KexiFormManager::View,
					m_view);

			}
			break;
		}

		case KexiBrowserItem::Table:
		{
			if ( r->type() == KexiBrowserItem::Child )
			{
				KexiDataTable *kt = new KexiDataTable(m_view,0, r->text(0), "table");
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

		case KexiBrowserItem::Query:
		{
			if ( r->type() == KexiBrowserItem::Child )
			{
				KexiQueryDesigner *kqd = new KexiQueryDesigner(m_view, 0,r->text(0), "oq");
				kqd->show();
			}
			break;
		}

		default:
		break;
	}
}


void KexiBrowser::slotCreateNewForm()
{
	QString name=m_view->project()->formManager()->newForm();

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
		if(m_view->project()->db()->query("CREATE TABLE " + name + " (id INT(10))"))
		{
			KexiBrowserItem* r = static_cast<KexiBrowserItem *>(selectedItems().first());
			KexiBrowserItem* parent;

			if (r->type() == KexiBrowserItem::Child)
			{
				parent = static_cast<KexiBrowserItem *>(r->parent());
			}
			else
			{
				parent = r;
			}
			
			KexiAlterTable* kat = new KexiAlterTable(m_view, 0,name, "alterTable");
			kat->show();
			KexiBrowserItem *item = new KexiBrowserItem(KexiBrowserItem::Child, KexiBrowserItem::Table, parent, name);
			item->setPixmap(0, iconLoader->loadIcon("table", KIcon::Small));
		}
	}
}

void KexiBrowser::slotAlterTable()
{
	KexiBrowserItem* r = static_cast<KexiBrowserItem *>(selectedItems().first());

	if (r->type() == KexiBrowserItem::Child)
	{
		KexiAlterTable* kat = new KexiAlterTable(m_view,0,r->text(0), "alterTable");
		kat->show();
	}
}

void KexiBrowser::slotShowReport()
{
//	new KexiKugarWrapper(m_view,0,"blah","report");
}

void KexiBrowser::slotCreateQuery()
{
	bool ok = false;
	QString name = KLineEditDlg::getText(i18n("New Query"), i18n("Query Name:"), "", &ok, this);

	if(ok && name.length() > 0)
	{
		KexiBrowserItem* r = static_cast<KexiBrowserItem *>(selectedItems().first());
		KexiBrowserItem* parent;

		if (r->type() == KexiBrowserItem::Child)
		{
			parent = static_cast<KexiBrowserItem *>(r->parent());
		}
		else
		{
			parent = r;
		}
		
		KexiQueryDesigner *kqd = new KexiQueryDesigner(m_view, 0,name, "query");
		KexiBrowserItem *item = new KexiBrowserItem(KexiBrowserItem::Child, KexiBrowserItem::Query, parent, name);
//		kexi->project()->addFileReference("/query/" + name + ".query");

		m_view->project()->setModified(true);

		item->parent()->setOpen(true);
		kqd->show();

		m_view->project()->setModified(true);
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
			if(m_view->project()->db()->query("DROP TABLE " + r->text(0)))
			{
				delete r;
			}
		}
	}
}

KexiBrowser::~KexiBrowser(){
}

#include "kexibrowser.moc"
