#include <iostream.h>
#include "KIllustrator_shell.h"
#include "KIllustrator_view.h"
#include "KIllustrator_view.moc"

#include "QwViewport.h"
#include "GDocument.h"
#include "Canvas.h"
#include "Tool.h"
#include "Ruler.h"
#include "ToolController.h"
#include "ToolPalette.h"
#include "RectangleTool.h"
#include "PolylineTool.h"
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
#include "ColorSelectionPalette.h"
// #include "KIllustratorApp.h"
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
#include "FilterManager.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kapp.h>
#include <kmsgbox.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <kcombo.h>
#include <qlayout.h>
#include <unistd.h>

KIllustratorChildFrame::KIllustratorChildFrame (KIllustratorView* view, 
						KIllustratorChild* child) :
PartFrame_impl (view) {
  m_pView = view;
  m_pChild = child;
}

KIllustratorView::KIllustratorView (QWidget* parent, const char* name, 
				    KIllustratorDocument* doc) :
QWidget (parent), View_impl (), KIllustrator::View_skel () {
  setWidget (this);
  Control_impl::setFocusPolicy (OPControls::Control::ClickFocus);

  m_lstFrames.setAutoDelete (true);
  m_pDoc = doc;
  m_bShowGUI = true;
//  setGeometry (5000, 5000, 100, 100);
  viewport = 0L;

  // restore default settings
  PStateManager* pmgr = PStateManager::instance ();
}

KIllustratorView::~KIllustratorView () {
  cleanUp ();
}

void KIllustratorView::createGUI () {
  cout << "create GUI ..." << endl;
  setupMenu ();
  setupCanvas ();
}

void KIllustratorView::setupCanvas () {
  QPixmap pixmap;
  KIconLoader* loader = kapp->getIconLoader ();
  
  cout << "create Canvas ..." << endl;
  QWidget *w = new QWidget (this);
  w->resize (500, 500);
  QHBoxLayout *hlayout = new QHBoxLayout (w, 0, -1, "hbox");
  QVBoxLayout *vlayout = new QVBoxLayout ();
  hlayout->addLayout (vlayout);
  
  tpalette = new ToolPalette (5, 2, w);
  pixmap = loader->loadIcon ("selecttool.xpm");
  tpalette->setPixmap (0, pixmap);
  pixmap = loader->loadIcon ("pointtool.xpm");
  tpalette->setPixmap (1, pixmap);
  pixmap = loader->loadIcon ("linetool.xpm");
  tpalette->setPixmap (2, pixmap);
  pixmap = loader->loadIcon ("beziertool.xpm");
  tpalette->setPixmap (3, pixmap);
  pixmap = loader->loadIcon ("recttool.xpm");
  tpalette->setPixmap (4, pixmap);
  pixmap = loader->loadIcon ("polygontool.xpm");
  tpalette->setPixmap (5, pixmap);
  pixmap = loader->loadIcon ("ellipsetool.xpm");
  tpalette->setPixmap (6, pixmap);
  pixmap = loader->loadIcon ("texttool.xpm");
  tpalette->setPixmap (7, pixmap);
  pixmap = loader->loadIcon ("zoomtool.xpm");
  tpalette->setPixmap (8, pixmap);
  
  vlayout->addWidget (tpalette, 0, AlignTop);
  
  QGridLayout *grid = new QGridLayout (2, 2);
  hlayout->addLayout (grid, 1);
  
  hRuler = new Ruler (Ruler::Horizontal, Ruler::Point, w);
  vRuler = new Ruler (Ruler::Vertical, Ruler::Point, w);
  grid->addWidget (hRuler, 0, 1);
  grid->addWidget (vRuler, 1, 0);

  viewport = new QwViewport (w);

  canvas = new Canvas (m_pDoc, 72.0, viewport, viewport->portHole ());
  QObject::connect (canvas, SIGNAL(sizeChanged ()), 
           viewport, SLOT(resizeScrollBars ()));
  QObject::connect (canvas, SIGNAL(visibleAreaChanged (int, int)),
	   hRuler, SLOT(updateVisibleArea (int, int)));
  QObject::connect (canvas, SIGNAL(visibleAreaChanged (int, int)),
	   vRuler, SLOT(updateVisibleArea (int, int)));

  QObject::connect (canvas, SIGNAL(zoomFactorChanged (float)),
	   hRuler, SLOT(setZoomFactor (float)));
  QObject::connect (canvas, SIGNAL(zoomFactorChanged (float)),
	   vRuler, SLOT(setZoomFactor (float)));
  //  QObject::connect (canvas, SIGNAL(zoomFactorChanged (float)),
  //	   this, SLOT(updateZoomFactor (float)));
  //  QObject::connect (canvas, SIGNAL(mousePositionChanged (int, int)),
  //	   this, SLOT(showCursorPosition(int, int)));
  QObject::connect (canvas, SIGNAL(mousePositionChanged (int, int)),
	   hRuler, SLOT(updatePointer(int, int)));
  QObject::connect (canvas, SIGNAL(mousePositionChanged (int, int)),
	   vRuler, SLOT(updatePointer(int, int)));

  grid->addWidget (viewport, 1, 1);
  grid->setRowStretch (1, 20);
  grid->setColStretch (1, 20);

  cpalette = new ColorSelectionPalette (18, w);
  //  QObject::connect (cpalette, SIGNAL(penColorSelected(const QBrush&)),
  //	   this, SLOT(setPenColor(const QBrush&)));
  //  QObject::connect (cpalette, SIGNAL(fillColorSelected(const QBrush&)),
  //	   this, SLOT(setFillColor(const QBrush&)));
  hlayout->addWidget (cpalette, 0, AlignTop);
  
//  setView (w);
  
  tcontroller = new ToolController (this);
  Tool* tool;
  tcontroller->registerTool (0, tool = new SelectionTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (1, tool = new EditPointTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (2, tool = new PolylineTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (3, tool = new BezierTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (4, tool = new RectangleTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (5, tool = new PolygonTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (6, tool = new OvalTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (7, tool = new TextTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->registerTool (8, tool = new ZoomTool (&cmdHistory));
  //  QObject::connect (tool, SIGNAL(modeSelected(const char*)),
  //	   this, SLOT(showCurrentMode(const char*)));
  tcontroller->toolSelected (0);

  QObject::connect (tpalette, SIGNAL (toolSelected (int)), tcontroller,
	   SLOT(toolSelected (int)));
  QObject::connect (tpalette, SIGNAL (toolConfigActivated (int)), tcontroller,
	   SLOT(configureTool (int)));
  canvas->setToolController (tcontroller);
  hlayout->activate ();
  w->show ();
  mainWidget = w;
  resizeEvent (0L);
}

void KIllustratorView::setupMenu () {
  m_vMenuBarFactory = m_vPartShell->menuBarFactory ();
  if (! CORBA::is_nil (m_vMenuBarFactory)) {
    m_rMenuBar = m_vMenuBarFactory->createMenuBar (this);

    /* ------------- Edit Menu ------------- */
    m_idMenuEdit = m_rMenuBar->insertMenu (CORBA::string_dup (i18n ("&Edit")));
  
    m_idMenuEdit_Undo = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Undo")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editUndo"));
    m_idMenuEdit_Redo = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Redo")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editRedo"));
    m_rMenuBar->insertSeparator (m_idMenuEdit);
    m_idMenuEdit_Copy = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("&Copy")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editCopy"));
    m_idMenuEdit_Paste = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("&Paste")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editPaste"));
    m_idMenuEdit_Cut = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("C&ut")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editCut"));

    m_rMenuBar->insertSeparator (m_idMenuEdit);

    m_idMenuEdit_SelectAll = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("&Select All")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editSelectAll"));

    m_rMenuBar->insertSeparator (m_idMenuEdit);

    m_idMenuEdit_InsertObject = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("&Insert Object...")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editInsertObject"));
    m_idMenuEdit_Properties = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Pr&operties")), 
			      m_idMenuEdit, this, 
			      CORBA::string_dup ("editProperties"));

    /* ------------- View Menu ------------- */
    m_idMenuView = m_rMenuBar->insertMenu (CORBA::string_dup (i18n ("&View")));
    m_idMenuView_Outline =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Outline")), 
			      m_idMenuView, this, 
			      CORBA::string_dup ("viewOutline"));
    m_rMenuBar->setCheckable (m_idMenuView_Outline, true);

    m_idMenuView_Draft =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Draft")), 
			      m_idMenuView, this, 
			      CORBA::string_dup ("viewDraft"));
    m_rMenuBar->setCheckable (m_idMenuView_Draft, true);

    m_idMenuView_Normal =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Normal")), 
			      m_idMenuView, this, 
			      CORBA::string_dup ("viewNormal"));
    m_rMenuBar->setCheckable (m_idMenuView_Normal, true);

    m_rMenuBar->insertSeparator (m_idMenuView);

    m_idMenuView_Ruler =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Ruler")), 
			      m_idMenuView, this, 
			      CORBA::string_dup ("showRuler"));
    m_rMenuBar->setCheckable (m_idMenuView_Ruler, true);

    m_idMenuView_Grid =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Grid")), 
			      m_idMenuView, this, 
			      CORBA::string_dup ("showGrid"));
    m_rMenuBar->setCheckable (m_idMenuView_Grid, true);
      
    /* ------------- Layout Menu ------------- */
    m_idMenuLayout = 
      m_rMenuBar->insertMenu (CORBA::string_dup (i18n ("&Layout")));
    m_idMenuLayout_InsertPage =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Insert Page")), 
			      m_idMenuLayout, this, 
			      CORBA::string_dup ("insertPage"));
    m_idMenuLayout_RemovePage =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Remove Page")), 
			      m_idMenuLayout, this, 
			      CORBA::string_dup ("removePage"));
    m_idMenuLayout_GotoPage =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Go to Page")), 
			      m_idMenuLayout, this, 
			      CORBA::string_dup ("gotoPage"));
    m_idMenuLayout_PageLayout =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Page Layoout")), 
			      m_idMenuLayout, this, 
			      CORBA::string_dup ("setupPage"));

    m_rMenuBar->insertSeparator (m_idMenuLayout);

    m_idMenuLayout_SetupGrid =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Setup Grid")), 
			      m_idMenuLayout, this, 
			      CORBA::string_dup ("setupGrid"));

    m_idMenuLayout_AlignToGrid =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Align to Grid")), 
			      m_idMenuLayout, this, 
			      CORBA::string_dup ("alignToGrid"));
    m_rMenuBar->setCheckable (m_idMenuLayout_AlignToGrid, true);

    /* ------------- Arrange Menu ------------- */
    m_idMenuArrange = 
      m_rMenuBar->insertMenu (CORBA::string_dup (i18n ("&Arrange")));
    m_idMenuArrange_Align =
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("Align")), 
			      m_idMenuArrange, this, 
			      CORBA::string_dup ("arrangeAlign"));


    /* ------------- Extras Menu ------------- */
    m_idMenuExtras = 
      m_rMenuBar->insertMenu (CORBA::string_dup (i18n ("Ex&tras")));

    /* ------------- Help Menu ------------- */

    CORBA::Long id = m_rMenuBar->helpMenuId ();
    m_idMenuHelp_About = 
      m_rMenuBar->insertItem (CORBA::string_dup (i18n ("About")), 
			      id, this, 
			      CORBA::string_dup ("helpAbout"));
  }
}

void KIllustratorView::cleanUp () {
  if (m_bIsClean)
    return;

  m_pDoc->removeView (this);
  m_lstFrames.clear ();
  
  View_impl::cleanUp ();
}

void KIllustratorView::construct () {
  cout << "KIllustratorView::construct ()" << endl;
  resizeEvent (0L);
}

void KIllustratorView::newView () {
  KIllustratorShell* shell = new KIllustratorShell;
  shell->enableMenuBar ();
  shell->PartShell_impl::enableStatusBar ();
  shell->enableToolBars ();
  shell->show ();
  shell->setDocument (m_pDoc);

  CORBA::release (shell);
}

void KIllustratorView::resizeEvent (QResizeEvent* ) {
  if (mainWidget) {
    cout << "resize mainWidget to: " << width () << ", " << height () << endl;
    mainWidget->resize (width (), height ()); 
  }
}

void KIllustratorView::setMode (OPParts::Part::Mode m) {
  Part_impl::setMode (m);
  if (mode () == OPParts::Part::ChildMode && ! m_bFocus)
    m_bShowGUI = false;
  else
    m_bShowGUI = true;
}

void KIllustratorView::setFocus (CORBA::Boolean m) {
  Part_impl::setFocus (m);
  bool old = m_bShowGUI;

  if (mode () == OPParts::Part::ChildMode && ! m_bFocus)
    m_bShowGUI = false;
  else
    m_bShowGUI = true;

  if (old != m_bShowGUI)
    resizeEvent (0L);
}

CORBA::Boolean KIllustratorView::printDlg () {
  return false;
}

void KIllustratorView::editUndo () {
}

void KIllustratorView::editRedo () {
}

void KIllustratorView::editCut () {
}

void KIllustratorView::editCopy () {
}

void KIllustratorView::editPaste () {
}

void KIllustratorView::editSelectAll () {
}

void KIllustratorView::editDelete () {
}

void KIllustratorView::editInsertOject () {
}

void KIllustratorView::editProperties () {
}

