/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
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

#ifndef __GDocument_h__
#define __GDocument_h__

#include <qobject.h>
#include <qptrlist.h>
#include <qvaluelist.h>
#include <qcolor.h>

#include "GStyle.h"
#include "GStyleList.h"

class KontourDocument;
class QDomDocument;
class QDomElement;
class KoRect;

namespace Kontour {
class GPage;

class GDocument : public QObject
{
  Q_OBJECT
public:
  GDocument(KontourDocument *aDoc);
  ~GDocument();

  KontourDocument *document() const {return mDoc; }

  GStyleList *styles() {return &mStyles; }

  void updateCanvas(bool b);

  /**
   *
   *
   */
  double zoomFactor() const {return mZoomFactor; }
  void zoomFactor(double factor);

  int xCanvas() const {return mXCanvas; }
  int yCanvas() const {return mYCanvas; }

  // Grid
  bool showGrid() const {return mShowGrid; }
  void showGrid(bool flag);

  bool snapToGrid() const {return mSnapToGrid; }
  void snapToGrid(bool flag);

  QColor gridColor() const {return mGridColor; }
  void gridColor(QColor color);

  void setGridDistance(double hdist, double vdist);
  double xGrid() const {return mXGrid; }
  double yGrid() const {return mYGrid; }
  double xGridZ() const {return mXGridZ; }
  double yGridZ() const {return mYGridZ; }

  // Helplines
  bool showHelplines() const {return mShowHelplines; }
  void showHelplines(bool flag);
  
  
  bool snapToHelplines() const {return mSnapToHelplines; }
  void snapToHelplines(bool flag);

  QValueList<double> &horizHelplines() {return mHorizHelplines; }
  QValueList<double> &vertHelplines() {return mVertHelplines; }
  void horizHelplines(const QValueList<double> &lines);
  void vertHelplines(const QValueList<double> &lines);

  int indexOfHorizHelpline(double pos);
  int indexOfVertHelpline(double pos);
  void updateHorizHelpline(int idx, double pos);
  void updateVertHelpline(int idx, double pos);

  void addHorizHelpline(double pos);
  void addVertHelpline(double pos);

  /**
   *
   *
   */
  QDomDocument saveToXml();
  bool readFromXml(const QDomDocument &document);

  /*
   * Pages management
   */

  QPtrList<GPage> &getPages() {return pages; }

  // retrieve the active page
  GPage *activePage() const {return mActivePage; }

  // set the active page
  void activePage(GPage *page);
  void activePage(int i);

  // add a new page
  GPage *addPage();

  // delete the given page
  void deletePage(GPage *page);

  // convert index to page
  GPage *pageForIndex(int i);

  // move page
  void movePage(int from, int to, bool before);

  // find page with name
  GPage *findPage(QString name);
  
  void emitChanged(const KoRect &r, bool handle = false);
  void emitSelectionChanged();

public slots:
  void setModified(bool flag = true);

signals:
  void zoomFactorChanged(double scale);  // zoom factor was changed
  void gridChanged();
  void helplinesChanged();
  void pageChanged();                    // active page was changed
  void selectionChanged();
  void updateLayerView();
  void changed(const KoRect &r, bool handle);

private slots:
  void changeCanvas();

private:
  KontourDocument *mDoc;

  GStyleList mStyles;                  // the list of styles

  double mXRes;
  double mYRes;

  /*Grid settings*/
  QColor mGridColor;                   // grid color
  double mXGridZ;                      //
  double mYGridZ;                      //
  double mXGrid;                       //
  double mYGrid;                       //
  bool mShowGrid:1;                    //
  bool mSnapToGrid:1;                  //

  /*Helplines settings*/
  QValueList<double> mHorizHelplines;  //
  QValueList<double> mVertHelplines;   //
  bool mShowHelplines:1;               //
  bool mSnapToHelplines:1;             //

  /*Zoom*/
  double mZoomFactor;                  // zoom factor

  int mXCanvas;
  int mYCanvas;

  QPtrList<GPage> pages;               // the array of all pages
  GPage *mActivePage;                  // the current page

  int mCurPageNum;
  bool mUpdateCanvas;                  // update canvas
};
};
using namespace Kontour;

#endif
