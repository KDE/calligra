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

#include <iostream.h>
#include <fstream.h>
#include <assert.h>

#include <qstrlist.h>

#include "filter/FilterManager.h"
#include "FilterInfo.h"
#include "filter_utils.h"
#include "GDocument.h"

void list_export_filters () {
  FilterManager* fmgr = FilterManager::instance ();
  assert (fmgr != NULL);
  QStrList filters = fmgr->getInstalledFilters ();
  const char *fstr = filters.first ();
  while (fstr != NULL) {
    FilterInfo* finfo = fmgr->getFilterForType (fstr);
    assert (finfo != NULL);
    if (finfo->kind () == FilterInfo::FKind_Export) {
      cout << fstr << "\t" << finfo->type().latin1() << endl;
    }
    fstr = filters.next ();
  }
}

bool convert_to_format (const char*/*fmt*/, char **/*files*/, int /*num*/) {

    // FIXME (Werner)
    /*FilterManager* fmgr = FilterManager::instance ();
  FilterInfo* finfo = fmgr->getFilterForType (fmt);
  if (finfo == NULL || finfo->exportFilter () == NULL)
    return false;

  ExportFilter* filter = finfo->exportFilter ();
  cout << num << endl;
  for (int i = 0; i < num; i++) {
    const char* fname = files[i];
    cout << "processing " << fname << "..." << flush;
    GDocument *document = new GDocument ();
    ifstream is (fname);
    if (is.fail ())
      return false;
    QString str1 (fname);
    int pos1 = str1.findRev ('/');
    int pos2 = str1.find (".kil");
    pos1 = (pos1 == -1 ? 0 : pos1 + 1);
    QString str2 = str1.mid (pos1, pos2 - pos1 + 1);
    str2.append (finfo->extension ());

    document->readFromXml (is);
    if (filter->setup (document, finfo->extension ())) {
      filter->setOutputFileName (str2.data ());
      filter->exportToFile (document);
      cout << "done !" << endl;
    }
    else
      cout << "failed !" << endl;
    delete document;
  }
    */
  return true;
}
