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

#ifndef __koffice_scan_tools_h__
#define __koffice_scan_tools_h__

#include <qstring.h>
#include <qstrlist.h>
#include <qlist.h>

#include <opApplication.h>

class KoToolEntry
{
public:
  KoToolEntry( const char *_name, const char *_exec, const char *_mode, const char *_comment,
	     QStrList& _mimes, QStrList& _repos, QStrList& _commands, QStrList& _commands_i18n );
  
  const char *name() { return m_strName.data(); }
  const char *exec() { return m_strExec.data(); }
  const char *comment() { return m_strComment.data(); }

  bool supports( const char *_mime_type );
 
  QStrListIterator repoID() { return QStrListIterator( m_strlstRepoID ); }
  QStrListIterator mimeTypes() { return QStrListIterator( m_strlstMimeTypes ); }
  QStrListIterator commands() { return QStrListIterator( m_strlstCommands ); }
  QStrListIterator commandsI18N() { return QStrListIterator( m_strlstCommandsI18N ); }

  static QList<KoToolEntry> findTools( const char *_mime_type );
  
protected:
  QStrList m_strlstMimeTypes;
  QStrList m_strlstRepoID;
  QStrList m_strlstCommands;
  QStrList m_strlstCommandsI18N;
  
  QString m_strExec;
  QString m_strComment;
  QString m_strActivationMode;
  QString m_strName;
};

void koScanToolsError( const char *_file, const char *_entry );
void koScanTools();
void koScanTools( CORBA::ImplRepository_ptr _imr );
void koScanTools( const char* _path, CORBA::ImplRepository_ptr _imr );
void koScanToolFile( const char* _file, CORBA::ImplRepository_ptr _imr );

#endif




