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

#include "FilterInfo.h"
#include "ExportFilter.h"
#include "ImportFilter.h"

FilterInfo::FilterInfo (Kind kind, const char* type, const char* ext,
			const char* vendor, 
			const char* release, ImportFilter* imp, 
			ExportFilter* exp) {
  f_kind = kind;
  f_type = type;
  f_extension = ext;
  f_vendor = vendor;
  f_release = release;
  import = imp;
  export = exp;
}

const char* FilterInfo::type () const {
  return (const char *) f_type;
}

const char* FilterInfo::extension () const {
  return (const char *) f_extension;
}

const char* FilterInfo::vendor () const {
  return (const char *) f_vendor;
}

const char* FilterInfo::release () const {
  return (const char *) f_release;
}

FilterInfo::Kind FilterInfo::kind () const {
  return f_kind;
}

ExportFilter* FilterInfo::exportFilter () {
  return export;
}

ImportFilter* FilterInfo::importFilter () {
  return import;
}
