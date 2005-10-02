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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __KSCRIPT_TYPES_H__
#define __KSCRIPT_TYPES_H__

#include <qstring.h>

struct KScript
{
  typedef long Long;
  typedef double Double;
  typedef QChar Char;
  typedef bool Boolean;
  typedef QString String;
  typedef QString WString;

  struct CharRef
  {
    String* s;
    uint p;

    CharRef( String* str, uint pos) : s(str), p(pos) { }
    CharRef( const CharRef& r ) : s(r.s), p(r.p) { }

    CharRef operator=( const Char& c ) { s->ref(p)=c; return *this; }
    operator Char () const { return s->constref(p); }
  };

};

#endif
