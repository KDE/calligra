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
#include <qstringlist.h>
#include <qlist.h>

#include <opApplication.h>

class KoToolEntry
{
public:
  KoToolEntry( const QString &_name, const QString &_comment, const QStringList& _mimes,
               const QStringList& _commands, const QStringList& _commands_i18n,
	       CORBA::Object_ptr _ref );
  
  QString name() { return m_strName.data(); }
  QString comment() { return m_strComment.data(); }
  
  CORBA::Object_ptr ref() { return CORBA::Object::_duplicate( m_vRef ); }

  bool supports( const QString &_mime_type );
 
  QStringList mimeTypes() { return m_strlstMimeTypes; }
  QStringList commands() { return m_strlstCommands; }
  QStringList commandsI18N() { return m_strlstCommandsI18N; }

  static QList<KoToolEntry> findTools( const QString &_mime_type );
  
protected:
  QStringList m_strlstMimeTypes;
  QStringList m_strlstCommands;
  QStringList m_strlstCommandsI18N;
  
  QString m_strComment;
  QString m_strName;
  CORBA::Object_var m_vRef;
};

#endif




