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
#include "koIMR.h"
#include "koView.h"

#include <opUIUtils.h>
#include <kstring.h>
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
  config.setGroup( "KDE Desktop Entry" );
  
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

KoPluginEntry::KoPluginEntry( const char *_name, const char *_exec, const char *_mode, const char *_comment,
			      const char *_icon, const char *_miniicon, bool _is_gui_plugin, QStrList& _repos )
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

int KoPluginManager::s_id = 0;

KoPluginManager::KoPluginManager()
{  
  m_lstPlugins.setAutoDelete( true );

  /*
   * Obtain a reference to the RootPOA and its Manager
   */

  CORBA::Object_var poaobj = opapp_orb->resolve_initial_references ("RootPOA");
  PortableServer::POA_var poa = PortableServer::POA::_narrow (poaobj);
  PortableServer::POAManager_var mgr = poa->the_POAManager();

  /*
   * The RootPOA has the USE_ACTIVE_OBJECT_MAP_ONLY policy; to register
   * our ServantManager, we must create our own POA with the
   * USE_SERVANT_MANAGER policy
   */

  CORBA::PolicyList pl;
  pl.length(1);
  pl[0] = poa->create_request_processing_policy (PortableServer::USE_SERVANT_MANAGER);
  QString name;
  name.sprintf( "KoPluginManagerPOA%i", s_id++ );
  m_vPoa = poa->create_POA( name, mgr, pl);

  /*
   * Activate our ServantManager
   */
  PortableServer::ServantManager_var servref = _this();
  m_vPoa->set_servant_manager( servref );

  mgr->activate();

  QListIterator<KoPluginEntry> it = KoPluginEntry::plugins();
  for( ; it.current() != 0L; ++it )
  {
    Plugin *p = new Plugin;
    p->m_vPlugin = createReference( it.current()->name(), "IDL:KOM/Plugin:1.0" );
    p->m_pEntry = it.current();
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

  if ( !CORBA::is_nil( m_vPoa ) )
  {    
    m_vPoa->destroy( true, true );
    m_vPoa = 0L;
  }
}

CORBA::Object_ptr KoPluginManager::createReference( const char *_implreponame, const char* _repoid )
{
  PortableServer::ObjectId id;
  unsigned int l = strlen( _implreponame );
  id.length( l );
  for( unsigned int i = 0; i < l; i++ )
    id[i] = _implreponame[i];
  
  CORBA::Object_ptr obj = m_vPoa->create_reference_with_id ( id, _repoid );
  assert( !CORBA::is_nil( obj ) );
  
  return obj;
}

PortableServer::Servant KoPluginManager::incarnate( const PortableServer::ObjectId & _oid,
						    PortableServer::POA_ptr )
{
  cerr << "Incarnating ...." << endl;
  
  /*
   * Incarnate the object
   */
  char buffer[ _oid.length() + 1 ];
  for( CORBA::ULong l = 0; l < _oid.length(); ++l )
    buffer[ l ] = _oid[ l ];
  buffer[ _oid.length() ] = 0;

  cerr << "Incarnating " << buffer << endl;
  
  CORBA::Object_var obj = imr_activate( buffer, "IDL:KOM/PluginFactory:1.0" );
  if ( CORBA::is_nil( obj ) )
  {
    QString tmp;
    ksprintf( &tmp, i18n("Could not activate plugin %s"), buffer );
    QMessageBox::critical( 0L, i18n("Error in plugin"), tmp, i18n("Ok") );
    return 0L;
  }
  
  KOM::PluginFactory_var factory = KOM::PluginFactory::_narrow( obj );
  if ( CORBA::is_nil( factory ) )
  {
    QString tmp;
    ksprintf( &tmp, i18n("%s is not a plugin"), buffer );
    QMessageBox::critical( 0L, i18n("Error in plugin"), tmp, i18n("Ok") );
    return 0L;
  }
  
  KOM::RequestedInterfaces ifaces;
  ifaces.length( 1 );
  ifaces[0].repoid = CORBA::string_dup( "IDL:OpenParts/View:1.0" );
  ifaces[0].obj = m_vView;
  KOM::Plugin_var plugin = factory->create( ifaces );
  if ( CORBA::is_nil( plugin ) )
  {    
    QString tmp;
    ksprintf( &tmp, i18n("Could not create plugin of type %s"), buffer );
    QMessageBox::critical( 0L, i18n("Error in plugin"), tmp, i18n("Ok") );
    return 0L;
  }
  
  PortableServer::ForwardRequest fw;
  fw.forward_reference = CORBA::Object::_duplicate( plugin );
  mico_throw( fw );
  
  cerr << "OOOOOOOOooooooooooooppppppppssss" << endl;
  
  // Never reached
  return 0L;
}

void KoPluginManager::etherealize( const PortableServer::ObjectId & oid,
				   PortableServer::POA_ptr poa,
				   PortableServer::Servant serv,
				   CORBA::Boolean cleanup_in_progress,
				   CORBA::Boolean remaining_activations)
{
  /* Not needed, since we never create a servant */
}

void KoPluginManager::fillMenuBar( OpenPartsUI::MenuBar_ptr _menubar )
{
}

void KoPluginManager::fillToolBar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
  if ( CORBA::is_nil( _factory ) )
  {
    m_vToolBar = 0L;
    return;
  }
  
  Plugin *p;
  for( p = m_lstPlugins.first(); p != 0L; p = m_lstPlugins.next() )
  {
    QListIterator<KoPluginEntry::Entry> it = p->m_pEntry->toolBarEntries();
    for( ; it.current() != 0L; ++it )
    {
      if ( CORBA::is_nil( m_vToolBar ) )
	m_vToolBar = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
      if ( !it.current()->m_strMiniIcon.isEmpty() )
      {
	QString tmp = it.current()->m_strMiniIcon.data();
	if ( tmp[0] != '/' )
	{
	  QString t2 = tmp;
	  tmp = kapp->kde_icondir();
	  tmp += "/mini/";
	  tmp += t2;
	}
	OpenPartsUI::Pixmap_var pix = OPUIUtils::loadPixmap( tmp );
	(void)m_vToolBar->insertButton2( pix, 1, SIGNAL( clicked() ), p->m_vPlugin,
					 it.current()->m_strSlot, true, it.current()->m_strName, -1 );

      }
    }
  }

  if ( !CORBA::is_nil( m_vToolBar ) )
    m_vToolBar->enable( OpenPartsUI::Show );
}
