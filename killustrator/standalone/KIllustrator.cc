/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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
#include <qframe.h>
#include <qclipboard.h>
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
#include "PathTextTool.h"
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
#include "DuplicateCmd.h"
#include "ReorderCmd.h"
#include "SetPropertyCmd.h"
#include "InsertPixmapCmd.h"
#include "filter/FilterManager.h"
#include "Preview.h"
#include "units.h"
#include "ScriptDialog.h"
#include "HelplineDialog.h"
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
#include <kimgio.h>
#include <ktoolbar.h>
#include "version.h"

#ifdef NEWKDE
#include <kio_job.h>
#include <kglobal.h>
#endif

#include "koPageLayoutDia.h"

#ifdef HAVE_CONFIG_H
#include "config.h"

#ifdef HAVE_TIME_H
#include <time.h>
#endif
#endif

#define DEFAULT_PALETTE "default.pal"

QList<KIllustrator> KIllustrator::windows;
bool KIllustrator::previewHandlerRegistered = false;

KIllustrator::KIllustrator (const char* url) : KTMainWindow () {
  windows.setAutoDelete (false);
  windows.append (this);

#ifndef NEWKDE
  // if kfmConn is null, there is no
  // current IO job.
  kfmConn = 0;
#endif

  psm = PStateManager::instance ();

  zFactors.resize (8);
  zFactors[0] = 0.5;
  zFactors[1] = 1.0;
  zFactors[2] = 1.5;
  zFactors[3] = 2.0;
  zFactors[4] = 4.0;
  zFactors[5] = 6.0;
  zFactors[6] = 8.0;
  zFactors[7] = 10.0;

  canvas = 0L;
  scriptDialog = 0L;
  layerDialog = 0L;
  initMenu ();
  initStatusBar ();

  document = new GDocument ();
  connect (document, SIGNAL (wasModified (bool)),
	   this, SLOT (documentIsModifiedSlot (bool)));

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
  connect (canvas, SIGNAL(gridStatusChanged ()), this,
	   SLOT(updateGridSettings ()));

  view->setItemChecked (ID_VIEW_GRID, canvas->showGrid ());
  layout->setItemChecked (ID_LAYOUT_ALIGN_GRID, canvas->snapToGrid ());
  view->setItemChecked (ID_VIEW_HELPLINES, canvas->showHelplines ());
  layout->setItemChecked (ID_LAYOUT_ALIGN_HLINES, canvas->alignToHelplines ());

  if (! previewHandlerRegistered) {
    kimgioRegister ();

    KFilePreviewDialog::registerPreviewModule ("kil", kilPreviewHandler,
					       PreviewPixmap);
    KFilePreviewDialog::registerPreviewModule ("wmf", wmfPreviewHandler,
					       PreviewPixmap);
    KFilePreviewDialog::registerPreviewModule ("gif", pixmapPreviewHandler,
					       PreviewPixmap);
    KFilePreviewDialog::registerPreviewModule ("jpg", pixmapPreviewHandler,
					       PreviewPixmap);
    KFilePreviewDialog::registerPreviewModule ("xpm", pixmapPreviewHandler,
					       PreviewPixmap);
    KFilePreviewDialog::registerPreviewModule ("xbm", pixmapPreviewHandler,
					       PreviewPixmap);
    KFilePreviewDialog::registerPreviewModule ("png", pixmapPreviewHandler,
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

  connect (canvas, SIGNAL(rightButtonAtObjectClicked (int, int, GObject*)),
	   this, SLOT(popupForObject (int, int, GObject *)));
  connect (canvas, SIGNAL(rightButtonAtSelectionClicked (int, int)),
	   this, SLOT(popupForSelection (int, int)));

  gridLayout->addWidget (viewport, 1, 1);
  gridLayout->setRowStretch (1, 20);
  gridLayout->setColStretch (1, 20);

  setView (w);

  restoreRulerStatus ();

  tcontroller = new ToolController (this);
  Tool* tool;
  tcontroller->registerTool (ID_TOOL_SELECT,
			     tool = new SelectionTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));

  tcontroller->registerTool (ID_TOOL_EDITPOINT,
			     editPointTool = new EditPointTool (&cmdHistory));
  connect (editPointTool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  connect (editPointTool, SIGNAL(operationDone ()),
  	   this, SLOT (resetTools ()));

  tcontroller->registerTool (ID_TOOL_FREEHAND,
			     tool = new FreeHandTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  connect (tool, SIGNAL(operationDone ()),
  	   this, SLOT (resetTools ()));

  tcontroller->registerTool (ID_TOOL_LINE,
			     tool = new PolylineTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  connect (tool, SIGNAL(operationDone ()),
  	   this, SLOT (resetTools ()));

  tcontroller->registerTool (ID_TOOL_BEZIER,
			     tool = new BezierTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  connect (tool, SIGNAL(operationDone ()),
  	   this, SLOT (resetTools ()));

  tcontroller->registerTool (ID_TOOL_RECTANGLE,
			     tool = new RectangleTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  connect (tool, SIGNAL(operationDone ()),
  	   this, SLOT (resetTools ()));

  tcontroller->registerTool (ID_TOOL_POLYGON,
			     tool = new PolygonTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  connect (tool, SIGNAL(operationDone ()),
  	   this, SLOT (resetTools ()));

  tcontroller->registerTool (ID_TOOL_ELLIPSE,
			     tool = new OvalTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  connect (tool, SIGNAL(operationDone ()),
  	   this, SLOT (resetTools ()));

  tcontroller->registerTool (ID_TOOL_TEXT,
			     tool = new TextTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  connect (tool, SIGNAL(operationDone ()),
  	   this, SLOT (resetTools ()));

  tcontroller->registerTool (ID_TOOL_ZOOM,
			     tool = new ZoomTool (&cmdHistory));
  connect (tool, SIGNAL(modeSelected(const char*)),
	   this, SLOT(showCurrentMode(const char*)));
  connect (tool, SIGNAL(operationDone ()),
  	   this, SLOT (resetTools ()));

  tcontroller->registerTool (ID_TOOL_PATHTEXT,
			     tool = new PathTextTool (&cmdHistory));
  connect (tool, SIGNAL(operationDone ()),
  	   this, SLOT (resetTools ()));

  canvas->setToolController (tcontroller);

  setUndoStatus(false, false);
  connect(&cmdHistory, SIGNAL(changed(bool, bool)),
	  SLOT(setUndoStatus(bool, bool)));

  setFocusPolicy (QWidget::StrongFocus);
  setFocusProxy (canvas);
}

void KIllustrator::initToolBars () {
  QPixmap pixmap;
  KIconLoader* loader = KGlobal::iconLoader ();

  /* main toolbar */
  toolbar = new KToolBar (this);

  toolbar->insertButton (loader->loadIcon ("filenew2.xpm"), ID_FILE_NEW, true,
			 i18n ("New Document"));
  toolbar->insertButton (loader->loadIcon ("fileopen.xpm"), ID_FILE_OPEN, true,
			 i18n ("Open Document"));
  toolbar->insertButton (loader->loadIcon ("filefloppy.xpm"), ID_FILE_SAVE,
			 true, i18n ("Save Document"));
  toolbar->insertSeparator ();
  toolbar->insertButton (loader->loadIcon ("fileprint.xpm"), ID_FILE_PRINT,
			 true, i18n ("Print Document"));
  toolbar->insertSeparator ();
  toolbar->insertButton (loader->loadIcon ("editcopy.xpm"), ID_EDIT_COPY, true,
			 i18n ("Copy"));
  toolbar->insertButton (loader->loadIcon ("editpaste.xpm"), ID_EDIT_PASTE,
			 true, i18n ("Paste"));
  toolbar->insertButton (loader->loadIcon ("editcut.xpm"), ID_EDIT_CUT, true,
			 i18n ("Cut"));
  toolbar->insertSeparator ();

  QStrList zoomStrList;
  for (int i = 0; i < (int) zFactors.size (); i++) {
    char buf[8];
    sprintf (buf, "%3.0f%%", zFactors[i] * 100);
    zoomStrList.append (buf);
  }

  toolbar->insertCombo (&zoomStrList, 10, false, SIGNAL(activated(int)),
			this, SLOT(zoomFactorSlot (int)),
			true, QSTR_NULL,
			85);
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
  KRadioGroup* toolGroup = new KRadioGroup (toolPalette);

  toolPalette->insertButton (loader->loadIcon ("selecttool.xpm"),
			     ID_TOOL_SELECT, true, i18n ("Selection Mode"));
  toolPalette->setToggle (ID_TOOL_SELECT);
  toolGroup->addButton (ID_TOOL_SELECT);

  toolPalette->insertButton (loader->loadIcon ("pointtool.xpm"),
			     ID_TOOL_EDITPOINT, true, i18n ("Edit Point"));
  toolPalette->setToggle (ID_TOOL_EDITPOINT);
  toolGroup->addButton (ID_TOOL_EDITPOINT);

  toolPalette->insertButton (loader->loadIcon ("freehandtool.xpm"),
			     ID_TOOL_FREEHAND, true,
			     i18n ("Create Freehand Line"));
  toolPalette->setToggle (ID_TOOL_FREEHAND);
  toolGroup->addButton (ID_TOOL_FREEHAND);

  toolPalette->insertButton (loader->loadIcon ("linetool.xpm"),
			     ID_TOOL_LINE, true, i18n ("Create Polyline"));
  toolPalette->setToggle (ID_TOOL_LINE);
  toolGroup->addButton (ID_TOOL_LINE);

  toolPalette->insertButton (loader->loadIcon ("beziertool.xpm"),
			     ID_TOOL_BEZIER, true,
			     i18n ("Create Bezier Curve"));
  toolPalette->setToggle (ID_TOOL_BEZIER);
  toolGroup->addButton (ID_TOOL_BEZIER);

  toolPalette->insertButton (loader->loadIcon ("recttool.xpm"),
			     ID_TOOL_RECTANGLE, true,
			     i18n ("Create Rectangle"));
  toolPalette->setToggle (ID_TOOL_RECTANGLE);
  toolGroup->addButton (ID_TOOL_RECTANGLE);

  toolPalette->insertButton (loader->loadIcon ("polygontool.xpm"),
			     ID_TOOL_POLYGON, true, i18n ("Create Polygon"));
  toolPalette->setToggle (ID_TOOL_POLYGON);
  toolGroup->addButton (ID_TOOL_POLYGON);

  toolPalette->insertButton (loader->loadIcon ("ellipsetool.xpm"),
			     ID_TOOL_ELLIPSE, true, i18n ("Create Ellipse"));
  toolPalette->setToggle (ID_TOOL_ELLIPSE);
  toolGroup->addButton (ID_TOOL_ELLIPSE);

  toolPalette->insertButton (loader->loadIcon ("texttool.xpm"),
			     ID_TOOL_TEXT, true, i18n ("Create Text"));
  toolPalette->setToggle (ID_TOOL_TEXT);
  toolGroup->addButton (ID_TOOL_TEXT);

  toolPalette->insertButton (loader->loadIcon ("zoomtool.xpm"),
			     ID_TOOL_ZOOM, true, i18n ("Zoom In"));
  toolPalette->setToggle (ID_TOOL_ZOOM);
  toolGroup->addButton (ID_TOOL_ZOOM);

  connect (toolPalette, SIGNAL (clicked (int)), tcontroller,
  	   SLOT(toolSelected (int)));
  connect (toolPalette, SIGNAL (clicked (int)), this,
  	   SLOT(toolSelected (int)));
  connect (toolPalette, SIGNAL (doubleClicked (int)), tcontroller,
	   SLOT(configureTool (int)));
  resetTools ();
  toolPalette->setBarPos (KToolBar::Left);
  addToolBar (toolPalette);

  /* the color toolbar */
  colorPalette = new KToolBar (this);
  QString palFile = kapp->kde_datadir ().copy ();
  palFile += "/killustrator/";
  palFile += DEFAULT_PALETTE;

  parseColorPalette ((const char *) palFile, palette);

  selectedColorIdx = -1;
  GObject::OutlineInfo oinfo = GObject::getDefaultOutlineInfo ();
  for (int i = 0; i < (int) palette.size (); i++) {
    QBrush brush (palette[i], i == 0 ? NoBrush : SolidPattern);
    ColorField* cfield = new ColorField (i, brush, colorPalette);
    connect (cfield, SIGNAL(colorSelected (int, int, const QBrush&)),
	     this, SLOT(selectColor (int, int, const QBrush&)));
    if (oinfo.color == palette[i] && selectedColorIdx == -1) {
      cfield->highlight (true);
      selectedColorIdx = i;
    }
    colorPalette->insertWidget (i, cfield->width (), cfield);
  }

  colorPalette->setBarPos (KToolBar::Right);
  addToolBar (colorPalette);

  /* the "edit point" toolbar */
  editPointToolbar = new KToolBar (this);
  KRadioGroup* toolGroup2 = new KRadioGroup (editPointToolbar);
  editPointToolbar->insertButton (loader->loadIcon ("moveNode.xpm"),
			     ID_TOOL_EP_MOVE, true, i18n ("Move Point"));
  editPointToolbar->setToggle (ID_TOOL_EP_MOVE);
  toolGroup2->addButton (ID_TOOL_EP_MOVE);
  editPointToolbar->insertButton (loader->loadIcon ("newNode.xpm"),
			     ID_TOOL_EP_ADD, true, i18n ("Insert Point"));
  editPointToolbar->setToggle (ID_TOOL_EP_ADD);
  toolGroup2->addButton (ID_TOOL_EP_ADD);
  editPointToolbar->insertButton (loader->loadIcon ("deleteNode.xpm"),
			     ID_TOOL_EP_DEL, true, i18n ("Remove Point"));
  editPointToolbar->setToggle (ID_TOOL_EP_DEL);
  toolGroup2->addButton (ID_TOOL_EP_DEL);
  editPointToolbar->insertButton (loader->loadIcon ("split.xpm"),
			     ID_TOOL_EP_SPLIT, true, i18n ("Split Line"));
  editPointToolbar->setToggle (ID_TOOL_EP_SPLIT);
  toolGroup2->addButton (ID_TOOL_EP_SPLIT);
  /*
  editPointToolbar->insertButton (loader->loadIcon ("joinNodes.xpm"),
			     ID_TOOL_EP_JOIN, true, i18n ("Join Lines"));
  editPointToolbar->setToggle (ID_TOOL_EP_JOIN);
  toolGroup2->addButton (ID_TOOL_EP_JOIN);
  */
  connect (editPointToolbar, SIGNAL (clicked(int)),
	   this, SLOT (menuCallback (int)));
  editPointToolbar->enable (KToolBar::Hide);
  editPointToolbar->move (10000, 10000);
  editPointToolbar->setBarPos (KToolBar::Floating);
  editPointToolbar->setFullWidth ();
  addToolBar (editPointToolbar);

  // Saving of toolbar positions
  restoreToolbarStatus ();

  connect (toolbar, SIGNAL (moved(BarPosition)),
	   this, SLOT (saveToolbarStatus ()));
  connect (toolPalette, SIGNAL (moved(BarPosition)),
	   this, SLOT (saveToolbarStatus ()));
  connect (colorPalette, SIGNAL (moved(BarPosition)),
	   this, SLOT (saveToolbarStatus ()));
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

  file->insertItem (i18n ("Import..."), ID_IMPORT);
  file->insertItem (i18n ("Export..."), ID_EXPORT);

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

  QPopupMenu* insert = new QPopupMenu ();
  insert->insertItem (i18n ("&Bitmap..."), ID_INSERT_BITMAP);
  insert->insertItem (i18n ("&Clipart..."), ID_INSERT_CLIPART);
  connect (insert, SIGNAL (activated (int)), SLOT (menuCallback (int)));

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
  edit->insertItem (i18n ("D&uplicate"), ID_EDIT_DUPLICATE);
  edit->setAccel (CTRL + Key_D, ID_EDIT_DUPLICATE);
  edit->insertItem (i18n ("&Delete"), ID_EDIT_DELETE);
  edit->setAccel (Key_Delete, ID_EDIT_DELETE);
  edit->insertSeparator ();
  edit->insertItem (i18n ("&Select All"), ID_EDIT_SELECT_ALL);
  edit->insertSeparator ();
  edit->insertItem (i18n ("&Insert"), insert);
  edit->insertSeparator ();
  edit->insertItem (i18n ("Pr&operties"), ID_EDIT_PROPERTIES);
  connect (edit, SIGNAL (activated (int)), SLOT (menuCallback (int)));

  view->insertItem (i18n ("Outline"), ID_VIEW_OUTLINE);
  view->setItemChecked (ID_VIEW_OUTLINE, false);
  view->insertItem (i18n ("Normal"), ID_VIEW_NORMAL);
  view->setItemChecked (ID_VIEW_NORMAL, true);
  view->insertSeparator ();
  view->insertItem (i18n ("Layers..."), ID_VIEW_LAYERS);
  view->insertSeparator ();
  view->insertItem (i18n ("Show Ruler"), ID_VIEW_RULER);
  view->setItemChecked (ID_VIEW_RULER, true);
  view->insertItem (i18n ("Show Grid"), ID_VIEW_GRID);
  view->setItemChecked (ID_VIEW_GRID, false);
  view->insertItem (i18n ("Show Helplines"), ID_VIEW_HELPLINES);
  view->setItemChecked (ID_VIEW_HELPLINES, false);
  connect (view, SIGNAL (activated (int)), SLOT (menuCallback (int)));

  layout->insertItem (i18n ("&Page"), ID_LAYOUT_PAGE);
  layout->insertItem (i18n ("&Grid"), ID_LAYOUT_GRID);
  layout->insertItem (i18n ("&Helplines"), ID_LAYOUT_HELPLINES);
  layout->insertSeparator ();
  layout->insertItem (i18n ("Align to Grid"), ID_LAYOUT_ALIGN_GRID);
  layout->setItemChecked (ID_LAYOUT_ALIGN_GRID, false);
  layout->setAccel (CTRL + Key_Y, ID_LAYOUT_ALIGN_GRID);
  layout->insertItem (i18n ("Align to Helplines"), ID_LAYOUT_ALIGN_HLINES);
  layout->setItemChecked (ID_LAYOUT_ALIGN_HLINES, false);
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
  arrangement->insertSeparator ();
  arrangement->insertItem (i18n ("Text along Path"), ID_ARRANGE_PATHTEXT);
  connect (arrangement, SIGNAL (activated (int)), SLOT (menuCallback (int)));

  //  extras->insertItem (i18n ("Load &Palette..."), ID_EXTRAS_LOAD_PALETTE);
  //  extras->insertSeparator ();
  extras->insertItem (i18n ("&Options..."), ID_EXTRAS_OPTIONS);
  //  extras->insertItem (i18n ("&Scripts..."), ID_EXTRAS_SCRIPTS);
  //  extras->insertItem (i18n ("Dump History"), ID_HISTORY_DUMP);
  connect (extras, SIGNAL (activated (int)), SLOT (menuCallback (int)));

  help->insertItem (i18n ("&Contents"), ID_HELP_HELP);
  help->setAccel (Key_F1, ID_HELP_HELP);
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

  popupMenu = new QPopupMenu ();
  popupMenu->insertItem (i18n ("Copy"), ID_EDIT_COPY);
  popupMenu->insertItem (i18n ("Cut"), ID_EDIT_CUT);
  popupMenu->insertSeparator ();
  popupMenu->insertItem (i18n ("Properties"), ID_EDIT_PROPERTIES);
  popupMenu->insertSeparator ();
  popupMenu->insertItem (i18n ("Align"), ID_ARRANGE_ALIGN);
  popupMenu->insertSeparator ();
  popupMenu->insertItem (i18n ("To Front"), ID_ARRANGE_FRONT);
  popupMenu->insertItem (i18n ("To Back"), ID_ARRANGE_BACK);
  popupMenu->insertItem (i18n ("Forward One"), ID_ARRANGE_1_FORWARD);
  popupMenu->insertItem (i18n ("Back One"), ID_ARRANGE_1_BACK);

  // Save position of menubar
  restoreMenubarStatus ();
  connect (menubar, SIGNAL (moved (menuPosition)), SLOT (saveMenubarStatus ()));
  connect (popupMenu, SIGNAL (activated (int)), SLOT (menuCallback (int)));
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
  case ID_HISTORY_DUMP:
    cmdHistory.dump ();
    break;
  case ID_FILE_NEW:
    if (askForSave ()) {
      document->initialize ();
      lastExport = "";
      cmdHistory.reset ();
      setFileCaption (UNNAMED_FILE);
      resetTools ();
    }
    break;
  case ID_FILE_OPEN:
    {
      if (askForSave ()) {
	QString fname =
	  KFilePreviewDialog::getOpenFileURL((const char *) lastOpenDir,
					     "*.kil | KIllustrator File",
					     this);
	if (! fname.isEmpty ()) {
          KURL u (fname);
#ifdef NEWKDE
	  lastOpenDir = u.directory( false /* keep trailing slash */ );
#else
          lastOpenDir = u.directoryURL();
#endif
	  document->initialize ();
	  openURL ((const char *)fname);
	  cmdHistory.reset ();
	  resetTools ();
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
    canvas->printDocument ();
    break;
  case ID_FILE_INFO:
    DocumentInfo::showInfo (document);
    break;
  case ID_FILE_NEW_WINDOW:
    {
      KTMainWindow* w = new KIllustrator ();
      w->show ();
      break;
    }
  case ID_IMPORT:
    importFromFile ();
    break;
  case ID_EXPORT:
    exportToFile ();
    break;
  case ID_FILE_EXIT:
    quit ();
    break;
  case ID_EDIT_UNDO:
    cmdHistory.undo ();
    resetTools ();
    break;
  case ID_EDIT_REDO:
    cmdHistory.redo ();
    resetTools ();
    break;
  case ID_EDIT_CUT:
    if (!document->selectionIsEmpty ())
      cmdHistory.addCommand (new CutCmd (document), true);
    break;
  case ID_EDIT_COPY:
    if (!document->selectionIsEmpty ())
      cmdHistory.addCommand (new CopyCmd (document), true);
    break;
  case ID_EDIT_PASTE:
    if (::strlen (QApplication::clipboard ()->text ()))
      cmdHistory.addCommand (new PasteCmd (document), true);
    break;
  case ID_EDIT_DUPLICATE:
    if (!document->selectionIsEmpty ())
      cmdHistory.addCommand (new DuplicateCmd (document), true);
    break;
  case ID_EDIT_DELETE:
    if (!document->selectionIsEmpty ())
      cmdHistory.addCommand (new DeleteCmd (document), true);
    break;
  case ID_EDIT_SELECT_ALL:
    document->selectAllObjects ();
    break;
  case ID_INSERT_BITMAP:
    {
      QString fname = KFilePreviewDialog::getOpenFileName
	     ((const char *) lastBitmapDir, "*.gif *.GIF | GIF Images\n"
	         "*.jpg *.jpeg *.JPG *.JPEG | JPEG Images\n"
	         "*.png | PNG Images\n"
	         "*.xbm | X11 Bitmaps\n"
	         "*.xpm | X11 Pixmaps",
	     this);
       if (! fname.isEmpty ()) {
         QFileInfo finfo (fname);
         lastBitmapDir = finfo.dirPath ();
	 InsertPixmapCmd *cmd = new InsertPixmapCmd (document,
						     (const char *) fname);
	 cmdHistory.addCommand (cmd, true);
       }
       break;
    }
  case ID_INSERT_CLIPART:
    {
      QString fname = KFilePreviewDialog::getOpenFileName
	      ((const char *) lastClipartDir,
	       "*.wmf *.WMF | Windows Metafiles", this);
      if (! fname.isEmpty ()) {
        QFileInfo finfo (fname);
        lastClipartDir = finfo.dirPath ();
	InsertClipartCmd *cmd = new InsertClipartCmd (document,
						      (const char *) fname);
	cmdHistory.addCommand (cmd, true);
      }
      break;
    }
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
  case ID_VIEW_OUTLINE:
    view->setItemChecked (ID_VIEW_OUTLINE, true);
    view->setItemChecked (ID_VIEW_NORMAL, false);
    canvas->setOutlineMode (true);
    break;
  case ID_VIEW_NORMAL:
    view->setItemChecked (ID_VIEW_OUTLINE, false);
    view->setItemChecked (ID_VIEW_NORMAL, true);
    canvas->setOutlineMode (false);
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
      saveRulerStatus (show_it);
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
  case ID_VIEW_HELPLINES:
    {
      bool show_it = !view->isItemChecked (ID_VIEW_HELPLINES);
      canvas->showHelplines (show_it);
      view->setItemChecked (ID_VIEW_HELPLINES, show_it);
    }
    break;
  case ID_LAYOUT_PAGE:
    {
      KoPageLayout pLayout = document->pageLayout ();
      KoHeadFoot header;
	
      if (KoPageLayoutDia::pageLayout (pLayout, header,
				       FORMAT_AND_BORDERS |
				       DISABLE_BORDERS | USE_NEW_STUFF)) {
	document->setPageLayout (pLayout);
      }
      break;
    }
  case ID_LAYOUT_GRID:
    GridDialog::setupGrid (canvas);
    break;
  case ID_LAYOUT_HELPLINES:
    HelplineDialog::setup (canvas);
    break;
  case ID_LAYOUT_ALIGN_GRID:
    {
      bool align_it = !layout->isItemChecked (ID_LAYOUT_ALIGN_GRID);
      canvas->snapToGrid (align_it);
      layout->setItemChecked (ID_LAYOUT_ALIGN_GRID, align_it);
    }
    break;
  case ID_LAYOUT_ALIGN_HLINES:
    {
      bool align_it = !layout->isItemChecked (ID_LAYOUT_ALIGN_HLINES);
      canvas->alignToHelplines (align_it);
      layout->setItemChecked (ID_LAYOUT_ALIGN_HLINES, align_it);
    }
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
    if (document->selectionCount () > 1)
      cmdHistory.addCommand (new GroupCmd (document), true);
    break;
  case ID_ARRANGE_UNGROUP:
    if (! document->selectionIsEmpty ()) {
      list<GObject*> sel = document->getSelection ();
      list<GObject*>::iterator iter = sel.begin ();
      bool groupExists = false;
      while (iter != sel.end ()) {
	if ((*iter)->isA ("GGroup")) {
	  groupExists = true;
	  break;
	}
	iter++;
      }
      if (groupExists)
	cmdHistory.addCommand (new UngroupCmd (document), true);
    }
    break;
  case ID_TRANSFORM_POSITION:
  case ID_TRANSFORM_DIMENSION:
  case ID_TRANSFORM_ROTATION:
  case ID_TRANSFORM_MIRROR:
    showTransformationDialog (item - ID_TRANSFORM_POSITION);
    break;
  case ID_ARRANGE_PATHTEXT:
    tcontroller->toolSelected (ID_TOOL_PATHTEXT);
    break;
  case ID_EXTRAS_OPTIONS:
    OptionDialog::setup ();
    break;
  case ID_EXTRAS_LOAD_PALETTE:
    loadPalette ();
    break;
  case ID_EXTRAS_SCRIPTS:
    if (!scriptDialog)
      scriptDialog = new ScriptDialog ();
    scriptDialog->setActiveDocument (document);
    scriptDialog->show ();
    scriptDialog->loadScripts ();
    break;
  case ID_HELP_HELP:
    kapp->invokeHTMLHelp ("", "");
    break;
  case ID_HELP_ABOUT_APP:
  case ID_HELP_ABOUT_KDE:
    about (item);
    break;
  case ID_TOOL_EP_MOVE:
    editPointTool->setMode (EditPointTool::MovePoint);
    break;
  case ID_TOOL_EP_ADD:
    editPointTool->setMode (EditPointTool::InsertPoint);
    break;
  case ID_TOOL_EP_DEL:
    editPointTool->setMode (EditPointTool::RemovePoint);
    break;
  case ID_TOOL_EP_SPLIT:
    editPointTool->setMode (EditPointTool::Split);
    break;
  case ID_TOOL_EP_JOIN:
    editPointTool->setMode (EditPointTool::Join);
    break;
  default:
    if (item > ID_FILE_OPEN_RECENT && item < ID_FILE_OPEN_RECENT + 10) {
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
#ifdef NEWKDE
    KIOJob *kiojob = new KIOJob;
    QString tmpFile;
    connect (kiojob, SIGNAL(sigFinished ( int )), this,
	     SLOT (slotKFMJobDone2 (int)));
    tmpFile.sprintf ("file:/tmp/killu%i", time (0L));
    kiojob->copy (surl, tmpFile);
    KURL tmpURL (tmpFile);
    localFile = tmpURL.path ();
#else
    kfmConn = new KFM;
    if (kfmConn->isOK ()) {
      QString tmpFile;

      connect (kfmConn, SIGNAL(finished ()), this, SLOT (slotKFMJobDone ()));
      tmpFile.sprintf ("file:/tmp/killu%i", time (0L));
      kfmConn->copy (surl, tmpFile);
      KURL tmpURL (tmpFile);
      localFile = tmpURL.path ();
    }
#endif
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
			    KMsgBox::QUESTION | KMsgBox::DB_FIRST,
			    i18n ("Yes"), i18n ("No"), i18n ("Cancel"));
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
    backupFile (document->fileName ());
    ofstream os ((const char *) document->fileName ());
    if (os.fail ())
      // write out an error message !!
      return;

    document->saveToXml (os);
    document->setFileName ((const char *) document->fileName ());
    PStateManager::instance ()->addRecentFile ((const char *)
					       document->fileName ());
    setUnsavedData (false);
  }
}

void KIllustrator::saveAsFile () {
  QString fname = KFileDialog::getSaveFileName ((const char *)
						lastSaveDir, "*.kil", this);
  QFileInfo finfo (fname);
    lastSaveDir = finfo.dirPath ();
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

      backupFile (fname);
    }
    ofstream os (fname);
    if (os.fail ())
      // write out an error message !!
      return;
    document->saveToXml (os);
    document->setFileName (fname);
    PStateManager::instance ()->addRecentFile ((const char *) fname);
    setFileCaption (fname);
    setUnsavedData (false);
    lastExport = "";
  }
}

void KIllustrator::backupFile (const QString& fname) {
  QString newName;
  int pos = fname.findRev ('/');
  if (pos == -1)
    newName = "backup_of_" + fname;
  else {
    newName = fname.left (pos + 1);
    newName += "backup_of_";
    newName += fname.right (fname.length () - pos - 1);
  }
  rename ((const char *) fname, (const char *) newName);
}

void KIllustrator::selectColor (int flag, int idx, const QBrush& b) {
  if (flag == 0) {
    setPenColor (b);
    ColorField* cfield =
      (ColorField *) colorPalette->getWidget (selectedColorIdx);
    assert (cfield);
    cfield->highlight (false);

    cfield = (ColorField *) colorPalette->getWidget (idx);
    assert (cfield);
    cfield->highlight (true);
    selectedColorIdx = idx;
  }
  else if (flag == 1)
    emit setFillColor (b);
}

void KIllustrator::setPenColor (const QBrush& b) {
  GObject::OutlineInfo oInfo;
  oInfo.mask = GObject::OutlineInfo::Color;
  oInfo.color = b.color ();
  if (b.style () == NoBrush) {
    // chaage pen style only for NoPen, otherwise keep the current style
    oInfo.style = NoPen;
    oInfo.mask |= GObject::OutlineInfo::Style;
  }

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

void KIllustrator::slotKFMJobDone2 (int) {
  slotKFMJobDone ();
}

void KIllustrator::slotKFMJobDone () {
#ifndef NEWKDE
  delete kfmConn;
  kfmConn = 0L;
#endif
  openFile ((const char *)localFile);
}

void KIllustrator::about (int id) {
  if (id == ID_HELP_ABOUT_APP) {
    AboutDialog dialog;
    dialog.exec ();
  }
}

void KIllustrator::zoomFactorSlot (int idx) {
  if (canvas) {
    canvas->setZoomFactor (zFactors[idx]);
    // set focus to the canvas
    canvas->setFocus ();
  }
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

QString KIllustrator::getExportFileName (FilterManager *filterMgr) {
    const char *defaultExt = 0L;
    QString extension;

    if (! lastExport.isEmpty ()) {
	int pos = lastExport.findRev ('.', -1, false);
	if (pos != -1) {
	    extension =
		lastExport.right (lastExport.length () - pos - 1);
	    defaultExt = (const char *) extension;
	}
    }
    QString filter = filterMgr->exportFilters (defaultExt);

    KFileDialog *dlg = new KFileDialog ((const char *) lastExportDir,
					(const char *) filter, this,
					0L, true, false);
    dlg->setCaption (i18n ("Save As"));
    if (! lastExport.isEmpty ()) {
	dlg->setSelection ((const char *) lastExport);
    }
    QString filename;

    if (dlg->exec() == QDialog::Accepted) {
	   filename = dlg->selectedFile ();
      QFileInfo finfo (filename);
      lastExportDir = finfo.dirPath ();
    }

    delete dlg;

    return filename;
}

void KIllustrator::exportToFile () {
  FilterManager* filterMgr = FilterManager::instance ();
  QString filter = filterMgr->exportFilters ();

  QString fname = getExportFileName (filterMgr);

  if (! fname.isEmpty ()) {
    FilterInfo* filterInfo = filterMgr->findFilter (fname,
						    FilterInfo::FKind_Export);

    if (filterInfo) {
      ExportFilter* filter = filterInfo->exportFilter ();
      if (filter->setup (document, filterInfo->extension ())) {
	filter->setOutputFileName (fname);
	filter->exportToFile (document);
	lastExport = fname;
      }
      else
	QMessageBox::critical (this, i18n ("KIllustrator Error"),
			       i18n ("Cannot export to file"), i18n ("OK"));
    }
    else
      QMessageBox::critical (this, i18n ("KIllustrator Error"),
			     i18n ("Unknown export format"), i18n ("OK"));
  }
  resetTools ();
}

void KIllustrator::importFromFile () {
  FilterManager* filterMgr = FilterManager::instance ();
  QString filter = filterMgr->importFilters ();

  QString fname =
    KFilePreviewDialog::getOpenFileName ((const char *) lastImportDir,
					 (const char *) filter, this);
  if (! fname.isEmpty ()) {
    QFileInfo finfo ((const char *) fname);
    if (!finfo.isFile () || !finfo.isReadable ())
      return;

    lastImportDir = finfo.dirPath ();
    FilterInfo* filterInfo = filterMgr->findFilter (fname,
						    FilterInfo::FKind_Import);
    if (filterInfo) {
      ImportFilter* filter = filterInfo->importFilter ();
      if (filter->setup (document, filterInfo->extension ())) {
	filter->setInputFileName (fname);
	filter->importFromFile (document);
      }
      else
	QMessageBox::critical (this, i18n ("KIllustrator Error"),
			       i18n ("Cannot import from file"), i18n ("OK"));
    }
    else
      QMessageBox::critical (this, i18n ("KIllustrator Error"),
			     i18n ("Unknown import format"), i18n ("OK"));
  }
  resetTools ();
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

void KIllustrator::popupForSelection (int x, int y) {
  popupMenu->popup (QCursor::pos ());
}

void KIllustrator::popupForObject (int x, int y, GObject* obj) {
}

void KIllustrator::resetTools () {
  if (toolPalette->isButtonOn (ID_TOOL_EDITPOINT))
    return;

  if (! toolPalette->isButtonOn (ID_TOOL_SELECT))
    toolPalette->toggleButton (ID_TOOL_SELECT);
  tcontroller->toolSelected (ID_TOOL_SELECT);
}

void KIllustrator::toolSelected (int id) {
  if (id == ID_TOOL_EDITPOINT) {
    if (! editPointToolbar->isButtonOn (ID_TOOL_EP_MOVE))
      editPointToolbar->toggleButton (ID_TOOL_EP_MOVE);
    editPointToolbar->enable (KToolBar::Show);
    editPointToolbar->move (QCursor::pos ());

  }
  else {
    editPointToolbar->move (10000, 10000);
    editPointToolbar->enable (KToolBar::Hide);
  }
}

void KIllustrator::documentIsModifiedSlot (bool flag) {
  setUnsavedData (flag);
}

void KIllustrator::saveProperties (KConfig* config) {
  if (document->objectCount () > 0 &&
      strcmp (document->fileName (), UNNAMED_FILE) != 0)
    config->writeEntry ("Name", document->fileName ());
}

void KIllustrator::readProperties (KConfig* config) {
  QString entry = config->readEntry ("Name", "");
  if (! entry.isEmpty ())
    openURL ((const char *) entry);
}

void KIllustrator::restoreRulerStatus () {
  KConfig* config = kapp->getConfig ();
  QString oldgroup = config->group ();

  config->setGroup ("Ruler");
  bool show_it = config->readBoolEntry ("showRuler", true);
  config->setGroup (oldgroup);

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

void KIllustrator::saveRulerStatus (bool show_it) {
  KConfig* config = kapp->getConfig ();
  QString oldgroup = config->group ();

  config->setGroup ("Ruler");
  config->writeEntry ("showRuler", show_it);

  config->setGroup (oldgroup);
  config->sync ();
}

void KIllustrator::restoreToolbarStatus () {
  KConfig* config = kapp->getConfig ();
  QString oldgroup = config->group ();

  config->setGroup ("Toolbar Positions");

  QString str = config->readEntry ("toolbar");
  if (! str.isNull ()) {
    if (str == "Left")
      toolbar->setBarPos (KToolBar::Left);
    else if (str == "Right")
      toolbar->setBarPos (KToolBar::Right);
    else if (str == "Bottom")
      toolbar->setBarPos (KToolBar::Bottom);
    else
      toolbar->setBarPos (KToolBar::Top);
  }

  str = config->readEntry ("toolpalette");
  if (! str.isNull ()) {
    if (str == "Top")
      toolPalette->setBarPos (KToolBar::Top);
    else if (str == "Right")
      toolPalette->setBarPos (KToolBar::Right);
    else if (str == "Bottom")
      toolPalette->setBarPos (KToolBar::Bottom);
    else
      toolPalette->setBarPos (KToolBar::Left);
  }

  str = config->readEntry ("colorpalette");
  if (! str.isNull ()) {
    if (str == "Left")
      colorPalette->setBarPos (KToolBar::Left);
    else if (str == "Top")
      colorPalette->setBarPos (KToolBar::Top);
    else if (str == "Bottom")
      colorPalette->setBarPos (KToolBar::Bottom);
    else
      colorPalette->setBarPos (KToolBar::Right);
  }

  config->setGroup (oldgroup);
}

void KIllustrator::saveToolbarStatus () {
  KConfig* config = kapp->getConfig ();
  QString oldgroup = config->group ();

  config->setGroup ("Toolbar Positions");
  QString str;
  if ( toolbar->barPos() == KToolBar::Left )
    str = "Left";
  else if ( toolbar->barPos() == KToolBar::Right )
    str = "Right";
  else if ( toolbar->barPos() == KToolBar::Bottom )
    str = "Bottom";
  else
    str = "Top";
  config->writeEntry( "toolbar", str.data() );

  if ( toolPalette->barPos() == KToolBar::Left )
    str = "Left";
  else if ( toolPalette->barPos() == KToolBar::Right )
    str = "Right";
  else if ( toolPalette->barPos() == KToolBar::Bottom )
    str = "Bottom";
  else
    str = "Top";
  config->writeEntry( "toolpalette", str.data() );

  if ( colorPalette->barPos() == KToolBar::Left )
    str = "Left";
  else if ( colorPalette->barPos() == KToolBar::Right )
    str = "Right";
  else if ( colorPalette->barPos() == KToolBar::Bottom )
    str = "Bottom";
  else
    str = "Top";
  config->writeEntry( "colorpalette", str.data() );

  config->setGroup (oldgroup);
  config->sync ();
}

void KIllustrator::restoreMenubarStatus () {
  KConfig* config = kapp->getConfig ();
  QString oldgroup = config->group ();

  QString str = config->readEntry ("Menubar Position");
  if (! str.isNull ()) {
    if (str == "Bottom")
      menubar->setMenuBarPos (KMenuBar::Bottom);
    else
      menubar->setMenuBarPos (KMenuBar::Top);
  }

   config->setGroup (oldgroup);
}

void KIllustrator::saveMenubarStatus () {
  KConfig* config = kapp->getConfig ();
  QString oldgroup = config->group ();

  QString str;
  if ( menubar->menuBarPos() == KMenuBar::Bottom )
    str = "Bottom";
  else
    str = "Top";
  config->writeEntry( "Menubar Position", str.data() );

  config->setGroup (oldgroup);
  config->sync ();
}

bool KIllustrator::parseColorPalette (const char* fname,
				      vector<QColor>& colors) {
  ifstream in (fname);
  if (in.fail ()) {
    cerr << "Cannot open color palette: " << fname << endl;
    return false;
  }

  char buf[80];
  in.getline (buf, 80);
  if (strcasecmp (buf, "KIllustrator Palette") == 0 ||
      strcasecmp (buf, "GIMP Palette") == 0) {
    while (! in.eof ()) {
      int r, g, b;
      char c;

      in.get (c);
      if (c != '#') {
	in.putback (c);
	in >> r >> g >> b;
	colors.push_back (QColor (r, g, b));
      }
      in.ignore (INT_MAX, '\n');
    }
    return true;
  }
  return false;
}

void KIllustrator::loadPalette () {
  QString pfile = KFileDialog::getOpenFileName ((const char *) lastPaletteDir);
  QFileInfo finfo (pfile);
  lastPaletteDir = finfo.dirPath ();
  if (! pfile.isEmpty ()) {
    vector<QColor> new_palette;
    if (parseColorPalette ((const char *) pfile, new_palette)) {
      // update color palette
      palette = new_palette;
      updatePalette ();
    }
  }
}

void KIllustrator::updatePalette () {
  GObject::OutlineInfo oinfo = GObject::getDefaultOutlineInfo ();
  for (int i = 0; i < (int) palette.size (); i++) {
    QBrush brush (palette[i], i == 0 ? NoBrush : SolidPattern);
    ColorField* cfield = (ColorField *) colorPalette->getWidget (i);
    if (cfield == 0L)
      break;
    cfield->setBrush (brush);
    if (oinfo.color == palette[i]) {
      cfield->highlight (true);
      selectedColorIdx = i;
    }
  }
}

void KIllustrator::updateGridSettings () {
  if (canvas->snapToGrid () != layout->isItemChecked (ID_LAYOUT_ALIGN_GRID))
    layout->setItemChecked (ID_LAYOUT_ALIGN_GRID, canvas->snapToGrid ());
  view->setItemChecked (ID_VIEW_GRID, canvas->showGrid ());
  if (canvas->alignToHelplines () !=
      layout->isItemChecked (ID_LAYOUT_ALIGN_HLINES))
    layout->setItemChecked (ID_LAYOUT_ALIGN_HLINES,
			    canvas->alignToHelplines ());
  view->setItemChecked (ID_VIEW_HELPLINES, canvas->showHelplines ());
}

