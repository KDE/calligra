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
#include <qstringlist.h>
#include <qlist.h>

#include <opApplication.h>
#include <openparts_ui.h>
#include <CORBA.h>
#include <komBase.h>
#include <komVar.h>

#include "koffice.h"

class KoPluginEntry
{
public:
  KoPluginEntry( const QString &_name, const QString &_comment, const QString &_icon, 
                 const QString &_miniicon, bool _is_gui_plugin, CORBA::Object_ptr obj );
  
  struct Entry
  {
    QString m_strIcon;
    QString m_strMiniIcon;
    QString m_strMenu;
    QString m_strName;
    QString m_strSlot;
  };
  
  QString name() { return m_strName; }
  QString comment() { return m_strComment; }
  QString icon() { return m_strIcon; }
  QString miniIcon() { return m_strMiniIcon; }
  CORBA::Object_ptr ref() { return CORBA::Object::_duplicate( m_vObj ); }
  
  bool isGUIPlugin() { return m_bIsGUIPlugin; }

  bool supports( const QString &_mime_type );
 
  QListIterator<Entry> menuEntries() { return QListIterator<Entry>( m_lstMenuEntries ); }
  QListIterator<Entry> toolBarEntries() { return QListIterator<Entry>( m_lstToolBarEntries ); }

  void addMenuEntry( const Entry& );
  void addToolBarEntry( const Entry& );
  
protected:
  QStringList m_strlstMimeTypes;
  
  QString m_strExec;
  QString m_strComment;
  QString m_strActivationMode;
  QString m_strName;
  QString m_strIcon;
  QString m_strMiniIcon;
  CORBA::Object_var m_vObj;
  
  bool m_bIsGUIPlugin;
  
  QList<Entry> m_lstMenuEntries;
  QList<Entry> m_lstToolBarEntries;
};

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
  KOMVar<KOM::Plugin> m_vPlugin;
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




