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

QList<KoPluginEntry> *g_plstPluginEntries = 0L;

void koScanPluginsError( const char *_file, const char *_entry )
{
  warning( i18n( "The plugins config file\n%s\ndoes not conatain a %s=... entry" ), _file, _entry );
}

void koScanPlugins()
{
  CORBA::Object_var obj = opapp_orb->resolve_initial_references ("ImplementationRepository");
  CORBA::ImplRepository_var imr = CORBA::ImplRepository::_narrow( obj );
  assert( !CORBA::is_nil( imr ) );

  koScanPlugins( imr );
}

void koScanPlugins( CORBA::ImplRepository_ptr _imr )
{
  QString p = kapp->kde_datadir().copy();
  p += "/koffice/pluginlnk";
  koScanPlugins( p, _imr );
}

void koScanPlugins( const char* _path, CORBA::ImplRepository_ptr _imr )
{
  if ( g_plstPluginEntries == 0L )
    g_plstPluginEntries = new QList<KoPluginEntry>;

  DIR *dp;
  struct dirent *ep;
  dp = opendir( _path );
  if ( dp == 0L )
    return;

  // Loop thru all directory entries
  while ( ( ep = readdir( dp ) ) != 0L )
  {
    if ( strcmp( ep->d_name, "." ) != 0 && strcmp( ep->d_name, ".." ) != 0 )
    {
      QString tmp = ep->d_name;
      QString file = _path;
      file += "/";
      file += ep->d_name;
      struct stat buff;
      stat( file.data(), &buff );
      if ( S_ISDIR( buff.st_mode ) )
      {
	koScanPlugins( file, _imr );
      }
      else if ( tmp.length() > 7 && tmp.right( 7 ) == ".kdelnk" )
      {
	FILE *f = fopen( file, "r" );
	if ( f == 0L )
	{
	  fclose( f );
	  continue;
	}
	
	koScanPluginFile( file, _imr );
      }
    }
  }

  closedir( dp );
}

void koScanPluginFile( const char* _file, CORBA::ImplRepository_ptr _imr )
{
  KSimpleConfig config( _file, true );
  config.setDesktopGroup();

  QString type = config.readEntry( "Type" );
  if ( type.isEmpty() )
  {
    koScanPluginsError( _file, "Type" );
    return;
  }
  if ( type != "GUIPlugin" )
  {
    warning("Not a plugin\n");
    return;
  }

  QString icon = config.readEntry( "Icon" );
  if ( icon.isEmpty() )
  {
    koScanPluginsError( _file, "Icon" );
    return;
  }
  QString miniicon = config.readEntry( "MiniIcon" );
  if ( miniicon.isEmpty() )
  {
    koScanPluginsError( _file, "MiniIcon" );
    return;
  }
  bool guiplugin = true;

  QString cmd = config.readEntry( "Exec" );
  if ( cmd.isEmpty() )
  {
    koScanPluginsError( _file, "Exec" );
    return;
  }
  QStrList repoids;
  if ( config.readListEntry( "RepoID", repoids ) == 0 )
  {
    koScanPluginsError( _file, "RepoID" );
    return;
  }
  QStrList menuEntries;
  if ( config.readListEntry( "MenuEntries", menuEntries ) == 0 )
  {
    koScanPluginsError( _file, "MenuEntries" );
    return;
  }
  QStrList toolBarEntries;
  if ( config.readListEntry( "ToolBarEntries", toolBarEntries ) == 0 )
  {
    koScanPluginsError( _file, "ToolBarEntries" );
    return;
  }
  QString name = config.readEntry( "Name" );
  if ( name.isEmpty() )
  {
    QString tmp( _file );
    int i = tmp.findRev( "/" );
    if ( i == -1 )
      return;
    name = tmp.right( tmp.length() - i - 1 );
    if ( name.isEmpty() )
      return;
  }

  QString comment = config.readEntry( "Comment" );
  if ( comment.isEmpty() )
    comment = name.data();
  QString mode = config.readEntry( "ActivationMode" );
  if ( mode.isEmpty() )
  {
    koScanPluginsError( _file, "ActivationMode" );
    return;
  }

  imr_create( name, mode, cmd, repoids, _imr );

  KoPluginEntry* plugin = new KoPluginEntry( name, cmd, mode, comment, icon, miniicon, guiplugin, repoids );

  const char *s;
  for( s = menuEntries.first(); s != 0L; s = menuEntries.next() )
  {
    config.setGroup( s );
    QString icon = config.readEntry( "Icon" );
    QString miniicon = config.readEntry( "MiniIcon" );
    QString name = config.readEntry( "Name" );
    if ( name.isEmpty() )
    {
      QString tmp = s;
      tmp += "::Name";
      koScanPluginsError( _file, tmp );
      continue;
    }
    QString slot = config.readEntry( "Slot" );
    if ( slot.isEmpty() )
    {
      QString tmp = s;
      tmp += "::Slot";
      koScanPluginsError( _file, tmp );
      continue;
    }

    KoPluginEntry::Entry e;
    e.m_strIcon = icon;
    e.m_strMiniIcon = miniicon;
    e.m_strName = name;
    e.m_strSlot = slot;
    plugin->addMenuEntry( e );
  }

  for( s = toolBarEntries.first(); s != 0L; s = toolBarEntries.next() )
  {
    config.setGroup( s );
    QString icon = config.readEntry( "Icon" );
    QString miniicon = config.readEntry( "MiniIcon" );
    QString name = config.readEntry( "Name" );
    if ( name.isEmpty() )
    {
      QString tmp = s;
      tmp += "::Name";
      koScanPluginsError( _file, tmp );
      continue;
    }
    QString menu = config.readEntry( "Menu" );
    if ( menu.isEmpty() )
    {
      QString tmp = s;
      tmp += "::Menu";
      koScanPluginsError( _file, tmp );
      continue;
    }
    QString slot = config.readEntry( "Slot" );
    if ( slot.isEmpty() )
    {
      QString tmp = s;
      tmp += "::Slot";
      koScanPluginsError( _file, tmp );
      continue;
    }

    KoPluginEntry::Entry e;
    e.m_strIcon = icon;
    e.m_strMiniIcon = miniicon;
    e.m_strName = name;
    e.m_strSlot = slot;
    e.m_strMenu = menu;

    plugin->addToolBarEntry( e );
  }

  g_plstPluginEntries->append( plugin );
}

KoPluginEntry::KoPluginEntry( const char *_name, const char *_exec, const char *_mode,
			      const char *_comment, const char *_icon,
			      const char *_miniicon, bool, QStrList& _repos )
{
  m_strName = _name;
  m_strExec = _exec;
  m_strActivationMode = _mode;
  m_strComment = _comment;
  m_strlstRepoID = _repos;
  m_strIcon = _icon;
  m_strMiniIcon = _miniicon;

  m_lstMenuEntries.setAutoDelete( false );
  m_lstToolBarEntries.setAutoDelete( false );
}

QListIterator<KoPluginEntry> KoPluginEntry::plugins()
{
  assert( g_plstPluginEntries != 0L );

  return QListIterator<KoPluginEntry>( *g_plstPluginEntries );
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

  CORBA::Request_var _req = obj->_request( m_pEntry->m_strSlot );
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
}

KOM::Plugin_ptr KoPluginProxy::ref()
{
  if ( !CORBA::is_nil( m_vPlugin ) )
    return KOM::Plugin::_duplicate( m_vPlugin );

  const char *repoid = m_pEntry->repoID().current();
  assert( repoid );
  kdebug( KDEBUG_INFO, 30003, "Creating %c", repoid );
  CORBA::Object_var obj = imr_activate( m_pEntry->name(), repoid );
  // CORBA::Object_var obj = imr_activate( m_pEntry->name(), "IDL:KOM/PluginFactory:1.0" );
  if ( CORBA::is_nil( obj ) )
  {
    QString tmp;
    tmp.sprintf( i18n("Could not activate plugin %s").ascii(), m_pEntry->name() );
    QMessageBox::critical( 0L, i18n("Error in plugin"), tmp, i18n("OK") );
    return 0L;
  }

  kdebug( KDEBUG_INFO, 30003, "Got factory" );

  KOM::PluginFactory_var factory = KOM::PluginFactory::_narrow( obj );
  if ( CORBA::is_nil( factory ) )
  {
    QString tmp;
    tmp.sprintf( i18n("%s is not a plugin").ascii(), m_pEntry->name() );
    QMessageBox::critical( 0L, i18n("Error in plugin"), tmp, i18n("OK") );
    return 0L;
  }

  kdebug( KDEBUG_INFO, 30003, "Narrow ok" );

  KOM::Component_var comp = m_pManager->view();
  m_vPlugin = factory->create( comp );
  if ( CORBA::is_nil( m_vPlugin ) )
  {
    QString tmp;
    tmp.sprintf( i18n("Could not create plugin of type %s").ascii(), m_pEntry->name() );
    QMessageBox::critical( 0L, i18n("Error in plugin"), tmp, i18n("OK") );
    return 0L;
  }

  kdebug( KDEBUG_INFO, 30003, "Plugin ok" );

  return KOM::Plugin::_duplicate( m_vPlugin );
}

KoPluginManager::KoPluginManager()
{
  m_lstPlugins.setAutoDelete( true );

  QListIterator<KoPluginEntry> it = KoPluginEntry::plugins();
  for( ; it.current() != 0L; ++it )
  {
    KoPluginProxy *p = new KoPluginProxy( this, it.current() );
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
	QString tmp = it.current()->entry()->m_strMiniIcon.data();
	if ( tmp[0] != '/' )
	{
	  QString t2 = tmp;
	  tmp = kapp->kde_datadir();
	  tmp += "/koffice/toolbar/";
	  tmp += t2;
	}
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












