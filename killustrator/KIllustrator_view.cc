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
#include <iostream.h>
#include <kdebug.h>
#include <qwidget.h>
#include <kapp.h>
#include <KIllustrator_view.h>
#include <KIllustrator_doc.h>
#include <KIllustrator_factory.h>
#include <koMainWindow.h>

#include "TabBar.h"
#include "LayerPanel.h"
#include "tooldockbase.h"
#include "tooldockmanager.h"
#include <GDocument.h>
#include "GPage.h"
#include <Canvas.h>
#include <Tool.h>
#include <Ruler.h>
#include <ToolController.h>
#include <RectangleTool.h>
#include <PolylineTool.h>
#include <FreeHandTool.h>
#include <SelectionTool.h>
#include <OvalTool.h>
#include <TextTool.h>
#include <PolygonTool.h>
#include <EditPointTool.h>
#include <BezierTool.h>
#include <ZoomTool.h>
#include <PathTextTool.h>
#include <InsertPartTool.h>
#include <PropertyEditor.h>
#include <AlignmentDialog.h>
#include <TransformationDialog.h>
#include <PStateManager.h>
#include <ExportFilter.h>
#include <GroupCmd.h>
#include <UngroupCmd.h>
#include <DeleteCmd.h>
#include <CutCmd.h>
#include <CopyCmd.h>
#include <PasteCmd.h>
#include <DuplicateCmd.h>
#include <ReorderCmd.h>
#include <InsertClipartCmd.h>
#include <InsertPixmapCmd.h>
#include <SetPropertyCmd.h>
#include <FilterManager.h>
#include <ToCurveCmd.h>
#include <BlendCmd.h>
#include <BlendDialog.h>
#include <OptionDialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <unistd.h>
#include <qfileinfo.h>
#include <qscrollview.h>
#include <koPartSelectDia.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kcoloractions.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>
#include <kxmlgui.h>
#include <kparts/event.h>
#include <GPart.h>
#include <qlayout.h>
#include <qdatetime.h>
#include <qtl.h>
#include <koPartSelectAction.h>


KIllustratorView::KIllustratorView (QWidget* parent, const char* name,
                                    KIllustratorDocument* doc)
   :KoView( doc, parent, name )
   ,objMenu(0)
   ,rulerMenu(0)
    ,mToolDockManager(0)
    ,mLayerPanel(0)
    ,mLayerDockBase(0)
{
   QTime time;
   time.start();
    setInstance( KIllustratorFactory::global() );
    m_pDoc = doc;
    m_bShowGUI = true;
    m_bShowRulers = true;
    mParent = parent;

    if (m_pDoc!=0)
       m_pDoc->setKilluView(this);
    //kdDebug(38000)<<"KIlluView after readConfig: "<<time.elapsed()<<endl;

    // restore default settings
    PStateManager::instance ();
    kdDebug(38000)<<"KIlluView after instance: "<<time.elapsed()<<endl;

    connect (m_pDoc,SIGNAL(partInserted(KIllustratorChild*,GPart*)),this,SLOT(insertPartSlot(KIllustratorChild*,GPart*)));
    connect (m_pDoc,SIGNAL(childGeometryChanged(KIllustratorChild*)),this,SLOT(changeChildGeometrySlot(KIllustratorChild*)));
    connect (this,SIGNAL(embeddImage(const QString &)),this,SLOT(slotInsertBitmap(const QString &)));

    setupCanvas();
    kdDebug(38000)<<"KIlluView after setupCanvas: "<<time.elapsed()<<endl;
    setXMLFile( "KIllustrator.rc" );
    kdDebug(38000)<<"KIlluView after setXMLFile(): "<<time.elapsed()<<endl
        ;
    canvas->docSizeChanged();

    createMyGUI();
    kdDebug(38000)<<"KIlluView after createMyGUI: "<<time.elapsed()<<endl;

    connect (activeDocument(),SIGNAL(pageChanged()),canvas,SLOT(repaint()));
    connect (activeDocument(),SIGNAL(pageChanged()),this,SLOT(refreshLayerPanel()));
    connect (canvas, SIGNAL( backSpaceCalled()),this,SLOT(slotDelete()));
    readConfig();
}

KIllustratorView::~KIllustratorView()
{
    if (m_pDoc!=0)
       m_pDoc->setKilluView(0);
   writeConfig();
   delete mZoomTool;
   if (objMenu!=0)
      delete objMenu;
   if (rulerMenu!=0)
      delete rulerMenu;
   delete mToolDockManager;
   delete hRuler;
   delete vRuler;
}


void KIllustratorView::createMyGUI()
{
   QTime time;
   time.start();
    // File menu
    new KAction( i18n("&Export..."), 0, this, SLOT( slotExport() ), actionCollection(), "export" );

    // Edit menu
    m_copy = KStdAction::copy(this, SLOT( slotCopy() ), actionCollection(), "copy" );
    KStdAction::paste(this, SLOT( slotPaste() ), actionCollection(), "paste" );
    m_cut = KStdAction::cut(this, SLOT( slotCut() ), actionCollection(), "cut" );
    m_undo = KStdAction::undo(this, SLOT( slotUndo() ), actionCollection(), "undo" );
    m_redo = KStdAction::redo(this, SLOT( slotRedo() ), actionCollection(), "redo" );
    m_duplicate=new KAction( i18n("Dup&licate"), 0, this, SLOT( slotDuplicate() ), actionCollection(), "duplicate" );
    m_delete=new KAction( i18n("&Delete"), "editdelete", Key_Delete, this, SLOT( slotDelete() ), actionCollection(), "delete" );
    KStdAction::selectAll( this, SLOT( slotSelectAll() ), actionCollection(), "selectAll" );
    m_properties = new KAction( i18n("&Properties..."), 0, this, SLOT( slotProperties() ), actionCollection(), "properties" );

    // View menu
    new KAction( i18n("Zoom in"), "viewmag+", CTRL+Key_Plus, this, SLOT( slotZoomIn() ), actionCollection(), "zoomin");
    new KAction( i18n("Zoom out"), "viewmag-", CTRL+Key_Minus, this, SLOT( slotZoomOut() ), actionCollection(), "zoomout");

    m_outline = new KToggleAction( i18n("Ou&tline"), 0,
                                   this, SLOT( slotOutline() ),
                                   actionCollection(), "outline" );
    m_outline->setExclusiveGroup( "Outline" );


    m_normal = new KToggleAction( i18n("&Normal"), 0,
                                  this,SLOT( slotNormal() ) ,
                                  actionCollection(), "normal" );
    m_normal->setExclusiveGroup( "Outline" );

    KToggleAction *m_showRuler = new KToggleAction( i18n("Show &Ruler"), 0, actionCollection(), "showRuler" );
    connect( m_showRuler, SIGNAL( toggled( bool ) ), this, SLOT( slotShowRuler( bool ) ) );

    m_showGrid = new KToggleAction( i18n("Show &Grid"), 0, actionCollection(), "showGrid" );
    connect( m_showGrid, SIGNAL( toggled( bool ) ), this, SLOT( slotShowGrid( bool ) ) );

    m_showHelplines = new KToggleAction( i18n("Show &Helplines"), 0, actionCollection(), "showHelplines" );
    connect( m_showHelplines, SIGNAL( toggled( bool ) ), this, SLOT( slotShowHelplines( bool ) ) );

    // Insert menu
    new KAction( i18n("Insert &Bitmap..."),"frame_image", 0, this, SLOT( slotInsertBitmap() ), actionCollection(), "insertBitmap" );
    new KAction( i18n("Insert &Clipart..."),"insertclipart", 0, this, SLOT( slotInsertClipart() ), actionCollection(), "insertClipart" );
    kdDebug(38000)<<"inside createMyGUI(): a: "<<time.elapsed()<<" msecs elapsed"<<endl;

    // Tools
    m_selectTool = new KToggleAction( i18n("Select objects"), "frame_edit", CTRL+Key_1, actionCollection(), "mouse" );
    m_selectTool->setExclusiveGroup( "Tools" );
    connect( m_selectTool, SIGNAL( toggled( bool ) ), this, SLOT( slotSelectTool( bool ) ) );

    KToggleAction* m_pointTool = new KToggleAction( i18n("Edit points"), "pointtool", CTRL+Key_2, actionCollection(), "point" );
    m_pointTool->setExclusiveGroup( "Tools" );
    connect( m_pointTool, SIGNAL( toggled( bool ) ), this, SLOT( slotPointTool( bool ) ) );

    KToggleAction *m_freehandTool = new KToggleAction( i18n("Freehand"), "freehandtool", CTRL+Key_3, actionCollection(), "freehand" );
    m_freehandTool->setExclusiveGroup( "Tools" );
    connect( m_freehandTool, SIGNAL( toggled( bool ) ), this, SLOT( slotFreehandTool( bool ) ) );

    KToggleAction *m_lineTool = new KToggleAction( i18n("Line"), "linetool", CTRL+Key_4, actionCollection(), "line" );
    m_lineTool->setExclusiveGroup( "Tools" );
    connect( m_lineTool, SIGNAL( toggled( bool ) ), this, SLOT( slotLineTool( bool ) ) );

    KToggleAction *m_bezierTool = new KToggleAction( i18n("Bezier"), "beziertool", CTRL+Key_5, actionCollection(), "bezier" );
    m_bezierTool->setExclusiveGroup( "Tools" );
    connect( m_bezierTool, SIGNAL( toggled( bool ) ), this, SLOT( slotBezierTool( bool ) ) );

    KToggleAction *m_rectTool = new KToggleAction( i18n("Rectangle"), "recttool", CTRL+Key_6, actionCollection(), "rectangle" );
    m_rectTool->setExclusiveGroup( "Tools" );
    connect( m_rectTool, SIGNAL( toggled( bool ) ), this, SLOT( slotRectTool( bool ) ) );

    KToggleAction *m_polygonTool = new KToggleAction( i18n("Polygon"), "polygontool", CTRL+Key_7, actionCollection(), "polygon" );
    m_polygonTool->setExclusiveGroup( "Tools" );
    connect( m_polygonTool, SIGNAL( toggled( bool ) ), this, SLOT( slotPolygonTool( bool ) ) );

    KToggleAction *m_ellipseTool = new KToggleAction( i18n("Ellipse"), "ellipsetool", CTRL+Key_8, actionCollection(), "ellipse" );
    m_ellipseTool->setExclusiveGroup( "Tools" );
    connect( m_ellipseTool, SIGNAL( toggled( bool ) ), this, SLOT( slotEllipseTool( bool ) ) );

    KToggleAction *m_textTool = new KToggleAction( i18n("Text"), "frame_text", CTRL+Key_9, actionCollection(), "text" );
    m_textTool->setExclusiveGroup( "Tools" );
    connect( m_textTool, SIGNAL( toggled( bool ) ), this, SLOT( slotTextTool( bool ) ) );

    KToggleAction *m_zoomTool = new KToggleAction( i18n("Zoom"), "viewmag", CTRL+Key_0, actionCollection(), "zoom" );
    m_zoomTool->setExclusiveGroup( "Tools" );
    connect( m_zoomTool, SIGNAL( toggled( bool ) ), this, SLOT( slotZoomTool( bool ) ) );

    m_insertPartTool = new KoPartSelectAction( i18n("Insert Part"), "frame_query", this, SLOT( slotInsertPartTool( ) ), actionCollection(), "insertpart" );
    //m_insertPartTool->setExclusiveGroup( "Tools" );

    // Layout menu
    new KAction( i18n("&Page..."), 0, this, SLOT( slotPage() ), actionCollection(), "page" );

    m_alignToGrid = new KToggleAction( i18n("&Align To Grid"), 0, actionCollection(), "alignToGrid" );
    connect( m_alignToGrid, SIGNAL( toggled( bool ) ), this, SLOT( slotAlignToGrid( bool ) ) );

    m_alignToHelplines = new KToggleAction( i18n("Align &To Helplines"), 0, actionCollection(), "alignToHelplines" );
    connect( m_alignToHelplines, SIGNAL( toggled( bool ) ), this, SLOT( slotAlignToHelplines( bool ) ) );

    // Transform menu
    new KAction( i18n("&Position..."), 0, this, SLOT( slotTransformPosition() ), actionCollection(), "transformPosition" );
    new KAction( i18n("&Dimension..."), 0, this, SLOT( slotTransformDimension() ), actionCollection(), "transformDimension" );
    new KAction( i18n("&Rotation..."), 0, this, SLOT( slotTransformRotation() ), actionCollection(), "transformRotation" );
    new KAction( i18n("&Mirror..."), 0, this, SLOT( slotTransformMirror() ), actionCollection(), "transformMirror" );

    // Arrange menu
    m_distribute = new KAction( i18n("&Align/Distribute..."), 0, this, SLOT( slotDistribute() ), actionCollection(), "distribute" );
    m_toFront = new KAction( i18n("To &Front"), 0, this, SLOT( slotToFront() ), actionCollection(), "toFront" );
    m_toBack = new KAction( i18n("To &Back"), 0, this, SLOT( slotToBack() ), actionCollection(), "toBack" );
    m_forwardOne = new KAction( i18n("Forward &One"), 0, this, SLOT( slotForwardOne() ), actionCollection(), "forwardOne" );
    m_backOne = new KAction( i18n("B&ack One"), 0, this, SLOT( slotBackOne() ), actionCollection(), "backOne" );
    new KAction( i18n("&Group"), 0, this, SLOT( slotGroup() ), actionCollection(), "group" );
    new KAction( i18n("&Ungroup"), 0, this, SLOT( slotUngroup() ), actionCollection(), "ungroup" );
    new KAction( i18n("Text Alon&g Path"), "texttool",0, this, SLOT( slotTextAlongPath() ), actionCollection(), "textAlongPath" );
    new KAction( i18n("&Convert to Curve"), 0, this, SLOT( slotConvertToCurve() ), actionCollection(), "convertToCurve" );

    // Extra menu
    new KAction( i18n("&Blend..."), 0, this, SLOT( slotBlend() ), actionCollection(), "blend" );
    //new KAction( i18n("&Load Palette..."), 0, this, SLOT( slotLoadPalette() ), actionCollection(), "loadPalette" ); // not implemented yet (Werner)

    // Settings
    new KAction( i18n("&Options..."), 0, this, SLOT( slotOptions() ), actionCollection(), "configure" );
    new KAction( i18n("&Ellipse..."), 0, this, SLOT( slotConfigureEllipse() ), actionCollection(), "ellipseSettings");
    new KAction( i18n("P&olygon..."), 0, this, SLOT( slotConfigurePolygon() ), actionCollection(), "polygonSettings");

    kdDebug(38000)<<"inside createMyGUI(): b: "<<time.elapsed()<<" msecs elapsed"<<endl;

    m_viewZoom = new KSelectAction (i18n ("&Zoom"), 0, actionCollection (), "view_zoom");
    QStringList zooms;
    zooms << "50%";
    zooms << "100%";
    zooms << "150%";
    zooms << "200%";
    zooms << "400%";
    zooms << "600%";
    zooms << "800%";
    zooms << "1000%";
    zooms << "100%";

    m_viewZoom->setItems (zooms);
    m_viewZoom->setEditable (true);
    connect(m_viewZoom, SIGNAL(activated(const QString &)),this, SLOT(slotViewZoom(const QString &)));
    m_viewZoom->setCurrentItem(1);

    // Colorbar action
    QValueList<QColor> colorList;
    colorList << Qt::white << Qt::red << Qt::green << Qt::blue << Qt::cyan << Qt::magenta << Qt::yellow
              << Qt::darkRed << Qt::darkGreen << Qt::darkBlue << Qt::darkCyan
              << Qt::darkMagenta << Qt::darkYellow << Qt::white << Qt::lightGray
              << Qt::gray << Qt::darkGray << Qt::black;

    new KColorBarAction( i18n( "&Colorbar" ), 0,
                         this,
                         SLOT( slotBrushChosen( const QColor & ) ),
                         SLOT( slotPenChosen( const QColor & ) ),
                         colorList,
                         actionCollection(), "colorbar" );
    // Node Toolbar
    m_moveNode = new KToggleAction( i18n("Move Node "), "moveNode", 0, actionCollection(), "moveNode" );
    m_moveNode->setExclusiveGroup( "Node" );
    connect( m_moveNode, SIGNAL( toggled( bool ) ), this, SLOT( slotMoveNode( bool ) ) );
    m_newNode = new KToggleAction( i18n("New Node"), "newNode", 0, actionCollection(), "newNode" );
    m_newNode->setExclusiveGroup( "Node" );
    connect( m_newNode, SIGNAL( toggled( bool ) ), this, SLOT( slotNewNode( bool ) ) );
    m_deleteNode = new KToggleAction( i18n("Delete Node "), "deleteNode", 0, actionCollection(), "deleteNode" );
    m_deleteNode->setExclusiveGroup( "Node" );
    connect( m_deleteNode, SIGNAL( toggled( bool ) ), this, SLOT( slotDeleteNode( bool ) ) );
    m_splitLine = new KToggleAction( i18n("Split line"), "split", 0, actionCollection(), "splitLine" );
    m_splitLine->setExclusiveGroup( "Node" );
    connect( m_splitLine, SIGNAL( toggled( bool ) ), this, SLOT( slotSplitLine( bool ) ) );
    m_normal->setChecked( true );
    m_showRuler->setChecked( true );
    m_showHelplines->setChecked(activeDocument()->showHelplines());
    m_alignToHelplines->setChecked(activeDocument()->alignToHelplines());
    m_showGrid->setChecked(activeDocument()->showGrid());
    m_alignToGrid->setChecked(activeDocument()->snapToGrid());
    m_selectTool->setChecked( true );
    if(m_pDoc->isReadWrite())
    {
      tcontroller->toolSelected( Tool::ToolSelect);
    }
    setUndoStatus (false, false);
    connect (&cmdHistory, SIGNAL(changed(bool, bool)),SLOT(setUndoStatus(bool, bool)));
    // Disable node actions
    toolActivated(Tool::ToolEditPoint,false);
    kdDebug(38000)<<"inside createMyGUI(): c: "<<time.elapsed()<<" msecs elapsed"<<endl;

}

void KIllustratorView::setupCanvas()
{
    MeasurementUnit mu = PStateManager::instance ()->defaultMeasurementUnit ();

    QGridLayout* layout = new QGridLayout(this,3,3);

    hRuler = new Ruler (m_pDoc, Ruler::Horizontal, mu, this);
    hRuler->setMeasurementUnit(PStateManager::instance()->defaultMeasurementUnit());
    hRuler->setCursor(Qt::pointingHandCursor);
    vRuler = new Ruler (m_pDoc, Ruler::Vertical, mu, this);
    vRuler->setMeasurementUnit(PStateManager::instance()->defaultMeasurementUnit());
    vRuler->setCursor(Qt::pointingHandCursor);

    connect(hRuler,SIGNAL(rmbPressed()),this,SLOT(popupForRulers()));
    connect(vRuler,SIGNAL(rmbPressed()),this,SLOT(popupForRulers()));

    TabBar *tabBar = new TabBar(this, this);
//    tabBar->setActiveTab(1);

    QScrollBar* vBar = new QScrollBar(QScrollBar::Vertical, this);
    QScrollBar* hBar = new QScrollBar(QScrollBar::Horizontal, this);

    canvas = new Canvas (m_pDoc->gdoc(), 72.0, hBar, vBar, this);
    canvas->center();
    canvas->setCursor(Qt::crossCursor);

    m_pTabBarFirst = newIconButton("tab_first", false, this);
    connect( m_pTabBarFirst,SIGNAL(clicked()), tabBar, SLOT(scrollFirst()));
    m_pTabBarLeft = newIconButton("tab_left", false, this);
    connect( m_pTabBarLeft, SIGNAL(clicked()), tabBar, SLOT(scrollLeft()));
    m_pTabBarRight = newIconButton("tab_right", false, this);
    connect( m_pTabBarRight, SIGNAL(clicked()), tabBar, SLOT(scrollRight()));
    m_pTabBarLast = newIconButton("tab_last", false, this);
    connect( m_pTabBarLast, SIGNAL(clicked()), tabBar, SLOT(scrollLast()));

    QHBoxLayout* tabLayout = new QHBoxLayout();
    tabLayout->addWidget(m_pTabBarFirst);
    tabLayout->addWidget(m_pTabBarLeft);
    tabLayout->addWidget(m_pTabBarRight);
    tabLayout->addWidget(m_pTabBarLast);
    tabLayout->addWidget(tabBar);
    tabLayout->addWidget(hBar);

    layout->addWidget(canvas,1,1);
    layout->addWidget(hRuler,0,1);
    layout->addWidget(vRuler,1,0);
    layout->addMultiCellWidget(vBar,0,1,2,2);
    layout->addMultiCellLayout(tabLayout,2,2,0,1);

/*    mToolDockManager = new ToolDockManager(canvas);

    //Layer Panel
    mLayerPanel = new LayerPanel(this);
    mLayerDockBase = mToolDockManager->createToolDock(mLayerPanel, i18n("Layers"));*/
    m_showLayers = new KToggleAction( i18n("Layers Panel"), "layers", CTRL+Key_L, actionCollection(), "layers" );

    connect( m_showLayers, SIGNAL(toggled(bool)), this, SLOT(createLayerPanel(bool)));
//    connect( m_showLayers, SIGNAL(toggled(bool)), mLayerDockBase, SLOT(makeVisible(bool)));
/*    connect(mLayerDockBase, SIGNAL(visibleChange(bool)), SLOT(slotLayersPanel(bool)));
    slotLayersPanel(false);*/

    connect(canvas,SIGNAL(visibleAreaChanged(const QRect&)),hRuler,SLOT(updateVisibleArea(const QRect&)));
    connect(canvas,SIGNAL(visibleAreaChanged(const QRect&)),vRuler,SLOT(updateVisibleArea(const QRect&)));

    connect (canvas, SIGNAL(zoomFactorChanged (float)),
                      this, SLOT(slotZoomFactorChanged(float)));

    connect(canvas,SIGNAL(mousePositionChanged(int,int)),hRuler,SLOT(updatePointer(int,int)));
    connect(canvas,SIGNAL(mousePositionChanged(int,int)),vRuler,SLOT(updatePointer(int,int)));

    connect(canvas,SIGNAL(rightButtonAtSelectionClicked(int,int)),this,SLOT(popupForSelection()));

    // helpline creation
    connect (hRuler, SIGNAL (drawHelpline(int, int, bool)),
             canvas, SLOT(drawTmpHelpline(int, int, bool)));
    connect (vRuler, SIGNAL (drawHelpline(int, int, bool)),
             canvas, SLOT(drawTmpHelpline(int, int, bool)));
    connect (hRuler, SIGNAL (addHelpline(int, int, bool)),
             this, SLOT(slotAddHelpline(int, int, bool)));
    connect (vRuler, SIGNAL (addHelpline(int, int, bool)),
             this, SLOT(slotAddHelpline(int, int, bool)));

    setFocusPolicy(QWidget::StrongFocus);
    setFocusProxy(canvas);

    tcontroller = new ToolController (this);
    connect(tcontroller,SIGNAL(operationDone(Tool::ToolID)),this,SLOT(resetTools(Tool::ToolID)));
    connect(tcontroller,SIGNAL(modeSelected(Tool::ToolID,const QString&)),this,SLOT(showCurrentMode(Tool::ToolID,const QString&)));
    connect(tcontroller,SIGNAL(activated(Tool::ToolID,bool)), this, SLOT(toolActivated(Tool::ToolID,bool)));
    connect(tcontroller,SIGNAL(partSelected(Tool::ToolID,GObject*)),this, SLOT(activatePart(Tool::ToolID,GObject*)));

    tcontroller->registerTool ( new SelectionTool (&cmdHistory) );
    tcontroller->registerTool (editPointTool = new EditPointTool (&cmdHistory));
    tcontroller->registerTool (new FreeHandTool (&cmdHistory));
    tcontroller->registerTool (new PolylineTool (&cmdHistory));
    tcontroller->registerTool (new BezierTool (&cmdHistory));
    tcontroller->registerTool (new RectangleTool (&cmdHistory));
    tcontroller->registerTool (new PolygonTool (&cmdHistory));
    tcontroller->registerTool (new OvalTool (&cmdHistory));
    tcontroller->registerTool (new TextTool (&cmdHistory));
    tcontroller->registerTool (mZoomTool = new ZoomTool (&cmdHistory));
    tcontroller->registerTool (new PathTextTool (&cmdHistory));
    tcontroller->registerTool (insertPartTool = new InsertPartTool (&cmdHistory));

    canvas->setToolController(tcontroller);
    canvas->installEventFilter(this);
}

void KIllustratorView::readConfig()
{
   KConfig* config = KIllustratorFactory::global()->config ();
   config->setGroup("Panels");
   bool b=config->readBoolEntry("Enabled",true);
/*   if (!b)
      mLayerDockBase->makeVisible(b);
   else*/
   if (b)
      createLayerPanel(false);
}

void KIllustratorView::writeConfig()
 {
   KConfig* config = KIllustratorFactory::global()->config ();
   config->setGroup("Panels");
   config->writeEntry("Enabled",m_showLayers->isChecked());
   config->sync();
 }

void KIllustratorView::showCurrentMode (Tool::ToolID, const QString& msg)
{
   KoMainWindow * tmpKo = shell();
   if (tmpKo)
      tmpKo->statusBarLabel()->setText(msg);
   //statusbar->changeItem (msg, 2);
}

void KIllustratorView::setUndoStatus(bool undoPossible, bool redoPossible)
{
    m_undo->setEnabled( undoPossible );
    m_redo->setEnabled( redoPossible );

    QString label = i18n ("Undo");
    if (undoPossible)
        label += " " + cmdHistory.getUndoName ();
    m_undo->setText( label );

    label = i18n ("Redo");
    if (redoPossible)
        label += " " + cmdHistory.getRedoName ();
    m_redo->setText( label );
}

//void KIllustratorView::resizeEvent(QResizeEvent* ) {
/*    if(m_bShowRulers) {
        hRuler->setGeometry(20, 0, width()-20, 20);
        vRuler->setGeometry(0, 20, 20, height()-20);
        canvas->setGeometry(20, 20, width()-20, height()-20);
    }
    else
        canvas->setGeometry(0, 0, width(), height());*/
//}

void KIllustratorView::updateReadWrite( bool /*readwrite*/ )
{
}

void KIllustratorView::showTransformationDialog( int id )
{
    TransformationDialog *transformationDialog = new TransformationDialog (&cmdHistory);
    QObject::connect (m_pDoc->gdoc(), SIGNAL (selectionChanged ()),
                      transformationDialog, SLOT (update ()));
    transformationDialog->setDocument ( m_pDoc->gdoc() );
    transformationDialog->showTab (id);
}

void KIllustratorView::setupPrinter( KPrinter &printer )
{
    canvas->setupPrinter( printer );
}

void KIllustratorView::print( KPrinter &printer )
{
    canvas->print( printer );
}


void KIllustratorView::editInsertObject ()
{
    m_pDoc->gdoc()->activePage()->unselectAllObjects();
    KoDocumentEntry docEntry = m_insertPartTool->documentEntry();
    if (docEntry.isEmpty ())
        return;

    insertPartTool->setPartEntry (docEntry);
    // ####### Torben
    // tcontroller->toolSelected (m_idActiveTool = TD_TOOL_INSERTPART);
}


/*
void KIllustratorView::setPenColor (long int id) {
  short int red, green, blue;
  bool fill;
  // ####### Torben
  // if(! m_vColorBar->getRgbColor (id, red, green, blue, fill))
  // return;

  GObject::OutlineInfo oInfo;
  oInfo.mask = GObject::OutlineInfo::Color | GObject::OutlineInfo::Style;
  oInfo.color = QColor (red, green, blue);
  oInfo.style = fill ? SolidLine : NoPen;

  GObject::FillInfo fInfo;
  fInfo.mask = 0;

  if (! m_pDoc->gdoc()->selectionIsEmpty ()) {
    SetPropertyCmd *cmd = new SetPropertyCmd (m_pDoc->gdoc(), oInfo, fInfo);
    cmdHistory.addCommand (cmd, true);
  }
  else {
    int result =
      KMessageBox::warningYesNo(this,
                            i18n ("This action will set the default\n"
                                  "properties for new objects !\n"
                                  "Would you like to do it ?"));
    if (result == KmessageBox::Yes)
      GObject::setDefaultOutlineInfo (oInfo);
  }
}
*/
/*
void KIllustratorView::setFillColor (long int id) {
  short int red, green, blue;
  bool fill;
  // ###### Torben
  // if(! m_vColorBar->getRgbColor (id, red, green, blue, fill))
  // return;

  GObject::OutlineInfo oInfo;
  oInfo.mask = 0;

  GObject::FillInfo fInfo;
  fInfo.mask = GObject::FillInfo::Color | GObject::FillInfo::FillStyle;
  fInfo.color = QColor (red, green, blue);
  fInfo.fstyle = fill ? GObject::FillInfo::SolidFill :
    GObject::FillInfo::NoFill;

  if (! m_pDoc->gdoc()->selectionIsEmpty ()) {
    SetPropertyCmd *cmd = new SetPropertyCmd (m_pDoc->gdoc(), oInfo, fInfo);
    cmdHistory.addCommand (cmd, true);
  }
  else {
    int result =
      KMessageBox::warningYesNo(this,
                           i18n ("This action will set the default\n"
                                 "properties for new objects !\n"
                                 "Would you like to do it ?"));
    if (result == KMessageBox::Yes)
      GObject::setDefaultFillInfo (fInfo);
  }
}
*/

void KIllustratorView::slotConfigurePolygon()
{
   tcontroller->configureTool (Tool::ToolPolygon);
}

void KIllustratorView::slotConfigureEllipse()
{
   tcontroller->configureTool (Tool::ToolEllipse);
}

void KIllustratorView::popupForSelection ()
{
   if (objMenu==0)
   {
      objMenu = new KPopupMenu();
      m_copy->plug( objMenu );
      m_cut->plug( objMenu );
      m_duplicate->plug(objMenu);
      m_delete->plug(objMenu);
      objMenu->insertSeparator ();
      m_distribute->plug( objMenu );
      m_toFront->plug( objMenu );
      m_toBack->plug( objMenu );
      m_forwardOne->plug( objMenu );
      m_backOne->plug( objMenu );
      objMenu->insertSeparator ();
      m_properties->plug( objMenu );
   }
   objMenu->popup( QCursor::pos () );
}

void KIllustratorView::popupForRulers()
{
    if(!m_pDoc->isReadWrite())
        return;

   if (!rulerMenu)
   {
      rulerMenu = new KPopupMenu();
      m_showGrid->plug(rulerMenu);
      m_showHelplines->plug(rulerMenu);
      m_alignToGrid->plug(rulerMenu);
      m_alignToHelplines->plug(rulerMenu);
   }
   rulerMenu->popup( QCursor::pos () );
}

void KIllustratorView::resetTools(Tool::ToolID id)
{
   if (id==Tool::ToolPathText)
   {
      m_selectTool->setChecked( true );
      tcontroller->toolSelected( Tool::ToolSelect);
   }
   //m_selectTool->setEnabled( true );
}

void KIllustratorView::activatePart (Tool::ToolID, GObject *obj)
{
   if (obj->isA ("GPart"))
   {
      GPart *part = (GPart *) obj;
      part->activate(this);
      /*
       setFramesToParts ();
       int xoff = 1, yoff = 1;
       if (m_bShowRulers) {
       xoff += 30;
       yoff += 30;
       }

       //part->activate (xoff, yoff);
       setFocusProxy (part->getView ());
       QWidget::setFocusPolicy (QWidget::StrongFocus);
       //   part->getView ()->setFocusPolicy (QWidget::StrongFocus);
       //   part->getView ()->setFocus ();*/
   }
}


GDocument* KIllustratorView::activeDocument()
{
    return m_pDoc->gdoc();
}


void KIllustratorView::insertPartSlot( KIllustratorChild *, GPart *)
 {

 }

void KIllustratorView::changeChildGeometrySlot(KIllustratorChild *)
 {

 }


QString KIllustratorView::getExportFileName (FilterManager *filterMgr)
{
    QString extension;

    if (! lastExport.isEmpty ()) {
        int pos = lastExport.findRev ('.', -1, false);
        if (pos != -1) {
            extension = lastExport.right (lastExport.length () - pos - 1);
        }
    }
    QString filter = filterMgr->exportFilters (extension.latin1());

    KFileDialog *dlg = new KFileDialog (lastExportDir,
                                        filter, this,
                                        "file dia", true);
    dlg->setCaption (i18n ("Save As"));
    if (! lastExport.isEmpty ()) {
        dlg->setSelection (lastExport);
    }
    QString filename;

    if (dlg->exec() == QDialog::Accepted) {
        KURL url = dlg->selectedURL ();
        if (!url.isLocalFile())
            KMessageBox::sorry( 0, i18n("Remote URLs not supported") );
        filename = url.path();
        if ( QFileInfo( url.path() ).extension().isEmpty() ) {
            // assume that the pattern ends with .extension
            QString s( dlg->currentFilter() );
            QString extension = s.mid( s.find( "." ) );
            extension = extension.left( extension.find( " " ) );
            filename+=extension;
        }
        lastExportDir = url.directory();
    }

    delete dlg;
    return filename;
}

// ---------------------------------------- actions

void KIllustratorView::slotExport()
{
    FilterManager* filterMgr = FilterManager::instance ();
    QString filter = filterMgr->exportFilters ();

    QString fname = getExportFileName (filterMgr);

    if (! fname.isEmpty ())
    {
        FilterInfo* filterInfo = filterMgr->findFilter (fname.latin1(),
                                                        FilterInfo::FKind_Export);

        if (filterInfo)
        {
            ExportFilter* filter = filterInfo->exportFilter ();
            if (filter->setup (m_pDoc->gdoc(), filterInfo->extension ().latin1()))
            {
                filter->setOutputFileName (fname);
                filter->exportToFile (m_pDoc->gdoc());
                lastExport = fname;
            }
            else
                KMessageBox::error(this, i18n ("Cannot export to file"),
                                    i18n ("KIllustrator Error"));
        }
        else
            KMessageBox::error(this, i18n ("Unknown export format"),
                               i18n ("KIllustrator Error"));
    }
    resetTools ();
}

void KIllustratorView::slotInsertBitmap()
{
    KURL url = KFileDialog::getOpenURL
               (lastBitmapDir, i18n("*.jpg *.jpeg *.JPG *.JPEG | JPEG Images\n"
                                    "*.png | PNG Images\n"
                                    "*.xbm | X11 Bitmaps\n"
                                    "*.xpm | X11 Pixmaps"),
                this);
    if (!url.isEmpty() && !url.isLocalFile())
        KMessageBox::sorry( 0, i18n("Remote URLs not supported") );
    QString fname = url.path();
    if (! fname.isEmpty ()) {
        QFileInfo finfo (fname);
        lastBitmapDir = finfo.dirPath ();
        InsertPixmapCmd *cmd = new InsertPixmapCmd (m_pDoc->gdoc(), fname);
        cmdHistory.addCommand (cmd, true);
    }
}

void KIllustratorView::slotInsertBitmap(const QString &filename)
{
    if (! filename.isEmpty ()) {
        QFileInfo finfo (filename);
        lastBitmapDir = finfo.dirPath ();
        InsertPixmapCmd *cmd = new InsertPixmapCmd (m_pDoc->gdoc(), filename);
        cmdHistory.addCommand (cmd, true);
    }
}

void KIllustratorView::slotInsertClipart()
{
    KURL url = KFileDialog::getOpenURL( lastClipartDir,
                                        i18n("*.wmf *.WMF | Windows Metafiles"), this);
    if (!url.isEmpty() && !url.isLocalFile())
        KMessageBox::sorry( 0, i18n("Remote URLs not supported") );
    QString fname = url.path();
    if ( !fname.isEmpty ())
    {
        QFileInfo finfo (fname);
        lastClipartDir = finfo.dirPath ();
        InsertClipartCmd *cmd = new InsertClipartCmd (m_pDoc->gdoc(), fname);
        cmdHistory.addCommand (cmd, true);
    }
}

void KIllustratorView::slotCopy()
{
    cmdHistory.addCommand (new CopyCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotPaste()
{
    cmdHistory.addCommand (new PasteCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotCut()
{
    cmdHistory.addCommand (new CutCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotUndo()
{
    cmdHistory.undo ();
    resetTools ();
}

void KIllustratorView::slotRedo()
{
    cmdHistory.redo ();
    resetTools ();
}

void KIllustratorView::slotDuplicate()
{
    cmdHistory.addCommand (new DuplicateCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotDelete()
{
    cmdHistory.addCommand (new DeleteCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotSelectAll()
{
    m_selectTool->setChecked( true );
    tcontroller->toolSelected( Tool::ToolSelect);
    m_pDoc->gdoc()->activePage()->selectAllObjects ();
}

void KIllustratorView::slotProperties()
{
    int result = KMessageBox::Yes;

    if (m_pDoc->gdoc()->activePage()->selectionIsEmpty ())
    {
        result = KMessageBox::warningYesNo(this,
                                           i18n ("This action will set the default\n"
                                                 "properties for new objects!\n"
                                                 "Would you like to do it?"),
                                            i18n("Warning"));
    }
    if (result == KMessageBox::Yes)
        PropertyEditor::edit( &cmdHistory, m_pDoc->gdoc() );
}

void KIllustratorView::slotOutline( )
{
    if ( m_outline->isChecked() )
    {
        canvas->setOutlineMode (true);
    }
    else
        m_outline->setChecked( true ); // always one has to be checked !
}

void KIllustratorView::slotNormal()
{
    if ( m_normal->isChecked() )
    {
        canvas->setOutlineMode (false);
    }
    else
        m_normal->setChecked( true ); // always one has to be checked !
}

void KIllustratorView::slotShowRuler( bool b )
{
    m_bShowRulers = b;

    if (m_bShowRulers)
    {
        hRuler->show ();
        vRuler->show ();
    }
    else
    {
        hRuler->hide ();
        vRuler->hide ();
    }
    // recalculate layout
    resizeEvent (0L);
}

void KIllustratorView::slotShowGrid( bool b )
{
  if (b!=activeDocument()->showGrid())
     activeDocument()->showGrid( b );
  canvas->update();
}

void KIllustratorView::slotShowHelplines( bool b )
{
   if (b!=activeDocument()->showHelplines())
   {
      activeDocument()->showHelplines( b );
   }
   canvas->update();
}

void KIllustratorView::slotPage()
{
    KoPageLayout pLayout = m_pDoc->gdoc()->activePage()->pageLayout ();
    KoHeadFoot header;

    if (KoPageLayoutDia::pageLayout (pLayout, header, FORMAT_AND_BORDERS))
        m_pDoc->gdoc()->activePage()->setPageLayout (pLayout);
}

void KIllustratorView::slotAlignToGrid( bool b )
{
   if (b!=activeDocument()->snapToGrid())
      activeDocument()->snapToGrid( b );
}

void KIllustratorView::slotAlignToHelplines( bool b )
{
   if (b!=activeDocument()->alignToHelplines())
     activeDocument()->alignToHelplines( b );
}

void KIllustratorView::slotTransformPosition()
{
    showTransformationDialog( 0 );
}

void KIllustratorView::slotTransformDimension()
{
    showTransformationDialog( 1 );
}

void KIllustratorView::slotTransformRotation()
{
    showTransformationDialog( 2 );
}

void KIllustratorView::slotTransformMirror()
{
    showTransformationDialog( 3 );
}

void KIllustratorView::slotDistribute()
{
    AlignmentDialog::alignSelection (m_pDoc->gdoc(), &cmdHistory);
}

void KIllustratorView::slotToFront()
{
    cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_ToFront), true);
}

void KIllustratorView::slotToBack()
{
    cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_ToBack), true);
}

void KIllustratorView::slotForwardOne()
{
    cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_ForwardOne), true);
}

void KIllustratorView::slotBackOne()
{
    cmdHistory.addCommand (new ReorderCmd (m_pDoc->gdoc(), RP_BackwardOne), true);
}

void KIllustratorView::slotGroup()
{
    cmdHistory.addCommand (new GroupCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotUngroup()
{
    cmdHistory.addCommand (new UngroupCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotTextAlongPath()
{
   tcontroller->toolSelected (Tool::ToolPathText);
}

void KIllustratorView::slotConvertToCurve()
{
    if ( !m_pDoc->gdoc()->activePage()->selectionIsEmpty() )
        cmdHistory.addCommand (new ToCurveCmd (m_pDoc->gdoc()), true);
}

void KIllustratorView::slotBlend()
{
    if ( m_pDoc->gdoc()->activePage()->selectionCount () == 2)
    {
        int steps = BlendDialog::getNumOfSteps ();
        if (steps > 0)
            cmdHistory.addCommand (new BlendCmd (m_pDoc->gdoc(), steps), true);
    }
    else
        KMessageBox::information(this,
                                 i18n("You have to select exactly two objects."),
                                 i18n("Blending"), "blending");
}

void KIllustratorView::slotOptions()
{
   if (OptionDialog::setup(activeDocument())==QDialog::Accepted)
   {
      hRuler->setMeasurementUnit(PStateManager::instance()->defaultMeasurementUnit());
      vRuler->setMeasurementUnit(PStateManager::instance()->defaultMeasurementUnit());
      PStateManager::instance()->saveDefaultSettings();
   }
}

void KIllustratorView::slotBrushChosen( const QColor & c )
{
    // #### Torben: ..... hmmmmm
    bool fill = true;

    GObject::OutlineInfo oInfo;
    //oInfo.mask = 0;
    oInfo.color = c;
    oInfo.mask = GObject::OutlineInfo::Color | GObject::OutlineInfo::Style;

    GObject::FillInfo fInfo;
    fInfo.mask = GObject::FillInfo::Color | GObject::FillInfo::FillStyle;

    fInfo.color = c;
    fInfo.fstyle = fill ? GObject::FillInfo::SolidFill :
                GObject::FillInfo::NoFill;

    if ( !m_pDoc->gdoc()->activePage()->selectionIsEmpty () )
    {
        SetPropertyCmd *cmd = new SetPropertyCmd (m_pDoc->gdoc(), oInfo, fInfo);
        cmdHistory.addCommand (cmd, true);
    }
    else
    {
        int result = KMessageBox::warningYesNo(this,
                                               i18n ("This action will set the default\n"
                                                     "properties for new objects!\n"
                                                     "Would you like to do it?"),
                                               i18n("Warning"));
        if (result == KMessageBox::Yes)
            GObject::setDefaultFillInfo (fInfo);
    }
}

void KIllustratorView::slotPenChosen( const QColor & c  )
{
    // #### Torben: ..... hmmmmm
    bool fill = true;

    GObject::OutlineInfo oInfo;
    oInfo.mask = GObject::OutlineInfo::Color | GObject::OutlineInfo::Style;
    oInfo.color = c                        ;
    oInfo.style = fill ? SolidLine : NoPen;

    GObject::FillInfo fInfo;
    fInfo.mask = 0;

    if (! m_pDoc->gdoc()->activePage()->selectionIsEmpty () )
    {
        SetPropertyCmd *cmd = new SetPropertyCmd (m_pDoc->gdoc(), oInfo, fInfo);
        cmdHistory.addCommand (cmd, true);
    }
    else
    {
        int result = KMessageBox::warningYesNo(this,
                                               i18n ("This action will set the default\n"
                                                     "properties for new objects!\n"
                                                     "Would you like to do it?"),
                                                i18n("Warning"));
        if (result == KMessageBox::Yes)
            GObject::setDefaultOutlineInfo (oInfo);
    }
}

void KIllustratorView::slotSelectTool( bool b )
{
    if ( b )
       tcontroller->toolSelected( Tool::ToolSelect );
}

bool KIllustratorView::eventFilter(QObject *o, QEvent *e)
{
    if(!tcontroller ||!tcontroller->getActiveTool() )
        return false;

    if ((e==0) || (tcontroller->getActiveTool()->id()==Tool::ToolSelect))
      return false;

   if ((o==canvas) && (e->type()==QEvent::MouseButtonPress))
   {
      QMouseEvent *me=(QMouseEvent*)e;
      if (me->button()==RightButton)
      {
         m_selectTool->setChecked(true);
         slotSelectTool(true);
      }
   }
   return false;
}

void KIllustratorView::slotPointTool(bool b)
{
   if ( b )
      tcontroller->toolSelected( Tool::ToolEditPoint );
}

void KIllustratorView::toolActivated(Tool::ToolID id, bool b )
{
   if (id==Tool::ToolEditPoint)
   {
      m_moveNode->setEnabled( b );
      m_newNode->setEnabled( b );
      m_deleteNode->setEnabled( b );
      m_splitLine->setEnabled( b );

      if ( b )
      {
         slotMoveNode( true );
         m_moveNode->setChecked(true);
      }
   }
}

void KIllustratorView::slotFreehandTool( bool b )
{
    if ( b )
       tcontroller->toolSelected( Tool::ToolFreeHand );
}

void KIllustratorView::slotLineTool( bool b )
{
    if ( b )
       tcontroller->toolSelected( Tool::ToolLine);
}

void KIllustratorView::slotBezierTool( bool b )
{
    if ( b )
       tcontroller->toolSelected( Tool::ToolBezier);
}

void KIllustratorView::slotRectTool( bool b )
{
    if ( b )
       tcontroller->toolSelected( Tool::ToolRectangle );
}

void KIllustratorView::slotPolygonTool( bool b )
{
    if ( b )
       tcontroller->toolSelected( Tool::ToolPolygon );
}

void KIllustratorView::slotEllipseTool( bool b )
{
    if ( b )
       tcontroller->toolSelected( Tool::ToolEllipse );
}

void KIllustratorView::slotTextTool( bool b )
{
    if ( b )
       tcontroller->toolSelected( Tool::ToolText );
}

void KIllustratorView::slotZoomTool( bool b  )
{
    if ( b )
       tcontroller->toolSelected( Tool::ToolZoom );
}

void KIllustratorView::slotInsertPartTool( )
{
 editInsertObject ();
 tcontroller->toolSelected( Tool::ToolInsertPart );
}

void KIllustratorView::slotMoveNode( bool b )
{
    if ( b )
        editPointTool->setMode (EditPointTool::MovePoint);
}

void KIllustratorView::slotNewNode( bool b )
{
    if ( b )
        editPointTool->setMode (EditPointTool::InsertPoint);
}

void KIllustratorView::slotDeleteNode( bool b )
{
    if ( b )
        editPointTool->setMode (EditPointTool::RemovePoint);
}

void KIllustratorView::slotSplitLine( bool b )
{
    if ( b )
        editPointTool->setMode (EditPointTool::Split);
}

void KIllustratorView::slotLayersPanel(bool b)
 {
  mLayerPanel->manageDocument(activeDocument());
  m_showLayers->setChecked(b);
  //((KToggleAction*)actionCollection()->action("layers"))->setChecked(b);
 }

void KIllustratorView::refreshLayerPanel()
{
   mLayerPanel->manageDocument(activeDocument());
}

void KIllustratorView::slotLoadPalette () {

}

void KIllustratorView::slotViewZoom (const QString& s)
{
   //kdDebug(38000)<<"slotViewZoom(): -"<<s<<"-"<<endl;
   QString z (s);
   z = z.replace (QRegExp ("%"), "");
   z = z.simplifyWhiteSpace ();
   float zoom = z.toFloat () / 100.0;
   //if (zoom != canvas->getZoomFactor ())
   canvas->setZoomFactor (zoom);
}

void KIllustratorView::slotZoomIn()
{
   mZoomTool->zoomIn(getCanvas());
}

void KIllustratorView::slotZoomOut()
{
   mZoomTool->zoomOut(getCanvas());
}

//when we get here, the canvas is already zoomed
void KIllustratorView::slotZoomFactorChanged(float factor)
{
   vRuler->setZoomFactor(factor,canvas->visibleArea().left(),canvas->visibleArea().top());
   hRuler->setZoomFactor(factor,canvas->visibleArea().left(),canvas->visibleArea().top());
   QStringList list=m_viewZoom->items();
   QString f=QString::number(qRound(factor*100.0));
   int i=0;
   //kdDebug(38000)<<"slotZoomFactorChanged(): -"<<f<<"-"<<endl;
   for(QValueList<QString>::Iterator it=list.begin(); it!=list.end(); ++it, ++i)
   {
      //kdDebug(38000)<<"slotZoomFactorChanged(): it -"<<(*it).left((*it).length()-1)<<"-"<<endl;
      if((*it).left((*it).length()-1)==f)
      {
         m_viewZoom->setCurrentItem(i);
         return;
      }
   }
   //current zoom value not found in list
   f+='%';
   m_viewZoom->changeItem(8,f);
   m_viewZoom->setCurrentItem(8);

}

void KIllustratorView::slotAddHelpline(int x, int y, bool d) {
    m_showHelplines->setChecked(true);
    canvas->addHelpline(x, y, d);
}

/*void KIllustratorView::slotSettingsChanged()
{
    hRuler->setMeasurementUnit(PStateManager::instance()->defaultMeasurementUnit());
    vRuler->setMeasurementUnit(PStateManager::instance()->defaultMeasurementUnit());
}*/

void KIllustratorView::slotViewResize()
 {
/*  int x = scrollview->viewport()->width()-canvas->width();
  int y = scrollview->viewport()->height()-canvas->height();
  if(x < 0)
   x = 0;
  if(y < 0)
   y = 0;
  canvas->move(x/2,y/2);*/
 }

QButton* KIllustratorView::newIconButton( const char* file, bool kbutton, QWidget* parent )
{
  if (!parent)
    parent = this;
  QPixmap *pixmap = new QPixmap(BarIcon(file));
  QButton *pb(0);
  if (!kbutton)
    pb = new QPushButton(parent);
//  else
//    pb = new QToolButton(parent);
  if (pixmap)
    pb->setPixmap(*pixmap);
  pb->setFixedSize(16,16);
  return pb;
}

void KIllustratorView::createLayerPanel(bool calledFromAction)
{
   if (mToolDockManager!=0)
      return;
   disconnect(m_showLayers,SIGNAL(toggled(bool)),this,SLOT(createLayerPanel(bool)));
   mToolDockManager = new ToolDockManager(canvas);
   //Layer Panel
   mLayerPanel = new LayerPanel(this);
   mLayerDockBase = mToolDockManager->createToolDock(mLayerPanel, i18n("Layers"));
   connect(mLayerDockBase, SIGNAL(visibleChange(bool)), SLOT(slotLayersPanel(bool)));
   connect( m_showLayers, SIGNAL(toggled(bool)), mLayerDockBase, SLOT(makeVisible(bool)));
   slotLayersPanel(false);
   mLayerPanel->stateOfButton();
   if (calledFromAction)
      mLayerDockBase->makeVisible(calledFromAction);
}


#include <KIllustrator_view.moc>
