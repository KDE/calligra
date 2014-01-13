/* This file is part of the KDE project
   Copyright (C) 2006  Olivier Goffart  <ogoffart@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "xmlgui/KoMenuMenuHandler_p.h"

#include "xmlgui/KoXMLGUIBuilder.h"
#include "xmlgui/KoXMLGUIClient.h"
#include "xmlgui/KoXMLGUIFactory.h"
#include "kmenu.h"
#include "kaction.h"
#include "kactioncollection.h"
#include <kdialog.h>
#include <kshortcutwidget.h>
#include <klocale.h>
#include <kdebug.h>


#include <QWidget>
#include <QDomDocument>
#include <QDomNode>
#include <kapplication.h>
#include <xmlgui/KoMainWindowBase.h>
#include <ktoolbar.h>
#include <kselectaction.h>


namespace KDEPrivate {

KoMenuMenuHandler::KoMenuMenuHandler( KoXMLGUIBuilder *builder )
  : QObject() , m_builder(builder)
{
  m_toolbarAction = new KSelectAction(i18n("Add to Toolbar"), this);
  connect(m_toolbarAction , SIGNAL(triggered(int)) , this , SLOT(slotAddToToolBar(int)));
}



void KoMenuMenuHandler::insertKMenu( KMenu *popup )
{
  popup->contextMenu()->addAction( i18n("Configure Shortcut...") , this , SLOT(slotSetShortcut()));

  KoMainWindowBase *window=qobject_cast<KoMainWindowBase*>(m_builder->widget());
  if(window)
  {
    popup->contextMenu()->addAction( m_toolbarAction );
    connect(popup, SIGNAL(aboutToShowContextMenu(KMenu*,QAction*,QMenu*)) , this, SLOT(buildToolbarAction()));
  }
}



void KoMenuMenuHandler::buildToolbarAction()
{
  KoMainWindowBase *window=qobject_cast<KoMainWindowBase*>(m_builder->widget());
  if(!window)
    return;
  QStringList toolbarlist;
  foreach(KToolBar *b , window->toolBars())
  {
    toolbarlist << (b->windowTitle().isEmpty() ? b->objectName() : b->windowTitle());
  }
  m_toolbarAction->setItems(toolbarlist);
}

static KActionCollection* findParentCollection(KoXMLGUIFactory* factory, QAction* action)
{
    foreach(KoXMLGUIClient *client, factory->clients()) {
        KActionCollection* collection = client->actionCollection();
        // if the call to actions() is too slow, add KActionCollection::contains(QAction*).
        if (collection->actions().contains(action)) {
            return collection;
        }
    }
    return 0;
}

void KoMenuMenuHandler::slotSetShortcut()
{
    KMenu * menu=KMenu::contextMenuFocus();
    if(!menu)
        return;
    KAction *action= qobject_cast<KAction*>(menu->contextMenuFocusAction());
    if(!action)
        return;

    KDialog dialog(m_builder->widget());
    KShortcutWidget swidget(&dialog);
    swidget.setShortcut(action->shortcut());
    dialog.setMainWidget(&swidget);
    KActionCollection* parentCollection = 0;
    if(dynamic_cast<KoXMLGUIClient*>(m_builder))
    {
        QList<KActionCollection*> checkCollections;
        KoXMLGUIFactory *factory=dynamic_cast<KoXMLGUIClient*>(m_builder)->factory();
        parentCollection = findParentCollection(factory, action);
        foreach(KoXMLGUIClient *client, factory->clients()) {
            checkCollections += client->actionCollection();
        }
        swidget.setCheckActionCollections(checkCollections);
    }

    if(dialog.exec())
    {
        action->setShortcut(swidget.shortcut(), KAction::ActiveShortcut);
        swidget.applyStealShortcut();
        if(parentCollection)
            parentCollection->writeSettings();
    }
}


void KoMenuMenuHandler::slotAddToToolBar(int tb)
{
//    KoMainWindowBase *window=qobject_cast<KoMainWindowBase*>(m_builder->widget());
//    if(!window)
//        return;

//    KMenu * menu=KMenu::contextMenuFocus();
//    if(!menu)
//        return;
//    QAction *action= qobject_cast<QAction*>(menu->contextMenuFocusAction());
//    if(!action)
//        return;

//    KoXMLGUIFactory *factory = dynamic_cast<KoXMLGUIClient*>(m_builder)->factory();
//    QString actionName = action->objectName(); // set by KActionCollection::addAction
//    KActionCollection *collection = 0;
//    if (factory) {
//        collection = findParentCollection(factory, action);
//    }
//    if(!collection) {
//         kWarning(240) << "Cannot find the action collection for action " << actionName;
//         return;
//    }

//    KToolBar *toolbar=window->toolBars()[tb];
//    toolbar->addAction(action);

//    const KoXMLGUIClient* client = collection->parentGUIClient ();
//    QString xmlFile = client->localXMLFile();
//    QDomDocument document;
//    document.setContent( KoXMLGUIFactory::readConfigFile( client->xmlFile(), client->componentData() ) );
//    QDomElement elem = document.documentElement().toElement();

//    static const QString &tagToolBar = KGlobal::staticQString( "ToolBar" );
//    static const QString &attrNoEdit = KGlobal::staticQString( "noEdit" );
//    static const QString &attrName     = KGlobal::staticQString( "name" );

//    QDomElement toolbarElem;
//    QDomNode n=elem.firstChild();
//    for( ; !n.isNull(); n = n.nextSibling() )
//    {
//        QDomElement elem = n.toElement();
//        if (!elem.isNull() && elem.tagName() == tagToolBar && elem.attribute( attrName ) == toolbar->objectName() )
//        {
//            if(elem.attribute( attrNoEdit ) == "true")
//            {
//                kWarning(240) << "The toolbar is not editable";
//                return;
//            }
//            toolbarElem = elem;
//            break;
//        }
//    }
//    if(toolbarElem.isNull())
//    {
//        toolbarElem=document.createElement( tagToolBar );
//        toolbarElem.setAttribute( attrName, toolbar->objectName() );
//        elem.appendChild( toolbarElem );
//    }

//    KoXMLGUIFactory::findActionByName(toolbarElem, actionName, true);
//    KoXMLGUIFactory::saveConfigFile(document, xmlFile);

}



} //END namespace KDEPrivate
