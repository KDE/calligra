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

#include <qlayout.h>
#include <qheader.h>
#include <qpoint.h>
#include <qlistview.h>

#include "kexi.h"
#include "kexibrowser.h"
#include "kexiformbase.h"
#include "kexiworkspace.h"
#include "kexibrowseritem.h"


KexiBrowser::KexiBrowser(Kexi *mainWin, QWidget *parent, const char *name ) : QWidget(parent,name)
{
	m_mainWin = mainWin;
	m_parent = parent;

	iconLoader = KGlobal::iconLoader();

	QVBoxLayout *l = new QVBoxLayout(this);
	
	m_list = new KListView(this);
	
	m_list->header()->hide();
	m_list->setRootIsDecorated(true);
	
	m_list->addColumn(i18n("Database content"));
	m_list->setResizeMode(QListView::LastColumn);
	
//	generateView();
		
	l->addWidget(m_list);
	
	
	connect(m_list, SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)),
		SLOT(slotContextMenu(KListView*, QListViewItem *, const QPoint&)));
	
	connect(m_list, SIGNAL(executed(QListViewItem *)), SLOT(slotCreate()));
}

void KexiBrowser::addTableItem(QString name)
{
	KexiBrowserItem *tblItem = new KexiBrowserItem(KexiBrowserItem::Child, KexiBrowserItem::Table, m_tables, name);
	tblItem->setPixmap(0, iconLoader->loadIcon("table", KIcon::Small));
	m_list->setOpen(m_tables, true);
}

void KexiBrowser::clearView()
{
	m_list->clear();
	generateView();
}

void KexiBrowser::generateView()
{
	m_database = new KexiBrowserItem(KexiBrowserItem::Parent, KexiBrowserItem::Table, m_list, i18n("Database"));
	m_tables = new KexiBrowserItem(KexiBrowserItem::Parent, KexiBrowserItem::Table, m_database, i18n("Tables"));
	m_queries = new KexiBrowserItem(KexiBrowserItem::Parent, KexiBrowserItem::Query, m_database, i18n("Queries"));
	m_forms = new KexiBrowserItem(KexiBrowserItem::Parent, KexiBrowserItem::Form, m_database, i18n("Forms"));
	m_reports = new KexiBrowserItem(KexiBrowserItem::Parent, KexiBrowserItem::Report, m_database, i18n("Reports"));

	m_database->setPixmap(0, iconLoader->loadIcon("db", KIcon::Small));
	m_tables->setPixmap(0, iconLoader->loadIcon("tables", KIcon::Small));
	m_queries->setPixmap(0, iconLoader->loadIcon("queries", KIcon::Small));
	m_forms->setPixmap(0, iconLoader->loadIcon("forms", KIcon::Small));
	m_reports->setPixmap(0, iconLoader->loadIcon("reports", KIcon::Small));

	m_list->setOpen(m_database, true);
}


void KexiBrowser::slotContextMenu(KListView* , QListViewItem *i, const QPoint &p)
{
	#warning "TODO: look up the type, wich we have to create"
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
				break;
			}
			
			default:
			{
				m->insertItem(i18n("Create Form"), this, SLOT(slotCreate()));
				m->insertItem(i18n("Delete Form"), this, SLOT(slotDelete()));
				m->insertItem(i18n("Edit Form"), this, SLOT(slotEdit()));
			}
		}
		m->exec(p);
	}
}


void KexiBrowser::slotCreate()
{
	KexiFormBase *fb = new KexiFormBase(m_mainWin->workspace(), "form");
	m_mainWin->workspace()->addItem(fb);
	fb->show();
}


void KexiBrowser::slotDelete() {};
void KexiBrowser::slotEdit() {};

void KexiBrowser::slotCreateTable()
{
}

KexiBrowser::~KexiBrowser(){
}

#include "kexibrowser.moc"
