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

#ifndef __koffice_scan_plugins_h__
#define __koffice_scan_plugins_h__

#include <qstring.h>
#include <qstrlist.h>
#include <qlist.h>

#include <opApplication.h>
#include <openparts_ui.h>
#include <CORBA.h>
#include <komBase.h>

#include "koffice.h"

class KoPluginEntry
{
public:
  KoPluginEntry( const char *_name, const char *_exec, const char *_mode, const char *_comment,
		 const char *_icon, const char *_miniicon, bool _is_gui_plugin, QStrList& _repos );
  
  struct Entry
  {
    QString m_strIcon;
    QString m_strMiniIcon;
    QString m_strMenu;
    QString m_strName;
    QString m_strSlot;
  };
  
  const char *name() { return m_strName.data(); }
  const char *exec() { return m_strExec.data(); }
  const char *comment() { return m_strComment.data(); }
  const char *activationMode() { return m_strActivationMode; }
  const char *icon() { return m_strIcon.data(); }
  const char *miniIcon() { return m_strMiniIcon.data(); }
  
  bool isGUIPlugin() { return m_bIsGUIPlugin; }
  
  bool supports( const char *_mime_type );
 
  QStrListIterator repoID() { return QStrListIterator( m_strlstRepoID ); }
  QListIterator<Entry> menuEntries() { return QListIterator<Entry>( m_lstMenuEntries ); }
  QListIterator<Entry> toolBarEntries() { return QListIterator<Entry>( m_lstToolBarEntries ); }

  void addMenuEntry( const Entry& );
  void addToolBarEntry( const Entry& );
  
  static QListIterator<KoPluginEntry> plugins();
  
protected:
  QStrList m_strlstMimeTypes;
  QStrList m_strlstRepoID;
  QStrList m_strlstCommands;
  QStrList m_strlstCommandsI18N;
  
  QString m_strExec;
  QString m_strComment;
  QString m_strActivationMode;
  QString m_strName;
  QString m_strIcon;
  QString m_strMiniIcon;
  
  bool m_bIsGUIPlugin;
  
  QList<Entry> m_lstMenuEntries;
  QList<Entry> m_lstToolBarEntries;
};

void koScanPluginsError( const char *_file, const char *_entry );
void koScanPlugins();
void koScanPlugins( CORBA::ImplRepository_ptr _imr );
void koScanPlugins( const char* _path, CORBA::ImplRepository_ptr _imr );
void koScanPluginFile( const char* _file, CORBA::ImplRepository_ptr _imr );

class KoViewIf;
class KoPluginManager;
class KoPluginProxy;

class KoPluginCallback : virtual public KOMBase,
			 virtual public KOffice::Callback_skel
{
public:
  KoPluginCallback( KoPluginProxy*, KoPluginEntry::Entry * );
  
  virtual void callback();
  
  KoPluginEntry::Entry* entry() { return m_pEntry; }

protected:
  KoPluginEntry::Entry* m_pEntry;
  KoPluginProxy *m_pProxy;
};

struct KoPluginProxy
{
  KoPluginProxy( KoPluginManager *_manager, KoPluginEntry* _entry );
  ~KoPluginProxy();
  
  void cleanUp();
  
  KOM::Plugin_ptr ref();
    
  KoPluginEntry* m_pEntry;
  KoPluginManager* m_pManager;
  
  QList<KoPluginCallback> m_lstToolBarCallbacks;
  QList<KoPluginCallback> m_lstMenuBarCallbacks;
  KOM::Plugin_var m_vPlugin;
};

class KoPluginManager
{
public:
  KoPluginManager();
  virtual ~KoPluginManager();
  
  void cleanUp();
  
  void setView( KOffice::View_ptr _view ) { m_vView = KOffice::View::_duplicate( _view ); }
  KOffice::View_ptr view() { return KOffice::View::_duplicate( m_vView ); }
  
  void fillMenuBar( OpenPartsUI::MenuBar_ptr _menubar );
  void fillToolBar( OpenPartsUI::ToolBarFactory_ptr _factory );
  
protected:  
  QList<KoPluginProxy> m_lstPlugins;

  OpenPartsUI::ToolBar_var m_vToolBar;
  KOffice::View_var m_vView;
};

#endif




