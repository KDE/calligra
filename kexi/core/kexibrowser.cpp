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
#include "kexiworkspace.h"
#include "kexibrowseritem.h"
#include "kexiproject.h"
#include "kexiprojecthandler.h"
#include "kexiprojecthandlerproxy.h"
#include "kexiprojecthandleritem.h"
#include "kexihandlerpopupmenu.h"
#include "kexidialogbase.h"

KexiBrowser::KexiBrowser(QWidget *parent, QString mime, KexiProjectHandler *part, const char *name )
 : KListView(parent,name)
{
//	iconLoader = KGlobal::iconLoader();
	m_mime = mime;
	m_part = part;

	header()->hide();

	addColumn("");
	setResizeMode(QListView::LastColumn);

	connect(this, SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)),
		SLOT(slotContextMenu(KListView*, QListViewItem *, const QPoint&)));
	connect(this, SIGNAL(executed(QListViewItem*)), SLOT(slotExecuteItem(QListViewItem*)));

	if(part)
	{
		connect(part, SIGNAL(itemListChanged(KexiProjectHandler *)), this, SLOT(slotItemListChanged(KexiProjectHandler *)));
		slotItemListChanged(part);
	}

}

void
KexiBrowser::addGroup(KexiProjectHandler *part)
{
	//A littlebit hacky at the moment
	KexiBrowserItem *item = new KexiBrowserItem(this, 
		part->proxy(static_cast<KexiDialogBase*>(parent()->parent())->kexiView())
	);
	setRootIsDecorated(true);
	
	item->setPixmap(0, part->groupPixmap());
	item->setOpen(true);
	m_baseItems.insert(part->mime(), item);
	slotItemListChanged(part);
}

void
KexiBrowser::addItem(KexiProjectHandlerItem *item)
{
	if(m_mime == "kexi/db" && m_baseItems.find(item->mime()))
	{
		KexiBrowserItem *parent = m_baseItems.find(item->mime());
		KexiBrowserItem *bitem = new KexiBrowserItem(parent, item);
		bitem->setPixmap(0, parent->proxy()->part()->itemPixmap());
	}
	else if(m_mime == item->mime())
	{
		KexiBrowserItem *bitem = new KexiBrowserItem(this, item);
		if(m_part)
			bitem->setPixmap(0, m_part->itemPixmap());
	}
}

void
KexiBrowser::slotItemListChanged(KexiProjectHandler *parent)
{

	kdDebug() << "KexiBrowser::slotItemListChanged() " << parent->items()->count() << " items" << endl;
	kdDebug() << "KexiBrowser::slotItemListChanged() " << m_mime << endl;

	if(m_mime == "kexi/db")
	{
		KexiBrowserItem *group = m_baseItems.find(parent->mime());
		group->clearChildren();
	}
	else
	{
		clear();
	}


	for(KexiProjectHandler::ItemIterator it(*(parent->items()));it.current();++it)
	//KexiProjectHandlerItem *item = plist->first(); item; item = plist->next())
	{
		kdDebug() << "KexiBrowser::slotItemListChanged() adding " << it.current()->mime() << endl;
		kdDebug() << "KexiBrowser::slotItemListChanged()  named " << it.current()->identifier() << endl;
		addItem(it.current());
	}
}

void
KexiBrowser::slotContextMenu(KListView *, QListViewItem *item, const QPoint &pos)
{
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(item);
	if(!it)
		return;

	if(it->proxy() || it->item())
	{
		KexiPartPopupMenu *pg = 0;
		if(it->identifier() == QString::null)
		{
			// FIXME: Make this less hacky please :)
			pg = it->proxy()->groupContext();
		}
		else
		{
			kdDebug() << "KexiBrowser::slotContextMenu() item @ " << it->item() << endl;
			//a littlebit hacky
			pg = it->item()->projectPart()->proxy(
			static_cast<KexiDialogBase*>(parent()->parent())->kexiView()
			)->itemContext(it->identifier());
		}

		pg->setIdentifier(it->identifier());
		pg->exec(pos);
		delete pg;
	}
}

void
KexiBrowser::slotExecuteItem(QListViewItem *item)
{
	KexiBrowserItem *it = static_cast<KexiBrowserItem*>(item);
	if(!it)
		return;

	if(it->proxy() || it->item())
	{
		if(it->identifier() != QString::null)
		{
			if (it->proxy())
			{
				it->proxy()->executeItem(it->identifier());
			}
			else
			{
				it->item()->projectPart()->proxy(
	                        static_cast<KexiDialogBase*>(parent()->parent())->kexiView()
        	                )->executeItem(it->identifier());
			}
		}
	}
}

#include "kexibrowser.moc"
