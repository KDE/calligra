/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

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

#ifndef GDocument_h_
#define GDocument_h_

#include <qobject.h>
#include <qlist.h>
#include <qvaluelist.h>
#include <qstring.h>

#include "Coord.h"

#define KILLUSTRATOR_MIMETYPE "application/x-killustrator"

class GObject;
class GLayer;
class GPage;
class QDomDocument;
class QDomElement;
class KIllustratorDocument;

class GDocument : public QObject
{
  Q_OBJECT
public:
  GDocument (KIllustratorDocument *_doc);
  ~GDocument ();

  void initialize ();
  void setAutoUpdate (bool flag);

  KIllustratorDocument *document(void) const {return doc;};
  QString fileName () const { return filename; }
  void setFileName (const QString &s) { filename = s; }

  virtual void setModified (bool flag = true);
  bool isModified () const { return modifyFlag; }

  /*
   * Pages management
   */

  // get an array with all pages of the document
  const QList<GPage>& getPages ();

  // retrieve the active page
  GPage *activePage();

  // set the active page
  void setActivePage (GPage *page);
  void setActivePage (int i);

  // add a new page
  GPage *addPage ();

  // delete the given page
  void deletePage (GPage *page);

  // find page with name
  GPage *findPage(QString name);

  QDomDocument saveToXml();
  bool readFromXml (const QDomDocument &document);

//  Grid, Helplines
  void setGrid (float dx, float dy, bool snap);
  void getGrid (float& dx, float& dy, bool& snap);

  void setHelplines (const QValueList<float>& hlines,
                     const QValueList<float>& vlines,
                     bool snap);
  void getHelplines (QValueList<float>& hlines, QValueList<float>& vlines,
                     bool& snap);

public slots:
  void helplineStatusChanged ();

signals:
  void pageChanged();
  void changed ();
  void changed (const Rect& r);
  void selectionChanged ();
  void sizeChanged ();
  void gridChanged ();

  void wasModified (bool flag);

protected:
  KIllustratorDocument *doc;

  int curPageNum;
  bool autoUpdate;
  bool modifyFlag; 
  QString filename;
  int paperWidth, paperHeight; // pt
  QList<GPage> pages; // the array of all pages
  GPage *active_page;     // the current page
  Rect selBox;
  bool selBoxIsValid;
  bool snapToGrid, snapToHelplines;
  float gridx, gridy;
  QValueList<float> hHelplines, vHelplines;
};

#endif
