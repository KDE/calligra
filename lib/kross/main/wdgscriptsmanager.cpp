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

#include <klistview.h>
#include <klocale.h>
#include <ktoolbar.h>
#include <kiconloader.h>

#include "scriptguiclient.h"
#include "scriptaction.h"

namespace Kross {
namespace Api {

class WdgScriptsManagerPrivate
{
    friend class WdgScriptsManager;
    ~WdgScriptsManagerPrivate() { delete m_qlviInstalledScripts; delete m_qlviExecutedScripts; }
    QListViewItem* m_qlviInstalledScripts, *m_qlviExecutedScripts/*,* m_qlviFilters,* m_qlviTools*/;
    ScriptGUIClient* m_scripguiclient;
    int loadbutton, execbutton, removebutton;
};
    
WdgScriptsManager::WdgScriptsManager(ScriptGUIClient* scr, QWidget* parent, const char* name, WFlags fl )
    : WdgScriptsManagerBase(parent, name, fl), d(new WdgScriptsManagerPrivate)
{
    d->m_scripguiclient = scr;
    scriptsList->addColumn(i18n("Text"));
    scriptsList->addColumn(i18n("Name"));

    fillScriptsList();

    connect(scriptsList, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(slotSelectionChanged(QListViewItem*)));

    d->loadbutton = toolBar->insertButton("fileopen",0, true, i18n("Load script"));
    toolBar->addConnection(d->loadbutton,SIGNAL(clicked()), this, SLOT(slotLoadScript()));

    d->execbutton = toolBar->insertButton("exec",1, false, i18n("Execute script"));
    toolBar->addConnection(d->execbutton,SIGNAL(clicked()), this, SLOT(slotExecuteScript()));

    d->removebutton = toolBar->insertButton("fileclose",2, false, i18n("Remove script"));
    toolBar->addConnection(d->removebutton,SIGNAL(clicked()), this, SLOT(slotRemoveScript()));
}


WdgScriptsManager::~WdgScriptsManager()
{
    delete d;
}

void WdgScriptsManager::fillScriptsList()
{
    scriptsList->clear();
    d->m_qlviInstalledScripts = new QListViewItem(scriptsList, i18n("Installed Scripts"));
    d->m_qlviInstalledScripts->setOpen(true);
    d->m_qlviExecutedScripts = new QListViewItem(scriptsList, i18n("Executed Scripts"));
    d->m_qlviExecutedScripts->setOpen(true);
//     m_qlviFilters = new QListViewItem(scriptsList, i18n("Filters"));
//     m_qlviTools = new QListViewItem(scriptsList, i18n("Tools"));
    // TODO: support for category, it's not needed for Krita 1.5 but it will for 2.0
    fillScriptsList(d->m_qlviInstalledScripts, d->m_scripguiclient->getInstalledScriptActions());
    fillScriptsList(d->m_qlviExecutedScripts, d->m_scripguiclient->getExecutedScriptActions());
}

void WdgScriptsManager::fillScriptsList(QListViewItem* item, ScriptAction::List list)
{
    for (ScriptAction::List::iterator it = list.begin(); it !=  list.end(); ++it) {
        QListViewItem* i = new QListViewItem(item, (*it)->text(), (*it)->getName() );

        QPixmap pm;
        if((*it)->hasIcon()) {
            KIconLoader* icons = KGlobal::iconLoader();
            pm = icons->loadIconSet((*it)->icon(), KIcon::Small).pixmap(QIconSet::Small, QIconSet::Active);
        }
        else {
            pm = (*it)->iconSet(KIcon::Small, 16).pixmap(QIconSet::Small, QIconSet::Active);
        }
        if(! pm.isNull()) {
            i->setPixmap(0, pm); // display the icon
        }
    }
}

void WdgScriptsManager::slotSelectionChanged(QListViewItem* item)
{
    toolBar->setItemEnabled(d->execbutton, item && item->parent());
    toolBar->setItemEnabled(d->removebutton, item && item->parent());
}

void WdgScriptsManager::slotLoadScript()
{
    if(d->m_scripguiclient->executeScriptFile()) {
        fillScriptsList();
    }
}
void WdgScriptsManager::slotExecuteScript()
{
    QListViewItem * current = scriptsList->currentItem ();
    if(current !=0 && current->text(1) != "")
    {
        ScriptAction * s = find(current->text(1));
        s->activate();
    }
}
void WdgScriptsManager::slotRemoveScript()
{
    QListViewItem * current = scriptsList->currentItem ();
    if(current !=0 && current->text(1) != "")
    {
//         KisScriptSP s = KisScriptsRegistry::instance()->remove(current->text(1));
//         if(s==0) kdDebug() << "Script not found" <<endl;
//         fillScriptsList();
    }
}

ScriptAction* WdgScriptsManager::find(const QString name) const
{
    ScriptAction* s = findInInstalled(name);
    if( s != 0)
        return s;
    return findInExecuted(name);
}
ScriptAction* WdgScriptsManager::findInInstalled(const QString name) const
{
    ScriptAction::List kl = d->m_scripguiclient->getInstalledScriptActions();
    for (ScriptAction::List::iterator it = kl.begin(); it !=  kl.end(); ++it) {
        if( (*it)->getName() == name )
        {
            return *it;
        }
    }
    return 0;
}

ScriptAction* WdgScriptsManager::findInExecuted(const QString name) const
{
    ScriptAction::List kl = d->m_scripguiclient->getExecutedScriptActions();
    for (ScriptAction::List::iterator it = kl.begin(); it !=  kl.end(); ++it) {
        if( (*it)->getName() == name )
        {
            return *it;
        }
    }
    return 0;
}

}

}
#include "wdgscriptsmanager.moc"
