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
#include <qcolor.h>

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

  // convert index to page
  GPage *pageForIndex (int i);
  
  // movq page
  void movePage( int from, int to, bool before );

  // delete the given page
  void deletePage (GPage *page);

  // find page with name
  GPage *findPage(QString name);

  QDomDocument saveToXml();
  bool readFromXml (const QDomDocument &document);

//  Grid
  void showGrid (bool flag);
  bool showGrid () const { return gridIsOn; }

  void snapToGrid (bool flag);
  bool snapToGrid () const { return gridSnapIsOn; }
  
  void gridColor(QColor color);
  QColor gridColor() const { return mGridColor; };

  void setGridDistance (float hdist, float vdist);
  float horizGridDistance () const { return gridx; }
  float vertGridDistance () const { return gridy; }

//  Helplines
  void setHorizHelplines(const QValueList<float>& lines);
  void setVertHelplines(const QValueList<float>& lines);
  QValueList<float>& horizHelplines () { return hHelplines;};
  QValueList<float>& vertHelplines () { return vHelplines;};
  void alignToHelplines (bool flag);
  bool alignToHelplines () { return helplinesSnapIsOn;};
  void showHelplines (bool flag);
  bool showHelplines () { return helplinesAreOn;};
  int indexOfHorizHelpline (float pos);
  int indexOfVertHelpline (float pos);
  void updateHorizHelpline (int idx, float pos);
  void updateVertHelpline (int idx, float pos);
  void addHorizHelpline (float pos);
  void addVertHelpline (float pos);

public slots:
  void setModified (bool flag = true);
  void helplineStatusChanged ();
  void emitChanged();
  void emitChanged(const Rect& r);
  void emitHandleChanged();
  void emitSizeChanged();

signals:
  void pageChanged();
  void handleChanged();
  void changed ();
  void changed (const Rect& r);
  void selectionChanged ();
  void sizeChanged ();
  void gridChanged ();

  void wasModified (bool flag);

protected:
  KIllustratorDocument *doc;
  QList<GPage> pages;            // the array of all pages
  GPage *active_page;            // the current page
  
  /*Grid settings*/
  QColor mGridColor;
  float gridx, gridy;
  bool gridSnapIsOn;
  bool gridIsOn;

  /*Helplines settings*/
  QValueList<float> hHelplines, vHelplines;
  bool helplinesSnapIsOn;
  bool helplinesAreOn;

  int curPageNum;
  bool autoUpdate;
  bool modifyFlag; 
  QString filename;
  Rect selBox;
  bool selBoxIsValid;
};

#endif
