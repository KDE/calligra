/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#if 0

#ifndef __KSCRIPT_TYPECODE
#define __KSCRIPT_TYPECODE

#include <qshared.h>

#include "kscript_value.h"

class KSCorbaFunction;

class KSTypeCode : public QShared
{
  friend KSCorbaFunction;

public:
  typedef KSSharedPtr<KSTypeCode> Ptr;

  KSTypeCode();
  KSTypeCode( const QString& stringified );
  KSTypeCode( void* tc );
  ~KSTypeCode();

  /**
   * @return the typecode without increasing its reference count.
   */
  void* tc();

  /**
   * @return FALSE is the argument was no valid typecode.
   */
  bool fromString( const QString& stringified );

  /**
   * If the value is a string, then it is translated to a typecode.
   * If no typecode could be generated an exception is raised and
   * FALSE is returned.
   */
  static bool convertToTypeCode( KSContext&, KSValue* );
  /**
   * Assumes that the value defines some type and tries to get the
   * typecode out of it. Raises an exception and returns FALSE if
   * that was not possible.
   */
  static KSTypeCode::Ptr typeCode( KSContext&, KSValue* );

protected:
#ifdef WITH_CORBA
  CORBA::TypeCode* m_typecode;
#else
  void* m_typcode;
#endif
};

#endif

#endif
