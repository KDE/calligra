/*
 * This file is part of the KDE project
 *
 * Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "wdgscriptsmanager.h"

#include <qobjectlist.h>
#include <qheader.h>
#include <klistview.h>
#include <klocale.h>
#include <ktoolbar.h>
#include <kiconloader.h>

#include "scriptguiclient.h"
#include "scriptaction.h"

namespace Kross { namespace Api {

class ListItem : public QListViewItem
{
    private:
        ScriptActionCollection* m_collection;
        ScriptAction::Ptr m_action;
    public:
        ListItem(QListView* parentview, ScriptActionCollection* collection)
            : QListViewItem(parentview), m_collection(collection), m_action(0) {}

        ListItem(ListItem* parentitem, QListViewItem* afteritem, ScriptAction::Ptr action)
            : QListViewItem(parentitem, afteritem), m_collection( parentitem->collection() ), m_action(action) {}

        ScriptAction::Ptr action() const { return m_action; }
        ScriptActionCollection* collection() const { return m_collection; }
        //ScriptActionMenu* actionMenu() const { return m_menu; }
};

class WdgScriptsManagerPrivate
{
    friend class WdgScriptsManager;
    ScriptGUIClient* m_scripguiclient;
    int loadbutton, execbutton, removebutton;
};

WdgScriptsManager::WdgScriptsManager(ScriptGUIClient* scr, QWidget* parent, const char* name, WFlags fl )
    : WdgScriptsManagerBase(parent, name, fl), d(new WdgScriptsManagerPrivate)
{
    d->m_scripguiclient = scr;

    scriptsList->header()->hide();
    //scriptsList->header()->setClickEnabled(false);
    scriptsList->setAllColumnsShowFocus(true);
    scriptsList->setRootIsDecorated(true);
    scriptsList->setSorting(-1);

    scriptsList->addColumn(i18n("Name"));
    //scriptsList->addColumn(i18n("Action"));

    fillScriptsList();

    connect(scriptsList, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(slotSelectionChanged(QListViewItem*)));

    d->loadbutton = toolBar->insertButton("fileopen",0, true, i18n("Load script"));
    toolBar->addConnection(d->loadbutton,SIGNAL(clicked()), this, SLOT(slotLoadScript()));

    d->execbutton = toolBar->insertButton("exec",1, false, i18n("Execute script"));
    toolBar->addConnection(d->execbutton,SIGNAL(clicked()), this, SLOT(slotExecuteScript()));

    d->removebutton = toolBar->insertButton("fileclose",2, false, i18n("Remove script"));
    toolBar->addConnection(d->removebutton,SIGNAL(clicked()), this, SLOT(slotRemoveScript()));

    connect(scr, SIGNAL( collectionChanged(ScriptActionCollection*) ),
            this, SLOT( fillScriptsList() ));
}

WdgScriptsManager::~WdgScriptsManager()
{
    delete d;
}

void WdgScriptsManager::fillScriptsList()
{
    scriptsList->clear();

    addItem( d->m_scripguiclient->getActionCollection("executedscripts"), false );
    addItem( d->m_scripguiclient->getActionCollection("loadedscripts") );
    addItem( d->m_scripguiclient->getActionCollection("installedscripts") );
}

void WdgScriptsManager::addItem(ScriptActionCollection* collection, bool opened)
{
    if(! collection)
        return;

    ListItem* i = new ListItem(scriptsList, collection);
    i->setText(0, collection->actionMenu()->text());
    i->setOpen( opened );

    QValueList<ScriptAction::Ptr> list = collection->actions();
    QListViewItem* lastitem = 0;
    for(QValueList<ScriptAction::Ptr>::Iterator it = list.begin(); it != list.end(); ++it)
        lastitem = addItem(*it, i, lastitem);
}

QListViewItem* WdgScriptsManager::addItem(ScriptAction::Ptr action, QListViewItem* parentitem, QListViewItem* afteritem)
{
    if(! action)
        return 0;

    ListItem* i = new ListItem(dynamic_cast<ListItem*>(parentitem), afteritem, action);
    i->setText(0, action->text());

    QPixmap pm;
    if(action->hasIcon()) {
        KIconLoader* icons = KGlobal::iconLoader();
        pm = icons->loadIconSet(action->icon(), KIcon::Small).pixmap(QIconSet::Small, QIconSet::Active);
    }
    else {
        pm = action->iconSet(KIcon::Small, 16).pixmap(QIconSet::Small, QIconSet::Active);
    }
    if(! pm.isNull())
        i->setPixmap(0, pm); // display the icon

    return i;
}

void WdgScriptsManager::slotSelectionChanged(QListViewItem* item)
{
    ListItem* i = dynamic_cast<ListItem*>(item);
    toolBar->setItemEnabled(d->execbutton, i && i->action());
    toolBar->setItemEnabled(d->removebutton, i && i->action() && i->collection() != d->m_scripguiclient->getActionCollection("installedscripts"));
}

void WdgScriptsManager::slotLoadScript()
{
    if(d->m_scripguiclient->loadScriptFile())
        fillScriptsList();
}

void WdgScriptsManager::slotExecuteScript()
{
    ListItem* current = dynamic_cast<ListItem*>( scriptsList->currentItem() );
    if(current && current->action())
        current->action()->activate();
}

void WdgScriptsManager::slotRemoveScript()
{
    ListItem* current = dynamic_cast<ListItem*>( scriptsList->currentItem() );
    if(current && current->action()) {
        current->collection()->detach( current->action() );
        fillScriptsList();
    }
}

}}

#include "wdgscriptsmanager.moc"
