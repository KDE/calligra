/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by  
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef FilterInfo_h_
#define FilterInfo_h_

#include <qstring.h>

class ExportFilter;
class ImportFilter;

class FilterInfo {
 public:
  enum Kind { FKind_Import, FKind_Export };
  
  FilterInfo (Kind kind, const char* type, const char* ext,
	      const char* vendor = 0L, 
	      const char* release = 0L, ImportFilter* imp = 0L, 
	      ExportFilter* exp = 0L);

  const char* type () const;
  const char* vendor () const;
  const char* release () const;
  const char* extension () const;
  Kind kind () const;
  
  ExportFilter* exportFilter ();
  ImportFilter* importFilter ();
  
private:
  QString f_type, f_vendor, f_release, f_extension;
  Kind f_kind;
  ExportFilter* export_;
  ImportFilter* import_;
};

#endif
