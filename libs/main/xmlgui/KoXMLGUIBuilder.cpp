/* This file is part of the KDE project
   Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>
                      David Faure <faure@kde.org>

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

#include "KoXMLGUIBuilder.h"

#include "kapplication.h"
#include "kauthorized.h"
#include "KoXMLGUIClient.h"
#include "kmenubar.h"
#include "kmenu.h"
#include "ktoolbar.h"
#include "kstatusbar.h"
#include "KoMainWindowBase.h"
#include "KoXMLGUIWindow.h"
#include "kaction.h"
#include "kglobalsettings.h"
#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <QtXml/QDomElement>
#include <QtCore/QObject>
#include <QtCore/QMutableStringListIterator>
#include "KoMenuMenuHandler_p.h"
#include <kcomponentdata.h>

using namespace KDEPrivate;

class KoXMLGUIBuilderPrivate
{
  public:
    KoXMLGUIBuilderPrivate() : m_client(0L)  {}
    ~KoXMLGUIBuilderPrivate() { }

    QWidget *m_widget;

    QString tagMainWindow;
    QString tagMenuBar;
    QString tagMenu;
    QString tagToolBar;
    QString tagStatusBar;

    QString tagSeparator;
    QString tagTearOffHandle;
    QString tagMenuTitle;

    QString attrName;
    QString attrLineSeparator;

    QString attrText1;
    QString attrText2;
    QString attrContext;

    QString attrIcon;

    KComponentData m_componentData;
    KoXMLGUIClient *m_client;

    KoMenuMenuHandler *m_menumenuhandler;
};


KoXMLGUIBuilder::KoXMLGUIBuilder( QWidget *widget )
  : d( new KoXMLGUIBuilderPrivate )
{
  d->m_widget = widget;

  d->tagMainWindow = QLatin1String( "mainwindow" );
  d->tagMenuBar = QLatin1String( "menubar" );
  d->tagMenu = QLatin1String( "menu" );
  d->tagToolBar = QLatin1String( "toolbar" );
  d->tagStatusBar = QLatin1String( "statusbar" );

  d->tagSeparator = QLatin1String( "separator" );
  d->tagTearOffHandle = QLatin1String( "tearoffhandle" );
  d->tagMenuTitle = QLatin1String( "title" );

  d->attrName = QLatin1String( "name" );
  d->attrLineSeparator = QLatin1String( "lineseparator" );

  d->attrText1 = QLatin1String( "text" );
  d->attrText2 = QLatin1String( "Text" );
  d->attrContext = QLatin1String( "context" );

  d->attrIcon = QLatin1String( "icon" );

  d->m_menumenuhandler=new KoMenuMenuHandler(this);
}

KoXMLGUIBuilder::~KoXMLGUIBuilder()
{
  delete d->m_menumenuhandler;
  delete d;
}

QWidget *KoXMLGUIBuilder::widget()
{
  return d->m_widget;
}

QStringList KoXMLGUIBuilder::containerTags() const
{
  QStringList res;
  res << d->tagMenu << d->tagToolBar << d->tagMainWindow << d->tagMenuBar << d->tagStatusBar;

  return res;
}

QWidget *KoXMLGUIBuilder::createContainer( QWidget *parent, int index, const QDomElement &element, QAction*& containerAction )
{
    containerAction = 0;

    if (element.attribute("deleted").toLower() == "true") {
        return 0;
    }

    const QString tagName = element.tagName().toLower();
    if ( tagName == d->tagMainWindow ) {
        KoMainWindowBase *mainwindow = qobject_cast<KoMainWindowBase*>( d->m_widget ); // could be 0
        return mainwindow;
    }

    if ( tagName == d->tagMenuBar ) {
        KoMainWindowBase *mainWin = qobject_cast<KoMainWindowBase*>( d->m_widget );
        KMenuBar *bar = 0;
        if (mainWin)
            bar = mainWin->menuBar();
        if (!bar)
            bar = new KMenuBar( d->m_widget );
        bar->show();
        return bar;
    }

  if ( tagName == d->tagMenu ) {
    // Look up to see if we are inside a mainwindow. If yes, then
    // use it as parent widget (to get kaction to plug itself into the
    // mainwindow). Don't use a popupmenu as parent widget, otherwise
    // the popup won't be hidden if it is used as a standalone menu as well.
    // And we don't want to set the parent for a standalone popupmenu,
    // otherwise its shortcuts appear.
    //
    // Note: menus with a parent of 0, coming from child clients, can be
    // leaked if the child client is deleted without a proper removeClient call, though.
    QWidget* p = parent;
    while ( p && !qobject_cast<QMainWindow*>( p ) )
        p = p->parentWidget();

    QByteArray name = element.attribute( d->attrName ).toUtf8();

    if (!KAuthorized::authorizeKAction(name))
       return 0;

    KMenu *popup = new KMenu(p);
    popup->setObjectName(name);

    d->m_menumenuhandler->insertKMenu(popup);

    QString i18nText;
    QDomElement textElem = element.namedItem( d->attrText1 ).toElement();
    if ( textElem.isNull() ) // try with capital T
      textElem = element.namedItem( d->attrText2 ).toElement();
    const QByteArray text = textElem.text().toUtf8();
    const QByteArray context = textElem.attribute(d->attrContext).toUtf8();

    if ( text.isEmpty() ) // still no luck
      i18nText = i18n( "No text" );
    else if ( context.isEmpty() )
      i18nText = i18n( text );
    else
      i18nText = i18nc( context, text );

    const QString icon = element.attribute( d->attrIcon );
    KIcon pix;
    if (!icon.isEmpty()) {
        pix = KIcon( icon );
    }

    if ( parent ) {
        QAction* act = popup->menuAction();
        if ( !icon.isEmpty() )
            act->setIcon(pix);
        act->setText(i18nText);
        if (index == -1 || index >= parent->actions().count())
            parent->addAction(act);
        else
            parent->insertAction(parent->actions().value(index), act);
        containerAction = act;
        containerAction->setObjectName( name );
    }

    return popup;
  }

  if ( tagName == d->tagToolBar ) {
    QByteArray name = element.attribute( d->attrName ).toUtf8();

    KToolBar *bar = static_cast<KToolBar*>(d->m_widget->findChild<KToolBar*>( name ));
    if( !bar )
    {
       bar = new KToolBar(name, d->m_widget, false);
    }

//    if ( qobject_cast<KoMainWindowBase*>( d->m_widget ) )
//    {
//        if ( d->m_client && !d->m_client->xmlFile().isEmpty() )
//            bar->addXMLGUIClient( d->m_client );
//    }

    bar->loadState( element );

    return bar;
  }

  if ( tagName == d->tagStatusBar ) {
      KoMainWindowBase *mainWin = qobject_cast<KoMainWindowBase *>(d->m_widget);
      if ( mainWin ) {
          mainWin->statusBar()->show();
          return mainWin->statusBar();
      }
      KStatusBar *bar = new KStatusBar( d->m_widget );
      return bar;
  }

  return 0L;
}

void KoXMLGUIBuilder::removeContainer( QWidget *container, QWidget *parent, QDomElement &element, QAction* containerAction )
{
  // Warning parent can be 0L

  if ( qobject_cast<QMenu*>( container ) )
  {
    if ( parent ) {
        parent->removeAction( containerAction );
    }

    delete container;
  }
  else if ( qobject_cast<KToolBar*>( container ) )
  {
    KToolBar *tb = static_cast<KToolBar *>( container );

    tb->saveState( element );
    delete tb;
  }
  else if ( qobject_cast<KMenuBar*>( container ) )
  {
    KMenuBar *mb = static_cast<KMenuBar *>( container );
    mb->hide();
    // Don't delete menubar - it can be reused by createContainer.
    // If you decide that you do need to delete the menubar, make
    // sure that QMainWindow::d->mb does not point to a deleted
    // menubar object.
  }
  else if ( qobject_cast<KStatusBar*>( container ) )
  {
    if ( qobject_cast<KoMainWindowBase*>( d->m_widget ) )
        container->hide();
    else
      delete static_cast<KStatusBar *>(container);
  }
  else
     kWarning() << "Unhandled container to remove : " << container->metaObject()->className();
}

QStringList KoXMLGUIBuilder::customTags() const
{
  QStringList res;
  res << d->tagSeparator << d->tagTearOffHandle << d->tagMenuTitle;
  return res;
}

QAction* KoXMLGUIBuilder::createCustomElement( QWidget *parent, int index, const QDomElement &element )
{
  QAction* before = 0L;
  if (index > 0 && index < parent->actions().count())
      before = parent->actions().at(index);

  const QString tagName = element.tagName().toLower();
  if (tagName == d->tagSeparator)
  {
    if ( QMenu *menu = qobject_cast<QMenu*>( parent ) )
    {
      // QMenu already cares for leading/trailing/repeated separators
      // no need to check anything
      return menu->insertSeparator( before );
    }
    else if ( QMenuBar* bar = qobject_cast<QMenuBar*>( parent ) )
    {
       QAction* separatorAction = new QAction(bar);
       separatorAction->setSeparator(true);
       bar->insertAction( before, separatorAction );
       return separatorAction;
    }
    else if ( KToolBar *bar = qobject_cast<KToolBar*>( parent ) )
    {
      /* FIXME KAction port - any need to provide a replacement for lineSeparator/normal separator?
      bool isLineSep = true;

      QDomNamedNodeMap attributes = element.attributes();
      unsigned int i = 0;
      for (; i < attributes.length(); i++ )
      {
        QDomAttr attr = attributes.item( i ).toAttr();

        if ( attr.name().toLower() == d->attrLineSeparator &&
             attr.value().toLower() == QLatin1String("false") )
        {
          isLineSep = false;
          break;
        }
      }

      if ( isLineSep )
          return bar->insertSeparator( index ? bar->actions()[index - 1] : 0L );
      else*/

      return bar->insertSeparator( before );
    }
  }
  else if (tagName == d->tagTearOffHandle)
  {
    static_cast<QMenu *>(parent)->setTearOffEnabled(true);
  }
  else if (tagName == d->tagMenuTitle)
  {
    if ( KMenu* m = qobject_cast<KMenu*>( parent ) )
    {
      QString i18nText;
      QByteArray text = element.text().toUtf8();

      if ( text.isEmpty() )
        i18nText = i18n( "No text" );
      else
        i18nText = i18n( text );

      QString icon = element.attribute( d->attrIcon );
      KIcon pix;

      if ( !icon.isEmpty() )
      {
        pix = KIcon( icon );
      }

      if ( !icon.isEmpty() ) {
        return m->addTitle( pix, i18nText, before );
      } else {
        return m->addTitle( i18nText, before );
      }
    }
  }

  QAction* blank = new QAction(parent);
  blank->setVisible(false);
  parent->insertAction(before, blank);
  return blank;
}

void KoXMLGUIBuilder::removeCustomElement( QWidget *parent, QAction* action )
{
  parent->removeAction(action);
}

KoXMLGUIClient *KoXMLGUIBuilder::builderClient() const
{
  return d->m_client;
}

void KoXMLGUIBuilder::setBuilderClient( KoXMLGUIClient *client )
{
  d->m_client = client;
  if ( client )
      setBuilderComponentData( client->componentData() );
}

KComponentData KoXMLGUIBuilder::builderComponentData() const
{
  return d->m_componentData;
}

void KoXMLGUIBuilder::setBuilderComponentData(const KComponentData &componentData)
{
  d->m_componentData = componentData;
}

void KoXMLGUIBuilder::finalizeGUI( KoXMLGUIClient * )
{
    KoXmlGuiWindow* window = qobject_cast<KoXmlGuiWindow*>(d->m_widget);
    if (!window)
        return;
#if 0
    KToolBar *toolbar = 0;
    QListIterator<KToolBar> it( ( (KoMainWindowBase*)d->m_widget )->toolBarIterator() );
    while ( ( toolbar = it.current() ) ) {
        kDebug(260) << "KoXMLGUIBuilder::finalizeGUI toolbar=" << (void*)toolbar;
        ++it;
        toolbar->positionYourself();
    }
#else
    window->finalizeGUI( false );
#endif
}

void KoXMLGUIBuilder::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

