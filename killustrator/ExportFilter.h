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

#ifndef ExportFilter_h_
#define ExportFilter_h_

#include <qstring.h>

class GDocument;

class ExportFilter {

public:
    virtual ~ExportFilter ();

    void setOutputFileName (const QString &fname);
    QString outputFileName () const;

    virtual bool setup (GDocument* doc, const char* format) = 0;
    virtual bool exportToFile (GDocument *doc) = 0;
    virtual bool installed () { return true; }

protected:
    ExportFilter ();
    QString filename;
};

#endif
