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

#include <fstream.h>
#include <qkeycode.h>
#include <qstrlist.h>
#include <qfileinfo.h>
#include <unistd.h>
#include "KIllustrator.h"
#include "KIllustrator.moc"
#include "QwViewport.h"
#include "GDocument.h"
#include "Canvas.h"
#include "Tool.h"
#include "RectangleTool.h"
#include "PolylineTool.h"
#include "FreeHandTool.h"
#include "SelectionTool.h"
#include "OvalTool.h"
#include "TextTool.h"
#include "PolygonTool.h"
#include "EditPointTool.h"
#include "BezierTool.h"
#include "ZoomTool.h"
#include "PropertyEditor.h"
#include "AlignmentDialog.h"
#include "GridDialog.h"
#include "TransformationDialog.h"
#include "OptionDialog.h"
#include "LayerDialog.h"
#include "ColorField.h"
#include "PStateManager.h"
#include "ExportFilter.h"
#include "AboutDialog.h"
#include "InsertClipartCmd.h"
#include "GroupCmd.h"
#include "UngroupCmd.h"
#include "DeleteCmd.h"
#include "CutCmd.h"
#include "CopyCmd.h"
#include "PasteCmd.h"
#include "ReorderCmd.h"
#include "SetPropertyCmd.h"
#include "filter/FilterManager.h"
#include "ToolButton.h"
#include "Preview.h"
#include "units.h"
#include "ScriptDialog.h"
#include "DocumentInfo.h"
#include <kiconloader.h>
#include <klocale.h>
#include <kapp.h>
#include <kmsgbox.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <kcombo.h>
#include <qlayout.h>
#include <unistd.h>

#include "koPageLayoutDia.h"

#ifdef HAVE_CONFIG_H
#include "config.h"

#ifdef HAVE_TIME_H
#include <time.h>
#endif
#endif

QList<KIllustrator> KIllustrator::windows;
bool KIllustrator::previewHandlerRegistered = false;

KIllustrator::KIllustrator (const char* url) : KTopLevelWidget () {
  windows.setAutoDelete (false);
  windows.append (this);

  // if kfmConn is null, there is no
  // current IO job.
  kfmConn = 0;

  psm = PStateManager::instance ();

  zFactors.resize (5);
  zFactors[0] = 0.5;
  zFactors[1] = 1.0;
  zFactors[2] = 1.5;
  zFactors[3] = 2.0;
  zFactors[4] = 4.0;

  canvas = 0L;
  scriptDialog = 0L;
  layerDialog = 0L;
  tgroup = new ToolGroup ();
  initMenu ();
  initStatusBar ();
  
  document = new GDocument ();

  setupMainView ();

  initToolBars ();
  toolbar->show ();
  statusbar->show ();
  adjustSize ();
  
  dropZone = new KDNDDropZone (canvas, DndURL);
  connect (dropZone, SIGNAL(dropAction (KDNDDropZone *)), 
	   this, SLOT (dropActionSlot (KDNDDropZone *)));

  Canvas::initZoomFactors (zFactors);

  setCaption ("KIllustrator");

  connect (PStateManager::instance (), SIGNAL (recentFilesChanged ()),
  	   this, SLOT (updateRecentFiles ()));
  connect (PStateManager::instance (), SIGNAL (settingsChanged ()),
  	   this, SLOT (updateSettings ()));

  view->setItemChecked (ID_VIEW_GRID, canvas->showGrid ());

  if (! previewHandlerRegistered) {
    KFilePreviewDialog::registerPreviewModule ("kil", kilPreviewHandler,
					       PreviewPixmap);
    KFilePreviewDialog::registerPreviewModule ("wmf", wmfPreviewHandler,
					       PreviewPixmap);
    previewHandlerRegistered = true;
  }

  setFileCaption (UNNAMED_FILE);

  if (url != 0L)
    openURL (url);
}

KIllustrator::~KIllustrator () {
  if (! localFile.isEmpty () && localFile.find ("/tmp/killu") != -1) {
    // remove temporary file
    unlink ((const char *) localFile);
  }

  windows.removeRef (this);
//  delete toolbar;
//  delete menubar;
  delete tgroup;
}

void KIllustrator::closeEvent (QCloseEvent*) {
//  delete this;
  closeWindow (this);
}

void KIllustrator::setupMainView () {
  QPixmap pixmap;
  
  QWidget *w = new QWidget (this);
  gridLayout = new QGridLayout (w, 2, 2);
  
  MeasurementUnit mu = psm->defaultMeasurementUnit ();
  hRuler = new Ruler (Ruler::Horizontal, mu, w);
  vRuler = new Ruler (Ruler::Vertical, mu, w);
  gridLayout->addWidget (hRuler, 0, 1);
  gridLayout->addWidget (vRuler, 1, 0);

  viewport = new QwViewport (w);

  canvas = new Canvas (document, 72.0, viewport, viewport->portHole ());
  connect (canvas, SIGNAL(sizeChanged ()), 
           viewport, SLOT(resizeScrollBars ()));
  connect (canvas, SIGNAL(visibleAreaChanged (int, int)),
	   hRuler, SLOT(updateVisibleArea (int, int)));
  connect (canvas, SIGNAL(visibleAreaChanged (int, int)),
	   vRuler, SLOT(updateVisibleArea (int, int)));

  connect (canvas, SIGNAL(zoomFactorChanged (float)),
	   hRuler, SLOT(setZoomFactor (float)));
  connect (canvas, SIGNAL(zoomFactorChanged (float)),
	   vRuler, SLOT(setZoomFactor (float)));
  connect (canvas, SIGNAL(zoomFactorChanged (float)),
	   this, SLOT(updateZoomFactor (float)));
  connect (canvas, SIGNAL(mousePositionChanged (int, int)),
	   this, SLOT(showCursorPosition(int, int)));
  connect (canvas, SIGNAL(mousePositionChanged (int, int)),
	   hRuler, SLOT(updatePointer(int, int)));
  connect (canvas, SIGNAL(mousePositionChanged (int, int)),
	   vRuler, SLOT(updatePointer(int, int)));

  gridLayout->addWidget (viewport, 1, 1);
  gridLayout->setRowStretch (1, 20);
  gridLayout->setColStretch (1, 20);

  setView (w);
  
  tcontroller = new ToolController (this);
  Tool* tool;
  tcontroller->registerTool (0, tool = new SelectionTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (1, tool = new EditPointTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (2, tool = new FreeHandTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (3, tool = new PolylineTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (4, tool = new BezierTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (5, tool = new RectangleTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (6, tool = new PolygonTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (7, tool = new OvalTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (8, tool = new TextTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (9, tool = new ZoomTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->toolSelected (0);

  canvas->setToolController (tcontroller);

  setUndoStatus(false, false);
  connect(&cmdHistory, SIGNAL(changed(bool, bool)), 
	  SLOT(setUndoStatus(bool, bool)));
}

void KIllustrator::initToolBars () {
  QPixmap pixmap;
  KIconLoader* loader = kapp->getIconLoader ();

  /* main toolbar */
  toolbar = new KToolBar (this);
  
  pixmap = loader->loadIcon ("filenew2.xpm");
  toolbar->insertButton (pixmap, ID_FILE_NEW, true,
			 i18n ("New Document"));
  
  pixmap = loader->loadIcon ("fileopen.xpm");
  toolbar->insertButton (pixmap, ID_FILE_OPEN, true,
			 i18n ("Open Document"));
  
  pixmap = loader->loadIcon ("filefloppy.xpm");
  toolbar->insertButton (pixmap, ID_FILE_SAVE, true,
			 i18n ("Save Document"));
  
  toolbar->insertSeparator ();
  
  pixmap = loader->loadIcon ("fileprint.xpm");
  toolbar->insertButton (pixmap, ID_FILE_PRINT, true,
			 i18n ("Print Document"));
  toolbar->insertSeparator ();
  
  pixmap = loader->loadIcon ("editcopy.xpm");
  toolbar->insertButton (pixmap, ID_EDIT_COPY, true,
			 i18n ("Copy"));
  
  pixmap = loader->loadIcon ("editpaste.xpm");
  toolbar->insertButton (pixmap, ID_EDIT_PASTE, true,
			 i18n ("Paste"));
  
  pixmap = loader->loadIcon ("editcut.xpm");
  toolbar->insertButton (pixmap, ID_EDIT_CUT, true,
			 i18n ("Cut"));

  toolbar->insertSeparator ();

  QStrList zoomStrList;
  for (int i = 0; i < 5; i++) {
    char buf[8];
    sprintf (buf, "%3.0f%%", zFactors[i] * 100);
    zoomStrList.append (buf);
  }

  toolbar->insertCombo (&zoomStrList, 10, true, SIGNAL(activated(int)), 
			this, SLOT(zoomFactorSlot (int)),
			true, 0L, 85);
  KCombo* combo = toolbar->getCombo (10);

  for (int i = 0; i < (int) zFactors.size (); i++) {
    if (zFactors[i] == 1) {
      combo->setCurrentItem (i);
      break;
    }
  }

  toolbar->setBarPos (KToolBar::Top);
  connect (toolbar, SIGNAL (clicked(int)), this, SLOT (menuCallback (int)));
  addToolBar (toolbar);

  /* the "tool" toolbar */
  toolPalette = new KToolBar (this);
  ToolButton* toolButton;

  toolButton = new ToolButton (loader->loadIcon ("selecttool.xpm"), 
			       toolPalette,
                               i18n ("Selection Mode"));
  toolPalette->insertWidget (0, toolButton->width (), toolButton);
  tgroup->insertButton (0, toolButton);

  toolButton = new ToolButton (loader->loadIcon ("pointtool.xpm"), 
			       toolPalette,
                               i18n ("Edit Point"));
  toolPalette->insertWidget (1, toolButton->width (), toolButton);
  tgroup->insertButton (1, toolButton);

  toolButton = new ToolButton (loader->loadIcon ("freehandtool.xpm"), 
			       toolPalette,
                               i18n ("Create FreeHand Line"));
  toolPalette->insertWidget (2, toolButton->width (), toolButton);
  tgroup->insertButton (2, toolButton);

  toolButton = new ToolButton (loader->loadIcon ("linetool.xpm"), 
			       toolPalette,
                               i18n ("Create Polyline"));
  toolPalette->insertWidget (3, toolButton->width (), toolButton);
  tgroup->insertButton (3, toolButton);

  toolButton = new ToolButton (loader->loadIcon ("beziertool.xpm"), 
			       toolPalette,
                               i18n ("Create Bezier Curve"));
  toolPalette->insertWidget (4, toolButton->width (), toolButton);
  tgroup->insertButton (4, toolButton);

  toolButton = new ToolButton (loader->loadIcon ("recttool.xpm"), 
			       toolPalette,
                               i18n ("Create Rectangle"));
  toolPalette->insertWidget (5, toolButton->width (), toolButton);
  tgroup->insertButton (5, toolButton);

  toolButton = new ToolButton (loader->loadIcon ("polygontool.xpm"), 
			       toolPalette,
                               i18n ("Create Polygon"));
  toolPalette->insertWidget (6, toolButton->width (), toolButton);
  tgroup->insertButton (6, toolButton);

  toolButton = new ToolButton (loader->loadIcon ("ellipsetool.xpm"), 
			       toolPalette,
                               i18n ("Create Ellipse"));
  toolPalette->insertWidget (7, toolButton->width (), toolButton);
  tgroup->insertButton (7, toolButton);

  toolButton = new ToolButton (loader->loadIcon ("texttool.xpm"), 
			       toolPalette,
                               i18n ("Create Text"));
  toolPalette->insertWidget (8, toolButton->width (), toolButton);
  tgroup->insertButton (8, toolButton);

  toolButton = new ToolButton (loader->loadIcon ("zoomtool.xpm"), 
			       toolPalette,
                               i18n ("Zoom In"));
  toolPalette->insertWidget (9, toolButton->width (), toolButton);
  tgroup->insertButton (9, toolButton);

  connect (tgroup, SIGNAL (toolSelected (int)), tcontroller,
  	   SLOT(toolSelected (int)));
  connect (tgroup, SIGNAL (toolConfigActivated (int)), tcontroller,
	   SLOT(configureTool (int)));

  toolPalette->setBarPos (KToolBar::Left);
  addToolBar (toolPalette);

  /* the color toolbar */
  colorPalette = new KToolBar (this);

  const QColor cpalette[] = { white, red, green, blue, cyan, magenta, yellow,
			      darkRed, darkGreen, darkBlue, darkCyan,
			      darkMagenta, darkYellow, white, lightGray,
			      gray, darkGray, black };

  for (int i = 0; i < 18; i++) {
    QBrush brush (cpalette[i], i == 0 ? NoBrush : SolidPattern);
    ColorField* cfield = new ColorField (brush, colorPalette);
    connect (cfield, SIGNAL(colorSelected (int, const QBrush&)),
	     this, SLOT(selectColor (int, const QBrush&)));
    colorPalette->insertWidget (i, cfield->width (), cfield);
  }
  colorPalette->setBarPos (KToolBar::Right);
  addToolBar (colorPalette);
}

void KIllustrator::initMenu () {
  file = new QPopupMenu ();
  edit = new QPopupMenu ();
  layout = new QPopupMenu ();
  view = new QPopupMenu ();
  arrangement = new QPopupMenu ();
  extras = new QPopupMenu ();
  help = new QPopupMenu ();
  openRecent = new QPopupMenu ();
  
  file->insertItem (i18n ("&New..."), ID_FILE_NEW);
  file->setAccel (CTRL + Key_N, ID_FILE_NEW);
  file->insertItem (i18n ("&Open..."), ID_FILE_OPEN);
  file->setAccel (CTRL + Key_O, ID_FILE_OPEN);

  connect (openRecent, SIGNAL (activated (int)), SLOT (menuCallback (int)));
  updateRecentFiles ();

  file->insertItem (i18n ("Open Recent"), openRecent);
  file->insertSeparator ();
  file->insertItem (i18n ("&Save"), ID_FILE_SAVE);
  file->setAccel (CTRL + Key_S, ID_FILE_SAVE);
  file->insertItem (i18n ("S&ave as..."), ID_FILE_SAVE_AS);
  file->insertItem (i18n ("&Close"), ID_FILE_CLOSE);
  file->setAccel (CTRL + Key_W, ID_FILE_CLOSE);
  file->insertSeparator ();

  QPopupMenu* efilters = new QPopupMenu ();
  QPopupMenu* ifilters = new QPopupMenu ();
  FilterManager* filterMgr = FilterManager::instance ();
  QStrList ftypes = filterMgr->getInstalledFilters ();
  const char* ftype = ftypes.first ();
  unsigned int tid = 1;
  while (ftype) {
    FilterInfo* finfo = filterMgr->getFilterForType (ftype);
    if (finfo->kind () == FilterInfo::FKind_Export) {
      efilters->insertItem (ftype, ID_EXPORT + tid);
    }
    else if (finfo->kind () == FilterInfo::FKind_Import) {
      ifilters->insertItem (ftype, ID_IMPORT + tid);
    }
    tid++;
    ftype = ftypes.next ();
  }
  connect (efilters, SIGNAL (activated (int)), SLOT (menuCallback (int)));
  connect (ifilters, SIGNAL (activated (int)), SLOT (menuCallback (int)));
  file->insertItem (i18n ("Import"), ifilters);
  file->insertItem (i18n ("Export"), efilters);

  file->insertSeparator ();
  file->insertItem (i18n ("&Print"), ID_FILE_PRINT);
  file->setAccel (CTRL + Key_P, ID_FILE_PRINT);
  file->insertItem (i18n ("Document Info"), ID_FILE_INFO);
  file->insertSeparator ();
  file->insertItem (i18n ("New &Window"), ID_FILE_NEW_WINDOW);
  file->insertSeparator ();
  file->insertItem (i18n ("E&xit"), ID_FILE_EXIT);
  file->setAccel (CTRL + Key_Q, ID_FILE_EXIT);

  connect (file, SIGNAL (activated (int)), SLOT (menuCallback (int)));
   
  edit->insertItem (i18n ("Undo"), ID_EDIT_UNDO);
  edit->setAccel (CTRL + Key_Z, ID_EDIT_UNDO);
  edit->insertItem (i18n ("Redo"), ID_EDIT_REDO);
  edit->insertSeparator ();
  edit->insertItem (i18n ("&Copy"), ID_EDIT_COPY);
  edit->setAccel (CTRL + Key_C, ID_EDIT_COPY);
  edit->insertItem (i18n ("&Paste"), ID_EDIT_PASTE);
  edit->setAccel (CTRL + Key_V, ID_EDIT_PASTE);
  edit->insertItem (i18n ("C&ut"), ID_EDIT_CUT);
  edit->setAccel (CTRL + Key_X, ID_EDIT_CUT);
  edit->insertSeparator ();
  edit->insertItem (i18n ("&Delete"), ID_EDIT_DELETE);
  edit->setAccel (Key_Delete, ID_EDIT_DELETE);
  edit->insertSeparator ();
  edit->insertItem (i18n ("&Select All"), ID_EDIT_SELECT_ALL);
  edit->insertSeparator ();
  edit->insertItem (i18n ("Pr&operties"), ID_EDIT_PROPERTIES);
  connect (edit, SIGNAL (activated (int)), SLOT (menuCallback (int)));

  view->insertItem (i18n ("Layers..."), ID_VIEW_LAYERS);
  view->insertSeparator ();
  view->insertItem (i18n ("Show Ruler"), ID_VIEW_RULER);
  view->setItemChecked (ID_VIEW_RULER, true);
  view->insertItem (i18n ("Show Grid"), ID_VIEW_GRID);
  view->setItemChecked (ID_VIEW_GRID, false);
  connect (view, SIGNAL (activated (int)), SLOT (menuCallback (int)));

  layout->insertItem (i18n ("&Page"), ID_LAYOUT_PAGE);
  layout->insertSeparator ();
  layout->insertItem (i18n ("&Grid"), ID_LAYOUT_GRID);
  connect (layout, SIGNAL (activated (int)), SLOT (menuCallback (int)));
  
  QPopupMenu* transformations = new QPopupMenu ();
  transformations->insertItem (i18n ("Position"), 
			       ID_TRANSFORM_POSITION);
  transformations->insertItem (i18n ("Dimension"), 
			       ID_TRANSFORM_DIMENSION);
  transformations->insertItem (i18n ("Rotation"), 
			       ID_TRANSFORM_ROTATION);
  transformations->insertItem (i18n ("Mirror"), 
			       ID_TRANSFORM_MIRROR);
  connect (transformations, SIGNAL (activated (int)), 
	   SLOT (menuCallback (int)));

  arrangement->insertItem (i18n ("Transform"), transformations);
  arrangement->insertItem (i18n ("Align"), ID_ARRANGE_ALIGN);
  arrangement->setAccel (CTRL + Key_A, ID_ARRANGE_ALIGN);
  arrangement->insertItem (i18n ("To Front"), ID_ARRANGE_FRONT);
  arrangement->setAccel (SHIFT + Key_PageUp, ID_ARRANGE_FRONT);
  arrangement->insertItem (i18n ("To Back"), ID_ARRANGE_BACK);
  arrangement->setAccel (SHIFT + Key_PageDown, ID_ARRANGE_BACK);
  arrangement->insertItem (i18n ("Forward One"),
			   ID_ARRANGE_1_FORWARD);
  arrangement->setAccel (CTRL + Key_PageUp, ID_ARRANGE_1_FORWARD);
  arrangement->insertItem (i18n ("Back One"), 
			   ID_ARRANGE_1_BACK); 
  arrangement->setAccel (CTRL + Key_PageDown, ID_ARRANGE_1_BACK);
  arrangement->insertSeparator ();
  arrangement->insertItem (i18n ("Group"), ID_ARRANGE_GROUP);
  arrangement->setAccel (CTRL + Key_G, ID_ARRANGE_GROUP);
  arrangement->insertItem (i18n ("Ungroup"), ID_ARRANGE_UNGROUP);
  arrangement->setAccel (CTRL + Key_U, ID_ARRANGE_UNGROUP);
  connect (arrangement, SIGNAL (activated (int)), SLOT (menuCallback (int)));

  extras->insertItem (i18n ("&Options..."), ID_EXTRAS_OPTIONS);
  extras->insertSeparator ();
  extras->insertItem (i18n ("&Clipart..."), ID_EXTRAS_CLIPART);
  extras->insertItem (i18n ("&Scripts..."), ID_EXTRAS_SCRIPTS);
  connect (extras, SIGNAL (activated (int)), SLOT (menuCallback (int)));

  help->insertItem (i18n ("&Help..."), ID_HELP_HELP);
  help->insertSeparator ();
  help->insertItem (i18n ("About..."), ID_HELP_ABOUT_APP);
  /*
    help->insertItem (i18n ("About &KDE..."), ID_HELP_ABOUT_KDE);
  */
  connect (help, SIGNAL (activated (int)), SLOT (menuCallback (int)));
  
  menubar = new KMenuBar (this);

  menubar->insertItem (i18n ("&File"), file);
  menubar->insertItem (i18n ("&Edit"), edit);
  menubar->insertItem (i18n ("&View"), view);
  menubar->insertItem (i18n ("&Layout"), layout);
  menubar->insertItem (i18n ("&Arrange"), arrangement);
  menubar->insertItem (i18n ("Ex&tras"), extras);
  menubar->insertItem (i18n ("&Help"), help);
  
  setMenu (menubar);
}

void KIllustrator::initStatusBar () {
  statusbar = new KStatusBar (this);
  setStatusBar (statusbar);

  statusbar->setInsertOrder (KStatusBar::RightToLeft);
  statusbar->insertItem ("XXXX.XX:XXXX.XX pt     ", 1);
  statusbar->insertItem ("                                        ", 2);
  statusbar->enable (KStatusBar::Show);
}

void KIllustrator::showCursorPosition (int x, int y) {
  char buf[100];
  switch (psm->defaultMeasurementUnit ()) {
  case UnitPoint:
    sprintf (buf, "%4.2f:%4.2f pt     ", (float) x, (float) y);
    break;
  case UnitMillimeter:
    sprintf (buf, "%4.2f:%4.2f mm     ", cvtPtToMm (x), cvtPtToMm (y));
    break;
  case UnitInch:
    sprintf (buf, "%4.2f:%4.2f inch   ", cvtPtToInch (x), cvtPtToInch (y));
    break;
  }
  statusbar->changeItem (buf, 1);
}

void KIllustrator::showCurrentMode (const char* msg) {
  statusbar->changeItem (msg, 2);
}

void KIllustrator::updateSettings () {
  MeasurementUnit munit = 
    PStateManager::instance ()->defaultMeasurementUnit ();
  hRuler->setMeasurementUnit (munit);
  vRuler->setMeasurementUnit (munit);
}

void KIllustrator::menuCallback (int item) {
  switch (item) {
  case ID_FILE_NEW:
    if (askForSave ()) {
      document->initialize ();
      cmdHistory.reset ();
      setFileCaption (UNNAMED_FILE);
      tcontroller->toolSelected (0);
    }
    break;
  case ID_FILE_OPEN: 
    {
      if (askForSave ()) {
	QString fname = 
	  KFilePreviewDialog::getOpenFileURL (0, 
					      "*.kil | KIllustrator File", this);
	if (! fname.isEmpty ()) {
	  document->initialize ();
	  openURL ((const char *)fname);
	  cmdHistory.reset ();
	  tcontroller->toolSelected (0);
	}
      }
      break;
    }
  case ID_FILE_SAVE:
    saveFile ();
    break;
  case ID_FILE_SAVE_AS:
    saveAsFile ();
    break;
  case ID_FILE_CLOSE:
    closeWindow (this);
    break;
  case ID_FILE_PRINT:
    //    canvas->printPSDocument ();
    canvas->printDocument ();
    break;
  case ID_FILE_INFO:
    DocumentInfo::showInfo (document);
    break;
  case ID_FILE_NEW_WINDOW:
    {
      KTopLevelWidget* w = new KIllustrator ();
      w->show ();
      break;
    }
  case ID_FILE_EXIT:
    quit ();
    break;
  case ID_EDIT_UNDO:
    cmdHistory.undo ();
//    tcontroller->toolSelected (0);
    tgroup->selectTool (0);
    break;
  case ID_EDIT_REDO:
    cmdHistory.redo ();
//    tcontroller->toolSelected (0);
    tgroup->selectTool (0);
    break;
  case ID_EDIT_CUT:
    cmdHistory.addCommand (new CutCmd (document), true);
    break;
  case ID_EDIT_COPY:
    cmdHistory.addCommand (new CopyCmd (document), true);
    break;
  case ID_EDIT_PASTE:
    cmdHistory.addCommand (new PasteCmd (document), true);
    break;
  case ID_EDIT_DELETE:
    cmdHistory.addCommand (new DeleteCmd (document), true);
    break;
  case ID_EDIT_SELECT_ALL:
    document->selectAllObjects ();
    break;
  case ID_EDIT_PROPERTIES:
    {
      int result = 1;
      
      if (document->selectionIsEmpty ()) {
	result = 
	  KMsgBox::yesNo (this, "Warning", 
			  i18n ("This action will set the default\nproperties for new objects !\nWould you like to do it ?"),
			  KMsgBox::QUESTION, i18n ("Yes"), i18n ("No"));
      }
      if (result == 1)
	PropertyEditor::edit (&cmdHistory, document);
    }
    break;
  case ID_VIEW_LAYERS:
    if (!layerDialog) 
      layerDialog = new LayerDialog ();
    layerDialog->manageDocument (document);
    layerDialog->show ();
    break;
  case ID_VIEW_RULER:
    {
      bool show_it = !view->isItemChecked (ID_VIEW_RULER);
      if (show_it) {
	hRuler->show ();
	vRuler->show ();
      }
      else {
	hRuler->hide ();
	vRuler->hide ();
      }
      gridLayout->activate ();
      view->setItemChecked (ID_VIEW_RULER, show_it);
    }
    break;
  case ID_VIEW_GRID:
    {
      bool show_it = !view->isItemChecked (ID_VIEW_GRID);
      canvas->showGrid (show_it);
      view->setItemChecked (ID_VIEW_GRID, show_it);
    }
    break;
  case ID_LAYOUT_PAGE:
    {
      KoPageLayout pLayout = document->pageLayout ();
      KoHeadFoot header;
	
      if (KoPageLayoutDia::pageLayout (pLayout, header, 
				       FORMAT_AND_BORDERS)) {
	document->setPageLayout (pLayout);
      }
      break;
    }
  case ID_LAYOUT_GRID:
    GridDialog::setupGrid (canvas);
    break;
  case ID_ARRANGE_ALIGN:
    AlignmentDialog::alignSelection (document, &cmdHistory);
    break;
  case ID_ARRANGE_FRONT:
    cmdHistory.addCommand (new ReorderCmd (document, RP_ToFront), true);
    break;
  case ID_ARRANGE_BACK:
    cmdHistory.addCommand (new ReorderCmd (document, RP_ToBack), true);
    break;
  case ID_ARRANGE_1_FORWARD:
    cmdHistory.addCommand (new ReorderCmd (document, RP_ForwardOne), true);
    break;
  case ID_ARRANGE_1_BACK:
    cmdHistory.addCommand (new ReorderCmd (document, RP_BackwardOne), true);
    break;
  case ID_ARRANGE_GROUP:
    cmdHistory.addCommand (new GroupCmd (document), true);
    break;
  case ID_ARRANGE_UNGROUP:
    cmdHistory.addCommand (new UngroupCmd (document), true);
    break;
  case ID_TRANSFORM_POSITION:
  case ID_TRANSFORM_DIMENSION:
  case ID_TRANSFORM_ROTATION:
  case ID_TRANSFORM_MIRROR:
    showTransformationDialog (item - ID_TRANSFORM_POSITION);
    break;
  case ID_EXTRAS_OPTIONS:
    OptionDialog::setup ();
    break;
  case ID_EXTRAS_CLIPART:
    {
      QString fname = KFilePreviewDialog::getOpenFileName (0, "*.wmf *.WMF | Windows Metafiles", this);
      if (! fname.isEmpty ()) {
	InsertClipartCmd *cmd = new InsertClipartCmd (document, 
						      (const char *) fname);
	cmdHistory.addCommand (cmd, true);
      }
      break;
    }
  case ID_EXTRAS_SCRIPTS:
    if (!scriptDialog) 
      scriptDialog = new ScriptDialog ();
    scriptDialog->setActiveDocument (document);
    scriptDialog->show ();
    scriptDialog->loadScripts ();
    break;
  case ID_HELP_ABOUT_APP:
  case ID_HELP_ABOUT_KDE:
      about (item);
      break;
  default:
    if (item > ID_IMPORT && item < ID_IMPORT + 100) {
      tcontroller->reset ();
      importFromFile (item - ID_IMPORT);
    }
    if (item > ID_EXPORT && item < ID_EXPORT + 100) {
      tcontroller->reset ();
      exportToFile (item - ID_EXPORT);
    }
    else if (item > ID_FILE_OPEN_RECENT && item < ID_FILE_OPEN_RECENT + 10) {
      QStrList recentFiles = PStateManager::instance ()->getRecentFiles ();
      const char* fname = recentFiles.at (item - ID_FILE_OPEN_RECENT - 1);
      if (fname) {
	askForSave ();
	document->initialize ();
	openURL (fname);
	cmdHistory.reset ();
      }
    }
    break;
  }
}

void KIllustrator::openFile (const char* fname) {
  localFile = fname;
  ifstream is (fname);
  if (is.fail ())
    return;
  document->readFromXml (is);
  document->setFileName (fname);
  document->setModified (false);
  canvas->calculateSize ();
  setFileCaption (fname);
}

void KIllustrator::setFileCaption (const char* fname) {
  QString caption = "KIllustrator: ";
  caption += fname;
  setCaption (caption.data ());
}

void KIllustrator::openURL (const char* surl) {
  KURL url (surl);
  if (url.isLocalFile ()) {
    // local file
    openFile (url.path ());
  }
  else {
    // network file
    kfmConn = new KFM;
    if (kfmConn->isOK ()) {
      QString tmpFile;

      connect (kfmConn, SIGNAL(finished ()), this, SLOT (slotKFMJobDone ()));
      tmpFile.sprintf ("file:/tmp/killu%i", time (0L));
      kfmConn->copy (surl, tmpFile);
      KURL tmpURL (tmpFile);
      localFile = tmpURL.path ();
    }
  }
}

bool KIllustrator::closeWindow (KIllustrator* win) {
  if (win->askForSave ()) {
//    win->close ();
      delete win;
    
    if (windows.count () == 0) {
      PStateManager::instance ()->saveDefaultSettings ();

      kapp->quit ();
    }

    return true;
  }
  return false;
}

void KIllustrator::setUndoStatus(bool undoPossible, bool redoPossible)
{
  // we do this " " trick to avoid double translation of "Undo" and "Undo "
  edit->setItemEnabled(ID_EDIT_UNDO, undoPossible);

  QString label = i18n("Undo");
  if (undoPossible) 
      label += " " + cmdHistory.getUndoName();
  edit->changeItem(label, ID_EDIT_UNDO);
  
  edit->setItemEnabled(ID_EDIT_REDO, redoPossible);
  
  label = i18n("Redo");
  if (redoPossible)
      label += " " + cmdHistory.getRedoName();
  edit->changeItem(label, ID_EDIT_REDO);

}

void KIllustrator::quit () {
  QListIterator<KIllustrator> it (windows);
  while (it.current ()) {
    if (! closeWindow (it.current ()))
      return;
  }
}

bool KIllustrator::askForSave () {
  if (document->isModified ()) {
    int result = 
      KMsgBox::yesNoCancel (this, "Message", 
			    i18n ("This Document has been modified.\nWould you like to save it ?"),
			    KMsgBox::QUESTION, i18n ("Yes"), 
			    i18n ("No"), 
			    i18n ("Cancel"));
    if (result == 1)
      saveFile ();

    return (result == 3 ? false : true);
  }
  return true;
}

void KIllustrator::saveFile () {
  if (strcmp (document->fileName (), UNNAMED_FILE) == 0) {
    saveAsFile ();
  }
  else {
    ofstream os ((const char *) document->fileName ());
    if (os.fail ())
      // write out an error message !!
      return;

    document->saveToXml (os);
    document->setFileName ((const char *) document->fileName ());
    PStateManager::instance ()->addRecentFile ((const char *) 
					       document->fileName ());
  }
}

void KIllustrator::saveAsFile () {
  QString fname = KFileDialog::getSaveFileName (0, "*.kil", this);
  if (! fname.isEmpty ()) {
    if (access ((const char *) fname, W_OK) == 0) {
      // there is already a file with the same name
      int result = 
	KMsgBox::yesNoCancel (this, "Message", 
			      i18n ("This Document already exists.\nWould you like to override it ?"),
			      KMsgBox::QUESTION, i18n ("Yes"), 
			      i18n ("No"), 
			      i18n ("Cancel"));
      if (result != 1)
	return;
    }
    ofstream os (fname);
    if (os.fail ())
      // write out an error message !!
      return;
    document->saveToXml (os);
    document->setFileName (fname);
    PStateManager::instance ()->addRecentFile ((const char *) fname);
    setFileCaption (fname);
  }
}

void KIllustrator::selectColor (int flag, const QBrush& b) {
  if (flag == 0)
    setPenColor (b);
  else if (flag == 1)
    emit setFillColor (b);
}

void KIllustrator::setPenColor (const QBrush& b) {
  GObject::OutlineInfo oInfo;
  oInfo.mask = GObject::OutlineInfo::Color | GObject::OutlineInfo::Style;;
  oInfo.color = b.color ();
  oInfo.style = (b.style () == NoBrush ? NoPen : SolidLine);
  
  GObject::FillInfo fInfo;
  fInfo.mask = 0;
    
  if (! document->selectionIsEmpty ()) {
    SetPropertyCmd *cmd = new SetPropertyCmd (document, oInfo, fInfo);
    cmdHistory.addCommand (cmd, true);
  }
  else {
    int result = 
      KMsgBox::yesNo (this, "Warning", 
		      i18n ("This action will set the default\nproperties for new objects !\nWould you like to do it ?"),
		      KMsgBox::QUESTION, i18n ("Yes"), i18n ("No"));
    if (result == 1)
      GObject::setDefaultOutlineInfo (oInfo);
  }
}

void KIllustrator::setFillColor (const QBrush& b) {
  GObject::OutlineInfo oInfo;
  oInfo.mask = 0;
  
  GObject::FillInfo fInfo;
  fInfo.mask = GObject::FillInfo::Color | GObject::FillInfo::FillStyle;
  fInfo.color = b.color ();
  if (b.style () == NoBrush)
    fInfo.fstyle = GObject::FillInfo::NoFill;
  else
    fInfo.fstyle = GObject::FillInfo::SolidFill;

  if (! document->selectionIsEmpty ()) {
    SetPropertyCmd *cmd = new SetPropertyCmd (document, oInfo, fInfo);
    cmdHistory.addCommand (cmd, true);
  }
  else {
    int result = 
      KMsgBox::yesNo (this, "Warning", 
		      i18n ("This action will set the default\nproperties for new objects !\nWould you like to do it ?"),
		      KMsgBox::QUESTION, i18n ("Yes"), i18n ("No"));
    if (result == 1)
      GObject::setDefaultFillInfo (fInfo);
  }
}

void KIllustrator::dropActionSlot (KDNDDropZone* dzone) {
  bool firstFile = true;
  QStrList urls = dzone->getURLList ();
  char *url = urls.first ();
  while (url) {
    if (firstFile && ! document->isModified ())
      openURL (url);
    else {
      KIllustrator* toplevel = new KIllustrator ();
      toplevel->openURL (url);
    }
    url = urls.next ();
    firstFile = false;
  }
}

void KIllustrator::slotKFMJobDone () {
  delete kfmConn;
  kfmConn = 0L;
  openFile ((const char *)localFile);
}

void KIllustrator::about (int id) {
  if (id == ID_HELP_ABOUT_APP) {
    AboutDialog dialog;
    dialog.exec ();
  }
}

void KIllustrator::zoomFactorSlot (int idx) {
  if (canvas)
    canvas->setZoomFactor (zFactors[idx]);
}

void KIllustrator::updateZoomFactor (float zFactor) {
  KCombo* combo = toolbar->getCombo (10);
  for (int i = 0; i < (int) zFactors.size (); i++) {
    if (zFactors[i] == zFactor) {
      combo->setCurrentItem (i);
      break;
    }
  }
}

void KIllustrator::exportToFile (int id) {
  FilterManager* filterMgr = FilterManager::instance ();
  QStrList ftypes = filterMgr->getInstalledFilters ();
  const char* format = ftypes.at (id - 1);
  FilterInfo* filterInfo = filterMgr->getFilterForType (format);
  if (filterInfo == 0L)
    return;

  QString mask;
  mask.sprintf ("*.%s", filterInfo->extension ());

  QString fname = KFileDialog::getSaveFileName (0, (const char *) mask, this);
  if (! fname.isEmpty ()) {
    ExportFilter* filter = filterInfo->exportFilter ();
    
    if (filter && filter->setup (document, format)) {
      filter->setOutputFileName (fname);
      filter->exportToFile (document);
    }
  }
}

void KIllustrator::importFromFile (int id) {
  FilterManager* filterMgr = FilterManager::instance ();
  QStrList ftypes = filterMgr->getInstalledFilters ();
  const char* format = ftypes.at (id - 1);
  FilterInfo* filterInfo = filterMgr->getFilterForType (format);
  if (filterInfo == 0L)
    return;

  QString mask;
  mask.sprintf ("*.%s", filterInfo->extension ());

  QString fname = KFilePreviewDialog::getOpenFileName (0, (const char *) mask,
						       this);
  if (! fname.isEmpty ()) {
    QFileInfo finfo ((const char *) fname);
    if (!finfo.isFile () || !finfo.isReadable ())
      return;

    ImportFilter* filter = filterInfo->importFilter ();
    
    if (filter && filter->setup (document, format)) {
      filter->setInputFileName (fname);
      filter->importFromFile (document);
    }
  }
}

void KIllustrator::showTransformationDialog (int id) {
  TransformationDialog *transformationDialog = 
    new TransformationDialog (&cmdHistory);
  connect (document, SIGNAL (selectionChanged ()), transformationDialog,
	   SLOT (update ()));
  transformationDialog->setDocument (document);
  transformationDialog->showTab (id);
}

void KIllustrator::updateRecentFiles () {
  //  KIllustratorApp* myapp = (KIllustratorApp *) kapp;
  QStrList files = PStateManager::instance ()->getRecentFiles ();
  const char* fname = files.first ();
  unsigned int id = 1;
  openRecent->clear ();
  while (fname) {
    openRecent->insertItem (fname, ID_FILE_OPEN_RECENT + id);
    fname = files.next ();
    id++;
  }
}
