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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "FilterManager.h"
#include "ExportFilter.h"
#include "ImportFilter.h"

#include "filter/ImageExport.h"
#include "filter/EPSExport.h"
#include "filter/KilluImport.h"

FilterManager* FilterManager::managerInstance = 0L;

FilterManager::FilterManager () {
  installDefaultFilters ();
}

void FilterManager::installDefaultFilters () {
  ImageExport* filter = new ImageExport ();
#ifdef HAVE_QIMGIO
  filters.insert ("JPEG", new FilterInfo (FilterInfo::FKind_Export,
					  "JPEG", "jpg",
					  "Kai-Uwe Sattler",
					  "1.0", 0L, filter));
  filters.insert ("PNG", new FilterInfo (FilterInfo::FKind_Export,
					  "PNG", "png",
					  "Kai-Uwe Sattler",
					  "1.0", 0L, filter));
#endif
  filters.insert ("PPM", new FilterInfo (FilterInfo::FKind_Export,
					  "PPM", "ppm",
					  "Kai-Uwe Sattler",
					  "1.0", 0L, filter));
  filters.insert ("XPM", new FilterInfo (FilterInfo::FKind_Export,
					  "XPM", "xpm",
					  "Kai-Uwe Sattler",
					  "1.0", 0L, filter));
  filters.insert ("GIF", new FilterInfo (FilterInfo::FKind_Export,
					  "GIF", "gif",
					  "Kai-Uwe Sattler",
					  "1.0", 0L, filter));
  KilluImport* killuFilter = new KilluImport ();
  filters.insert ("KIllustrator", new FilterInfo (FilterInfo::FKind_Import,
						  "KIllustrator", "kil",
						  "Kai-Uwe Sattler",
						  "0.1", killuFilter, 0L));
  EPSExport* epsFilter = new EPSExport ();
  filters.insert ("EPS", new FilterInfo (FilterInfo::FKind_Export,
					  "EPS", "eps",
					  "Kai-Uwe Sattler",
					  "0.1", 0L, epsFilter));
}

FilterManager* FilterManager::instance () {
  if (managerInstance == 0L)
    managerInstance = new FilterManager ();
  return managerInstance;
}

FilterInfo* FilterManager::getFilterForType (const char* ftype) {
  return filters.find (ftype);
}

QStrList FilterManager::getInstalledFilters () {
  QStrList flist;
  QDictIterator<FilterInfo> iter (filters);
  for (; iter.current (); ++iter)
    flist.append (iter.current ()->type ());
  return flist;
}
