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

#ifndef KIllustrator_h_
#define KIllustrator_h_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ktopwidget.h>
#include <ktoolbar.h>
#include <kstatusbar.h>
#include <kfm.h>
#include <drag.h>

#include <qlist.h>
#include "ToolPalette.h"
#include "ToolGroup.h"
// #include "ColorSelectionPalette.h"
#include "ToolController.h"
#include "Ruler.h"
#include "GObject.h"
#include "CommandHistory.h"
#include "MainView.h"

class QwViewport;
class Canvas;
class GDocument;
class TransformationDialog;

#define ID_FILE_NEW              1
#define ID_FILE_OPEN             2
#define ID_FILE_OPEN_RECENT     20
#define ID_FILE_SAVE             4
#define ID_FILE_SAVE_AS          5
#define ID_FILE_CLOSE            6
#define ID_FILE_PRINT            7
#define ID_FILE_NEW_WINDOW       8 
#define ID_FILE_EXIT            10 

#define ID_IMPORT             1000
#define ID_EXPORT             2000

#define ID_EDIT_UNDO           200
#define ID_EDIT_REDO           201
#define ID_EDIT_CUT            202
#define ID_EDIT_COPY           203
#define ID_EDIT_PASTE          204
#define ID_EDIT_DELETE         205
#define ID_EDIT_SELECT_ALL     206
#define ID_EDIT_PROPERTIES     207

#define ID_VIEW_RULER          300
#define ID_VIEW_GRID           301

#define ID_LAYOUT_PAGE         400
#define ID_LAYOUT_GRID         401

#define ID_ARRANGE_ALIGN       500
#define ID_ARRANGE_FRONT       501
#define ID_ARRANGE_BACK        502
#define ID_ARRANGE_1_FORWARD   503
#define ID_ARRANGE_1_BACK      504
#define ID_ARRANGE_GROUP       505
#define ID_ARRANGE_UNGROUP     506

#define ID_TRANSFORM_POSITION  600
#define ID_TRANSFORM_DIMENSION 601
#define ID_TRANSFORM_ROTATION  602
#define ID_TRANSFORM_MIRROR    603

#define ID_EXTRAS_OPTIONS      700
#define ID_EXTRAS_CLIPART      701

#define ID_HELP_HELP           800
#define ID_HELP_ABOUT_APP      801
#define ID_HELP_ABOUT_KDE      802

class KIllustrator : public KTopLevelWidget, public MainView {
  Q_OBJECT

public:
  enum TransferDirection { Transfer_Get, Transfer_Put };

  KIllustrator (const char* url = NULL);
  ~KIllustrator ();
  
  GDocument* activeDocument () { return document; }
  Canvas* getCanvas () { return canvas; }

protected:
  QSize sizeHint () const { return QSize (700, 500); }
  void closeEvent (QCloseEvent*);
  
public slots:
  void menuCallback (int item);

  void setPenColor (const QBrush& b);
  void setFillColor (const QBrush& b);

  void slotKFMJobDone ();
  void dropActionSlot (KDNDDropZone* dzone);

  void showCursorPosition (int x, int y);
  void showCurrentMode (const char* msg);

  void updateZoomFactor (float zFactor);

  void updateRecentFiles ();
  void updateSettings ();

protected slots:
  void zoomFactorSlot (int);
  void selectColor (int flag, const QBrush& b);

private:
  static bool closeWindow (KIllustrator* win);
  static void quit ();

  void saveFile ();
  void saveAsFile ();
  bool askForSave ();

  void setupMainView ();
  void initToolBars ();
  void initMenu ();
  void initStatusBar ();

  void about (int id);

  void openFile (const char* fname);
  void openURL (const char* url);
  void exportToFile (int id);
  void importFromFile (int id);

  void setFileCaption (const char* fname);

  void showTransformationDialog (int id);

  KToolBar *toolbar, *toolPalette, *colorPalette;
  KStatusBar* statusbar;
  KMenuBar* menubar;
  
  QGridLayout *gridLayout;

  QPopupMenu *file, *edit, *view, *layout, *effects, 
    *arrangement, *extras, *help;
  QPopupMenu *openRecent;
  ToolGroup *tgroup;
  ToolController *tcontroller;
  QwViewport *viewport;
  GDocument *document;
  Canvas *canvas;
  Ruler *hRuler, *vRuler;
  TransformationDialog *transformationDialog;

  // direction of file transfer
  TransferDirection ioDir;
  // the KFM connection
  KFM *kfmConn;
  // the drop zone
  KDNDDropZone *dropZone;

  CommandHistory cmdHistory;

  QString localFile;
  QArray<float> zFactors;

  static QList<GObject> clipboard;
  static QList<KIllustrator> windows;
};

#endif
