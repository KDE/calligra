/* This file is part of the KDE libraries
    Copyright (C) 1998 Torben Weis <weis@kde.org>

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

#include "koScanPlugins.h"
#include "koView.h"

#include <opIMR.h>
#include <opUIUtils.h>
#include <qmsgbox.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <assert.h>

#include <ksimpleconfig.h>
#include <klocale.h>
#include <kapp.h>
#include <kded_instance.h>
#include <ktrader.h>
#include <kactivator.h>
#include <kstddirs.h>

/**
 * Port to KActivator/KTrader (kded) by Simon Hausmann
 * (c) 1999 Simon Hausmann <hausmann@kde.org>
 */

KoPluginEntry::KoPluginEntry( const QString &_name, const QString &_comment, const QString &_icon,
			      const QString &_miniicon, bool, CORBA::Object_ptr obj )
{
  m_strName = _name;
  m_strComment = _comment;
  m_strIcon = _icon;
  m_strMiniIcon = _miniicon;
  m_vObj = CORBA::Object::_duplicate( obj );

  m_lstMenuEntries.setAutoDelete( false );
  m_lstToolBarEntries.setAutoDelete( false );
}

void KoPluginEntry::addMenuEntry( const KoPluginEntry::Entry& _entry )
{
  Entry *e = new Entry( _entry );
  m_lstMenuEntries.append( e );
}

void KoPluginEntry::addToolBarEntry( const KoPluginEntry::Entry& _entry )
{
  Entry *e = new Entry( _entry );
  m_lstToolBarEntries.append( e );
}

KoPluginCallback::KoPluginCallback( KoPluginProxy* _proxy, KoPluginEntry::Entry *_entry )
{
  m_pProxy = _proxy;
  m_pEntry = _entry;
}

void KoPluginCallback::callback()
{
  KOM::Plugin_var obj = m_pProxy->ref();
  if ( CORBA::is_nil( obj ) )
    return;

  CORBA::Request_var _req = obj->_request( m_pEntry->m_strSlot.ascii() );
  _req->result()->value()->type( CORBA::_tc_void );
  _req->invoke();
}

KoPluginProxy::KoPluginProxy( KoPluginManager *_manager, KoPluginEntry *_entry )
{
  m_pManager = _manager;
  m_pEntry = _entry;

  m_lstToolBarCallbacks.setAutoDelete( false );
  m_lstMenuBarCallbacks.setAutoDelete( false );

  QListIterator<KoPluginEntry::Entry> it = _entry->toolBarEntries();
  for( ; it.current() != 0L; ++it )
  {
    m_lstToolBarCallbacks.append( new KoPluginCallback( this, it.current() ) );
  }

  it = _entry->menuEntries();
  for( ; it.current() != 0L; ++it )
  {
    m_lstMenuBarCallbacks.append( new KoPluginCallback( this, it.current() ) );
  }
}

KoPluginProxy::~KoPluginProxy()
{
  cleanUp();
}

void KoPluginProxy::cleanUp()
{
  QListIterator<KoPluginCallback> it( m_lstToolBarCallbacks );
  for( ; it.current() != 0L; ++it )
    CORBA::release( it );
  m_lstToolBarCallbacks.clear();

  it = m_lstMenuBarCallbacks;
  for( ; it.current() != 0L; ++it )
    CORBA::release( it );
  m_lstMenuBarCallbacks.clear();

  delete m_pEntry;
}

KOM::Plugin_ptr KoPluginProxy::ref()
{
  if ( !CORBA::is_nil( m_vPlugin ) )
    return KOM::Plugin::_duplicate( m_vPlugin );

  CORBA::Object_var obj = m_pEntry->ref();

  KOM::PluginFactory_var factory = KOM::PluginFactory::_narrow( obj );
  if ( CORBA::is_nil( factory ) )
  {
    QString tmp;
    tmp.sprintf( i18n("%s is not a plugin").ascii(), m_pEntry->name().ascii() );
    QMessageBox::critical( 0L, i18n("Error in plugin"), tmp, i18n("OK") );
    return 0L;
  }

  kdebug( KDEBUG_INFO, 30003, "Narrow ok" );

  KOM::Component_var comp = m_pManager->view();
  m_vPlugin = factory->create( comp );
  if ( CORBA::is_nil( m_vPlugin ) )
  {
    QString tmp;
    tmp.sprintf( i18n("Could not create plugin of type %s").ascii(), m_pEntry->name().ascii() );
    QMessageBox::critical( 0L, i18n("Error in plugin"), tmp, i18n("OK") );
    return 0L;
  }

  kdebug( KDEBUG_INFO, 30003, "Plugin ok" );

  return KOM::Plugin::_duplicate( m_vPlugin );
}

KoPluginManager::KoPluginManager()
{
  m_lstPlugins.setAutoDelete( true );

  KTrader *trader = KdedInstance::self()->ktrader();
  KActivator *activator = KdedInstance::self()->kactivator();

  KTrader::OfferList offers = trader->query( "KOfficePlugin" );
  KTrader::OfferList::ConstIterator it = offers.begin();
  for (; it != offers.end(); ++it )
  {
    QString icon = (*it)->icon();
    //HACK
    QString miniIcon = (*it)->icon();
    QString comment = (*it)->comment();

    QStringList menuEntriesList = (*it)->property( "MenuEntries" )->stringListValue();
    QStringList toolBarEntriesList = (*it)->property( "ToolBarEntries" )->stringListValue();

    QString repoId = *( (*it)->repoIds().begin() );
    QString tag = (*it)->name();
    int tagPos = repoId.findRev( "#" );
    if ( tagPos != -1 )
    {
      tag = repoId.mid( tagPos+1 );
      repoId.truncate( tagPos );
    }

    CORBA::Object_var obj = activator->activateService( (*it)->name(), repoId, tag );

    KoPluginEntry *plugin = new KoPluginEntry( (*it)->name(), comment, icon,
                                               miniIcon, true, obj );
					
    QStringList::ConstIterator it2 = menuEntriesList.begin();
    for (; it2 != menuEntriesList.end(); ++it2 )
    {
      KoPluginEntry::Entry e;

      e.m_strName = *(it2++);
      e.m_strIcon = *(it2++);
      e.m_strMiniIcon = *(it2++);
      e.m_strSlot = *it2;

      plugin->addMenuEntry( e );
    }

    it2 = toolBarEntriesList.begin();
    for (; it2 != toolBarEntriesList.end(); ++it2 )
    {
      KoPluginEntry::Entry e;

      e.m_strName = *(it2++);
      e.m_strIcon = *(it2++);
      e.m_strMiniIcon = *(it2++);
      e.m_strSlot = *(it2++);
      e.m_strMenu = *it2;

      plugin->addToolBarEntry( e );
    }

    //the proxy object takes care of deleting the plugin!
    KoPluginProxy *p = new KoPluginProxy( this, plugin );
    m_lstPlugins.append( p );
  }
}

KoPluginManager::~KoPluginManager()
{
  cleanUp();
}

void KoPluginManager::cleanUp()
{
  m_vView = 0L;
  m_lstPlugins.clear();
}

void KoPluginManager::fillMenuBar( OpenPartsUI::MenuBar_ptr  )
{
}

void KoPluginManager::fillToolBar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
  if ( CORBA::is_nil( _factory ) )
  {
    m_vToolBar = 0L;
    return;
  }

  KoPluginProxy *p;
  for( p = m_lstPlugins.first(); p != 0L; p = m_lstPlugins.next() )
  {
    QListIterator<KoPluginCallback> it( p->m_lstToolBarCallbacks );
    for( ; it.current() != 0L; ++it )
    {
      if ( CORBA::is_nil( m_vToolBar ) )
      {
	m_vToolBar = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
	m_vToolBar->setFullWidth(false);
      }
      if ( !it.current()->entry()->m_strMiniIcon.isEmpty() )
      {
	QString tmp = locate("toolbar", it.current()->entry()->m_strMiniIcon);
	OpenPartsUI::Pixmap_var pix = OPUIUtils::loadPixmap( tmp );
	CORBA::WString_var toolTip = Q2C( it.current()->entry()->m_strName );
	(void)m_vToolBar->insertButton2( pix, 1, SIGNAL( clicked() ), it.current(),
					 "callback", true, toolTip, -1 );

      }
    }
  }

  if ( !CORBA::is_nil( m_vToolBar ) )
  {
    m_vToolBar->enable( OpenPartsUI::Show );
    // HACK
    m_vToolBar->enable( OpenPartsUI::Hide );
    m_vToolBar->setBarPos(OpenPartsUI::Floating);
    m_vToolBar->setBarPos(OpenPartsUI::Top);
    m_vToolBar->enable( OpenPartsUI::Show );
  }
}












