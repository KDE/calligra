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
#include "filter/XfigImport.h"
#include "filter/CmxImport.h"
#include "filter/PSImport.h"
#include "filter/SVGExport.h"

#include <qstrlist.h>

FilterManager* FilterManager::managerInstance = 0L;

FilterManager::FilterManager () {
  installDefaultFilters ();
}

void FilterManager::installDefaultFilters () {
  ImageExport* filter = new ImageExport ();
  if (filter->installed ()) {
#ifdef HAVE_QIMGIO
    filters.insert ("JPEG", new FilterInfo (FilterInfo::FKind_Export,
                                            "JPEG Image Format", "jpg",
                                            "Kai-Uwe Sattler",
                                            "1.0", 0L, filter));
    filters.insert ("PNG", new FilterInfo (FilterInfo::FKind_Export,
                                           "PNG Image Format", "png",
                                           "Kai-Uwe Sattler",
                                           "1.0", 0L, filter));
#endif
    filters.insert ("PPM", new FilterInfo (FilterInfo::FKind_Export,
                                           "PPM Image Format", "ppm",
                                           "Kai-Uwe Sattler",
                                           "1.0", 0L, filter));
    filters.insert ("XPM", new FilterInfo (FilterInfo::FKind_Export,
                                           "XPM Image Format", "xpm",
                                           "Kai-Uwe Sattler",
                                           "1.0", 0L, filter));
  }
  XfigImport* xfigFilter = new XfigImport ();
  filters.insert ("Xfig", new FilterInfo (FilterInfo::FKind_Import,
                                                  "Xfig Document", "fig",
                                                  "Kai-Uwe Sattler",
                                                  "0.1", xfigFilter, 0L));
  EPSExport* epsFilter = new EPSExport ();
  filters.insert ("EPS", new FilterInfo (FilterInfo::FKind_Export,
                                          "Encapsulated PostScript", "eps",
                                          "Kai-Uwe Sattler",
                                          "0.1", 0L, epsFilter));
  SVGExport* svgFilter = new SVGExport ();
  filters.insert ("SVG", new FilterInfo (FilterInfo::FKind_Export,
                                          "Scalable Vector Graphics", "svg",
                                          "Kai-Uwe Sattler",
                                          "0.1", 0L, svgFilter));
#if defined(not_yet)
  CmxImport* cmxFilter = new CmxImport ();
  filters.insert ("CMX", new FilterInfo (FilterInfo::FKind_Import,
                                                  "Corel Exchange", "cmx",
                                                  "Kai-Uwe Sattler",
                                                  "0.1", cmxFilter, 0L));
  PSImport* psFilter = new PSImport ();
  if (psFilter->installed ())
    filters.insert ("PS", new FilterInfo (FilterInfo::FKind_Import,
                                          "Postscript", "ps",
                                          "Kai-Uwe Sattler",
                                          "0.1", psFilter, 0L));
#endif
}

QString FilterManager::importFilters () {
  QString s;
  QDictIterator<FilterInfo> iter (filters);
  for (; iter.current (); ++iter) {
    FilterInfo *fi = iter.current ();
    if (fi->kind () == FilterInfo::FKind_Import) {
      QString buf="*.";
      buf+=fi->extension();
      buf+="|";
      buf+=fi->type();
      buf+=" (*.";
      buf+=fi->extension ();
      buf+=")";
      if (s.length () > 0)
        s += "\n";
      s += buf;
    }
  }
  return s;
}

QString FilterManager::exportFilters (const char* defaultExt) {
  // filter info for defaultExt should be at top of the list
  QString s;
  QDictIterator<FilterInfo> iter (filters);
  if (defaultExt != 0L) {
      for (; iter.current (); ++iter) {
          FilterInfo *fi = iter.current ();
          if (fi->kind () == FilterInfo::FKind_Export &&
              fi->extension ()==QString(defaultExt)) {
              QString buf="*.";
              buf+=fi->extension();
              buf+="|";
              buf+=fi->type();
              buf+=" (*.";
              buf+=fi->extension ();
              buf+=")";
              s += buf;
          }
      }
  }
  for (iter.toFirst (); iter.current (); ++iter) {
    FilterInfo *fi = iter.current ();
    if (fi->kind () == FilterInfo::FKind_Export) {
        if (defaultExt != 0L &&
            fi->extension()==QString(defaultExt))
            continue;

        QString buf="*.";
        buf+=fi->extension();
        buf+="|";
        buf+=fi->type();
        buf+=" (*.";
        buf+=fi->extension ();
        buf+=")";
      if (s.length () > 0)
        s += "\n";
      s += buf;
    }
  }
  return s;
}

QString FilterManager::extension (const char *fname) {
  QString file (fname);
  QString ext;
  int pos = file.findRev ('.', -1, false);
  if (pos != -1)
    ext = file.mid (pos + 1, file.length () - pos);
  return ext;
}

FilterInfo* FilterManager::findFilter (const char* fname,
                                       FilterInfo::Kind kind) {
  FilterInfo* info = 0L;
  QString ext = extension (fname);

  QDictIterator<FilterInfo> iter (filters);
  for (; iter.current (); ++iter) {
    FilterInfo *fi = iter.current ();
    if (fi->kind () == kind && ext == fi->extension ()) {
      info = fi;
      break;
    }
  }
  return info;
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
    flist.append(iter.currentKey().latin1());
  return flist;
}
