/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2004 theKompany.com & Dave Marotti,
 *                         Peter Simonsson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <kprinter.h> // has to be first

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include <qlayout.h>
#include <qwidgetstack.h>
#include <qsplitter.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qprintdialog.h>
#include <qptrcollection.h>
#include <qkeycode.h>
#include <qcheckbox.h>
#include <qmime.h>
#include <qtoolbutton.h>
#include <qtimer.h>
#include <qbutton.h>
#include <qclipboard.h>
#include <qprogressbar.h>

#include <qstringlist.h>
#include <qstrlist.h>
#include <qimage.h>
#include <kfiledialog.h>

#include <kdialogbase.h>
#include <kaction.h>
#include <kcolorbutton.h>
#include <kapplication.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kstdaccel.h>
#include <kstdaction.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <kstatusbar.h>
#include <kinputdialog.h>
#include <knotifyclient.h>

#include <dcopclient.h>
#include <dcopref.h>

#include <kparts/event.h>
#include <koPartSelectDia.h>
#include <koQueryTrader.h>
#include <koPageLayoutDia.h>
#include <koRuler.h>
#include <kozoomhandler.h>
#include <koUnitWidgets.h>
#include <koApplication.h>
#include <kotabbar.h>
#include <koPageLayoutDia.h>
#include <kolinewidthaction.h>
#include <kolinestyleaction.h>
#include "kotooldockmanager.h"
#include "kotooldockbase.h"

#include "kivio_view.h"
#include "kivio_dlg_pageshow.h"
#include "kivio_factory.h"
#include "kivio_map.h"
#include "kivio_page.h"
#include "kivio_doc.h"
#include "kivio_canvas.h"
#include "kivio_stencil_spawner.h"
#include "kivio_grid_data.h"

#include "tkcoloractions.h"

#include "kivio_protection_panel.h"
#include "kivio_stencil_geometry_panel.h"
#include "kivio_layer_panel.h"
#include "kivio_birdeye_panel.h"
#include "export_page_dialog.h"

#include "kivioaligndialog.h"
#include "kiviooptionsdialog.h"

#include "stencilbardockmanager.h"
#include "kivio_common.h"
#include "kivio_painter.h"
#include "kivio_stencil.h"
#include "kivio_stencil_spawner_set.h"
#include "kivio_screen_painter.h"

#include "kivio_pluginmanager.h"

#include "handler.h"

#include "kivio_stackbar.h"
#include "kivio_icon_view.h"

#include "KIvioViewIface.h"
#include "kivio_command.h"
#include "kiviostencilsetaction.h"
#include <qiconview.h>
#include "kivioarrowheadaction.h"
#include "kiviotextformatdlg.h"
#include "kiviostencilformatdlg.h"
#include "kivioarrowheadformatdlg.h"
#include "kiviodragobject.h"
#include "kivioglobal.h"
#include "kivio_config.h"
#include "kivioaddstencilsetpanel.h"
#include "kiviostencilsetinstaller.h"

#define TOGGLE_ACTION(X) ((KToggleAction*)actionCollection()->action(X))
#define MOUSEPOS_TEXT 1000

using namespace Kivio;

KivioView::KivioView( QWidget *_parent, const char *_name, KivioDoc* doc )
: KoView( doc, _parent, _name )
{
  m_pluginManager = new PluginManager(this, "Kivio Plugin Manager");
  m_zoomHandler = new KoZoomHandler();
  zoomHandler()->setZoomAndResolution(100, KoGlobal::dpiX(),
    KoGlobal::dpiY());
  m_pDoc = doc;
  m_pActivePage = 0;
  dcop = 0;
  dcopObject(); // build it

  // Add coords to the statusbar
  QString unit = KoUnit::unitName(m_pDoc->units());
  KoPoint xy(0, 0);
  QString text = i18n("X: %1 %3 Y: %2 %4").arg(KGlobal::_locale->formatNumber(xy.x(), 2))
  .arg(KGlobal::_locale->formatNumber(xy.y(), 2)).arg(unit).arg(unit);
  m_coordSLbl = new KStatusBarLabel(text, 1000);
  addStatusBarItem(m_coordSLbl, 0, true);

  // Handle progress information from the doc
  m_statusBarProgress = 0;

  connect(m_pDoc, SIGNAL(initProgress()), this, SLOT(initStatusBarProgress()));
  connect(m_pDoc, SIGNAL(progress(int)), this, SLOT(setStatusBarProgress(int)));
  connect(m_pDoc, SIGNAL(endProgress()), this, SLOT(removeStatusBarProgress()));

  bool isModified = doc->isModified();
  m_pStencilBarDockManager = new StencilBarDockManager(this);
  m_pStencilBarDockManager->setDoc( doc );

  // QGridLayout for the entire view
  QGridLayout *viewGrid = new QGridLayout(this);
  viewGrid->addWidget(m_pStencilBarDockManager, 0, 0);

  // A widget to hold the entire right side (old view)
  QWidget *pRightSide = new QWidget(m_pStencilBarDockManager);
  m_pStencilBarDockManager->setView(pRightSide);

  // Split tabbar and Horz. Scroll Bar
  QSplitter* tabSplit = new QSplitter(pRightSide);

  // Tab Bar
  m_pTabBar = new KoTabBar(tabSplit);
  connect( m_pTabBar,
           SIGNAL(tabChanged(const QString&)),
           SLOT( changePage(const QString&)));
  connect( m_pTabBar, SIGNAL( doubleClicked() ), SLOT( renamePage() ) );
  connect( m_pTabBar, SIGNAL( contextMenu( const QPoint& ) ),
      SLOT( popupTabBarMenu( const QPoint& ) ) );
  m_pTabBar->setReverseLayout( QApplication::reverseLayout() );

  // Scroll Bar
  QScrollBar* vertScrollBar = new QScrollBar(QScrollBar::Vertical,pRightSide);
  QScrollBar* horzScrollBar = new QScrollBar(QScrollBar::Horizontal,tabSplit);

  QValueList<int> sizes;
  sizes << tabSplit->width() / 2 << tabSplit->width() / 2;
  tabSplit->setSizes(sizes);
  QHBoxLayout* tabLayout = new QHBoxLayout();
  tabLayout->addWidget(tabSplit);

  // The widget on which we display the page
  QWidgetStack* canvasBase = new QWidgetStack(pRightSide);
  m_pCanvas = new KivioCanvas(canvasBase,this,doc,vertScrollBar,horzScrollBar);
  canvasBase->addWidget(m_pCanvas,0);
  canvasBase->raiseWidget(m_pCanvas);
  m_pCanvas->setFocusPolicy(QWidget::StrongFocus);

  // Rulers
  vRuler = new KoRuler(pRightSide, m_pCanvas, Qt::Vertical, Kivio::Config::defaultPageLayout(),
    KoRuler::F_HELPLINES, m_pDoc->units());
  vRuler->showMousePos(true);
  vRuler->setZoom(zoomHandler()->zoomedResolutionY());
  hRuler = new KoRuler(pRightSide, m_pCanvas, Qt::Horizontal, Kivio::Config::defaultPageLayout(),
    KoRuler::F_HELPLINES, m_pDoc->units());
  hRuler->showMousePos(true);
  hRuler->setZoom(zoomHandler()->zoomedResolutionX());
  connect(vertScrollBar, SIGNAL(valueChanged(int)), SLOT(setRulerVOffset(int)));
  connect(horzScrollBar, SIGNAL(valueChanged(int)), SLOT(setRulerHOffset(int)));
  connect(vRuler, SIGNAL(unitChanged(KoUnit::Unit)), SLOT(rulerChangedUnit(KoUnit::Unit)));
  connect(hRuler, SIGNAL(unitChanged(KoUnit::Unit)), SLOT(rulerChangedUnit(KoUnit::Unit)));
  connect(vRuler, SIGNAL(doubleClicked()), SLOT(paperLayoutDlg()));
  connect(hRuler, SIGNAL(doubleClicked()), SLOT(paperLayoutDlg()));
  connect( m_pDoc, SIGNAL(unitsChanged(KoUnit::Unit)), SLOT(setRulerUnit(KoUnit::Unit)) );
  vRuler->installEventFilter(m_pCanvas);
  hRuler->installEventFilter(m_pCanvas);

  QGridLayout* layout = new QGridLayout(pRightSide);
  layout->addWidget(hRuler, 0, 1);
  layout->addWidget(vRuler, 1, 0);
  layout->addWidget(canvasBase, 1, 1);
  layout->addMultiCellLayout(tabLayout, 2, 2, 0, 1);
  layout->addMultiCellWidget(vertScrollBar, 0, 1, 2, 2);
  layout->setRowStretch(1, 10);
  layout->setColStretch(1, 10);

  QWidget::setFocusPolicy( QWidget::StrongFocus );
  setFocusProxy( m_pCanvas );

  connect( this, SIGNAL( invalidated() ), m_pCanvas, SLOT( update() ) );
  connect( this, SIGNAL( regionInvalidated( const QRegion&, bool ) ), m_pCanvas, SLOT( repaint( const QRegion&, bool ) ) );

  m_pToolDockManager = new KoToolDockManager(canvasBase);

  setInstance(KivioFactory::global());
  if ( !m_pDoc->isReadWrite() )
    setXMLFile("kivio_readonly.rc");
  else
    setXMLFile("kivio.rc");


  // Must be executed before setActivePage() and before setupActions()
  createGeometryDock();
  createLayerDock();
  createBirdEyeDock();
  createProtectionDock();
  createAddStencilSetDock();

  setupActions();


  KivioPage* page;
  for ( page = m_pDoc->map()->firstPage(); page; page = m_pDoc->map()->nextPage() )
    addPage(page);

  setActivePage(m_pDoc->map()->firstPage());


  connect( m_pDoc, SIGNAL( sig_selectionChanged() ), SLOT( updateToolBars() ) );
  connect( m_pDoc, SIGNAL( sig_addPage(KivioPage*) ), SLOT( slotAddPage(KivioPage*) ) );
  connect( m_pDoc, SIGNAL( sig_addSpawnerSet(KivioStencilSpawnerSet*) ), SLOT(addSpawnerToStackBar(KivioStencilSpawnerSet*)) );
  connect( m_pDoc, SIGNAL( sig_updateView(KivioPage*) ), SLOT(slotUpdateView(KivioPage*)) );
  connect( m_pDoc, SIGNAL( sig_pageNameChanged(KivioPage*,const QString&)), SLOT(slotPageRenamed(KivioPage*,const QString&)) );

  connect( m_pDoc, SIGNAL( sig_updateGrid()),SLOT(slotUpdateGrid()));

  initActions();

  // Load any already-loaded stencils into the stencil dock
  if( m_pDoc->isReadWrite() ) // only if not embedded in Konqueror
  {
    KivioStencilSpawnerSet *pSet;
    pSet = m_pDoc->spawnerSets()->first();
    while( pSet )
    {
      addSpawnerToStackBar( pSet );
      pSet = m_pDoc->spawnerSets()->next();
    }
  }

  m_pDoc->setModified(isModified);
  pluginManager()->activateDefaultTool();
  clipboardDataChanged();  // Enable/disable the paste action
}

KivioView::~KivioView()
{
  delete dcop;
  delete m_zoomHandler;
}

DCOPObject* KivioView::dcopObject()
{
  if ( !dcop ) {
    dcop = new KIvioViewIface( this );
  }

  return dcop;
}

void KivioView::createGeometryDock()
{
  m_pStencilGeometryPanel = new KivioStencilGeometryPanel(this);
  KoToolDockBase* stencilGeometryBase = toolDockManager()->createSimpleToolDock(m_pStencilGeometryPanel, "geometry");
  stencilGeometryBase -> setCaption(i18n("Geometry"));
  stencilGeometryBase -> restore();

  connect( m_pStencilGeometryPanel, SIGNAL(positionChanged(double, double)), this, SLOT(slotChangeStencilPosition(double, double)) );
  connect( m_pStencilGeometryPanel, SIGNAL(sizeChanged(double, double)), this, SLOT(slotChangeStencilSize(double, double)) );
  connect(m_pStencilGeometryPanel, SIGNAL(rotationChanged(int)), SLOT(slotChangeStencilRotation(int)));

  connect( m_pDoc, SIGNAL(unitsChanged(KoUnit::Unit)), m_pStencilGeometryPanel, SLOT(setUnit(KoUnit::Unit)) );

  KToggleAction* showStencilGeometry = new KToggleAction( i18n("Stencil Geometry Panel"), "stencil_geometry", 0, actionCollection(), "stencilGeometry" );
  connect( showStencilGeometry, SIGNAL(toggled(bool)), stencilGeometryBase, SLOT(makeVisible(bool)));
  connect( stencilGeometryBase, SIGNAL(visibleChange(bool)), SLOT(toggleStencilGeometry(bool)));
}

void KivioView::createBirdEyeDock()
{
  m_pBirdEyePanel = new KivioBirdEyePanel(this, this);
  KoToolDockBase* birdEyeBase = toolDockManager()->createSimpleToolDock(m_pBirdEyePanel,  "birdeye");
  birdEyeBase -> setCaption(i18n("Bird's Eye"));
  birdEyeBase -> restore();

  KToggleAction* showBirdEye = new KToggleAction( i18n("Bird's Eye"), 0, actionCollection(), "birdEye" );
  connect( showBirdEye, SIGNAL(toggled(bool)), birdEyeBase, SLOT(makeVisible(bool)));
  connect( birdEyeBase, SIGNAL(visibleChange(bool)), SLOT(toggleBirdEyePanel(bool)));
}

void KivioView::createLayerDock()
{
  m_pLayersPanel = new KivioLayerPanel( this, this);
  KoToolDockBase* layersBase = toolDockManager()->createSimpleToolDock(m_pLayersPanel, "layers");
  layersBase -> setCaption(i18n("Layers"));
  layersBase -> restore();

  KToggleAction* showLayers = new KToggleAction( i18n("Layers Manager"), CTRL+Key_L, actionCollection(), "layersPanel" );
  connect( showLayers, SIGNAL(toggled(bool)), layersBase, SLOT(makeVisible(bool)));
  connect( layersBase, SIGNAL(visibleChange(bool)), SLOT(toggleLayersPanel(bool)));
}

void KivioView::createProtectionDock()
{
  m_pProtectionPanel = new KivioProtectionPanel(this,this);
  KoToolDockBase* protectionBase = toolDockManager()->createSimpleToolDock(m_pProtectionPanel, "protection");
  protectionBase -> setCaption(i18n("Protection"));
  protectionBase -> restore();

  KToggleAction *showProtection = new KToggleAction( i18n("Protection"), CTRL+SHIFT+Key_P, actionCollection(), "protection" );
  connect( showProtection, SIGNAL(toggled(bool)), protectionBase, SLOT(makeVisible(bool)));
  connect( protectionBase, SIGNAL(visibleChange(bool)), SLOT(toggleProtectionPanel(bool)));
}

void KivioView::createAddStencilSetDock()
{
  m_addStencilSetPanel = new Kivio::AddStencilSetPanel(this);
  KoToolDockBase* addStencilSetBase = toolDockManager()->createSimpleToolDock(m_addStencilSetPanel,  "addstencilset");
  addStencilSetBase -> setCaption(i18n("Add Stencil Set"));
  addStencilSetBase -> restore();

  KToggleAction *showAddStencilSet = new KToggleAction( i18n("Add Stencil Set"), 0, actionCollection(), "addStencilSetDock" );
  connect(showAddStencilSet, SIGNAL(toggled(bool)), addStencilSetBase, SLOT(makeVisible(bool)));
  connect(addStencilSetBase, SIGNAL(visibleChange(bool)), SLOT(toggleAddStencilSetPanel(bool)));
  connect(m_addStencilSetPanel, SIGNAL(addStencilSet(const QString&)), this, SLOT(addStencilSet(const QString&)));
  connect(this, SIGNAL(updateStencilSetList()), m_addStencilSetPanel, SLOT(updateList()));
}


void KivioView::setupActions()
{
  KivioStencilSetAction* addStSet =  new KivioStencilSetAction( i18n("Add Stencil Set"),
    "open_stencilset", actionCollection(), "addStencilSet" );
  connect(addStSet,SIGNAL(activated(const QString&)),SLOT(addStencilSet(const QString&)));
  connect(this, SIGNAL(updateStencilSetList()), addStSet, SLOT(updateMenu()));

  m_alignAndDistribute = new KAction( i18n("Align && Distribute..."), CTRL+ALT+Key_A, this,
    SLOT(alignStencilsDlg()), actionCollection(), "alignStencils" );

  m_editCut = KStdAction::cut( this, SLOT(cutStencil()), actionCollection(), "cutStencil" );
  m_editCopy = KStdAction::copy( this, SLOT(copyStencil()), actionCollection(), "copyStencil" );
  m_editPaste = KStdAction::paste( this, SLOT(pasteStencil()), actionCollection(), "pasteStencil" );
  connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));

  m_selectAll = KStdAction::selectAll(this, SLOT(selectAllStencils()), actionCollection(), "selectAllStencils");
  m_selectNone = KStdAction::deselect(this, SLOT(unselectAllStencils()), actionCollection(), "unselectAllStencils");

  m_groupAction = new KAction( i18n("Group Selection"), "group", CTRL+Key_G, this, SLOT(groupStencils()), actionCollection(), "groupStencils" );
  m_groupAction->setWhatsThis(i18n("Group selected objects into a single stencil"));
  m_ungroupAction = new KAction( i18n("Ungroup"), "ungroup", CTRL+SHIFT+Key_G, this, SLOT(ungroupStencils()), actionCollection(), "ungroupStencils" );
  m_ungroupAction->setWhatsThis(i18n("Break up a selected group stencil"));

  m_stencilToFront = new KAction( i18n("Bring to Front"), "bring_forward", 0, this, SLOT(bringStencilToFront()), actionCollection(), "bringStencilToFront" );
  m_stencilToBack = new KAction( i18n("Send to Back"), "send_backward", 0, this, SLOT(sendStencilToBack()), actionCollection(), "sendStencilToBack" );

  m_menuTextFormatAction = new KAction(i18n("&Text..."), 0, 0, this, SLOT(textFormat()),
  actionCollection(), "textFormat");

  m_menuStencilConnectorsAction = new KAction(i18n("&Stencils && Connectors..."), 0, 0, this, SLOT(stencilFormat()),
  actionCollection(), "stencilFormat");

  m_arrowHeadsMenuAction = new KAction(i18n("&Arrowheads..."), 0, 0, this, SLOT(arrowHeadFormat()),
  actionCollection(), "arrowHeadFormat");
  m_arrowHeadsMenuAction->setWhatsThis(i18n("Arrowheads allow you to add an arrow to the beginning and/or end of a line."));

  /* Create the fg color button */
  m_setFGColor = new TKSelectColorAction( i18n("Line Color"), TKSelectColorAction::LineColor, actionCollection(), "setFGColor" );
  m_setFGColor->setWhatsThis(i18n("The line color allows you to choose a color for the lines of the stencils."));
  connect(m_setFGColor,SIGNAL(activated()),SLOT(setFGColor()));
  m_setBGColor = new TKSelectColorAction( i18n("Fill Color"), TKSelectColorAction::FillColor, actionCollection(), "setBGColor" );
  m_setBGColor->setWhatsThis(i18n("You can choose a color for the background of a stencil by using this button."));
  connect(m_setBGColor,SIGNAL(activated()),SLOT(setBGColor()));

  // Text bar
  m_setFontFamily = new KFontAction( i18n( "Font Family" ), 0, actionCollection(), "setFontFamily" );
  connect( m_setFontFamily, SIGNAL(activated(const QString&)), SLOT(setFontFamily(const QString&)) );

  m_setFontSize = new KFontSizeAction( i18n( "Font Size" ), 0, actionCollection(), "setFontSize" );
  connect( m_setFontSize, SIGNAL( fontSizeChanged( int ) ),
           this, SLOT( setFontSize(int ) ) );


  m_setTextColor = new TKSelectColorAction( i18n("Text Color"), TKSelectColorAction::TextColor, actionCollection(), "setTextColor" );
  connect( m_setTextColor, SIGNAL(activated()), SLOT(setTextColor()) );

  m_setBold = new KToggleAction( i18n("Toggle Bold Text"), "text_bold", 0, actionCollection(), "setFontBold" );
  connect( m_setBold, SIGNAL(toggled(bool)), SLOT(toggleFontBold(bool)) );

  m_setItalics = new KToggleAction( i18n("Toggle Italics Text"), "text_italic", 0, actionCollection(), "setFontItalics" );
  connect( m_setItalics, SIGNAL(toggled(bool)), SLOT(toggleFontItalics(bool)) );

  m_setUnderline = new KToggleAction( i18n("Toggle Underline Text"), "text_under", 0, actionCollection(), "setFontUnderline" );
  connect( m_setUnderline, SIGNAL(toggled(bool)), SLOT(toggleFontUnderline(bool)));

  m_textAlignLeft = new KToggleAction( i18n( "Align &Left" ), "text_left", CTRL + Key_L,
                                    this, SLOT( textAlignLeft() ),
                                    actionCollection(), "textAlignLeft" );
  m_textAlignLeft->setExclusiveGroup( "align" );
  m_textAlignCenter = new KToggleAction( i18n( "Align &Center" ), "text_center", CTRL + ALT + Key_C,
                                      this, SLOT( textAlignCenter() ),
                                      actionCollection(), "textAlignCenter" );
  m_textAlignCenter->setExclusiveGroup( "align" );
  m_textAlignCenter->setChecked( TRUE );
  m_textAlignRight = new KToggleAction( i18n( "Align &Right" ), "text_right", CTRL + ALT + Key_R,
                                      this, SLOT( textAlignRight() ),
                                      actionCollection(), "textAlignRight" );
  m_textAlignRight->setExclusiveGroup( "align" );
  m_textVAlignSuper = new KToggleAction( i18n( "Superscript" ), "super", 0,
                                            this, SLOT( textSuperScript() ),
                                            actionCollection(), "textVAlignSuper" );
  m_textVAlignSuper->setExclusiveGroup( "valign" );
  m_textVAlignSub = new KToggleAction( i18n( "Subscript" ), "sub", 0,
                                            this, SLOT( textSubScript() ),
                                            actionCollection(), "textVAlignSub" );
  m_textVAlignSub->setExclusiveGroup( "valign" );

  m_lineWidthAction = new KoLineWidthAction(i18n("Line Width"), "linewidth", this, SLOT(setLineWidth(double)),
    actionCollection(), "setLineWidth");
  m_lineWidthAction->setUnit(m_pDoc->units());
  connect(m_pDoc, SIGNAL(unitsChanged(KoUnit::Unit)), m_lineWidthAction, SLOT(setUnit(KoUnit::Unit)));

  m_lineStyleAction = new KoLineStyleAction(i18n("Line Style"), "linestyle", this, SLOT(setLineStyle(int)),
    actionCollection(), "setLineStyle");

  m_paperLayout = new KAction( i18n("Page Layout..."), 0, this, SLOT(paperLayoutDlg()), actionCollection(), "paperLayout" );
  m_insertPage = new KAction( i18n("Insert Page"),"item_add", 0, this, SLOT(insertPage()), actionCollection(), "insertPage" );
  m_removePage = new KAction( i18n("Remove Page"), "item_remove",0,this, SLOT(removePage()), actionCollection(), "removePage" );

  m_renamePage = new KAction( i18n("Rename Page..."), "item_rename",0,this, SLOT(renamePage()), actionCollection(), "renamePage" );

  m_showPage = new KAction( i18n("Show Page..."),0 ,this,SLOT(showPage()), actionCollection(), "showPage" );
  m_hidePage = new KAction( i18n("Hide Page"),0 ,this,SLOT(hidePage()), actionCollection(), "hidePage" );
  m_exportPage = new KAction( i18n("Export Page..."),0,this,SLOT(exportPage()), actionCollection(), "exportPage");

  showPageMargins = new KToggleAction( i18n("Show Page Margins"), "view_margins", 0, actionCollection(), "showPageMargins" );
  connect( showPageMargins, SIGNAL(toggled(bool)), SLOT(togglePageMargins(bool)));
#if KDE_IS_VERSION(3,2,90)
  showPageMargins->setCheckedState(i18n("Hide Page Margins"));
#endif

  showRulers = new KToggleAction( i18n("Show Rulers"), "view_ruler", 0, actionCollection(), "showRulers" );
  connect( showRulers, SIGNAL(toggled(bool)), SLOT(toggleShowRulers(bool)));
#if KDE_IS_VERSION(3,2,90)
  showRulers->setCheckedState(i18n("Hide Rulers"));
#endif

  // Grid actions
  showGrid = new KToggleAction( i18n("Show Grid"), "view_grid", 0, actionCollection(), "showGrid" );
  connect( showGrid, SIGNAL(toggled(bool)), SLOT(toggleShowGrid(bool)));
#if KDE_IS_VERSION(3,2,90)
  showGrid->setCheckedState(i18n("Hide Grid"));
#endif

  KToggleAction* snapGrid = new KToggleAction( i18n("Snap Grid"), "view_grid", 0, actionCollection(), "snapGrid" );
  connect( snapGrid, SIGNAL(toggled(bool)), SLOT(toggleSnapGrid(bool)));

  // Guides actions
  showGuides = new KToggleAction( i18n("Show Guides"), 0, actionCollection(), "showGuides" );
  connect( showGuides, SIGNAL(toggled(bool)), SLOT(toggleShowGuides(bool)));
#if KDE_IS_VERSION(3,2,90)
  showGuides->setCheckedState(i18n("Hide Guides"));
#endif

  KToggleAction* snapGuides = new KToggleAction( i18n("Snap Guides"), 0, actionCollection(), "snapGuides" );
  connect( snapGuides, SIGNAL(toggled(bool)), SLOT(toggleSnapGuides(bool)));
  //--

  m_setArrowHeads = new KivioArrowHeadAction(i18n("Arrowheads"), "arrowheads", actionCollection(), "arrowHeads");
  m_setArrowHeads->setWhatsThis(i18n("Arrowheads allow you to add an arrow to the beginning and/or end of a line."));
  connect( m_setArrowHeads, SIGNAL(endChanged(int)), SLOT(slotSetEndArrow(int)));
  connect( m_setArrowHeads, SIGNAL(startChanged(int)), SLOT(slotSetStartArrow(int)));
  connect( m_pDoc, SIGNAL(unitsChanged(KoUnit::Unit)), SLOT(setRulerUnit(KoUnit::Unit)) );

  KStdAction::preferences(this, SLOT(optionsDialog()), actionCollection(), "options" );

  (void) new KAction(i18n("Install Stencil Set..."), 0, this,
    SLOT(installStencilSet()), actionCollection(), "installStencilSet");
}

void KivioView::initActions()
{
  togglePageMargins(true);
  toggleShowRulers(true);

  updateButton();

  m_setFontFamily->setFont( doc()->defaultFont().family() );
  m_setFontSize->setFontSize( doc()->defaultFont().pointSize() );
  m_setBold->setChecked( false );
  m_setItalics->setChecked( false );
  m_setUnderline->setChecked( false );
  m_lineWidthAction->setCurrentWidth(1.0);
  m_lineStyleAction->setCurrentSelection(Qt::SolidLine);
  showAlign(Qt::AlignHCenter);
  showVAlign(Qt::AlignVCenter);

  m_pStencilGeometryPanel->setSize(0.0,0.0);
  m_pStencilGeometryPanel->setPosition(0.0,0.0);
  m_pStencilGeometryPanel->setRotation(0);

  m_setArrowHeads->setCurrentStartArrow(0);
  m_setArrowHeads->setCurrentEndArrow(0);

  m_menuTextFormatAction->setEnabled( false );
  m_menuStencilConnectorsAction->setEnabled( false );

  m_setFGColor->setCurrentColor(QColor(0, 0, 0));
  m_setBGColor->setCurrentColor(QColor(255, 255, 255));
  m_setTextColor->setCurrentColor(QColor(0, 0, 0));

  viewZoom(zoomHandler()->zoom());
}

void KivioView::updateReadWrite( bool readwrite )
{
  QValueList<KAction*> actions = actionCollection()->actions();
  QValueList<KAction*>::ConstIterator aIt = actions.begin();
  QValueList<KAction*>::ConstIterator aEnd = actions.end();
  for (; aIt != aEnd; ++aIt )
    (*aIt)->setEnabled( readwrite );
  if ( !readwrite )
  {
      showPageMargins->setEnabled( true );
      showRulers->setEnabled( true );
      showGrid->setEnabled( true );
      showGuides->setEnabled( true );
      m_selectAll->setEnabled( true );
      m_selectNone->setEnabled( true );
      m_editCopy->setEnabled( true );
  }
  m_showPage->setEnabled( true );
  m_hidePage->setEnabled( true );
  updateMenuPage();
}


void KivioView::addPage( KivioPage* page )
{
  insertPage(  page );

  QObject::connect( page, SIGNAL( sig_PageHidden( KivioPage* ) ),
                    this, SLOT( slotPageHidden( KivioPage* ) ) );
  QObject::connect( page, SIGNAL( sig_PageShown( KivioPage* ) ),
                    this, SLOT( slotPageShown( KivioPage* ) ) );

}

void KivioView::insertPage( KivioPage* page )
{
    if( !page->isHidden() ) {
    m_pTabBar->addTab(page->pageName());
    setActivePage(page);
  updateMenuPage();
  }
}

void KivioView::removePage( KivioPage *_t )
{
  QString m_pageName=_t->pageName();
  m_pTabBar->removeTab( _t->pageName() );
  QString n = m_pDoc->map()->visiblePages().first();
  setActivePage( m_pDoc->map()->findPage( n ) );
}

void KivioView::renamePage()
{
    bool ok;
    QString activeName = m_pActivePage->pageName();
    QString newName = KInputDialog::getText( i18n("Rename Page"),
        i18n("Enter page name:"), activeName, &ok, this );

    // Have a different name ?
    if ( ok ) // User pushed an OK button.
    {
        if ( (newName.stripWhiteSpace()).isEmpty() ) // Page name is empty.
        {
            KNotifyClient::beep();
            KMessageBox::information( this, i18n("Page name cannot be empty."),
                i18n("Change Page Name") );
            // Recursion
            renamePage();
        }
        else if ( newName != activeName ) // Page name changed.
        {
             // Is the name already used
             if ( !m_pActivePage->setPageName( newName ) )
             {
                KNotifyClient::beep();
                KMessageBox::information( this, i18n("This name is already used."),
                    i18n("Change Page Name") );
                // Recursion
                renamePage();
             }
             KivioChangePageNameCommand *cmd = new KivioChangePageNameCommand(i18n("Rename Page"), activeName, newName, m_pActivePage);
             m_pDoc->addCommand( cmd );
        }
    }
}

void KivioView::setActivePage( KivioPage* page )
{
  if ( page == m_pActivePage )
    return;

  if(m_pActivePage) {
    disconnect(m_pActivePage, SIGNAL(sig_pageLayoutChanged(const KoPageLayout&)), this,
      SLOT(setRulerPageLayout(const KoPageLayout&)));
  }

  m_pActivePage = page;

  m_pTabBar->setActiveTab(page->pageName());

  updateToolBars();

  m_pLayersPanel->reset();

  m_pDoc->updateView(m_pActivePage);
  setRulerPageLayout(m_pActivePage->paperLayout());
  connect(m_pActivePage, SIGNAL(sig_pageLayoutChanged(const KoPageLayout&)),
    SLOT(setRulerPageLayout(const KoPageLayout&)));
}

void KivioView::setActiveSpawnerSet( KivioStencilSpawnerSet *set )
{
    if( set == m_pActiveSpawnerSet )
        return;

    m_pActiveSpawnerSet = set;
}

void KivioView::slotPageRenamed( KivioPage* page, const QString& old_name )
{
  m_pTabBar->renameTab( old_name, page->pageName() );
}

void KivioView::changePage( const QString& name )
{
  if ( m_pActivePage->pageName() == name )
    return;

  KivioPage *t = m_pDoc->map()->findPage(name);
  if (!t)
  	return;

  setActivePage(t);
}

void KivioView::insertPage()
{
    KivioPage * t =m_pDoc->createPage();
    m_pDoc->addPage(t);
    KivioAddPageCommand * cmd = new KivioAddPageCommand(i18n("Insert Page"), t);
    m_pDoc->addCommand( cmd );
}

void KivioView::hidePage()
{
  if (!m_pActivePage)
    return;

  if ( m_pDoc->map()->visiblePages().count() ==  1)
  {
      KMessageBox::error( this, i18n("You cannot hide the last visible page.") );
      return;
  }

  m_pActivePage->setHidden(true);
  QString activeName = m_pActivePage->pageName();

  m_pTabBar->removeTab( activeName );

  KivioHidePageCommand * cmd = new KivioHidePageCommand(i18n("Hide Page"), m_pActivePage);
  m_pDoc->addCommand( cmd );

  changePage( m_pDoc->map()->visiblePages().first() );
  updateMenuPage();
}

void KivioView::showPage()
{
  KivioPageShow* dlg = new KivioPageShow(this,"Page show");
  if ( !dlg->exec() )
  {
      delete dlg;
      return;
  }
  m_pTabBar->setTabs( m_pDoc->map()->visiblePages() );
  updateMenuPage();
  delete dlg;
}

int KivioView::leftBorder() const
{
  return 0;
}

int KivioView::rightBorder() const
{
  return 0;
}

int KivioView::topBorder() const
{
  return 0;
}

int KivioView::bottomBorder() const
{
  return 0;
}

void KivioView::paperLayoutDlg()
{
  KivioPage* page = activePage();
  KoPageLayout l = page->paperLayout();
  KoHeadFoot headfoot;
  int tabs = FORMAT_AND_BORDERS | DISABLE_UNIT;
  KoUnit::Unit unit = doc()->units();

  if(KoPageLayoutDia::pageLayout(l, headfoot, tabs, unit))
  {
    KivioDoc* doc = page->doc();
    KivioChangeLayoutCommand * cmd = new KivioChangeLayoutCommand(
      i18n("Change Page Layout"),page ,page->paperLayout(), l);
    doc->addCommand( cmd );
    page->setPaperLayout(l);
  }
}

void KivioView::removePage()
{
  if ( doc()->map()->count() <= 1 || doc()->map()->visiblePages().count()<=1 ) {
    QApplication::beep();
    KMessageBox::sorry( this, i18n("You cannot delete the only page of the document."), i18n("Remove Page") );
    return;
  }
  QApplication::beep();
  int ret = KMessageBox::warningYesNo(this,i18n("You are going to remove the active page.\nDo you want to continue?"),i18n("Remove Page"));

  if ( ret == 3 ) {
      KivioPage* tbl = m_pActivePage;
      KivioRemovePageCommand *cmd = new KivioRemovePageCommand(i18n("Remove Page"), tbl);
      cmd->execute();
      doc()->addCommand( cmd );
  }
}

void KivioView::slotAddPage( KivioPage* page )
{
  addPage(page);
}

void KivioView::slotUpdateView( KivioPage* page )
{
  if (page && page != m_pActivePage)
    return;

  if (!page) {
    // global view updates (toolbar, statusbar.... actions...)
    updateToolBars();
  }

  m_pCanvas->update();
  m_pCanvas->updateScrollBars();
  vRuler->update();
  hRuler->update();
}

void KivioView::paintContent( KivioPainter&, const QRect&, bool)
{
//  m_pDoc->paintContent( painter, rect, transparent, m_pActivePage );
//  temporary
  m_pCanvas->update();
}

QWidget *KivioView::canvas()
{
  return canvasWidget();
}

int KivioView::canvasXOffset() const
{
  return canvasWidget()->xOffset();
}

int KivioView::canvasYOffset() const
{
  return canvasWidget()->yOffset();
}

void KivioView::print(KPrinter& ptr)
{
  ptr.setFullPage(TRUE);
  m_pDoc->printContent( ptr );
}


void KivioView::viewZoom(int zoom)
{
  if(zoom < 10 || zoom > 2000 || zoom == zoomHandler()->zoom()) {
    return;
  }

  zoomHandler()->setZoomAndResolution(zoom, KoGlobal::dpiX(),
    KoGlobal::dpiY());
  m_pCanvas->update();
  m_pCanvas->updateScrollBars();
  vRuler->setZoom(zoomHandler()->zoomedResolutionY());
  hRuler->setZoom(zoomHandler()->zoomedResolutionX());
  KoPageLayout l = activePage()->paperLayout();
  vRuler->setFrameStartEnd(zoomHandler()->zoomItY(l.ptTop), zoomHandler()->zoomItY(l.ptHeight - l.ptBottom));
  hRuler->setFrameStartEnd(zoomHandler()->zoomItX(l.ptLeft), zoomHandler()->zoomItX(l.ptWidth - l.ptRight));
  KoView::setZoom(zoomHandler()->zoomedResolutionY());

  emit zoomChanged(zoom);
}

KivioPage* KivioView::activePage() const
{
  return m_pActivePage;
}

void KivioView::togglePageMargins(bool b)
{
  TOGGLE_ACTION("showPageMargins")->setChecked(b);
  m_bShowPageMargins = b;

  m_pCanvas->update();
}

void KivioView::toggleShowRulers(bool b)
{
  TOGGLE_ACTION("showRulers")->setChecked(b);
  m_bShowRulers = b;

  if(b) {
    hRuler->show();
    vRuler->show();
  } else {
    hRuler->hide();
    vRuler->hide();
  }
}

void KivioView::toggleShowGuides(bool b)
{
  TOGGLE_ACTION("showGuides")->setChecked(b);
  m_bShowGuides = b;

  m_pCanvas->update();
}

void KivioView::toggleSnapGuides(bool b)
{
  TOGGLE_ACTION("snapGuides")->setChecked(b);
  m_bSnapGuides = b;
}

void KivioView::toggleShowGrid(bool b)
{
  TOGGLE_ACTION("showGrid")->setChecked(b);

  Kivio::Config::setShowGrid(b);
  m_pDoc->setModified( true );
}

void KivioView::slotUpdateGrid()
{
  m_pCanvas->update();
}

void KivioView::toggleSnapGrid(bool b)
{
  TOGGLE_ACTION("snapGrid")->setChecked(b);

  Kivio::Config::setSnapGrid(b);
  m_pDoc->setModified( true );
}

void KivioView::addStencilSet( const QString& name )
{
  m_pDoc->addSpawnerSet(name);
}

void KivioView::addSpawnerToStackBar( KivioStencilSpawnerSet *pSpawner )
{
  if( !pSpawner )
  {
    kdDebug(43000) << "KivioView::addSpawnerToStackBar() - NULL pSpawner" << endl;
    return;
  }

  KivioIconView *pView = new KivioIconView(m_pDoc->isReadWrite()  );
  QObject::connect( pView, SIGNAL(createNewStencil(KivioStencilSpawner*)), this, SLOT(addStencilFromSpawner(KivioStencilSpawner*)));

  pView->setStencilSpawnerSet( pSpawner );

  m_pStencilBarDockManager->insertStencilSet(pView, pSpawner->name());
}

void KivioView::setFGColor()
{
  KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();
  if (!pStencil)
    return;
  KMacroCommand * macro = new KMacroCommand( i18n("Change Foreground Color"));
  bool createMacro = false;
  while( pStencil )
  {
    QColor col( m_setFGColor->color());

    if ( col != pStencil->fgColor() )
    {
      KivioChangeStencilColorCommand * cmd = new KivioChangeStencilColorCommand( i18n("Change Fg Color"), m_pActivePage, pStencil, pStencil->fgColor(), col, KivioChangeStencilColorCommand::CT_FGCOLOR);

      pStencil->setFGColor( col );
      macro->addCommand( cmd );
      createMacro = true;
    }

    pStencil = m_pActivePage->selectedStencils()->next();
  }

  if ( createMacro )
    m_pDoc->addCommand( macro );
  else
    delete macro;

  m_pDoc->updateView(m_pActivePage);
}

void KivioView::setBGColor()
{
  KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();
  if (!pStencil)
    return;
  KMacroCommand * macro = new KMacroCommand( i18n("Change Background Color"));
  bool createMacro = false;

  while( pStencil )
  {
    QColor col( m_setBGColor->color());

    if ( col != pStencil->bgColor() )
    {
      KivioChangeStencilColorCommand * cmd = new KivioChangeStencilColorCommand( i18n("Change Bg Color"), m_pActivePage, pStencil, pStencil->bgColor(), col, KivioChangeStencilColorCommand::CT_BGCOLOR);

      pStencil->setBGColor( col );
      macro->addCommand( cmd );
      createMacro = true;
    }

    pStencil = m_pActivePage->selectedStencils()->next();
  }

  if ( createMacro )
    m_pDoc->addCommand( macro );
  else
    delete macro;

  m_pDoc->updateView(m_pActivePage);
}

void KivioView::setTextColor()
{
  KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();

  if (!pStencil)
    return;

  KMacroCommand * macro = new KMacroCommand( i18n("Change Text Color"));
  bool createMacro = false;
  while( pStencil )
  {
    QColor col(m_setTextColor->color());

    if ( col != pStencil->textColor() )
    {
      KivioChangeStencilColorCommand * cmd = new KivioChangeStencilColorCommand( i18n("Change Text Color"), m_pActivePage, pStencil, pStencil->textColor(), col, KivioChangeStencilColorCommand::CT_TEXTCOLOR);
      pStencil->setTextColor( col );
      macro->addCommand( cmd );
      createMacro = true;
    }

    pStencil = m_pActivePage->selectedStencils()->next();
  }

  if ( createMacro )
    m_pDoc->addCommand( macro );
  else
    delete macro;

  m_pDoc->updateView(m_pActivePage);
}

void KivioView::setLineWidth(double width)
{
  KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();

  if (!pStencil)
    return;

  KMacroCommand * macro = new KMacroCommand( i18n("Change Line Width") );
  bool createMacro = false ;

  while( pStencil )
  {
    if ( width != pStencil->lineWidth() )
    {
      KivioChangeLineWidthCommand * cmd = new KivioChangeLineWidthCommand( i18n("Change Line Width"),
        m_pActivePage, pStencil, pStencil->lineWidth(), width );

      pStencil->setLineWidth( width );
      macro->addCommand( cmd );
      createMacro = true;
    }

    pStencil = m_pActivePage->selectedStencils()->next();
  }

  if ( createMacro ) {
    m_pDoc->addCommand( macro );
  } else {
    delete macro;
  }

  m_pDoc->updateView(m_pActivePage);
}

void KivioView::setLineStyle(int style)
{
  KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();

  if(!pStencil)
    return;

  KMacroCommand * macro = new KMacroCommand( i18n("Change Line Style") );
  bool createMacro = false ;

  while(pStencil)
  {
    if(style != pStencil->linePattern())
    {
        KivioChangeLineStyleCommand * cmd = new KivioChangeLineStyleCommand( i18n("Change Line Style"),
        m_pActivePage, pStencil, pStencil->linePattern(), style );
        pStencil->setLinePattern(style);
        macro->addCommand( cmd );
        createMacro = true;
    }

    pStencil = m_pActivePage->selectedStencils()->next();
  }

    if ( createMacro ) {
    m_pDoc->addCommand( macro );
  } else {
    delete macro;
  }
}

void KivioView::groupStencils()
{
    m_pActivePage->groupSelectedStencils();
    KoRect r = m_pActivePage->getRectForAllStencils();

    m_pDoc->updateView(m_pActivePage);
}

void KivioView::ungroupStencils()
{
    m_pActivePage->ungroupSelectedStencils();
    m_pDoc->updateView(m_pActivePage);
}

void KivioView::selectAllStencils()
{
    m_pActivePage->selectAllStencils();
    m_pCanvas->repaint();
}

void KivioView::unselectAllStencils()
{
    m_pActivePage->unselectAllStencils();
    m_pCanvas->repaint();
}

QColor KivioView::fgColor() const
{
    return m_setFGColor->color();
}

QColor KivioView::bgColor() const
{
    return m_setBGColor->color();
}

double KivioView::lineWidth() const
{
    return m_lineWidthAction->currentWidth();
}

int KivioView::lineStyle() const
{
  return m_lineStyleAction->currentSelection();
}


void KivioView::setFontFamily( const QString &str )
{
    KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();
    if (!pStencil)
      return;

    QFont f;
    KMacroCommand * macro = 0L;
    while( pStencil )
    {
        f = pStencil->textFont();
        f.setFamily( str );
        if ( pStencil->textFont() != f )
        {
            if ( !macro )
                macro = new KMacroCommand( i18n("Change Stencil Font"));

            KivioChangeStencilFontCommand *cmd = new KivioChangeStencilFontCommand( i18n("Change Stencil Font"), m_pActivePage, pStencil,pStencil->textFont(),  f);
            pStencil->setTextFont( f );

            macro->addCommand( cmd );
        }
        pStencil = m_pActivePage->selectedStencils()->next();

    }
    if ( macro )
        m_pDoc->addCommand( macro  );
    m_pDoc->updateView(m_pActivePage);
}

void KivioView::setFontSize(int size )
{
    KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();
    if (!pStencil)
      return;

    QFont f;
    KMacroCommand * macro = 0L;
    while( pStencil )
    {
        f = pStencil->textFont();
        f.setPointSize( size );
        if ( pStencil->textFont() != f )
        {
            if ( !macro )
                macro = new KMacroCommand( i18n("Change Stencil Font"));

            KivioChangeStencilFontCommand *cmd = new KivioChangeStencilFontCommand( i18n("Change Stencil Font"), m_pActivePage, pStencil,pStencil->textFont(),  f);

            pStencil->setTextFont( f );
            macro->addCommand( cmd );
        }
        pStencil = m_pActivePage->selectedStencils()->next();

    }
    if ( macro )
        m_pDoc->addCommand( macro   );
    m_pDoc->updateView(m_pActivePage);
}

void KivioView::toggleFontBold(bool b)
{
    KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();
    if (!pStencil)
      return;

    QFont f;
    KMacroCommand * macro = 0L;
    while( pStencil )
    {
        f = pStencil->textFont();
        f.setBold(b);
        if ( pStencil->textFont() != f )
        {
            if ( !macro )
                macro = new KMacroCommand( i18n("Change Stencil Font"));
            KivioChangeStencilFontCommand *cmd = new KivioChangeStencilFontCommand( i18n("Change Stencil Font"), m_pActivePage, pStencil,pStencil->textFont(),  f);

            pStencil->setTextFont( f );
            macro->addCommand( cmd );
        }
        pStencil = m_pActivePage->selectedStencils()->next();

    }
    if ( macro )
        m_pDoc->addCommand( macro );
    m_pDoc->updateView(m_pActivePage);
}

void KivioView::toggleFontItalics(bool b)
{
    KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();
    if (!pStencil)
        return;

    QFont f;
    KMacroCommand * macro = new KMacroCommand( i18n("Change Stencil Font"));
    while( pStencil )
    {
        f = pStencil->textFont();
        f.setItalic(b);
        if ( pStencil->textFont() != f )
        {
            if ( !macro )
                macro = new KMacroCommand( i18n("Change Stencil Font"));

            KivioChangeStencilFontCommand *cmd = new KivioChangeStencilFontCommand( i18n("Change Stencil Font"), m_pActivePage, pStencil,pStencil->textFont(),  f);

            pStencil->setTextFont( f );

            macro->addCommand( cmd );
        }
        pStencil = m_pActivePage->selectedStencils()->next();
    }
    if ( macro )
        m_pDoc->addCommand( macro );
    m_pDoc->updateView(m_pActivePage);
}

void KivioView::toggleFontUnderline( bool b)
{
    KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();
    if (!pStencil)
      return;
    KMacroCommand * macro = 0L;
    QFont f;
    while( pStencil )
    {
        f = pStencil->textFont();
        f.setUnderline(b);
        if ( pStencil->textFont() != f )
        {
            if ( !macro )
                macro = new KMacroCommand( i18n("Change Stencil Font"));

            KivioChangeStencilFontCommand *cmd = new KivioChangeStencilFontCommand( i18n("Change Stencil Font"), m_pActivePage, pStencil,pStencil->textFont(),  f);

            pStencil->setTextFont( f );

            macro->addCommand( cmd );
        }
        pStencil = m_pActivePage->selectedStencils()->next();
    }
    if ( macro )
        m_pDoc->addCommand( macro );
    m_pDoc->updateView(m_pActivePage);
}



void KivioView::updateToolBars()
{
    KivioStencil *pStencil;
    pStencil = m_pActivePage->selectedStencils()->first();
    m_pStencilGeometryPanel->setEmitSignals(false);
    m_setArrowHeads->setEmitSignals(false);

    if( !pStencil )
    {
        m_setFontFamily->setFont( doc()->defaultFont().family() );
        m_setFontSize->setFontSize( doc()->defaultFont().pointSize() );
        m_setBold->setChecked( false );
        m_setItalics->setChecked( false );
        m_setUnderline->setChecked( false );
        m_lineWidthAction->setCurrentWidth(1.0);
        m_lineStyleAction->setCurrentSelection(Qt::SolidLine);
        showAlign(Qt::AlignHCenter);
        showVAlign(Qt::AlignVCenter);

        m_pStencilGeometryPanel->setSize(0.0,0.0);
        m_pStencilGeometryPanel->setPosition(0.0,0.0);
        m_pStencilGeometryPanel->setRotation(0);

        m_setArrowHeads->setCurrentStartArrow(0);
        m_setArrowHeads->setCurrentEndArrow(0);

        m_menuTextFormatAction->setEnabled( false );
        m_menuStencilConnectorsAction->setEnabled( false );
    }
    else
    {
        QFont f = pStencil->textFont();

        m_setFontFamily->setFont( f.family() );
        m_setFontSize->setFontSize( f.pointSize() );
        m_setBold->setChecked( f.bold() );
        m_setItalics->setChecked( f.italic() );
        m_setUnderline->setChecked( f.underline() );

        m_lineWidthAction->setCurrentWidth(pStencil->lineWidth());
        m_lineStyleAction->setCurrentSelection(pStencil->linePattern());

        showAlign(pStencil->hTextAlign());
        showVAlign(pStencil->vTextAlign());

        m_pStencilGeometryPanel->setSize( pStencil->w(), pStencil->h() );
        m_pStencilGeometryPanel->setPosition( pStencil->x(), pStencil->y() );
        m_pStencilGeometryPanel->setRotation(pStencil->rotation());

        m_menuTextFormatAction->setEnabled( true );
        m_menuStencilConnectorsAction->setEnabled( true );

        if ( pStencil->type() != kstConnector )
        {
           m_setArrowHeads->setEnabled (false);
           m_arrowHeadsMenuAction->setEnabled (false);
        }
        else
        {
            m_setArrowHeads->setEnabled (true);
            m_arrowHeadsMenuAction->setEnabled (true);
            m_setArrowHeads->setCurrentStartArrow( pStencil->startAHType() );
            m_setArrowHeads->setCurrentEndArrow( pStencil->endAHType() );
        }

        if ( pStencil->type() != kstText )
        {
            m_setFGColor->setEnabled (true);
            m_setBGColor->setEnabled (true);
        }
        else
        {
            m_setFGColor->setEnabled (false);
            m_setBGColor->setEnabled (false);
        }
    }

    m_pStencilGeometryPanel->setEmitSignals(true);
    m_setArrowHeads->setEmitSignals(true);
    m_pProtectionPanel->updateCheckBoxes();

    if(activePage()->selectedStencils()->count() > 1) {
      m_groupAction->setEnabled(true);
      m_alignAndDistribute->setEnabled(true);
    } else {
      m_groupAction->setEnabled(false);
      m_alignAndDistribute->setEnabled(false);
    }

    if(activePage()->selectedStencils()->count() > 0) {
      m_editCut->setEnabled(true);
      m_editCopy->setEnabled(true);

      if(activePage()->checkForStencilTypeInSelection(kstGroup)) {
        m_ungroupAction->setEnabled(true);
      } else {
        m_ungroupAction->setEnabled(false);
      }

      m_stencilToBack->setEnabled(true);
      m_stencilToFront->setEnabled(true);
    } else {
      m_editCut->setEnabled(false);
      m_editCopy->setEnabled(false);
      m_ungroupAction->setEnabled(false);
      m_stencilToBack->setEnabled(false);
      m_stencilToFront->setEnabled(false);
      m_setArrowHeads->setEnabled (false);
      m_arrowHeadsMenuAction->setEnabled (false);
    }
}

void KivioView::slotSetStartArrow( int i )
{
    KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();
    if (!pStencil)
      return;
    KMacroCommand *macro = new KMacroCommand( i18n("Change Begin Arrow"));
    bool createMacro = false;
    while( pStencil )
    {
        if (pStencil->startAHType()!=i)
        {
            pStencil->setStartAHType(i);
            KivioChangeBeginEndArrowCommand *cmd=new KivioChangeBeginEndArrowCommand( i18n("Change Arrow"),
              m_pActivePage, pStencil,  pStencil->startAHType(),  i, true);
            pStencil->setStartAHType(i);

            macro->addCommand( cmd );
            createMacro= true;
        }
        pStencil = m_pActivePage->selectedStencils()->next();
    }
    if (createMacro )
        m_pDoc->addCommand( macro );
    else
        delete macro;
    m_pDoc->updateView(m_pActivePage);
}

void KivioView::slotSetEndArrow( int i )
{
    KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();
    if (!pStencil)
      return;
    KMacroCommand *macro = new KMacroCommand( i18n("Change End Arrow"));
    bool createMacro = false;

    while( pStencil )
    {
        if (pStencil->endAHType()!=i)
        {
            KivioChangeBeginEndArrowCommand *cmd=new KivioChangeBeginEndArrowCommand( i18n("Change Arrow"),
              m_pActivePage, pStencil, pStencil->endAHType(),  i, false);
            pStencil->setEndAHType(i);

            macro->addCommand( cmd );
            createMacro= true;
        }
        pStencil = m_pActivePage->selectedStencils()->next();
    }
    if (createMacro )
        m_pDoc->addCommand( macro );
    else
        delete macro;

    m_pDoc->updateView(m_pActivePage);
}

void KivioView::slotSetStartArrowSize()
{
    KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();
    if (!pStencil)
      return;

    float w,h;
    KMacroCommand *macro = new KMacroCommand( i18n("Change Size of Begin Arrow"));
    bool createMacro = false;
    while( pStencil )
    {
        if (pStencil->startAHLength() != h || pStencil->startAHWidth()!=w)
        {
            KivioChangeBeginEndSizeArrowCommand * cmd = new KivioChangeBeginEndSizeArrowCommand( i18n("Change Size of End Arrow"), m_pActivePage, pStencil, pStencil->startAHLength(),pStencil->startAHWidth(), h,w, true);

            pStencil->setStartAHWidth(w);
            pStencil->setStartAHLength(h);
            macro->addCommand( cmd );
            createMacro= true;
        }
        pStencil = m_pActivePage->selectedStencils()->next();
    }
    if (createMacro )
        m_pDoc->addCommand( macro );
    else
        delete macro;
    m_pDoc->updateView(m_pActivePage);
}

void KivioView::slotSetEndArrowSize()
{
    KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();
    if (!pStencil)
      return;

    float w,h;
    KMacroCommand *macro = new KMacroCommand( i18n("Change Size of End Arrow"));
    bool createMacro = false;
    while( pStencil )
    {
        if ( pStencil->endAHLength() != h || pStencil->endAHWidth()!=w)
        {
            KivioChangeBeginEndSizeArrowCommand * cmd = new KivioChangeBeginEndSizeArrowCommand( i18n("Change Size of End Arrow"), m_pActivePage, pStencil, pStencil->endAHLength(),pStencil->endAHWidth(), h,w, false);
            pStencil->setEndAHWidth(w);
            pStencil->setEndAHLength(h);
            macro->addCommand( cmd );
            createMacro= true;
        }
        pStencil = m_pActivePage->selectedStencils()->next();
    }
    if ( createMacro )
        m_pDoc->addCommand( macro );
    else
        delete macro;
    m_pDoc->updateView(m_pActivePage);
}

void KivioView::setHParaAlign( int i )
{
    KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();
    if (!pStencil)
      return;
    KMacroCommand *macro = new KMacroCommand( i18n("Change Stencil Horizontal Alignment"));
    bool createMacro = false;
    while( pStencil )
    {
        if ( pStencil->hTextAlign() != i)
        {
            KivioChangeStencilHAlignmentCommand * cmd = new KivioChangeStencilHAlignmentCommand( i18n("Change Stencil Horizontal Alignment"), m_pActivePage, pStencil, pStencil->hTextAlign(), i);

            pStencil->setHTextAlign(i);
            macro->addCommand( cmd );
            createMacro = true;
        }
        pStencil = m_pActivePage->selectedStencils()->next();

    }
    if (createMacro )
        m_pDoc->addCommand( macro );
    else
        delete macro;
    m_pDoc->updateView(m_pActivePage);
}


void KivioView::setVParaAlign( int i )
{
    KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();
    if (!pStencil)
      return;
    KMacroCommand *macro = new KMacroCommand( i18n("Change Stencil Vertical Alignment"));
    bool createMacro = false;
    while( pStencil )
    {
        if ( pStencil->vTextAlign() != i )
        {
            KivioChangeStencilVAlignmentCommand * cmd = new KivioChangeStencilVAlignmentCommand( i18n("Change Stencil Vertical Alignment"), m_pActivePage, pStencil, pStencil->vTextAlign(), i);
            pStencil->setVTextAlign( i );
            macro->addCommand( cmd );
            createMacro = true;
        }
        pStencil =  m_pActivePage->selectedStencils()->next();

    }
    if ( createMacro )
        m_pDoc->addCommand( macro );
    else
        delete macro;
    m_pDoc->updateView(m_pActivePage);
}

void KivioView::bringStencilToFront()
{
    m_pActivePage->bringToFront();
    m_pDoc->updateView(m_pActivePage);
}

void KivioView::sendStencilToBack()
{
    m_pActivePage->sendToBack();
    m_pDoc->updateView(m_pActivePage);
}

void KivioView::cutStencil()
{
    m_pActivePage->cut();
    m_pDoc->updateView(m_pActivePage);
}

void KivioView::copyStencil()
{
    m_pActivePage->copy();
}

void KivioView::pasteStencil()
{
    m_pActivePage->paste(this);
    m_pDoc->updateView(m_pActivePage);
}

void KivioView::slotChangeStencilSize(double newW, double newH)
{
  KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();

  if ( pStencil )
  {
    KoRect oldPos(pStencil->rect());
    pStencil->setDimensions(newW, newH);
    if ((oldPos.width() != pStencil->rect().width()) || (oldPos.height() != pStencil->rect().height()))
    {
      KivioMoveStencilCommand * cmd = new KivioMoveStencilCommand( i18n("Resize Stencil"), pStencil, oldPos , pStencil->rect(), m_pCanvas->activePage());
      m_pDoc->updateView(m_pActivePage);
      m_pDoc->addCommand( cmd );
    }
  }
}

void KivioView::slotChangeStencilPosition(double newW, double newH)
{
  KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();

  if ( pStencil )
  {
    KoRect oldPos(pStencil->rect());
    pStencil->setPosition(newW, newH);
    if ((oldPos.x() != pStencil->rect().x()) || (oldPos.y() != pStencil->rect().y()))
    {
      KivioMoveStencilCommand * cmd = new KivioMoveStencilCommand( i18n("Move Stencil"), pStencil, oldPos , pStencil->rect(), m_pCanvas->activePage());
      m_pDoc->updateView(m_pActivePage);
      m_pDoc->addCommand( cmd );
    }
  }
}

void KivioView::slotChangeStencilRotation(int d)
{
  KivioStencil *pStencil = m_pActivePage->selectedStencils()->first();

  if(pStencil && pStencil->rotation() != d) {
      pStencil->setRotation(d);
      KivioChangeRotationCommand * cmd = new KivioChangeRotationCommand( i18n("Change Stencil Rotation"), m_pCanvas->activePage(), pStencil, pStencil->rotation() , d);
      m_pDoc->updateView(m_pActivePage);
      m_pDoc->addCommand( cmd );
  }
}

/**
 * When passed a spawner, this will create a new stencil at x, y.
*/
void KivioView::addStencilFromSpawner( KivioStencilSpawner *pSpawner, double x, double y )
{
    KivioStencil *pStencil;

    // Allocate the new stencil and set it's default size/style
    pStencil = pSpawner->newStencil();

    pStencil->setPosition( x, y );

    // Only set these properties if we held ctrl down
    if(KApplication::keyboardModifiers() == KApplication::ControlModifier) {
      pStencil->setFGColor(m_setFGColor->color());
      pStencil->setBGColor(m_setBGColor->color());
      QFont f = m_setFontFamily->font();
      f.setPointSize(m_setFontSize->fontSize());
      f.setBold(m_setBold->isChecked());
      f.setItalic(m_setItalics->isChecked());
      f.setUnderline(m_setUnderline->isChecked());
      pStencil->setTextFont(f);
      pStencil->setTextColor(m_setTextColor->color());
      pStencil->setVTextAlign(vTextAlign());
      pStencil->setHTextAlign(hTextAlign());
      pStencil->setLinePattern(m_lineStyleAction->currentSelection());
      pStencil->setLineWidth(m_lineWidthAction->currentWidth());
    } else {
      pStencil->setTextFont(doc()->defaultFont());
    }

    // Unselect everything, then the stencil to the page, and select it
    m_pActivePage->unselectAllStencils();
    m_pActivePage->addStencil( pStencil );
    m_pActivePage->selectStencil( pStencil );

    // Mark the page as modified and repaint
    m_pDoc->updateView(m_pActivePage);
    pluginManager()->activateDefaultTool();
}

void KivioView::alignStencilsDlg()
{
  KivioAlignDialog* dlg = new KivioAlignDialog(this, "AlignDialog");

  if( dlg->exec() == QDialog::Accepted )
  {
    m_pActivePage->alignStencils(dlg->align());
    m_pActivePage->distributeStencils(dlg->distribute());

    m_pCanvas->repaint();
  }

  delete dlg;
}

void KivioView::optionsDialog()
{
  KivioOptionsDialog dlg(this, "setupDialog");
  dlg.exec();
}

void KivioView::toggleStencilGeometry(bool b)
{
    TOGGLE_ACTION("stencilGeometry")->setChecked(b);
}

void KivioView::toggleViewManager(bool b)
{
    TOGGLE_ACTION("viewManager")->setChecked(b);
}

void KivioView::toggleLayersPanel(bool b)
{
    TOGGLE_ACTION("layersPanel")->setChecked(b);
}

void KivioView::toggleProtectionPanel(bool b)
{
    TOGGLE_ACTION("protection")->setChecked(b);
}

void KivioView::toggleBirdEyePanel(bool b)
{
    TOGGLE_ACTION("birdEye")->setChecked(b);
}

void KivioView::toggleAddStencilSetPanel(bool b)
{
    TOGGLE_ACTION("addStencilSetDock")->setChecked(b);
}

void KivioView::setupPrinter(KPrinter &p)
{
  p.setMinMax(1, m_pDoc->map()->pageList().count());
  KoPageLayout pl = activePage()->paperLayout();
  p.setPageSize( static_cast<KPrinter::PageSize>( KoPageFormat::printerPageSize( pl.format ) ) );

  if ( pl.orientation == PG_LANDSCAPE || pl.format == PG_SCREEN ) {
    p.setOrientation( KPrinter::Landscape );
  } else {
    p.setOrientation( KPrinter::Portrait );
  }
}

void KivioView::exportPage()
{
   // First build a filter list
   QString extList;
   char *pStr;
   QStrList strList;
   ExportPageDialog dlg(this, "Export Page Dialog");

   strList = QImageIO::outputFormats();
   QString last = Kivio::Config::lastFormat();

   if(strList.remove(last.lower().local8Bit()) || strList.remove(last.upper().local8Bit())) {
     strList.insert(0, last.lower().local8Bit());
   }

   pStr = static_cast<char*>(strList.first());
   while( pStr )
   {
      extList += "*." + QString(pStr).lower();

      pStr = static_cast<char*>(strList.next());

      if(pStr) {
        extList += "\n";
      }
   }

   KFileDialog fileDlg(":pageExport", extList, this, "exportPageFileDlg", true);
   fileDlg.setOperationMode(KFileDialog::Saving);
   fileDlg.setMode(KFile::File);
   QString fileName;

   if(fileDlg.exec() == QDialog::Accepted) {
     fileName = fileDlg.selectedFile();
   } else {
     return;
   }

   if( dlg.exec()!=QDialog::Accepted ) {
      return;
   }

   if(!m_pDoc->exportPage( m_pActivePage, fileName, &dlg ))
   {
      kdDebug(43000) << "KivioView::exportPage() failed\n";
      return;
   }

   kdDebug(43000) << "KivioView::exportPage() succeeded\n";
   Kivio::Config::setLastFormat(fileDlg.currentFilter().remove(0, 2));
}

void KivioView::popupTabBarMenu( const QPoint & _point )
{
    if(!koDocument()->isReadWrite() || !factory())
        return;
     static_cast<QPopupMenu*>(factory()->container("menupage_popup",this))->popup(_point);
}

void KivioView::updateMenuPage()
{
    bool state = (doc()->map()->count() > 1 && doc()->map()->visiblePages().count()>1);
    m_removePage->setEnabled(state);
    m_hidePage->setEnabled( doc()->map()->visiblePages().count()>1 );
    m_showPage->setEnabled( doc()->map()->hiddenPages().count()>0 );
}

void KivioView::updateButton()
{
  toggleShowGrid(Kivio::Config::showGrid());
  toggleSnapGrid(Kivio::Config::snapGrid());

  toggleShowGuides(koDocument()->isReadWrite());
  toggleSnapGuides(koDocument()->isReadWrite());

}

void KivioView::slotPageHidden( KivioPage* page )
{
    //m_pTabBar->hidePage( page->pageName() );
}

void KivioView::slotPageShown( KivioPage* page )
{
    m_pTabBar->setTabs( m_pDoc->map()->visiblePages() );
}

void KivioView::resetLayerPanel()
{
    if ( m_pLayersPanel )
    {
        m_pLayersPanel->reset();
    }
}

void KivioView::updateProtectPanelCheckBox()
{
    if ( m_pProtectionPanel )
    {
        m_pProtectionPanel->updateCheckBoxes();
    }
}

void KivioView::setMousePos( int mx, int my )
{
  vRuler->setMousePos(mx, my);
  hRuler->setMousePos(mx, my);

  if((mx >= 0) && (my >= 0)) {
    QString unit = KoUnit::unitName(m_pDoc->units());
    KoPoint xy = m_pCanvas->mapFromScreen(QPoint(mx, my));
    xy.setX(KoUnit::toUserValue(xy.x(), m_pDoc->units()));
    xy.setY(KoUnit::toUserValue(xy.y(), m_pDoc->units()));
    QString text = i18n("X: %1 %3 Y: %2 %4").arg(KGlobal::_locale->formatNumber(xy.x(), 2))
      .arg(KGlobal::_locale->formatNumber(xy.y(), 2)).arg(unit).arg(unit);
    m_coordSLbl->setText(text);
  }
}

void KivioView::setRulerUnit(KoUnit::Unit u)
{
  vRuler->setUnit(u);
  hRuler->setUnit(u);
}

void KivioView::setRulerHOffset(int h)
{
  if(hRuler) {
    hRuler->setOffset(h, 0);
  }
}

void KivioView::setRulerVOffset(int v)
{
  if(vRuler) {
    vRuler->setOffset(0, v);
  }
}

void KivioView::rulerChangedUnit(KoUnit::Unit u)
{
  m_pDoc->setUnits(u);
}

KoZoomHandler* KivioView::zoomHandler() const
{
  return m_zoomHandler;
}

void KivioView::setRulerPageLayout(const KoPageLayout& l)
{
  vRuler->setPageLayout(l);
  hRuler->setPageLayout(l);
  vRuler->setFrameStartEnd(zoomHandler()->zoomItY(l.ptTop), zoomHandler()->zoomItY(l.ptHeight - l.ptBottom));
  hRuler->setFrameStartEnd(zoomHandler()->zoomItX(l.ptLeft), zoomHandler()->zoomItX(l.ptWidth - l.ptRight));
  m_pStencilGeometryPanel->setPageLayout(l);
}

void KivioView::viewZoom(const QString& s)
{
  QString z(s);
  z.remove('%');
  z.simplifyWhiteSpace();
  bool ok = false;
  int zoom = z.toInt(&ok);

  if(!ok || zoom < 10 || zoom > 2000) {
    zoom = zoomHandler()->zoom();
  }

  if(zoom != zoomHandler()->zoom()) {
    viewZoom(zoom);
  }
}

void KivioView::textAlignLeft()
{
  if ( m_textAlignLeft->isChecked() ) {
    setHParaAlign( Qt::AlignLeft );
  } else {
    m_textAlignLeft->setChecked( true );
  }
}

void KivioView::textAlignCenter()
{
  if ( m_textAlignCenter->isChecked() ) {
    setHParaAlign( Qt::AlignHCenter );
  } else {
    m_textAlignCenter->setChecked( true );
  }
}

void KivioView::textAlignRight()
{
  if ( m_textAlignRight->isChecked() ) {
    setHParaAlign( Qt::AlignRight );
  } else {
    m_textAlignRight->setChecked( true );
  }
}

void KivioView::textSuperScript()
{
  if ( m_textVAlignSuper->isChecked() ) {
    setVParaAlign( Qt::AlignTop );
  } else {
    if ( !m_textVAlignSub->isChecked() ) {
      setVParaAlign( Qt::AlignVCenter );
    }
  }
}

void KivioView::textSubScript()
{
  if ( m_textVAlignSub->isChecked() ) {
    setVParaAlign( Qt::AlignBottom );
  } else {
    if ( !m_textVAlignSuper->isChecked() ) {
      setVParaAlign( Qt::AlignVCenter );
    }
  }
}

void KivioView::showAlign( int align )
{
  switch ( align ) {
    case Qt::AlignAuto: // In left-to-right mode it's align left. TODO: alignright if text->isRightToLeft()
      kdWarning() << k_funcinfo << "shouldn't be called with AlignAuto" << endl;
      // fallthrough
    case Qt::AlignLeft:
      m_textAlignLeft->setChecked( true );
      break;
    case Qt::AlignHCenter:
      m_textAlignCenter->setChecked( true );
      break;
    case Qt::AlignRight:
      m_textAlignRight->setChecked( true );
      break;
  }
}

void KivioView::showVAlign( int align )
{
  switch(align) {
    case Qt::AlignTop:
      m_textVAlignSuper->setChecked(true);
      break;
    case Qt::AlignVCenter:
      m_textVAlignSuper->setChecked(false);
      m_textVAlignSub->setChecked(false);
      break;
    case Qt::AlignBottom:
      m_textVAlignSub->setChecked(true);
      break;
  }
}

void KivioView::textFormat()
{
  KivioTextFormatDlg dlg(this);
  KivioStencil* stencil = activePage()->selectedStencils()->getLast();

  if(stencil) {
    dlg.setFont(stencil->textFont());
    dlg.setTextColor(stencil->textColor());
    dlg.setHAlign(stencil->hTextAlign());
    dlg.setVAlign(stencil->vTextAlign());
  } else {
    dlg.setFont(doc()->defaultFont());
    dlg.setTextColor(QColor(0, 0, 0));
    dlg.setHAlign(Qt::AlignHCenter);
    dlg.setVAlign(Qt::AlignVCenter);
  }

  if(dlg.exec()) {
    QPtrListIterator<KivioStencil> it(*activePage()->selectedStencils());

    while((stencil = it.current()) != 0) {
      ++it;
      stencil->setTextFont(dlg.font());
      stencil->setTextColor(dlg.textColor());
      stencil->setVTextAlign(dlg.valign());
      stencil->setHTextAlign(dlg.halign());
    }

    updateToolBars();
  }
}

void KivioView::stencilFormat()
{
  KivioStencilFormatDlg dlg(this);
  KivioStencil* stencil = activePage()->selectedStencils()->getLast();
  KivioLineStyle ls;

  if(stencil) {
    ls = stencil->lineStyle();
    dlg.setFillColor(stencil->bgColor());
    dlg.setFillPattern(stencil->fillPattern());
  } else {
    dlg.setLineWidth(1.0, m_pDoc->units());
    dlg.setLineColor(QColor(0, 0, 0));
  }

  dlg.setLineWidth(ls.width(), m_pDoc->units());
  dlg.setLineColor(ls.color());
  dlg.setLinePattern(ls.style());
  dlg.setLineEndStyle(ls.capStyle());

  if(dlg.exec()) {
    QPtrListIterator<KivioStencil> it(*activePage()->selectedStencils());

    while((stencil = it.current()) != 0) {
      ++it;
      stencil->setLineWidth(dlg.lineWidth());
      stencil->setFGColor(dlg.lineColor());
      stencil->setLinePattern(dlg.linePattern());
      stencil->setBGColor(dlg.fillColor());
      stencil->setFillPattern(dlg.fillPattern());
    }

    updateToolBars();
  }
}

void KivioView::arrowHeadFormat()
{
  KivioArrowHeadFormatDlg dlg(this);
  dlg.setUnit(m_pDoc->units());
  dlg.setStartAHType(0);
  dlg.setEndAHType(0);
  dlg.setStartAHWidth(10.0);
  dlg.setStartAHHeight(10.0);
  dlg.setEndAHWidth(10.0);
  dlg.setEndAHHeight(10.0);

  KivioStencil* stencil = activePage()->selectedStencils()->getLast();

  if(stencil) {
    if(stencil->type() == kstConnector) {
      dlg.setUnit(m_pDoc->units());
      dlg.setStartAHType(stencil->startAHType());
      dlg.setEndAHType(stencil->endAHType());
      dlg.setStartAHWidth(stencil->startAHWidth());
      dlg.setStartAHHeight(stencil->startAHLength());
      dlg.setEndAHWidth(stencil->endAHWidth());
      dlg.setEndAHHeight(stencil->endAHLength());
    }
  }

  if(dlg.exec()) {
    QPtrListIterator<KivioStencil> it(*activePage()->selectedStencils());

    while((stencil = it.current()) != 0) {
      ++it;

      if(stencil->type() == kstConnector) {
        stencil->setStartAHType(dlg.startAHType());
        stencil->setEndAHType(dlg.endAHType());
        stencil->setStartAHWidth(dlg.startAHWidth());
        stencil->setStartAHLength(dlg.startAHHeight());
        stencil->setEndAHWidth(dlg.endAHWidth());
        stencil->setEndAHLength(dlg.endAHHeight());
      }
    }

    updateToolBars();
  }
}

Kivio::PluginManager* KivioView::pluginManager()
{
  return m_pluginManager;
}

QPtrList<KAction> KivioView::clipboardActionList()
{
  QPtrList<KAction> tmp;
  tmp.append(m_editCut);
  tmp.append(m_editCopy);
  tmp.append(m_editPaste);

  return tmp;
}

QPtrList<KAction> KivioView::alignActionList()
{
  QPtrList<KAction> tmp;
  tmp.append(m_alignAndDistribute);

  return tmp;
}

QPtrList<KAction> KivioView::groupActionList()
{
  QPtrList<KAction> tmp;
  tmp.append(m_groupAction);
  tmp.append(m_ungroupAction);

  return tmp;
}

QPtrList<KAction> KivioView::layerActionList()
{
  QPtrList<KAction> tmp;
  tmp.append(m_stencilToFront);
  tmp.append(m_stencilToBack);

  return tmp;
}

void KivioView::clipboardDataChanged()
{
  QMimeSource* data = QApplication::clipboard()->data();
  KivioDragObject decoder(this);
  bool paste = decoder.canDecode(data);
  m_editPaste->setEnabled(paste);
}

void KivioView::partActivateEvent(KParts::PartActivateEvent* event)
{
  if((event->widget() == this) && event->activated()) {
    updateToolBars();
    clipboardDataChanged();
  }

  KoView::partActivateEvent(event);
}

void KivioView::initStatusBarProgress()
{
  if(!m_statusBarProgress) {
    m_statusBarProgress = new QProgressBar(100, this);
    addStatusBarItem(m_statusBarProgress);
  }

  m_statusBarProgress->reset();
}

void KivioView::setStatusBarProgress(int progress)
{
  if(m_statusBarProgress) {
    m_statusBarProgress->setProgress(progress);
  }
}

void KivioView::removeStatusBarProgress()
{
  if(m_statusBarProgress) {
    removeStatusBarItem(m_statusBarProgress);
    delete m_statusBarProgress;
    m_statusBarProgress = 0;
  }
}

void KivioView::installStencilSet()
{
  Kivio::StencilSetInstaller dlg(this);
  dlg.exec();
  emit updateStencilSetList();
}

int KivioView::hTextAlign()
{
  if(m_textAlignRight->isChecked()) {
    return Qt::AlignRight;
  } else if(m_textAlignLeft->isChecked()) {
    return Qt::AlignLeft;
  }

  return Qt::AlignHCenter;
}

int KivioView::vTextAlign()
{
  if(m_textVAlignSub->isChecked() && !m_textVAlignSuper->isChecked()) {
    return Qt::AlignBottom;
  } else if(!m_textVAlignSub->isChecked() && m_textVAlignSuper->isChecked()) {
    return Qt::AlignTop;
  }

  return Qt::AlignVCenter;
}

#include "kivio_view.moc"
