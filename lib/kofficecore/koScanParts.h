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

#ifndef __koffice_scan_parts_h__
#define __koffice_scan_parts_h__

#include <qstring.h>
#include <qstrlist.h>
#include <qlist.h>

#include <opApplication.h>

class KoPartEntry
{
public:
  KoPartEntry( const char *_name, const char *_exec, const char *_mode, const char *_comment,
	     QStrList& _mimes, QStrList& _repos );
  
  const char *name() { return m_strName.data(); }
  const char *exec() { return m_strExec.data(); }
  const char *comment() { return m_strComment.data(); }

  bool supports( const char *_mime_type );
 
  QStrListIterator repoID() { return QStrListIterator( m_strlstRepoID ); }
  QStrListIterator mimeTypes() { return QStrListIterator( m_strlstMimeTypes ); }

protected:
  QStrList m_strlstMimeTypes;
  QStrList m_strlstRepoID;

  QString m_strExec;
  QString m_strComment;
  QString m_strActivationMode;
  QString m_strName;
};

extern QList<KoPartEntry> *g_plstPartEntries;

void koScanPartsError( const char *_file, const char *_entry );
void koScanParts();
void koScanParts( CORBA::ImplRepository_ptr _imr );
void koScanParts( const char* _path, CORBA::ImplRepository_ptr _imr );
void koScanPartFile( const char* _file, CORBA::ImplRepository_ptr _imr );

#endif
