/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Janssen (rm@linux.ru.net)

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

#include "kontour_view.h"

#include <qscrollbar.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qregexp.h>
#include <qdockarea.h>

#include <kaction.h>
#include <kprinter.h>
#include <klocale.h>
#include <kstdaction.h>
#include <kpopupmenu.h>
#include <kcursor.h>
#include <kiconloader.h>
#include <kconfig.h>
#include <kparts/event.h>
#include <kstatusbar.h>
#include <ktoolbarbutton.h>
#include <koMainWindow.h>

#include "kontour_global.h"
#include "kontour_doc.h"
#include "kontour_factory.h"
#include "kontour_view_iface.h"
#include "GDocument.h"
#include "GPage.h"
#include "GObject.h"
#include "Canvas.h"
#include "Ruler.h"
#include "TabBar.h"
#include "DashEditDialog.h"
#include "OptionsDialog.h"
#include "ToolController.h"
#include "SelectTool.h"
#include "EditPointTool.h"
#include "ZoomTool.h"
#include "PathTool.h"
#include "RectTool.h"
#include "OvalTool.h"
#include "PolygonTool.h"
#include "TextTool.h"
#include "InsertImageTool.h"
#include "LayerPanel.h"
#include "StylePanel.h"
#include "TransformPanel.h"
#include "AlignmentPanel.h"
#include "CopyCmd.h"
#include "CutCmd.h"
#include "PasteCmd.h"
#include "DuplicateCmd.h"
#include "DeleteCmd.h"
#include "ReorderCmd.h"
#include "GroupCmd.h"
#include "UngroupCmd.h"
#include "ToPathCmd.h"

KontourView::KontourView(QWidget *parent, const char *name, KontourDocument *doc):
KoView(doc, parent, name)
{
  mDoc = doc;

  mDCOP = 0;
  dcopObject(); // build it

  objMenu = 0;
  rulerMenu = 0;

  setInstance(KontourFactory::global());
  setXMLFile("kontour.rc");

  readConfig();
  setupActions();
  initActions();
  setupCanvas();
  setupPanels();
  changeSelection();
  readConfigAfter();
}

KontourView::~KontourView()
{
  writeConfig();
  delete tcontroller;
  if(objMenu)
    delete objMenu;
  if(rulerMenu)
    delete rulerMenu;

  // Delete GUI
  delete hRuler;
  delete vRuler;
  delete mSBCoords;
  delete mSBState;

  //
  if(mLayerPanel)
    delete mLayerPanel;
  if(mPaintPanel)
    delete mPaintPanel;
  if(mOutlinePanel)
    delete mOutlinePanel;
  if(mTransformPanel)
    delete mTransformPanel;
  if(mAlignmentPanel)
    delete mAlignmentPanel;
  delete mDCOP;
}

DCOPObject *KontourView::dcopObject()
{
  if(!mDCOP )
    mDCOP = new KontourViewIface(this);
  return mDCOP;
}


void KontourView::unit(MeasurementUnit u)
{
  mUnit = u;
}

void KontourView::setupActions()
{
  // Edit menu

  m_copy = KStdAction::copy(this, SLOT(slotCopy()), actionCollection(), "copy");
  m_paste = KStdAction::paste(this, SLOT(slotPaste()), actionCollection(), "paste");
  m_cut = KStdAction::cut(this, SLOT(slotCut()), actionCollection(), "cut");

  m_duplicate = new KAction(i18n("Dup&licate"), 0, this, SLOT(slotDuplicate()), actionCollection(), "duplicate");
  m_delete = new KAction(i18n("&Delete"), "editdelete", 0, this, SLOT(slotDelete()), actionCollection(), "delete");
  m_selectAll = KStdAction::selectAll(this, SLOT(slotSelectAll()), actionCollection(), "selectAll");
  m_deselectAll = KStdAction::deselect(this, SLOT(slotDeselectAll()), actionCollection(), "deselectAll");

  // View menu

  m_zoomIn = new KAction(i18n("Zoom In"), "viewmag+", CTRL+Key_Plus, this, SLOT(slotZoomIn()), actionCollection(), "zoomin");
  m_zoomOut = new KAction(i18n("Zoom Out"), "viewmag-", CTRL+Key_Minus, this, SLOT(slotZoomOut()), actionCollection(), "zoomout");

  m_viewZoom = new KSelectAction(i18n("&Zoom"), 0, actionCollection(), "view_zoom");
  QStringList zooms;
  zooms << "50%";
  zooms << "75%";
  zooms << "100%";
  zooms << "150%";
  zooms << "200%";
  zooms << "400%";
  zooms << "600%";
  zooms << "800%";
  zooms << "1000%";

  m_viewZoom->setItems(zooms);
  m_viewZoom->setEditable(true);
  connect(m_viewZoom, SIGNAL(activated(const QString &)),this, SLOT(slotViewZoom(const QString &)));
  m_viewZoom->setCurrentItem(2);

  m_outline = new KToggleAction(i18n("Ou&tline"), 0, this, SLOT(slotOutline()), actionCollection(), "outline");
  m_outline->setExclusiveGroup("Outline");
  m_normal = new KToggleAction(i18n("&Normal"), 0, this, SLOT(slotNormal()), actionCollection(), "normal");
  m_normal->setExclusiveGroup("Outline");
  m_normal->setChecked(true);  //TODO: from config

  m_showRuler = new KToggleAction(i18n("Show &Ruler"), 0, actionCollection(), "showRuler");
  connect(m_showRuler, SIGNAL(toggled(bool)), this, SLOT(slotShowRuler(bool)));

  m_showGrid = new KToggleAction(i18n("Show &Grid"), 0, actionCollection(), "showGrid");
  connect(m_showGrid, SIGNAL(toggled(bool)), this, SLOT(slotShowGrid(bool)));

  m_showHelplines = new KToggleAction(i18n("Show &Helplines"), 0, actionCollection(), "showHelplines");
  connect(m_showHelplines, SIGNAL(toggled(bool)), this, SLOT(slotShowHelplines(bool)));

  // Layout menu
  m_snapToGrid = new KToggleAction(i18n("&Align to Grid"), "snap_to_grid", 0, actionCollection(), "alignToGrid");
  connect(m_snapToGrid, SIGNAL(toggled(bool)), this, SLOT(slotAlignToGrid(bool)));
  m_snapToGrid->setChecked(activeDocument()->snapToGrid());

  m_alignToHelplines = new KToggleAction( i18n("Align &to Helplines"), 0, actionCollection(), "alignToHelplines" );
  connect( m_alignToHelplines, SIGNAL( toggled( bool ) ), this, SLOT( slotAlignToHelplines( bool ) ) );
  m_alignToHelplines->setChecked(activeDocument()->snapToHelplines());

  m_toFront = new KAction( i18n("To &Front"), 0, this, SLOT( slotToFront() ), actionCollection(), "toFront");
  m_toBack = new KAction( i18n("To &Back"), 0, this, SLOT( slotToBack() ), actionCollection(), "toBack");
  m_forwardOne = new KAction( i18n("Forward &One"), 0, this, SLOT( slotForwardOne() ), actionCollection(), "forwardOne");
  m_backOne = new KAction( i18n("B&ack One"), 0, this, SLOT( slotBackOne() ), actionCollection(), "backOne");

  m_group = new KAction(i18n("&Group"), "group", 0, this, SLOT(slotGroup()), actionCollection(), "group");
  m_ungroup = new KAction(i18n("&Ungroup"), "ungroup", 0, this, SLOT(slotUngroup()), actionCollection(), "ungroup");

  m_convertToPath = new KAction(i18n("&Convert to Path"), 0, this, SLOT(slotConvertToPath()), actionCollection(), "convertToPath");

  // Style menu
  m_styles = new KSelectAction(i18n("&Styles"), 0, actionCollection(), "styles");
  connect(m_styles, SIGNAL(activated(const QString &)),this, SLOT(slotStyles(const QString &)));

  m_addStyle = new KAction(i18n("&Add Style"), 0, this, SLOT(slotAddStyle()), actionCollection(), "addStyle");
  m_deleteStyle = new KAction(i18n("&Delete Style"), 0, this, SLOT(slotDeleteStyle()), actionCollection(), "deleteStyle");
  m_dashEdit = new KAction(i18n("&Edit Dashes..."), 0, this, SLOT(slotDashEdit()), actionCollection(), "dashEdit");

  // Settings menu
  m_showLayerPanel = new KToggleAction(i18n("Show &Layer Panel"), 0, actionCollection(), "showLayerPanel");
  connect(m_showLayerPanel, SIGNAL(toggled(bool)), this, SLOT(slotShowLayerPanel(bool)));

  m_showOutlinePanel = new KToggleAction(i18n("Show &Outline Panel"), 0, actionCollection(), "showOutlinePanel");
  connect(m_showOutlinePanel, SIGNAL(toggled(bool)), this, SLOT(slotShowOutlinePanel(bool)));

  m_showPaintPanel = new KToggleAction(i18n("Show &Paint Panel"), 0, actionCollection(), "showPaintPanel");
  connect(m_showPaintPanel, SIGNAL(toggled(bool)), this, SLOT(slotShowPaintPanel(bool)));

  m_showTransformationPanel = new KToggleAction(i18n("Show &Transformation Panel"), 0, actionCollection(), "showTransformationPanel");
  connect(m_showTransformationPanel, SIGNAL(toggled(bool)), this, SLOT(slotShowTransformationPanel(bool)));

  m_options = KStdAction::preferences(this, SLOT(slotOptions()), actionCollection(), "options");
}

void KontourView::initActions()
{
  m_showRuler->setChecked(true);
  m_showGrid->setChecked(activeDocument()->showGrid());
  m_showHelplines->setChecked(activeDocument()->showHelplines());
  updateStyles();
}

void KontourView::setupCanvas()
{
  // create horizontal ruler
  hRuler = new Ruler(mDoc, Ruler::Horizontal, mUnit, this);
  hRuler->setCursor(KCursor::handCursor());

  // create vertical ruler
  vRuler = new Ruler(mDoc, Ruler::Vertical, mUnit, this);
  vRuler->setCursor(KCursor::handCursor());

  connect(hRuler, SIGNAL(rmbPressed()), this, SLOT(popupForRulers()));
  connect(vRuler, SIGNAL(rmbPressed()), this, SLOT(popupForRulers()));

  QSplitter *tabSplit = new QSplitter(this);

  mTabBar = new TabBar(tabSplit, this);

  vBar = new QScrollBar(QScrollBar::Vertical, this);
  hBar = new QScrollBar(QScrollBar::Horizontal, tabSplit);

  mCanvas = new Canvas(mDoc->document(), this, hBar, vBar, this);

  QPixmap *pm;

  mTabBarFirst = new QPushButton(this);
  mTabBarFirst->setFixedSize(16,16);
  pm = new QPixmap(BarIcon("tab_first"));
  if(pm)
    mTabBarFirst->setPixmap(*pm);
  delete pm;
  mTabBarLeft = new QPushButton(this);
  mTabBarLeft->setFixedSize(16,16);
  pm = new QPixmap(BarIcon("tab_left"));
  if(pm)
    mTabBarLeft->setPixmap(*pm);
  delete pm;

  mTabBarRight = new QPushButton(this);
  mTabBarRight->setFixedSize(16,16);
  pm = new QPixmap(BarIcon("tab_right"));
  if(pm)
    mTabBarRight->setPixmap(*pm);

  delete pm;

  mTabBarLast = new QPushButton(this);
  mTabBarLast->setFixedSize(16,16);
  pm = new QPixmap(BarIcon("tab_last"));
  if(pm)
    mTabBarLast->setPixmap(*pm);
  delete pm;
  tabLayout = new QHBoxLayout();
  tabLayout->addWidget(mTabBarFirst);
  tabLayout->addWidget(mTabBarLeft);
  tabLayout->addWidget(mTabBarRight);
  tabLayout->addWidget(mTabBarLast);
  tabLayout->addWidget(tabSplit);
  connect(mTabBarFirst, SIGNAL(clicked()), mTabBar, SLOT(scrollFirst()));
  connect(mTabBarLeft, SIGNAL(clicked()), mTabBar, SLOT(scrollLeft()));
  connect(mTabBarRight, SIGNAL(clicked()), mTabBar, SLOT(scrollRight()));
  connect(mTabBarLast, SIGNAL(clicked()), mTabBar, SLOT(scrollLast()));

  QGridLayout *layout = new QGridLayout(this);
  layout->addWidget(mCanvas, 1, 1);
  layout->addWidget(hRuler, 0, 1);
  layout->addWidget(vRuler, 1, 0);
  layout->addMultiCellWidget(vBar, 0, 1, 2, 2);
  layout->addMultiCellLayout(tabLayout, 2, 2, 0, 1);

  KStatusBar * sb = statusBar();
  mSBCoords = 0L;
  mSBState = 0L;
  if(sb)
  {
    mSBCoords = new KStatusBarLabel(QString::null, 0, sb);
    mSBState = new KStatusBarLabel(QString::null, 0, sb);
    mSBCoords->setMinimumWidth(100);
    mSBState->setMinimumWidth(300);
    addStatusBarItem(mSBCoords, 0);
    addStatusBarItem(mSBState, 0);
  }

  connect(mCanvas, SIGNAL(rmbAtSelection(int,int)), this, SLOT(popupForSelection()));
  connect(mCanvas, SIGNAL(mousePositionChanged(int, int)), hRuler, SLOT(updatePointer(int, int)));
  connect(mCanvas, SIGNAL(mousePositionChanged(int, int)), vRuler, SLOT(updatePointer(int, int)));
  connect(mCanvas, SIGNAL(coordsChanged(double, double)), SLOT(slotCoordsChanged(double, double)));

  connect(mCanvas, SIGNAL(offsetXChanged(int)), hRuler, SLOT(updateOffset(int)));
  connect(mCanvas, SIGNAL(offsetYChanged(int)), vRuler, SLOT(updateOffset(int)));

  connect(activeDocument(), SIGNAL(zoomFactorChanged(double)), this, SLOT(slotZoomFactorChanged()));
  connect(activeDocument(), SIGNAL(selectionChanged()), this, SLOT(changeSelection()));

  // helpline creation
  connect(hRuler, SIGNAL(drawHelpline(int, int, bool)), mCanvas, SLOT(drawTmpHelpline(int, int, bool)));
  connect(vRuler, SIGNAL(drawHelpline(int, int, bool)), mCanvas, SLOT(drawTmpHelpline(int, int, bool)));
  connect(hRuler, SIGNAL(addHelpline(int, int, bool)), mCanvas, SLOT(addHelpline(int, int, bool)));
  connect(vRuler, SIGNAL(addHelpline(int, int, bool)), mCanvas, SLOT(addHelpline(int, int, bool)));

  setFocusPolicy(QWidget::StrongFocus);
  setFocusProxy(mCanvas);
  mCanvas->installEventFilter(this);
}

void KontourView::setupPanels()
{
  mLayerPanel = 0L;
  mOutlinePanel = 0L;
  mPaintPanel = 0L;
  mTransformPanel = 0L;
  mAlignmentPanel = 0L;

  if(!mDoc->isReadWrite())
    return;

  // Layer panel
  mLayerPanel = new LayerPanel(activeDocument(), this);
  connect(activeDocument(), SIGNAL(updateLayerView()), mLayerPanel, SLOT(updatePanel()));
  connect(mLayerPanel, SIGNAL(visibilityChanged(bool)), m_showLayerPanel, SLOT(setChecked(bool)));
  shell()->addDockWindow(mLayerPanel, DockRight);

  // Outline properties panel
  mOutlinePanel = new OutlinePanel(this, this);
  connect(mOutlinePanel, SIGNAL(visibilityChanged(bool)), m_showOutlinePanel, SLOT(setChecked(bool)));
  shell()->addDockWindow(mOutlinePanel, DockRight);

  // Paint properties panel
  mPaintPanel = new PaintPanel(this, this);
  connect(mPaintPanel, SIGNAL(visibilityChanged(bool)), m_showPaintPanel, SLOT(setChecked(bool)));
  shell()->addDockWindow(mPaintPanel, DockRight);

  // Transform panel
  mTransformPanel = new TransformPanel(this, this);
  connect(mTransformPanel, SIGNAL(visibilityChanged(bool)), m_showTransformationPanel, SLOT(setChecked(bool)));
  shell()->addDockWindow(mTransformPanel, DockRight);

  // Alignment panel
  mAlignmentPanel = new AlignmentPanel(this, this);
//  shell()->addDockWindow(mAlignmentPanel, DockRight);
}

void KontourView::setupTools()
{
  tcontroller = new ToolController(this);

  mSelectTool = new SelectTool("Select", tcontroller);
  tcontroller->registerTool(mSelectTool);

  mEditPointTool = new EditPointTool("EditPoint", tcontroller);
  tcontroller->registerTool(mEditPointTool);

  mZoomTool = new ZoomTool("Zoom", tcontroller);
  tcontroller->registerTool(mZoomTool);

  mPathTool = new PathTool("Path", tcontroller);
  tcontroller->registerTool(mPathTool);

  mRectTool = new RectTool("Rect", tcontroller);
  tcontroller->registerTool(mRectTool);

  mOvalTool = new OvalTool("Oval", tcontroller);
  tcontroller->registerTool(mOvalTool);

  mPolygonTool = new PolygonTool("Polygon", tcontroller);
  tcontroller->registerTool(mPolygonTool);

  mTextTool = new TextTool("Text", tcontroller);
  tcontroller->registerTool(mTextTool);

  mInsertImageTool = new InsertImageTool("InsertImage", tcontroller);
  tcontroller->registerTool(mInsertImageTool);

  tcontroller->initToolBar();
  tcontroller->selectTool("Select");
}

void KontourView::readConfig()
{
  KConfig *config = KontourFactory::global()->config();

  /* Read default unit */
  config->setGroup("General");
  QString value = config->readEntry("DefaultUnit", "pt");
  if(value == "mm")
    mUnit = UnitMillimeter;
  else if(value == "inch")
    mUnit = UnitInch;
  else if(value == "cm")
    mUnit = UnitCentimeter;
  else if(value == "pica")
    mUnit = UnitPica;
  else if(value == "didot")
    mUnit = UnitDidot;
  else if(value == "cicero")
    mUnit = UnitCicero;
  else
    mUnit = UnitPoint;

  mWorkSpaceColor = lightGray;
//  UnitBox::setDefaultMeasurementUnit(defaultUnit);
}

void KontourView::readConfigAfter()
{
  KConfig *config = KontourFactory::global()->config();

  config->setGroup("General");

  config->setGroup("Panels");
  slotShowLayerPanel(config->readBoolEntry("LayerPanel", false));
  slotShowOutlinePanel(config->readBoolEntry("OutlinePanel", false));
  slotShowPaintPanel(config->readBoolEntry("PaintPanel", false));
  slotShowTransformationPanel(config->readBoolEntry("TransformationPanel", false));
}

void KontourView::writeConfig()
{
  KConfig *config = KontourFactory::global()->config();

  config->setGroup("General");

  /* Write default unit */
  switch(mUnit)
  {
  case UnitPoint:
    config->writeEntry("DefaultUnit", "pt");
    break;
  case UnitMillimeter:
    config->writeEntry("DefaultUnit", "mm");
    break;
  case UnitInch:
    config->writeEntry("DefaultUnit", "inch");
    break;
  case UnitCentimeter:
    config->writeEntry("DefaultUnit", "cm");
    break;
  case UnitPica:
    config->writeEntry("DefaultUnit", "pica");
    break;
  case UnitDidot:
    config->writeEntry("DefaultUnit", "didot");
    break;
  case UnitCicero:
    config->writeEntry("DefaultUnit", "cicero");
    break;
  }

  config->setGroup("Panels");
  config->writeEntry("LayerPanel", m_showLayerPanel->isChecked());
  config->writeEntry("OutlinePanel", m_showOutlinePanel->isChecked());
  config->writeEntry("PaintPanel", m_showPaintPanel->isChecked());
  config->writeEntry("TransformationPanel", m_showTransformationPanel->isChecked());
}

void KontourView::workSpaceColor(QColor c)
{
  mWorkSpaceColor = c;
}

void KontourView::setStatus(QString s)
{
  if(mSBState)
    mSBState->setText(s);
}

void KontourView::updateStyles()
{
  QStringList *styles = activeDocument()->styles()->stringList();
  m_styles->setItems(*styles);
  delete styles;
  m_styles->setCurrentItem(activeDocument()->styles()->currentNum());
}

void KontourView::customEvent(QCustomEvent *e)
{
  if(KParts::GUIActivateEvent::test(e))
    if(((KParts::GUIActivateEvent*)e)->activated())
    {
      setupTools();
      KStatusBar * sb = statusBar();
      if(sb)
        sb->show();
    }
  KoView::customEvent(e);
}

bool KontourView::eventFilter(QObject *o, QEvent *e)
{
// TODO TOOLS
//  if(!tcontroller ||!tcontroller->getActiveTool())
//    return false;

//  if((e==0) || (tcontroller->getActiveTool()->id()==Tool::ToolSelect))
//      return false;

  if(o == mCanvas && e->type() == QEvent::MouseButtonPress)
  {
    QMouseEvent *me = (QMouseEvent*)e;
    if(me->button() == RightButton)
    {
      //m_selectTool->setChecked(true);
      //slotSelectTool(true);
    }
  }
  return false;
}

void KontourView::updateReadWrite(bool /*readwrite*/)
{
}

void KontourView::popupForSelection()
{
  if(objMenu)
    delete objMenu;
  objMenu = new KPopupMenu();
  //m_distribute->plug( objMenu );
  m_copy->plug(objMenu);
  m_cut->plug(objMenu);
  m_delete->plug(objMenu);
  m_duplicate->plug(objMenu);
  objMenu->insertSeparator();
  m_toFront->plug(objMenu);
  m_toBack->plug(objMenu);
  m_forwardOne->plug(objMenu);
  m_backOne->plug(objMenu);
  objMenu->insertSeparator();
  m_convertToPath->plug(objMenu);
  objMenu->popup(QCursor::pos());
}

void KontourView::popupForRulers()
{
  if(!mDoc->isReadWrite())
    return;

  if(!rulerMenu)
  {
    rulerMenu = new KPopupMenu();
    m_showGrid->plug(rulerMenu);
    m_showHelplines->plug(rulerMenu);
    m_snapToGrid->plug(rulerMenu);
    m_alignToHelplines->plug(rulerMenu);
  }
  rulerMenu->popup(QCursor::pos());
}

void KontourView::changeSelection()
{
  GPage *page = activeDocument()->activePage();
  if(!page)
    return;
  if(mOutlinePanel)
    mOutlinePanel->slotUpdate();
  if(mPaintPanel)
    mPaintPanel->slotUpdate();
  if(mTransformPanel)
    mTransformPanel->slotUpdate();
  if(page->selectionIsEmpty())
  {
    m_copy->setEnabled(false);
    m_cut->setEnabled(false);
    m_delete->setEnabled(false);
    m_duplicate->setEnabled(false);
    m_toFront->setEnabled(false);
    m_toBack->setEnabled(false);
    m_forwardOne->setEnabled(false);
    m_backOne->setEnabled(false);
    m_convertToPath->setEnabled(false);
    m_deselectAll->setEnabled(false);
  }
  else
  {
    m_copy->setEnabled(true);
    m_cut->setEnabled(true);
    m_convertToPath->setEnabled(true);
    m_delete->setEnabled(true);
    m_toFront->setEnabled(true);
    m_toBack->setEnabled(true);
    m_forwardOne->setEnabled(true);
    m_backOne->setEnabled(true);
    m_duplicate->setEnabled(true);
    m_deselectAll->setEnabled(true);
  }
  if(page->objectCount() <= 1)
    m_group->setEnabled(false);
  else
    m_group->setEnabled(true);
  if(page->objectCount() == page->selectionCount())
    m_selectAll->setEnabled(false);
  else
    m_selectAll->setEnabled(true);
  if(page->convertibleCount() == 0)
    m_convertToPath->setEnabled(false);
  else
    m_convertToPath->setEnabled(true);
}

void KontourView::slotZoomFactorChanged()
{
  double zoom = activeDocument()->zoomFactor();
  vRuler->zoomFactor(canvas()->xOffset(), canvas()->yOffset());
  hRuler->zoomFactor(canvas()->xOffset(), canvas()->yOffset());
  QStringList list = m_viewZoom->items();
  QString f = QString::number(qRound(zoom * 100.0));
  int i = 0;
  for(QValueList<QString>::Iterator it = list.begin(); it != list.end(); ++it, ++i)
    if((*it).left((*it).length() - 1) == f)
    {
      m_viewZoom->setCurrentItem(i);
      return;
    }
  /* current zoom value not found in list */
  f += '%';
  m_viewZoom->changeItem(8, f);
  m_viewZoom->setCurrentItem(8);
  m_zoomIn->setEnabled(zoom != Kontour::maxZoomFactor);
  m_zoomOut->setEnabled(zoom != Kontour::minZoomFactor);
}

void KontourView::slotCoordsChanged(double x, double y)
{
  if(mSBCoords)
    mSBCoords->setText(QString("%1:%2").arg(x, 0, 'f', 1).arg(y, 0, 'f', 1));
}

/*******************[Actions]*******************/

void KontourView::setupPrinter(KPrinter &/*printer*/)
{
}

void KontourView::print(KPrinter &/*printer*/)
{
}

void KontourView::slotCopy()
{
  CopyCmd *cmd = new CopyCmd(activeDocument());
  mDoc->history()->addCommand(cmd);
}

void KontourView::slotPaste()
{
  PasteCmd *cmd = new PasteCmd(activeDocument());
  mDoc->history()->addCommand(cmd);
}

void KontourView::slotCut()
{
  CutCmd *cmd = new CutCmd(activeDocument());
  mDoc->history()->addCommand(cmd);
}

void KontourView::slotDuplicate()
{
  DuplicateCmd *cmd = new DuplicateCmd(activeDocument());
  mDoc->history()->addCommand(cmd);
}

void KontourView::slotDelete()
{
  DeleteCmd *cmd = new DeleteCmd(activeDocument());
  mDoc->history()->addCommand(cmd);
}

void KontourView::slotSelectAll()
{
  tcontroller->selectTool("Select");
  activeDocument()->activePage()->selectAllObjects();
}

void KontourView::slotDeselectAll()
{
  activeDocument()->activePage()->unselectAllObjects();
}

void KontourView::slotZoomIn()
{
  double zoom = activeDocument()->zoomFactor();
  zoom *= 1.25;
  activeDocument()->zoomFactor(zoom);
}

void KontourView::slotZoomOut()
{
  double zoom = activeDocument()->zoomFactor();
  zoom *= 0.8;
  activeDocument()->zoomFactor(zoom);
}

void KontourView::slotViewZoom(const QString &s)
{
  QString z(s);
  z = z.replace(QRegExp("%"), "");
  z = z.simplifyWhiteSpace();
  double zoom = z.toDouble() / 100.0;
  activeDocument()->zoomFactor(zoom);
}

void KontourView::slotOutline()
{
  if(m_outline->isChecked())
    mCanvas->outlineMode(true);
  else
    m_outline->setChecked(true);
}

void KontourView::slotNormal()
{
  if(m_normal->isChecked())
    mCanvas->outlineMode(false);
  else
    m_normal->setChecked(true);
}

void KontourView::slotShowRuler(bool b)
{
  mShowRulers = b;

  if(mShowRulers)
  {
    hRuler->show();
    vRuler->show();
  }
  else
  {
    hRuler->hide();
    vRuler->hide();
  }
}

void KontourView::slotShowGrid(bool b)
{
  if(b != activeDocument()->showGrid())
  {
    activeDocument()->showGrid(b);
    mCanvas->update();
  }
}

void KontourView::slotShowHelplines(bool b)
{
  if(b != activeDocument()->showHelplines())
  {
    activeDocument()->showHelplines(b);
    mCanvas->update();
  }
}

void KontourView::slotAlignToGrid(bool b)
{
  activeDocument()->snapToGrid(b);
}

// TODO change name
void KontourView::slotAlignToHelplines(bool b)
{
  activeDocument()->snapToHelplines(b);
}

void KontourView::slotToFront()
{
  ReorderCmd *cmd = new ReorderCmd(activeDocument(), ReorderCmd::RP_ToFront);
  mDoc->history()->addCommand(cmd);
}

void KontourView::slotToBack()
{
  ReorderCmd *cmd = new ReorderCmd(activeDocument(), ReorderCmd::RP_ToBack);
  mDoc->history()->addCommand(cmd);
}

void KontourView::slotForwardOne()
{
  ReorderCmd *cmd = new ReorderCmd(activeDocument(), ReorderCmd::RP_ForwardOne);
  mDoc->history()->addCommand(cmd);
}

void KontourView::slotBackOne()
{
  ReorderCmd *cmd = new ReorderCmd(activeDocument(), ReorderCmd::RP_BackwardOne);
  mDoc->history()->addCommand(cmd);
}

void KontourView::slotGroup()
{
  GroupCmd *cmd = new GroupCmd(activeDocument());
  mDoc->history()->addCommand(cmd);
}

void KontourView::slotUngroup()
{
//  UngroupCmd *cmd = new UngroupCmd(activeDocument());
//  mDoc->history()->addCommand(cmd);
}

void KontourView::slotConvertToPath()
{
  if(!activeDocument()->activePage()->selectionIsEmpty())
    mDoc->history()->addCommand(new ToPathCmd(activeDocument()));
}

void KontourView::slotStyles(const QString &/*s*/)
{
  activeDocument()->styles()->style(m_styles->currentText());
}

void KontourView::slotAddStyle()
{
  // TODO : we need to read the current pen and brush from the color chooser(s)
  // here. Should we update KoColorChooser to read the current color value instead of
  // catching signals ? (Rob)
  activeDocument()->styles()->addStyle();
  updateStyles();
}

void KontourView::slotDeleteStyle()
{

}

void KontourView::slotDashEdit()
{
  DashEditDialog dialog(this, activeDocument(), 0L, "DashEdit");
  dialog.exec();
}

void KontourView::slotShowLayerPanel(bool b)
{
  if(b)
    mLayerPanel->show();
  else
    mLayerPanel->hide();
}

void KontourView::slotShowOutlinePanel(bool b)
{
  if(b)
    mOutlinePanel->show();
  else
    mOutlinePanel->hide();
}

void KontourView::slotShowPaintPanel(bool b)
{
  if(b)
    mPaintPanel->show();
  else
    mPaintPanel->hide();
}

void KontourView::slotShowTransformationPanel(bool b)
{
  if(b)
    mTransformPanel->show();
  else
    mTransformPanel->hide();
}

void KontourView::slotOptions()
{
  OptionsDialog dialog(this, activeDocument(), 0L, "Options");
  dialog.exec();
}

/*
// Reimplemented from KoView. Automatically called for embedded views
void KontourView::setZoom( double zoom ) {
    kdDebug() << "KontourView::setZoom " << zoom << endl;
    if (zoom != mCanvas->getZoomFactor() )
        mCanvas->setZoomFactor(zoom);
}

// Reimplemented from KoView.
double KontourView::zoom() const {
    return mCanvas->getZoomFactor();
}
*/

#include "kontour_view.moc"
