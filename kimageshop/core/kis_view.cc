/*
 *  kis_view.cc - part of Krayon
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
 *                1999 Michael Koch    <koch@kde.org>
 *                1999 Carsten Pfeiffer <pfeiffer@kde.org>
 *
 *  Copyright (c) 2000 John Califf <jcaliff@compuzone.net>
 * 
 *             
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

// qt includes
#include <qevent.h>
#include <qpainter.h>
#include <qbutton.h>
#include <qscrollbar.h>
#include <qstringlist.h>
#include <qclipboard.h>

// kde includes
#include <kmessagebox.h>
#include <kruler.h>
#include <kaction.h>
#include <klocale.h>
#include <khelpmenu.h>
#include <kaboutdata.h>
#include <kstdaction.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kapp.h>

// core classes
#include "kis_view.h"
#include "kis_doc.h"
#include "kis_util.h"
#include "kis_canvas.h"
#include "kis_framebuffer.h"
#include "kis_painter.h"
#include "kis_sidebar.h"
#include "kis_tabbar.h"
#include "kis_krayon.h"
#include "kis_brush.h"
#include "kis_pattern.h"
#include "kis_tool.h"
#include "kis_factory.h"
#include "kis_gradient.h"
#include "kis_pluginserver.h"
#include "kis_selection.h"
#include "kfloatingdialog.h"

// sidebar 
#include "kis_brushchooser.h"
#include "kis_patternchooser.h"
#include "kis_krayonchooser.h"
#include "kis_layerview.h"
#include "kis_channelview.h"

// dialogs
#include "kis_dlg_gradient.h"
#include "kis_dlg_gradienteditor.h"
#include "kis_dlg_preferences.h"
#include "kis_dlg_new.h"
#include "kis_dlg_new_layer.h"

// tools
#include "kis_tool_select_rectangular.h"
#include "kis_tool_select_polygonal.h"
#include "kis_tool_select_elliptical.h"
#include "kis_tool_select_contiguous.h"

#include "kis_tool_paste.h"
#include "kis_tool_move.h"
#include "kis_tool_zoom.h"
#include "kis_tool_brush.h"
#include "kis_tool_airbrush.h"
#include "kis_tool_pen.h"
#include "kis_tool_gradient.h"
#include "kis_tool_line.h"
#include "kis_tool_polyline.h"
#include "kis_tool_rectangle.h"
#include "kis_tool_ellipse.h"
#include "kis_tool_colorpicker.h"
#include "kis_tool_colorchanger.h"
#include "kis_tool_eraser.h"
#include "kis_tool_fill.h"
#include "kis_tool_stamp.h"

// debug
#include <kdebug.h>
#include "kis_timer.h"

//#define TEST_PIXMAP
//#define KISBarIcon( x ) BarIcon( x, KisFactory::global() )

/*
    KisView - constructor.  What is a KoView?  A widget, but it 
    also seems to be a hybrid or composite of several koffice objects 
    designed to contain the document and display it. Every koffice app 
    does it differently but krayon's is the exemplary view which sets 
    the standard.   A document can have more than one view. 
*/
KisView::KisView( KisDoc* doc, QWidget* parent, const char* name )
    : KoView( doc, parent, name )
    , m_pDoc( doc )
    , m_zoomFactor( 1.0 )

{
    setInstance( KisFactory::global() );
    setXMLFile( "krayon.rc" );

    QObject::connect( m_pDoc, SIGNAL( docUpdated( ) ),
                    this, SLOT( slotDocUpdated ( ) ) );
                    
    QObject::connect( m_pDoc, SIGNAL( docUpdated( const QRect& ) ),
                    this, SLOT( slotDocUpdated ( const QRect& ) ) );

    m_fg = KisColor::black();
    m_bg = KisColor::white();
 
    m_xPaintOffset = 0;
    m_yPaintOffset = 0; 
 
    buttonIsDown = false;

    m_pTool     = 0L;
    m_pBrush    = 0L;
    m_pPattern  = 0L;
    m_pPixmap   = 0L;
                 
    setupPainter();
    setupCanvas();
#ifdef TEST_PIXMAP
    setupPixmap();
#endif    
    setupScrollBars();
    setupRulers();
    setupTabBar();
    setupActions();
    setupDialogs();
    setupSideBar();
    setupTools();
}

/*
    KisView destructor.  Delete objects created apart from the
    widget and its children.
*/
KisView::~KisView()
{
    if(m_pPixmap) delete m_pPixmap;
}

/*
    Set up painter object for use of QPainter methods to draw
    into KisLayer memory
*/
void KisView::setupPainter()
{
    m_pPainter = new KisPainter(m_pDoc, this);
}


/*
    Canvas for document (image) area - it's just a plain QWidget used to
    pass signals (messages) on to the tools and to create an area on
    which to show the content of the document (the image).  Note that
    while the depth of the image is not limited, the depth of the 
    pixmap displayed on the widget is limited to the hardware display 
    depth, which is often 16 bit even though the KisImage is 32 bit and
    can eventually be extended to 64 bit.
*/
void KisView::setupCanvas()
{
    m_pCanvas = new KisCanvas(this, "kis_canvas");
        
    QObject::connect( m_pCanvas, SIGNAL( mousePressed( QMouseEvent* ) ),
                    this, SLOT( canvasGotMousePressEvent ( QMouseEvent* ) ) );

    QObject::connect( m_pCanvas, SIGNAL( mouseMoved( QMouseEvent* ) ),
                    this, SLOT( canvasGotMouseMoveEvent ( QMouseEvent* ) ) );

    QObject::connect( m_pCanvas, SIGNAL( mouseReleased (QMouseEvent* ) ),
		    this, SLOT( canvasGotMouseReleaseEvent ( QMouseEvent* ) ) );

    QObject::connect( m_pCanvas, SIGNAL( gotPaintEvent (QPaintEvent* ) ),
		    this, SLOT( canvasGotPaintEvent ( QPaintEvent* ) ) );
}


/*
    Canvas pixmap for offscreen paint device - experimental way
    to speed up rendering of zoomed views.  Not used and not needed 
    anymore because a better method was discovered! But, ther may
    be a use for it with guidelines and grids, so keep it.  It uses
    very little memory until the pixmap is given a size.
*/
void KisView::setupPixmap()
{
    m_pPixmap = new QPixmap();
}


/*
    SideBar - has tabs for brushes, layers, channels, etc.
    Nonstandard, but you who sets the standards?  Unrelieved 
    uniformity is the mark of small minds.
*/
void KisView::setupSideBar()
{
    m_pSideBar = new KisSideBar(this, "kis_sidebar");

    // krayon chooser
    m_pKrayonChooser = new KisKrayonChooser(this);
    m_pKrayon = m_pKrayonChooser->currentKrayon();
    QObject::connect(m_pKrayonChooser, SIGNAL(selected(const KisKrayon *)),
			   this, SLOT(slotSetKrayon(const KisKrayon*)));

    m_pKrayonChooser->setCaption(i18n("Krayons"));
    m_pSideBar->plug(m_pKrayonChooser);

    // brush chooser
    m_pBrushChooser = new KisBrushChooser(this);
    m_pBrush = m_pBrushChooser->currentBrush();
    QObject::connect(m_pBrushChooser, SIGNAL(selected(const KisBrush *)),
			   this, SLOT(slotSetBrush(const KisBrush*)));

    m_pBrushChooser->setCaption(i18n("Brushes"));
    m_pSideBar->plug(m_pBrushChooser);

    // pattern chooser
    m_pPatternChooser = new KisPatternChooser(this);
    m_pPattern = m_pPatternChooser->currentPattern();
    QObject::connect(m_pPatternChooser, SIGNAL(selected(const KisPattern *)),
			   this, SLOT(slotSetPattern(const KisPattern*)));

    m_pPatternChooser->setCaption(i18n("Patterns"));
    m_pSideBar->plug(m_pPatternChooser);

   // gradient chooser
    m_pGradientChooser = new QWidget(this);
    //m_pPattern = m_pPatternChooser->currentPattern();
    //QObject::connect(m_pPaletternChooser, SIGNAL(selected(const KisPattern *)),
    //   this, SLOT(slotSetPattern(const KisPattern*)));
    m_pGradientChooser->setCaption("Gradients");
    m_pSideBar->plug(m_pGradientChooser);

   // image chooser
    m_pImageChooser = new QWidget(this);
    //m_pPattern = m_pPatternChooser->currentPattern();
    //QObject::connect(m_pPaletternChooser, SIGNAL(selected(const KisPattern *)),
    //   this, SLOT(slotSetPattern(const KisPattern*)));
    m_pImageChooser->setCaption("Images");
    m_pSideBar->plug(m_pImageChooser);

   // palette chooser
    m_pPaletteChooser = new QWidget(this);
    //m_pPattern = m_pPatternChooser->currentPattern();
    //QObject::connect(m_pPaletternChooser, SIGNAL(selected(const KisPattern *)),
    //   this, SLOT(slotSetPattern(const KisPattern*)));

    m_pPaletteChooser->setCaption("Palettes");
    m_pSideBar->plug(m_pPaletteChooser);
    
    // layer view
    m_pLayerView = new KisLayerView(m_pDoc, this);
    m_pLayerView->setCaption(i18n("Layers"));
    m_pSideBar->plug(m_pLayerView);

    // channel view
    m_pChannelView = new KisChannelView(m_pDoc, this);
    m_pChannelView->setCaption(i18n("Channels"));
    m_pSideBar->plug(m_pChannelView);

    // activate brushes tab
    m_pSideBar->slotActivateTab(i18n("Brushes"));

    // init sidebar
    m_pSideBar->slotSetBrush(*m_pBrush);
    m_pSideBar->slotSetFGColor(m_fg);
    m_pSideBar->slotSetBGColor(m_bg);

    connect(m_pSideBar, SIGNAL(fgColorChanged(const KisColor&)), this,
		  SLOT(slotSetFGColor(const KisColor&)));
    connect(m_pSideBar, SIGNAL(bgColorChanged(const KisColor&)), this,
		  SLOT(slotSetBGColor(const KisColor&)));

    connect(this, SIGNAL(fgColorChanged(const KisColor&)), m_pSideBar,
		  SLOT(slotSetFGColor(const KisColor&)));
    connect(this, SIGNAL(bgColorChanged(const KisColor&)), m_pSideBar,
		  SLOT(slotSetBGColor(const KisColor&)));

    m_side_bar->setChecked( true );
}

/*
    setupScrollBars - setting them up is easy.  Now why don't the
    darned scroll bars always show up when they should?
*/
void KisView::setupScrollBars()
{
    m_pVert = new QScrollBar( QScrollBar::Vertical, this );
    m_pHorz = new QScrollBar( QScrollBar::Horizontal, this );

    m_pVert->setGeometry(width()-16, 20, 16, height()-36);
    m_pHorz->setGeometry(20, height()-16, width()-36, 16);
    m_pHorz->setValue(0);
    m_pVert->setValue(0);

    QObject::connect(m_pVert, 
        SIGNAL(valueChanged(int)), this, SLOT(scrollV(int)));
    QObject::connect(m_pHorz, 
        SIGNAL(valueChanged(int)), this, SLOT(scrollH(int)));
}

/*
    Where are the numbers on the ruler?  What about a grid aligned
    to the rulers. We also need to change the tick marks as zoom
    levels change. Coming....
*/
void KisView::setupRulers()
{
    m_pHRuler = new KRuler(Qt::Horizontal, this);
    m_pVRuler = new KRuler(Qt::Vertical, this);

    m_pHRuler->setGeometry(20, 0, width()-20, 20);
    m_pVRuler->setGeometry(0, 20, 20, height()-20);

    m_pVRuler->setRulerMetricStyle(KRuler::Pixel);
    m_pHRuler->setRulerMetricStyle(KRuler::Pixel);

    m_pVRuler->setFrameStyle(QFrame::Panel | QFrame::Raised);
    m_pHRuler->setFrameStyle(QFrame::Panel | QFrame::Raised);
    m_pHRuler->setLineWidth(1);
    m_pVRuler->setLineWidth(1);
}


/*
    TabBar for the image(s) - one tab per image. This Nonstandard(tm) 
    sidebar with custom tabbed widgets violates koffice style guidelines, 
    but krayon's users want a gui that works!  They have no patience with
    kde's toolbar/sdi religion and its ayatollahs.
*/
void KisView::setupTabBar()
{
    // tabbar
    m_pTabBar = new KisTabBar(this, m_pDoc);
    m_pTabBar->slotImageListUpdated();

    QObject::connect( m_pTabBar, SIGNAL( tabSelected( const QString& ) ),
		    m_pDoc, SLOT( setCurrentImage( const QString& ) ) );

    QObject::connect( m_pDoc, SIGNAL( imageListUpdated() ),
		    m_pTabBar, SLOT( slotImageListUpdated( ) ) );

    // tabbar control buttons
    m_pTabFirst = new QPushButton( this );
    m_pTabFirst->setPixmap( QPixmap( BarIcon( "tab_first" ) ) );
    QObject::connect( m_pTabFirst, SIGNAL( clicked() ), m_pTabBar, SLOT( slotScrollFirst() ) );

    m_pTabLeft = new QPushButton( this );
    m_pTabLeft->setPixmap( QPixmap( BarIcon( "tab_left" ) ) );
    QObject::connect( m_pTabLeft, SIGNAL( clicked() ), m_pTabBar, SLOT( slotScrollLeft() ) );

    m_pTabRight = new QPushButton( this );
    m_pTabRight->setPixmap( QPixmap( BarIcon( "tab_right" ) ) );
    QObject::connect( m_pTabRight, SIGNAL( clicked() ), m_pTabBar, SLOT( slotScrollRight() ) );

    m_pTabLast = new QPushButton( this );
    m_pTabLast->setPixmap( QPixmap( BarIcon( "tab_last" ) ) );
    QObject::connect( m_pTabLast, SIGNAL( clicked() ), m_pTabBar, SLOT( slotScrollLast() ) );
}

/*
    setupToos - create tools. Possibly there is no need to create all 
    these objects until we first click on them - only create default tool 
    to start. However, these don't take long to set up.  The entire 
    kis_view takes less than 2 seconds to initiate from start to finish, 
    so why not.
*/
void KisView::setupTools()
{
    m_pZoomTool = new ZoomTool(this);
    m_pMoveTool = new MoveTool(m_pDoc, this);

    m_pRectangularSelectTool = new RectangularSelectTool( m_pDoc, this, m_pCanvas );
    m_pPolygonalSelectTool = new PolygonalSelectTool( m_pDoc, this, m_pCanvas );
    m_pEllipticalSelectTool = new EllipticalSelectTool( m_pDoc, this, m_pCanvas );
    m_pContiguousSelectTool = new ContiguousSelectTool( m_pDoc, this, m_pCanvas );

    m_pPasteTool = new PasteTool( m_pDoc, this, m_pCanvas );
    m_pBrushTool = new BrushTool(m_pDoc, this, m_pBrush);
    m_pAirBrushTool = new AirBrushTool(m_pDoc, this, m_pBrush);
    m_pPenTool = new PenTool(m_pDoc, this, m_pCanvas, m_pBrush);
    m_pEraserTool = new EraserTool(m_pDoc, this, m_pBrush);
    m_pColorPicker = new ColorPicker(m_pDoc, this);
    m_pColorChangerTool = new ColorChangerTool(m_pDoc, this);
    m_pGradientTool = new GradientTool( m_pDoc, this, m_pCanvas, m_pGradient);
    m_pLineTool = new LineTool( m_pDoc, this, m_pCanvas );
    m_pPolyLineTool = new PolyLineTool( m_pDoc, this, m_pCanvas );
    m_pRectangleTool = new RectangleTool( m_pDoc, this, m_pCanvas );
    m_pEllipseTool = new EllipseTool( m_pDoc, this, m_pCanvas );
    m_pFillTool = new FillTool( m_pDoc, this );
    m_pStampTool = new StampTool(m_pDoc, this, m_pCanvas, m_pPattern);
    
    // start with brush as active tool
    m_tool_brush->setChecked( true );
    slotSetBrush(m_pBrush);
    activateTool(m_pBrushTool);
}

/*
    setupDialogs -jwc- todo: gradient dialog should be a tool 
    options dialog setupDialogs should be used for docking and undocking
    tabbed widgets in the sidebar, which can also be dialogs
*/
void KisView::setupDialogs()
{
    // gradient dialog
    m_pGradientDialog = new GradientDialog( m_pDoc, this );
    m_pGradientDialog->resize( 206, 185 );
    m_pGradientDialog->move( 200, 290 );
    m_pGradientDialog->hide();
    connect( m_pGradientDialog, SIGNAL( sigClosed() ), 
        SLOT( updateToolbarButtons() ) );

    // gradient editor dialog
    m_pGradientEditorDialog = new GradientEditorDialog( m_pDoc, this );
    m_pGradientEditorDialog->resize( 400, 200 );
    m_pGradientEditorDialog->move( 100, 190 );
    m_pGradientEditorDialog->hide();
    connect( m_pGradientEditorDialog, SIGNAL( sigClosed() ), 
        SLOT( updateToolbarButtons() ) );

    updateToolbarButtons();
    
}

/*
    Actions - these seem to be menu actions, toolbar actions
    and keyboard actions.  Any action can take on any of these forms,
    at least.  However, using Kde's brain-damaged xmlGui because it
    is the "right(tm)" thing to do, slots cannot take any paramaters 
    and each handler must have its own method, greatly increasing 
    code size and precluding consolidation or related actions, not to
    mention slower startup required by interpretation of rc files.
    For every action there is an equal and opposite reaction.  
*/

void KisView::setupActions()
{
    // history actions
    m_undo = KStdAction::undo( this, SLOT( undo() ),
        actionCollection(), "undo");
        
    m_redo = KStdAction::redo( this, SLOT( redo() ),
        actionCollection(), "redo");

    // navigation actions
    new KAction( i18n("Refresh Canvas"),
        "reload", 0, this, SLOT( slotDocUpdated() ),
        actionCollection(), "refresh_canvas" );
    
    new KAction( i18n("Panic Button"),
        "stop", 0, this, SLOT( slotHalt() ),
        actionCollection(), "panic_button" );
    /*    
    new KAction( i18n("Gimp"),
        "wilbur", 0, this, SLOT( slotGimp() ),
        actionCollection(), "gimp" );
    */
    
    // selection actions
    m_cut = KStdAction::cut( this, SLOT( cut() ),
        actionCollection(), "cut");

    m_copy = KStdAction::copy( this, SLOT( copy() ),
        actionCollection(), "copy");

    m_paste = KStdAction::paste( this, SLOT( paste() ),
        actionCollection(), "paste");

    new KAction( i18n("Remove selection"),
        "remove", 0, this, SLOT( removeSelection() ),
        actionCollection(), "remove");

    m_crop = new KAction( i18n("Copy selection to new layer"),
        "crop", 0,  this, SLOT( crop() ),
        actionCollection(), "crop");

    m_select_all = KStdAction::selectAll( this, SLOT( selectAll() ),
        actionCollection(), "select_all");

    m_unselect_all = new KAction( i18n("Select None"),
        0, this, SLOT( unSelectAll() ),
        actionCollection(), "select_none");

    // import/export actions
    new KAction( i18n("Import Image"),
        "wizard", 0, this, SLOT( import_image() ),
        actionCollection(), "import_image" );

    new KAction( i18n("Export Image"),
        "wizard", 0, this, SLOT( export_image() ),
        actionCollection(), "export_image" );

    // view actions
    new KAction( i18n("Zoom &in"),
        "viewmag+", 0, this, SLOT( zoom_in() ),
        actionCollection(), "zoom_in" );

    new KAction( i18n("Zoom &out"),
        "viewmag-", 0, this, SLOT( zoom_out() ),
        actionCollection(), "zoom_out" );

    // tool settings actions
    
    m_dialog_gradient = new KToggleAction( i18n("&Gradient Dialog"),
        "gradient_dialog", 0, this, SLOT( dialog_gradient() ),
        actionCollection(), "dialog_gradient");

    m_dialog_gradienteditor = new KToggleAction( i18n("Gradient &Editor"),
        "gradienteditor_dialog",  0, this, SLOT( dialog_gradienteditor() ),
        actionCollection(), "dialog_gradienteditor");
    

    // tool actions - lots of them

    m_tool_select_rectangular = new KToggleAction( i18n( "&Rectangular select" ),
        "rectangular", 0, this,  SLOT( tool_select_rectangular() ),
        actionCollection(), "tool_select_rectangular" );

    m_tool_select_rectangular->setExclusiveGroup( "tools" );

    m_tool_select_polygonal = new KToggleAction( i18n( "&Polygonal select" ),
        "handdrawn" , 0, this, SLOT( tool_select_polygonal() ),
        actionCollection(), "tool_select_polygonal" );

    m_tool_select_polygonal->setExclusiveGroup( "tools" );

    m_tool_select_elliptical = new KToggleAction( i18n( "&Elliptical select" ),
        "elliptical" , 0, this, SLOT( tool_select_elliptical() ),
        actionCollection(), "tool_select_elliptical" );

    m_tool_select_elliptical->setExclusiveGroup( "tools" );

    m_tool_select_contiguous = new KToggleAction( i18n( "&Contiguous select" ),
        "contiguous" , 0, this, SLOT( tool_select_contiguous() ),
        actionCollection(), "tool_select_contiguous" );

    m_tool_select_contiguous->setExclusiveGroup( "tools" );

    m_tool_move = new KToggleAction( i18n("&Move tool"),
        "move", 0, this, SLOT( tool_move() ),
        actionCollection(), "tool_move");

    m_tool_move->setExclusiveGroup( "tools" );

    m_tool_zoom = new KToggleAction( i18n("&Zoom tool"),
        "viewmag", 0, this, SLOT( tool_zoom() ),
        actionCollection(), "tool_zoom");

    m_tool_zoom->setExclusiveGroup( "tools" );

    m_tool_pen = new KToggleAction( i18n("&Pen tool"),
        "pencil", 0, this, SLOT( tool_pen() ),
        actionCollection(), "tool_pen");

    m_tool_pen->setExclusiveGroup( "tools" );

    m_tool_brush = new KToggleAction( i18n("&Brush tool"),
        "paintbrush", 0, this, SLOT( tool_brush() ),
        actionCollection(), "tool_brush");

    m_tool_brush->setExclusiveGroup( "tools" );

    m_tool_airbrush = new KToggleAction( i18n("&Airbrush tool"),
        "airbrush", 0, this, SLOT( tool_airbrush() ),
        actionCollection(), "tool_airbrush");

    m_tool_airbrush->setExclusiveGroup( "tools" );

    m_tool_fill = new KToggleAction( i18n("&Filler tool"),
        "fill", 0, this, SLOT( tool_fill() ),
        actionCollection(), "tool_fill");

    m_tool_fill->setExclusiveGroup( "tools" );

    m_tool_stamp = new KToggleAction( i18n("&Stamp (Pattern) tool"),
        "stamp", 0, this, SLOT( tool_stamp() ),
        actionCollection(), "tool_stamp");

    m_tool_stamp->setExclusiveGroup( "tools" );

    m_tool_eraser = new KToggleAction( i18n("&Eraser tool"),
        "eraser", 0, this, SLOT( tool_eraser() ),actionCollection(),
        "tool_eraser");

    m_tool_eraser->setExclusiveGroup( "tools" );

    m_tool_colorpicker = new KToggleAction( i18n("&Color picker"),
        "colorpicker", 0, this, SLOT( tool_colorpicker() ),
        actionCollection(), "tool_colorpicker");

    m_tool_colorpicker->setExclusiveGroup( "tools" );

    m_tool_colorchanger = new KToggleAction( i18n("Color changer"),
        "colorize", 0, this, SLOT( tool_colorchanger() ),
        actionCollection(), "tool_colorchanger");

    m_tool_colorchanger->setExclusiveGroup( "tools" );

    m_tool_gradient = new KToggleAction( i18n("&Gradient tool"),
        "blend", 0, this, SLOT( tool_gradient() ),
        actionCollection(), "tool_gradient");

    m_tool_gradient->setExclusiveGroup( "tools" );

    m_tool_line = new KToggleAction( i18n("&Line tool"),
        "line", 0, this, SLOT( tool_line() ),
        actionCollection(), "tool_line");

    m_tool_line->setExclusiveGroup( "tools" );

    m_tool_polyline = new KToggleAction( i18n("&Polyline tool"),
        "polyline", 0, this, SLOT( tool_polyline() ),
        actionCollection(), "tool_polyline");

    m_tool_polyline->setExclusiveGroup( "tools" );

    m_tool_rectangle = new KToggleAction( i18n("&Rectangle tool"),
        "rectangle", 0, this, SLOT( tool_rectangle() ),
        actionCollection(), "tool_rectangle");

    m_tool_rectangle->setExclusiveGroup( "tools" );

    m_tool_ellipse = new KToggleAction( i18n("&Ellipse tool"),
        "ellipse", 0, this, SLOT( tool_ellipse() ),
        actionCollection(), "tool_ellipse");

    m_tool_ellipse->setExclusiveGroup( "tools" );

    m_tool_paste = new KToggleAction( i18n("&Paste tool"),
        "editpaste", 0, this, SLOT( tool_paste() ),
        actionCollection(), "tool_paste");

    m_tool_paste->setExclusiveGroup( "tools" );


    (void) new KAction( i18n("&Current Tool Properties..."),
        "configure", 0, this, SLOT( tool_properties() ),
        actionCollection(), "current_tool_properties" );

    // layer actions

    (void) new KAction( i18n("&Add layer..."),
        0, this, SLOT( insert_layer() ),
        actionCollection(), "insert_layer" );

    (void) new KAction( i18n("&Remove layer..."),
        0, this, SLOT( remove_layer() ),
        actionCollection(), "remove_layer" );

    (void) new KAction( i18n("&Link/Unilnk layer..."),
        0, this, SLOT( link_layer() ),
        actionCollection(), "link_layer" );

    (void) new KAction( i18n("&Hide/Show layer..."),
        0, this, SLOT( hide_layer() ),
        actionCollection(), "hide_layer" );

    (void) new KAction( i18n("&Next layer..."),
        "forward", 0, this, SLOT( next_layer() ),
        actionCollection(), "next_layer" );

    (void) new KAction( i18n("&Previous layer..."),
        "back", 0, this, SLOT( previous_layer() ),
        actionCollection(), "previous_layer" );

    (void) new KAction( i18n("Layer Properties..."),
        0, this, SLOT( layer_properties() ),
        actionCollection(), "layer_properties" );

    (void) new KAction( i18n("I&nsert image as layer..."),
        0, this, SLOT( insert_image_as_layer() ),
        actionCollection(), "insert_image_as_layer" );

    (void) new KAction( i18n("Save layer as image..."),
        0, this, SLOT( save_layer_as_image() ),
        actionCollection(), "save_layer_as_image" );

    // layer transformations - should be generic, for selection too
    
    (void) new KAction( i18n("Scale layer smoothly"),
        0, this, SLOT( layer_scale_smooth() ),
        actionCollection(), "layer_scale_smooth");

    (void) new KAction( i18n("Scale layer - keep palette"),
        0, this, SLOT( layer_scale_rough() ),
        actionCollection(), "layer_scale_rough");

    m_layer_rotate180 = new KAction( i18n("Rotate &180"),
        0, this, SLOT( layer_rotate180() ),
        actionCollection(), "layer_rotate180");

    m_layer_rotate270 = new KAction( i18n("Rotate &270"),
        0, this, SLOT( layer_rotateleft90() ),
        actionCollection(), "layer_rotateleft90");

    m_layer_rotate90 = new KAction( i18n("Rotate &90"),
        0, this, SLOT( layer_rotateright90() ),
        actionCollection(), "layer_rotateright90");

    m_layer_mirrorX = new KAction( i18n("Mirror &X"),
        0, this, SLOT( layer_mirrorX() ),
        actionCollection(), "layer_mirrorX");

    m_layer_mirrorY = new KAction( i18n("Mirror &Y"),
        0, this, SLOT( layer_mirrorY() ),
        actionCollection(), "layer_mirrorY");

    // image actions

    (void) new KAction( i18n("Add new image"),
        0, this, SLOT( add_new_image_tab() ),
        actionCollection(), "add_new_image_tab");

    (void) new KAction( i18n("Remove current image"),
        0, this, SLOT( remove_current_image_tab() ),
        actionCollection(), "remove_current_image_tab");

    (void) new KAction( i18n("Merge &all layers"),
        0, this, SLOT( merge_all_layers() ),
        actionCollection(), "merge_all_layers");

    (void) new KAction( i18n("Merge &visible layers"),
        0, this, SLOT( merge_visible_layers() ),
        actionCollection(), "merge_visible_layers");

    (void) new KAction( i18n("Merge &linked layers"),
        0, this, SLOT( merge_linked_layers() ),
        actionCollection(), "merge_linked_layers");

    // setting actions

    /*
    (void) KStdAction::showMenubar( this, SLOT( showMenubar() ),
        actionCollection(), "show_menubar" );

    (void) KStdAction::showToolbar( this, SLOT( showToolbar() ),
        actionCollection(), "show_toolbar" );

    (void) KStdAction::showStatusbar( this, SLOT( showStatusbar() ),
        actionCollection(), "show_statusbar" );
    */

   m_toggle_paint_offset = new KToggleAction( i18n("Toggle Paint Offset"),
        "border_outline", 0, this, SLOT( slotSetPaintOffset() ),
        actionCollection(), "toggle_paint_offset" );
 
    m_side_bar = new KToggleAction( i18n("Show/Hide Sidebar"),
        "ok", 0, this, SLOT( showSidebar() ),
        actionCollection(), "show_sidebar" );

    m_float_side_bar = new KToggleAction( i18n("Dock/Undock Sidebar"),
         "attach", 0, this, SLOT( floatSidebar() ),
        actionCollection(), "float_sidebar" );

    m_lsidebar = new KToggleAction( i18n("Left/Right Sidebar"),
         "view_right", 0, this, SLOT( leftSidebar() ),
        actionCollection(), "left_sidebar" );

    (void) KStdAction::saveOptions( this, SLOT( saveOptions() ),
        actionCollection(), "save_options" );

    (void) new KAction( i18n("Krayon Preferences"),
        "edit", 0, this, SLOT( preferences() ),
        actionCollection(), "preferences");

	 // krayon box toolbar actions - these will be used only
     // to dock and undock wideget in the krayon box

      m_dialog_colors = new KToggleAction( i18n("&Colors"),
        "color_dialog", 0, this, SLOT( dialog_colors() ),
        actionCollection(), "colors_dialog");

      m_dialog_krayons = new KToggleAction( i18n("&Krayons"),
        "krayon_box", 0, this, SLOT( dialog_krayons() ),
        actionCollection(), "krayons_dialog");

      m_dialog_brushes = new KToggleAction( i18n("Brushes"),
        "brush_dialog", 0, this, SLOT( dialog_brushes() ),
        actionCollection(), "brushes_dialog");

      m_dialog_patterns = new KToggleAction( i18n("Patterns"),
        "pattern_dialog", 0, this, SLOT( dialog_patterns() ),
        actionCollection(), "patterns_dialog");

      m_dialog_layers = new KToggleAction( i18n("Layers"),
        "layer_dialog", 0, this, SLOT( dialog_layers() ),
        actionCollection(), "layers_dialog");

      m_dialog_channels = new KToggleAction( i18n("Channels"),
        "channel_dialog", 0, this, SLOT( dialog_channels() ),
        actionCollection(), "channels_dialog");
     
     // help actions - these are standard kde actions

      m_helpMenu = new KHelpMenu( this );

    (void) KStdAction::helpContents( m_helpMenu, SLOT( appHelpActivated() ), 
        actionCollection(), "help_contents" );
        
    (void) KStdAction::whatsThis( m_helpMenu, SLOT( contextHelpActivated() ),
        actionCollection(), "help_whatsthis" );
        
    (void) KStdAction::reportBug( m_helpMenu, SLOT( reportBug() ), 
        actionCollection(), "help_bugreport" );
        
    (void) KStdAction::aboutApp( m_helpMenu, SLOT( aboutApplication() ), 
        actionCollection(), "help_about" );

    // disable at startup unused actions

    m_undo->setEnabled( false );
    m_redo->setEnabled( false );

    m_layer_rotate180->setEnabled( false );
    m_layer_rotate270->setEnabled( false );
    m_layer_rotate90->setEnabled( false );
    m_layer_mirrorX->setEnabled( false );
    m_layer_mirrorY->setEnabled( false );
    
    m_tool_select_polygonal->setEnabled( false );
    m_tool_select_elliptical->setEnabled( false );
    m_tool_select_contiguous->setEnabled( false );
}

/*
    slotHalt - try to restore reasonable defaults for a user
    who may have pushed krayon beyond its limits or the
    limits of his/her hardware and system memory!  This can happen
    when someone sets a ridiculously high zoom factor which
    requires a supercomputer for all the floating point 
    calculatons.  Krayon is not idiot proof!
*/
void KisView::slotHalt()
{
    KMessageBox::error(NULL, 
        "STOP! In the name of Love ...", "System Error", FALSE); 
        
    zoom(0, 0, 1.0);
    slotUpdateImage();            
}

/*
    slotGimp - a copout for the weak of mind and faint
    of heart.  Wiblur sucks, remember that!
*/
void KisView::slotGimp()
{
    KMessageBox::error(NULL, 
        "Have you lost your mind?", "User Error", FALSE); 
    // save current image, export to xcf, open in gimp - coming!
}

/*
    slotTabSelected - these refer to the tabs for images
*/
void KisView::slotTabSelected(const QString& name)
{
    m_pDoc->setCurrentImage(name);
    resizeEvent(0L);
}

/*
    showScrollBars - force showing of scrollbars for
    the view with a fake resize event
*/
void KisView::showScrollBars()
{
    if(isVisible())
    {
        int w = width();
        int h = height();

        resize(w-1, h-1);
        resize(w, h);
    }    
}


/*
    resizeEvent - only via a resize event are things shown
    in the view.  To start, nothing is shown.  The first
    resize comes when the objects to be shown are created.
    This methold handles much which is not obvious, reducing
    the need for so many methods to explicitly resize things.
*/
void KisView::resizeEvent(QResizeEvent*)
{
    // sidebar width right or left side
    int rsideW = 0; 
    int lsideW = 0;
    
    // ruler thickness
    int ruler = 20; 

    // tab bar dimensions
    int tbarOffset = 64; 
    int tbarBtnH = 16;
    int tbarBtnW = 16;
    
    // show or hide sidebar
    if(!m_pSideBar)
    {
        rsideW = 0;
        lsideW = 0;
    }    
    else
    {
        if(m_side_bar->isChecked() && !m_float_side_bar->isChecked())
        {
            if(m_lsidebar->isChecked())
            {
                rsideW = 0;
                lsideW = m_pSideBar->width();
            }    
            else     
            {    
                rsideW = m_pSideBar->width();
                lsideW = 0;
            }
        }   
        else
        {
           rsideW = 0;
           lsideW = 0;
        }   
    }        
    
    // sidebar geometry - only set if visible and NOT free floating
    if (m_pSideBar && !m_float_side_bar->isChecked() && m_side_bar->isChecked())
    {
        if(m_lsidebar->isChecked())
            m_pSideBar->setGeometry(0, 0, lsideW, height());
        else
            m_pSideBar->setGeometry(width() - rsideW, 0, rsideW, height()); 
            
        m_pSideBar->show();
    }
    
    // ruler geometry
    m_pHRuler->setGeometry(ruler + lsideW, 0, 
        width() - ruler - rsideW - lsideW, ruler);
    m_pVRuler->setGeometry( 0 + lsideW, ruler, 
        ruler, height() - (ruler + tbarBtnH));

    // tabbar control buttons
    m_pTabFirst->setGeometry(0 + lsideW, height() - tbarBtnH, 
        tbarBtnW, tbarBtnH);
    m_pTabFirst->show();
    
    m_pTabLeft->setGeometry(tbarBtnW + lsideW, height() - tbarBtnH, 
        tbarBtnW, tbarBtnH);
    m_pTabLeft->show();
    
    m_pTabRight->setGeometry(2 * tbarBtnW + lsideW, height() - tbarBtnH, 
        tbarBtnW, tbarBtnH);
    m_pTabRight->show();
    
    m_pTabLast->setGeometry(3 * tbarBtnW + lsideW, height() - tbarBtnH, 
        tbarBtnW, tbarBtnH);
    m_pTabLast->show();

    // KisView height/width - ruler height/width
    int drawH = height() - ruler - tbarBtnH;
    int drawW = width() - ruler - lsideW - rsideW;

    // doc width and height are exactly same as the
    // current image's width and height
    int docW = docWidth();    
    int docH = docHeight();

    // adjust for zoom scaling - the higher the scaling,
    // the larger the image in direct proportion
    docW = (int)((zoomFactor()) * docW);
    docH = (int)((zoomFactor()) * docH);
    
    // resize the pixmap for drawing zoomed doc content.
    // this must be done *before* canvas is shown

#ifdef TEST_PIXMAP
    m_pPixmap->resize(drawW, drawH);
#endif

     // we need no scrollbars
    if (docH <= drawH && docW <= drawW)
    {
        m_pVert->hide();
        m_pHorz->hide();
        m_pVert->setValue(0);
        m_pHorz->setValue(0);

        m_pCanvas->setGeometry(ruler + lsideW, ruler, drawW, drawH);
        m_pCanvas->show();

        m_pTabBar->setGeometry(tbarOffset + lsideW, height() - tbarBtnH, 
           width() - rsideW - lsideW - tbarOffset, tbarBtnH);
        m_pTabBar->show();
    }
    // we need a horizontal scrollbar only
    else if (docH <= drawH) 
    {
        m_pVert->hide();
        m_pVert->setValue(0);

        //m_pHorz->setRange(0, docW - drawW);
        m_pHorz->setRange(0, (int)((docW - drawW)/zoomFactor()));
        m_pHorz->setGeometry(tbarOffset + lsideW + (width() - rsideW -lsideW - tbarOffset)/2, 
            height() - tbarBtnH, (width() - rsideW -lsideW - tbarOffset)/2, tbarBtnH);
        m_pHorz->show();

        m_pCanvas->setGeometry(ruler + lsideW, ruler, drawW, drawH);
        m_pCanvas->show();

        m_pTabBar->setGeometry(tbarOffset + lsideW, height() - tbarBtnH, 
           (width() - rsideW - lsideW - tbarOffset)/2, tbarBtnH);
        m_pTabBar->show();
    }
    // we need a vertical scrollbar only
    else if(docW <= drawW) 
    {
        m_pHorz->hide();
        m_pHorz->setValue(0);

        //m_pVert->setRange(0, docH - drawH);
        m_pVert->setRange(0, (int)((docH - drawH)/zoomFactor()));                
        m_pVert->setGeometry(width() - tbarBtnW - rsideW, ruler, 
            tbarBtnW, height() - (ruler + tbarBtnH));
        m_pVert->show();
      
        m_pCanvas->setGeometry(ruler + lsideW, ruler, drawW - tbarBtnW, drawH);
        m_pCanvas->show();
       
        m_pTabBar->setGeometry(tbarOffset + lsideW, height() - tbarBtnH, 
           width() - rsideW -lsideW - tbarOffset, tbarBtnH);
        m_pTabBar->show();
    }
    // we need both scrollbars
    else 
    {
        //m_pVert->setRange(0, docH - drawH);
        m_pVert->setRange(0, (int)((docH - drawH)/zoomFactor()));        
        m_pVert->setGeometry(width() - tbarBtnW - rsideW, ruler, 
            tbarBtnW, height() - (ruler + tbarBtnH));    
        m_pVert->show();
      
        //m_pHorz->setRange(0, docW - drawW);
        m_pHorz->setRange(0, (int)((docW - drawW)/zoomFactor()));        
        m_pHorz->setGeometry(tbarOffset + lsideW + (width() - rsideW -lsideW - tbarOffset)/2, 
           height() - tbarBtnH, (width() - rsideW -lsideW - tbarOffset)/2, tbarBtnH);
        m_pHorz->show();
      
        m_pCanvas->setGeometry(ruler + lsideW, ruler, drawW - tbarBtnW, drawH);
        m_pCanvas->show();

        m_pTabBar->setGeometry(tbarOffset + lsideW, height() - tbarBtnH, 
                (width() - rsideW -lsideW - tbarOffset)/2, tbarBtnH);
        m_pTabBar->show();
    }

    // ruler geometry - need to adjust for zoom factor -jwc-
    
    // ruler ranges
    m_pVRuler->setRange(0, docH);
    m_pHRuler->setRange(0, docW);

    // ruler offset
    if(m_pVert->isVisible())
        m_pVRuler->setOffset(m_pVert->value());
    else
        m_pVRuler->setOffset(-yPaintOffset());

    if(m_pHorz->isVisible())
        m_pHRuler->setOffset(m_pHorz->value());
    else
        m_pHRuler->setOffset(-xPaintOffset());
        
    m_pHRuler->show();    
    m_pVRuler->show();
    
    //kdDebug() << "Canvas width: "   << m_pCanvas->width() 
    //   << " Canvas Height: " << m_pCanvas->height() << endl;
}

/*
    updateReadWrite - for the functionally illiterate
*/
void KisView::updateReadWrite( bool /*readwrite*/ )
{
}


/* 
    scrollH - This sends a paint event to canvas
    when you scroll horizontally, handled in canvasGotPaintEvent().  
    The ruler offset is adjusted to the scrollbar value.  Its scale
    needs to adjusted for zoom factor here.
*/
void KisView::scrollH(int)
{
    
    m_pHRuler->setOffset(m_pHorz->value());
    m_pCanvas->repaint();
}

/* 
    scrollH - This sends a paint event to canvas
    when you scroll vertically, handled in canvasGotPaintEvent().  
    The ruler offset is adjusted to the scrollbar value.  Its scale
    needs to adjusted for zoom factor here.
*/
void KisView::scrollV(int)
{
    m_pVRuler->setOffset(m_pVert->value());
    m_pCanvas->repaint();
}

/*
    slotUpdateImage - a cheap hack to mark the entire image 
    dirty to force a repaint AND to send a fake resize event 
    to force the view to show the scrollbars
*/
void KisView::slotUpdateImage()
{
    KisImage* img = m_pDoc->current();
    if(img)
    {
         QRect updateRect(0, 0, img->width(), img->height());
        img->markDirty(updateRect);
        showScrollBars();
    }   
}

/*
    slotDocUpdated - response to a signal from 
    the document that content has changed and that we
    need to update the canvas -  no size of update area given, 
    so show entire image (that portion which should be shown 
    in canvas viewport).
*/
void KisView::slotDocUpdated()
{
    // kdDebug() << "slotDocUpdated()" << endl;
    m_pCanvas->repaint();
}

/*
    slotDocUpdated - response to a signal from 
    the document that content has changed and that we
    need to update the canvas -  a definite update area 
    is given, so only update that rectangle's contents.
*/
void KisView::slotDocUpdated(const QRect& rect)
{
    //kdDebug() << "slotDocUpdated(const QRect& rect)" << endl;
    KisImage* img = m_pDoc->current();
    if (!img) return;

    QRect ur = rect;
    
    ur = ur.intersect(img->imageExtents());
    ur.setBottom(ur.bottom()+1);
    ur.setRight(ur.right()+1);

    int xt = xPaintOffset() + ur.x() - m_pHorz->value();
    int yt = yPaintOffset() + ur.y() - m_pVert->value();

    QPainter p;
    p.begin( m_pCanvas );
    p.scale( zoomFactor(), zoomFactor() ); 
    p.translate(xt, yt);

    // This is the place where image is drawn on the canvas  
    // p is the paint device, ur is the update rectangle,
    // bool transparency, ptr to the particular view to update
   
    koDocument()->paintEverything( p, ur, FALSE, this );
    p.end();
}

/*
    updateCanvas - update canvas regardless of paint event
    for transferring offscreen updates that do not generate 
    paint events for the canvas widget which is the viewport 
    for the view.
*/
void KisView::updateCanvas( QRect & ur )
{
    //kdDebug() << "updateCanvas(QRect & ur)" << endl;
    KisImage* img = m_pDoc->current();
    if (!img)
    {
        kdDebug(0) << "No curent image" << endl;

        QPainter p; 
        p.begin(m_pCanvas);
        p.eraseRect(ur);
        p.end();
        return;
    }

    QPainter p;
    p.begin( m_pCanvas);
    p.scale( zoomFactor(), zoomFactor() );

    // erase strip along left side 
    p.eraseRect(0, 0, xPaintOffset(), height());
 
    // erase strip along top
    p.eraseRect(xPaintOffset(), 0, width(), yPaintOffset());

    // erase area to the right of canvas - account for zoomed width of doc
    p.eraseRect( xPaintOffset(), yPaintOffset() + docHeight(), 
        width(), height() );

    // erase area below the canvas - account for zoomed height of doc
    p.eraseRect( xPaintOffset() + docWidth(),  yPaintOffset(),  
        width(), height() );

    // reduce size of update rectangle by inverse of zoom factor
    // only do this at higher zooms because otherwise the overhead
    // of this scaling isn't worth it. 
        
    if(zoomFactor() > 1.0 || zoomFactor() < 1.0)
    {
        int urW = ur.width();
        int urH = ur.height();
    
        urW = (int)((float)(urW)/zoomFactor());
        urH = (int)((float)(urH)/zoomFactor());
    
        ur.setWidth(urW);
        ur.setHeight(urH);
    }

    ur.moveBy( - xPaintOffset() + m_pHorz->value(), 
        - yPaintOffset() + m_pVert->value());
    ur = ur.intersect(img->imageExtents());

    ur.setBottom(ur.bottom()+1);
    ur.setRight(ur.right()+1);

    if (ur.top() > img->height()
    || ur.left() > img->width())
    {
       p.end();
       return;
    }

    int xt = xPaintOffset() + ur.x() - m_pHorz->value();
    int yt = yPaintOffset() + ur.y() - m_pVert->value();

    p.translate(xt, yt);
  
    // This is the place where image is drawn on the canvas  
    // p is the paint device, ur is the update rectangle,
    // bool transparency, ptr to the particular view to update

    m_pDoc->paintContent(p, ur); 
    p.end();
}

/*
    canvasGotPaintEvent - handles repaint of canvas (image) area
*/
void KisView::canvasGotPaintEvent( QPaintEvent*e )
{
    //kdDebug() << "canvasGotPaintEvent()" << endl;
    KisImage* img = m_pDoc->current();
    if (!img)
    {
        kdDebug(0) << "No m_pDoc->curent()" << endl;
        QPainter p; 
        p.begin(m_pCanvas);
        p.eraseRect(e->rect());
        p.end();
        return;
    }

    QRect ur = e->rect(); 
   
    QPainter p;
    p.begin( m_pCanvas );
    p.scale( zoomFactor(), zoomFactor() );

    // erase strip along left side 
    p.eraseRect( 0, 0, xPaintOffset(), height() );
 
    // erase strip along top
    p.eraseRect( xPaintOffset(), 0, width(), yPaintOffset() );

    // erase area to the right of canvas - account for zoomed width of doc
    p.eraseRect( xPaintOffset(), yPaintOffset() + docHeight(), 
        width(), height() );

    // erase area below the canvas - account for zoomed height of doc
    p.eraseRect( xPaintOffset() + docWidth(),  yPaintOffset(),  
        width(), height() );

    // reduce size of update rectangle by inverse of zoom factor
    // also reduce offset into image by same factor (1/zoomFactor())
    // only do this at higher/lower zooms because the overhead
    // of this scaling isn't worth it at normal zoom. 
        
    if(zoomFactor() > 1.0 || zoomFactor() < 1.00)
    {
        int urL = ur.left();
        int urT = ur.top();    
        int urW = ur.width();
        int urH = ur.height();
    
        urL = (int)((float)(urL)/zoomFactor());
        urT = (int)((float)(urT)/zoomFactor());
        urW = (int)((float)(urW)/zoomFactor());
        urH = (int)((float)(urH)/zoomFactor());
    
        ur.setLeft(urL);
        ur.setTop(urT);
        ur.setWidth(urW);
        ur.setHeight(urH);
    }

    ur.moveBy( - xPaintOffset() + m_pHorz->value(), 
        - yPaintOffset() + m_pVert->value());
    ur = ur.intersect(img->imageExtents());

    ur.setBottom(ur.bottom()+1);
    ur.setRight(ur.right()+1);

    if (ur.top() > img->height()
    || ur.left() > img->width())
    {
        p.end();
        return;
    }

    int xt = xPaintOffset() + ur.x() - m_pHorz->value();
    int yt = yPaintOffset() + ur.y() - m_pVert->value();

    p.translate(xt, yt);

    // This is the place where image is drawn on the canvas  
    // p is the paint device, ur is the update rectangle,
    // bool transparency, ptr to the particular view to update

    koDocument()->paintEverything( p, ur, FALSE, this );
    p.end();
}


/*
    canvasGotMousePressEvent - just passes the signal on 
    to the appropriate tool
*/
void KisView::canvasGotMousePressEvent( QMouseEvent *e )
{
    buttonIsDown = true;
    
    QMouseEvent ev( QEvent::MouseButtonPress,
        QPoint( e->pos().x() - xPaintOffset() + (int)(zoomFactor() * m_pHorz->value()),
		        e->pos().y() - yPaintOffset() + (int)(zoomFactor() * m_pVert->value())), 
        e->globalPos(), e->button(), e->state() );

    emit canvasMousePressEvent( &ev );
}


/*
    canvasGotMouseMoveEvent - just passes the signal on 
    to the appropriate tool
*/
void KisView::canvasGotMouseMoveEvent ( QMouseEvent *e )
{
    int x = e->pos().x() - xPaintOffset() + (int)(zoomFactor() * m_pHorz->value());
    int y = e->pos().y() - yPaintOffset() + (int)(zoomFactor() * m_pVert->value());

    QMouseEvent ev( QEvent::MouseMove, QPoint(x, y), 
        e->globalPos(), e->button(), e->state() );

    emit canvasMouseMoveEvent( &ev );
}

/*
    canvasGotMouseReleaseEvent - just passes the signal on 
    to the appropriate tool
*/
void KisView::canvasGotMouseReleaseEvent ( QMouseEvent *e )
{
    buttonIsDown = false;

    QMouseEvent ev( QEvent::MouseButtonRelease, 
        QPoint(e->pos().x() - xPaintOffset() + (int)(zoomFactor() * m_pHorz->value()),
			   e->pos().y() - yPaintOffset() + (int)(zoomFactor() * m_pVert->value())),
        e->globalPos(), e->button(), e->state() );

    emit canvasMouseReleaseEvent( &ev );
}

/*
    activateTool - make the selected tool the active tool and
    establish connections via the base kis_tool class
*/
void KisView::activateTool(KisTool* t)
{
    if (!t) return;
    
    // remove the selection outline, if any
    if(m_pTool) m_pTool->clearOld();

    // prevent old tool from receiving events from canvas
    if(m_pTool) QObject::disconnect(m_pTool);

    m_pTool = t;

    QObject::connect( this, SIGNAL( canvasMousePressEvent( QMouseEvent* ) ),
                    m_pTool, SLOT( mousePress ( QMouseEvent* ) ) );

    QObject::connect( this, SIGNAL( canvasMouseMoveEvent( QMouseEvent* ) ),
                    m_pTool, SLOT( mouseMove ( QMouseEvent* ) ) );

    QObject::connect( this, SIGNAL( canvasMouseReleaseEvent (QMouseEvent* ) ),
		    m_pTool, SLOT( mouseRelease ( QMouseEvent* ) ) );

    if (m_pCanvas) m_pCanvas->setCursor(m_pTool->cursor());
}


/*-------------------------------------------------------------
    tool action slots - these are just slots to set the active 
    tool to the one selected from the menu or toolbar - due to 
    the limitations of kde's xmlgui, threre must be a slot for
    each menu item/toolbar item.  A brief description of each 
    tool follows...
--------------------------------------------------------------*/

/*
    tool_properties invokes the optionsDialog() method for the
    current active tool.  There should be an options dialog for 
    each tool, but these can also be consolidated into a master 
    options dialog by reparenting the widgets for each tool to a 
    tabbed properties dialog, each tool getting a tab  - later
*/
void KisView::tool_properties()
{
    m_pTool->optionsDialog();
}

/*
    tool_select_rectangular - select a rectangular
    area with the mouse or by entering coordinates
    and size of the rectangle       
*/
void KisView::tool_select_rectangular()
{
    activateTool(m_pRectangularSelectTool);
}

/*
    tool_select_polygonal - select a polygonal area
    a variation of the polyline tool, applied
    to select rather than to draw
*/
void KisView::tool_select_polygonal()
{
    activateTool(m_pPolygonalSelectTool);
}

/*
    tool_select_elliptical - select an elliptical
    areas with the mouse - or with a dialog for
    entering center, and the two radii or a rectangle
    bounding the ellipse
*/
void KisView::tool_select_elliptical()
{
    activateTool(m_pEllipticalSelectTool);
}

/*
    tool_select_contiguous - select a contiguous area based
    on color, hue, value or saturation.  Tolerances (a range)
    of values for all these paramaters are allowed.
*/
void KisView::tool_select_contiguous()
{
    activateTool(m_pContiguousSelectTool);
}

/*
    tool_move - move a layer with the mouse or by 
    entering coordinates to move the layer to within
    the image 
*/
void KisView::tool_move()
{
    activateTool(m_pMoveTool);
}

/*
    tool_zoom - click on the image to multiply the zoom factor by
    2 with left button or by 1/2 with right button.  The view
    should also center on the point clicked on. (todo)
*/
void KisView::tool_zoom()
{
  activateTool(m_pZoomTool);
}

/*
    tool_brush - the basic painting tool for krayon. The color
    value of the brush image is applied to the current foreground
    color and blended with the current layer pixels.  Various
    predefined blending methods can be selected with the options
    dialog (todo).
*/
void KisView::tool_brush()
{
  activateTool(m_pBrushTool);
}

/*
    tool_airbrush - randomly paints pixels of the brush image as
    the button is held down over time. Compound blending is 
    disallowed with the airbrush tool as this tends to increase
    contrast with the background over time too much.
*/
void KisView::tool_airbrush()
{
  activateTool(m_pAirBrushTool);
}

/*
    tool_eraser - use the current brush as an eraser to decrease
    the alpha value of the pixels painted over or set those pixels
    to the background color is there is no alpha channel for the
    image.
*/
void KisView::tool_eraser()
{
  activateTool(m_pEraserTool);
}

/*
    tool_pen - like brush, except there is no blending with the 
    background.  The current fgColor is painted or not painted
    depending on the value of the pixel in the brush and the
    threshold, which can be set with he options dialog. The alpha
    value of the layer pixel can also be changed, if desired,
    depending on the saturation of the color in the brush pixel.
*/
void KisView::tool_pen()
{
  activateTool(m_pPenTool);
}

/*
    tool_colorpicker - change the fgColor to the color of the
    pixel clicked on with left click or the bgColor with right
    click. 
*/
void KisView::tool_colorpicker()
{
  activateTool(m_pColorPicker);
}

/*
    tool_colorchanger
*/
void KisView::tool_colorchanger()
{
  activateTool(m_pColorChangerTool);
}

/*
    tool_gradient
*/
void KisView::tool_gradient()
{
  activateTool( m_pGradientTool );
}

/*
    tool_line
*/
void KisView::tool_line()
{
  activateTool( m_pLineTool );
}

/*
    tool_polyline
*/
void KisView::tool_polyline()
{
  activateTool( m_pPolyLineTool );
}

/*
    tool_rectangle
*/

void KisView::tool_rectangle()
{
  activateTool( m_pRectangleTool );
}

/*
    tool_ellipse
*/
void KisView::tool_ellipse()
{
  activateTool( m_pEllipseTool );
}

/*
    tool_fill
*/
void KisView::tool_fill()
{
  activateTool( m_pFillTool );
}

/*
    tool_stamp
*/
void KisView::tool_stamp()
{
  activateTool( m_pStampTool );
}

/*  
    tool_paste is same as paste action from selection group
    but paste still needes to be a tool because you can paint with it
*/
void KisView::tool_paste()
{
    if(m_pDoc->getClipImage())
    {
        m_pPasteTool->setClip();
        activateTool(m_pPasteTool);
        slotUpdateImage();
    }    
    else
    {
        KMessageBox::sorry(NULL, i18n("Nothing to paste!"), "", FALSE); 
    }
}

/*---------------------------
    history action slots
----------------------------*/

void KisView::undo()
{
    kdDebug() << "UNDO called" << endl;
    //m_pDoc->commandHistory()->undo();
}

void KisView::redo()
{
    kdDebug() << "REDO called" << endl;
    //m_pDoc->commandHistory()->redo();
}

/*---------------------------------
    edit selection action slots
----------------------------------*/

/*
    copy - copy selection contents to global kapp->clipboard()
*/
void KisView::copy()
{
    if(!m_pDoc->setClipImage())
        kdDebug() << "m_pDoc->setClipImage() failed" << endl;
        
    if(m_pDoc->getClipImage())
    {
        kdDebug() << "got m_pDoc->getClipImage()" << endl;
        QImage cImage = *m_pDoc->getClipImage();
        kapp->clipboard()->setImage(cImage); 
        {
            if(kapp->clipboard()->image().isNull())
                kdDebug() << "clip image is null" << endl; 
            else
               kdDebug() << "clip image is NOT null" << endl; 
        }
    }    
}

/*
    cut - move selection contents to global kapp->clipboard()
*/
void KisView::cut()
{
    if(!m_pDoc->setClipImage())
        kdDebug() << "m_pDoc->setClipImage() failed" << endl;
        
    if(m_pDoc->getClipImage())
        kapp->clipboard()->setImage(*(m_pDoc->getClipImage()));    
        
    if(!m_pDoc->getSelection()->erase())
        kdDebug() << "m_pDoc->m_Selection.erase() failed" << endl;

    // clear old selection outline
    m_pTool->clearOld();

    /* refresh canvas */
    slotUpdateImage();
}

/*
    same as cut but don't move selection contents to clipboard
*/
void KisView::removeSelection()
{
    if(!m_pDoc->getSelection()->erase())
        kdDebug() << "m_pDoc->m_Selection.erase() failed" << endl;

    // clear old selection outline
    m_pTool->clearOld();

    /* refresh canvas */
    slotUpdateImage();
}


/*
    paste - always from the global kapp->clipboard(). The image
    in the clipboard (if any) is copied to the past tool clip
    image so it can be used like a brush or stamp tool to paint
    with.
*/
void KisView::paste()
{
    if(m_pDoc->getClipImage())
    {
        m_pPasteTool->setClip();
        activateTool(m_pPasteTool);
        slotUpdateImage();
    }    
    else
    {
        KMessageBox::sorry(NULL, i18n("Nothing to paste!"), "", FALSE); 
    }
}

/*
    create a new layer from the selection, same size as the
    selection (in the case of a non-rectangular selection, find
    and use the bounding rectangle for selected pixels)
*/
void KisView::crop()
{
    if(!m_pDoc->hasSelection())
    {
        KMessageBox::sorry(NULL, i18n("No selection to crop!"), "", FALSE); 
        return;
    }
    // copy contents of the current selection to a QImage
    if(!m_pDoc->setClipImage())
    {
        kdDebug() << "m_pDoc->setClipImage() failed" << endl;
        return;    
    }    
    // contents of current selection - make sure it's good
    if(!m_pDoc->getClipImage())
    {
        kdDebug() << "m_pDoc->getClipImage() failed" << endl;
        return;
    }

    QImage cImage = *m_pDoc->getClipImage();
    
    // add new layer same size as selection rectangle,
    // then paste cropped image into it at 0,0 offset
    // keep old image - user can remove it later if he wants
    // by removing its layer or may want to keep the original.    

    KisImage* img = m_pDoc->current();     
    if(!img) return;
    
    QRect layerRect(0, 0, cImage.width(), cImage.height());    
    QString name; name.sprintf("layer %d", img->layerList().count());

    img->addLayer(layerRect, white, false, name);
    uint indx = img->layerList().count() - 1;    
    img->setCurrentLayer(indx);    
    img->setFrontLayer(indx);    

    m_pLayerView->layerTable()->updateTable();    
    m_pLayerView->layerTable()->updateAllCells();

    // copy the image into the layer - this should now
    // be handled by the framebuffer object, not the doc
    if(!m_pDoc->QtImageToLayer(&cImage, this))
    {
        kdDebug(0) << "crop: can't load image into layer." << endl;        
    }

    slotUpdateImage();
    
    // remove the current clip image which now belongs to the 
    // previous layer - selection also is removed. To crop again, 
    // you must first make another selection in the current layer.
    
    m_pDoc->removeClipImage();
    m_pDoc->clearSelection();    
}

/*
    selectAll - use the bounding rectangle of the layer itself
*/
void KisView::selectAll()
{
    KisImage *img = m_pDoc->current();
    if(!img) return;
    
    QRect imageR = img->getCurrentLayer()->imageExtents();
    m_pDoc->setSelection(imageR);
}

/*
    unSelectAll - clear the selection, if any
*/
void KisView::unSelectAll()
{
    m_pDoc->clearSelection();
}


/*--------------------------
       Zooming
---------------------------*/

void KisView::zoom( int _x, int _y, float zf )
{
    /* Avoid divide by zero errors by disallowing a zoom
    factor of zero, which is impossible anyway, as it would
    make the image infinitely small in size. */
    if (zf == 0) zf = 1;

    /* Set a reasonable lower limit for a zoom factor of 1/16.
    At this level a 1600x1600 image would be 100 x 100 in size.
    Extremely low zooms, like extremely high ones, can be very
    expensive in terms of processor cycles and degrade performace, 
    although not nearly as much as extrmely high zooms.*/
    else if (zf < (float)(1/16)) zf = (float)(1/16);
 
    /* Set a reasonable upper limit for a zoom factor of 16. At this 
    level each pixel in the layer is shown as a 16x16 rectangle.  
    Zoom levels higher than this serve no useful purpose and are 
    *VERY* expensive in terms of processor cycles and slow down 
    everything unnecessarily.  It's possible to accidentally set a
    very high zoom by continuing to click on the image with the zoom 
    tool, without this limit. */
    else if(zf > 16.0) zf = 16.0;
    
    setZoomFactor( zf );    

    int x = static_cast<int> (_x * zf - docWidth() / 2);
    int y = static_cast<int> (_y * zf - docHeight() / 2);

    if (x < 0) x = 0;
    if (y < 0) y = 0;

    // clear everything
    QPainter p;
    p.begin( m_pCanvas );
    p.eraseRect( 0, 0, width(), height() );
    p.end();

    // scroll to the point clicked on and update the
    // canvas.  Currently scrollTo() doesn't do anything
    // but the zoomed view does have the same offset
    // as the prior view so it approximately works
    scrollTo( QPoint( x, y ) );
    m_pCanvas->update();
    
    // at low zoom levels mark everything dirty and
    // redraw the entire image to insure that the 
    // previous image is erased completely from border 
    // areas.  Otherwise screen artificats can be seen.
    if(zf < 1.0) slotUpdateImage();
        
    // at high and normal zoom levels just send a fake
    // resize event to make sure that scroll bars show up.
    // It can take a *very* long time to recalculate a large
    // image a a high zoom level, so don't mark it dirty.
    else showScrollBars();       
}


void KisView::zoom_in( int x, int y )
{
    float zf = zoomFactor();
    zf *= 2;
    zoom( x, y, zf);
}

void KisView::zoom_out( int x, int y )
{
    float zf = zoomFactor();
    zf /= 2;
    zoom( x, y, zf);
}

void KisView::zoom_in()
{
    zoom_in( 0, 0 );
}

void KisView::zoom_out()
{
    zoom_out( 0, 0 );
}

void KisView::dialog_gradient()
{
    if (m_dialog_gradient->isChecked())
    {
        m_pGradientDialog->show();
        m_pGradientDialog->setFocus();
    }
    else
        m_pGradientDialog->hide();
}


void KisView::dialog_gradienteditor()
{
    if (m_dialog_gradienteditor->isChecked())
    {
        m_pGradientEditorDialog->show();
        m_pGradientEditorDialog->setFocus();
    }
    else
        m_pGradientEditorDialog->hide();
}

void KisView::dialog_colors()
{
}
void KisView::dialog_krayons()
{
}
void KisView::dialog_brushes()
{
}
void KisView::dialog_patterns()
{
}
void KisView::dialog_layers()
{
}
void KisView::dialog_channels()
{
}

void KisView::updateToolbarButtons()
{
    //kdDebug() << "KisView::updateToolbarButtons" << endl;

    //m_dialog_gradient->setChecked(m_pGradientDialog->isVisible() );
    //m_dialog_gradienteditor->setChecked(m_pGradientEditorDialog->isVisible() );
}


/*-------------------------------
    layer action slots
--------------------------------*/

/* 
    Properties dialog for the current layer. 
    Only for changing name and opacity so far.
*/

void KisView::layer_properties()
{
    KisImage * img = m_pDoc->current();
    if (!img)  return;    

    KisLayer *lay = img->getCurrentLayer();
    if (!lay)  return;    

    if(LayerPropertyDialog::editProperties(*(lay)))
    {
        QRect updateRect = lay->imageExtents();
        m_pLayerView->layerTable()->updateAllCells( );
        img->markDirty( updateRect );
    }
}

/*
    insert new layer into the current image - using 
    "new layer" dialog for layer size (should also
    have fields for name and opacity) This new layer
    will also be made uppermost so it is visble
*/
void KisView::insert_layer()
{
    KisImage* img = m_pDoc->current();
    if(!img) return;
    
    NewLayerDialog *pNewLayerDialog = new NewLayerDialog();
    pNewLayerDialog->exec();
    if(!pNewLayerDialog->result() == QDialog::Accepted)
        return;

    QRect layerRect(0, 0, pNewLayerDialog->width(), pNewLayerDialog->height());       
    QString name; name.sprintf("layer %d", img->layerList().count());
    
    // new layers are currently appended - perhaps they should
    // be prepended so more recent ones are on top in the
    // list and the background layer is at the bottom
    
    img->addLayer(layerRect, white, true, name);
    uint indx = img->layerList().count() - 1;    
    img->setCurrentLayer(indx); 
    img->setFrontLayer(indx);    
    m_pLayerView->layerTable()->selectLayer(indx);       
    
    // update layerview table so change show up there    
    m_pLayerView->layerTable()->updateTable();    
    m_pLayerView->layerTable()->updateAllCells(); 
    slotUpdateImage();
}

/*
    remove current layer - to remove other layers, a user must
    access the layers tableview in a dialog or sidebar widget
*/
void KisView::remove_layer()
{
    m_pLayerView->layerTable()->slotRemoveLayer(); 
}

/*
    hide/show the current layer - to hide other layers, a user must
    access the layers tableview in a dialog or sidebar widget
*/
void KisView::hide_layer()
{
    KisImage * img = m_pDoc->current();
    if (!img)  return;    

    uint indx = img->getCurrentLayerIndex();
    m_pLayerView->layerTable()->slotInverseVisibility(indx);    

    m_pLayerView->layerTable()->updateTable();    
    m_pLayerView->layerTable()->updateAllCells();    
}

/*
    link/unlink the current layer - to link other layers, a user must
    access the layers tableview in a dialog or sidebar widget
*/
void KisView::link_layer()
{
    KisImage * img = m_pDoc->current();
    if (!img) return;    

    uint indx = img->getCurrentLayerIndex();
    m_pLayerView->layerTable()->slotInverseLinking(indx);    

    m_pLayerView->layerTable()->updateTable();    
    m_pLayerView->layerTable()->updateAllCells();    
}

/*
    make the next layer in the layers list the active one and
    bring it to the front of the view
*/
void KisView::next_layer()
{
    KisImage * img = m_pDoc->current();
    if (!img)  return;    

    uint indx = img->getCurrentLayerIndex();
    if(indx < img->layerList().count() - 1)
    { 
        // make the next layer the current one, select it,
        // and make sure it's visible
        ++indx;
        img->setCurrentLayer(indx);   
        m_pLayerView->layerTable()->selectLayer(indx);
        img->layerList().at(indx)->setVisible(true);
        
        // hide all layers on top of this one so this
        // one is clearly visible and can be painted on!
        
        while(++indx <= img->layerList().count() - 1)
        {
            img->layerList().at(indx)->setVisible(false);   
        }

        img->markDirty(img->getCurrentLayer()->layerExtents());        
        m_pLayerView->layerTable()->updateTable();    
        m_pLayerView->layerTable()->updateAllCells();
    }    
}


/*
    make the previous layer in the layers list the active one and
    bring it to the front of the view
*/
void KisView::previous_layer()
{
    KisImage * img = m_pDoc->current();
    if (!img)  return;    

    uint indx = img->getCurrentLayerIndex();
    if(indx > 0)
    { 
        // make the previous layer the current one, select it,
        // and make sure it's visible
        --indx;
        img->setCurrentLayer(indx);
        m_pLayerView->layerTable()->selectLayer(indx);
        img->layerList().at(indx)->setVisible(true);

        // hide all layers beyond this one so this
        // one is clearly visible and can be painted on!
        while(++indx <= img->layerList().count() - 1)
        {
            img->layerList().at(indx)->setVisible(false);
        }

        img->markDirty(img->getCurrentLayer()->layerExtents());                
        m_pLayerView->layerTable()->updateTable();    
        m_pLayerView->layerTable()->updateAllCells();
   }     
}


void KisView::import_image()
{
    insert_layer_image(true);
}

void KisView::export_image()
{
    save_layer_image(true);
}

void KisView::insert_image_as_layer()
{
    insert_layer_image(false);
}

void KisView::save_layer_as_image()
{
    save_layer_image(false);
}

/*
    Insert a standard image like png or jpg into the current layer.
    This is the same as "import" in other koffice apps, but since
    everything is organized by layers, one must insert into a 
    specific layer (the current active layer).  This them becomes part
    of the image, including other layers visible and invisible
*/
void KisView::insert_layer_image(bool newImage)
{
    KURL url = KFileDialog::getOpenURL( QString::null,
        KisUtil::readFilters(), 0, i18n("Image file for layer") );

    if( !url.isEmpty() )
    {
        /* convert indexed images, all gifs and some pngs of 8 bits 
        or less, to 16 bit by creating a QPixmap from the file and 
        blitting it into a 16 bit RGBA pixmap - you can blit from a 
        lesser depth to a greater but not the other way around */
        
        QPixmap *filePixmap = new QPixmap(url.path());
        QPixmap *buffer = new QPixmap(filePixmap->width(), 
            filePixmap->height());
        buffer->fill (Qt::white);
        bitBlt (buffer, 0, 0, filePixmap, 
            0, 0, filePixmap->width(), filePixmap->height());
        QImage fileImage = buffer->convertToImage();
        
        delete filePixmap;
        delete buffer;

        KisImage* img = m_pDoc->current(); 
        QRect layerRect(0, 0, fileImage.width(), fileImage.height());
        QString layerName(url.fileName()); 

        // add image from file as new layer for existing image        
        if(!newImage)
        { 
            img->addLayer(layerRect, white, false, layerName);
            uint indx = img->layerList().count() - 1;    
            img->setCurrentLayer(indx);
            img->setFrontLayer(indx);                
            m_pLayerView->layerTable()->selectLayer(indx);            

            m_pLayerView->layerTable()->updateTable();    
            m_pLayerView->layerTable()->updateAllCells();    
        } 
        // add image from file as new image and append to image list   
        else
        {
            // this creates the new image and appends it to
            // the image list for the document
            KisImage *newimg = m_pDoc->newImage(layerName, 
                layerRect.width(), layerRect.height());
   
            // add background layer - should this always be white?
            bgMode bg = bm_White;
            
            if (bg == bm_White)
	            newimg->addLayer(QRect(0, 0, newimg->width(), newimg->height()), 
                    KisColor::white(), false, "background");
            else if (bg == bm_Transparent)
	            newimg->addLayer(QRect(0, 0, newimg->width(), newimg->height()), 
                    KisColor::white(), true, "background");
            else if (bg == bm_ForegroundColor)
	            newimg->addLayer(QRect(0, 0, newimg->width(), newimg->height()), 
                    KisColor::white(), false, "background");
            else if (bg == bm_BackgroundColor)
	            newimg->addLayer(QRect(0, 0, newimg->width(), newimg->height()), 
                    KisColor::white(), false, "background");

            //kdDebug() << "KisView ret. from addLayer() for new image" << endl;

            newimg->markDirty(QRect(0, 0, newimg->width(), newimg->height()));
            m_pDoc->setCurrentImage(newimg);
        }   

        // copy the image into the layer regardless of whether 
        // a new image or just a new layer was created for it above.
        if(!m_pDoc->QtImageToLayer(&fileImage, this))
        {
            kdDebug(0) << "inset_layer_image: Can't load image into layer." << endl;
        }

        slotUpdateImage();
    }
}

/*
    save_layer_image - export the current image after merging
    layers or just export the current layer
*/
void KisView::save_layer_image(bool mergeLayers)
{
    KURL url = KFileDialog::getSaveURL( QString::null,
        KisUtil::readFilters(), 0, i18n("Image file for layer") );

    if( !url.isEmpty() )
    {
        if(mergeLayers)
        {
            /* merge should not always remove layers -
            merged into another but should have an option
            for keeping old layers and merging into a new
            one - Yes/No dialog to confirm.*/
            merge_all_layers();
        }

        //  save as standard image file (jpg, png, xpm, bmp, NO gif)
        if(!m_pDoc->saveAsQtImage(url.path(), mergeLayers))
            kdDebug(0) << "Can't save doc as image" << endl;
    }
}


void KisView::layer_scale_smooth()
{
    layerScale(true);
}

void KisView::layer_scale_rough()
{
    layerScale(false);
}


void KisView::layerScale(bool smooth)
{
    KisImage * img = m_pDoc->current();
    if (!img)  return;    

    KisLayer *lay = img->getCurrentLayer();
    if (!lay)  return;    
    
    KisFrameBuffer *fb = m_pDoc->frameBuffer();
    if (!fb)  return;    

    NewLayerDialog *pNewLayerDialog = new NewLayerDialog();
    pNewLayerDialog->exec();
    if(!pNewLayerDialog->result() == QDialog::Accepted)
        return;

    QRect srcR(lay->imageExtents());        
    
    bool ok;
    
    if(smooth)
        ok = fb->scaleSmooth(srcR, 
            pNewLayerDialog->width(), pNewLayerDialog->height());
    else 
        ok = fb->scaleRough(srcR, 
            pNewLayerDialog->width(), pNewLayerDialog->height());
           
    if(!ok)
    {
        kdDebug() << "layer_scale() failed" << endl; 
    }
    else
    {
        // bring new scaled layer to front
        uint indx = img->layerList().count() - 1;    
        img->setCurrentLayer(indx);
        img->markDirty(img->getCurrentLayer()->layerExtents());                
        m_pLayerView->layerTable()->selectLayer(indx);

        m_pLayerView->layerTable()->updateTable();    
        m_pLayerView->layerTable()->updateAllCells();    
    }
}


void KisView::layer_rotate180()
{

}

void KisView::layer_rotateleft90()
{

}

void KisView::layer_rotateright90()
{

}

void KisView::layer_mirrorX()
{

}


void KisView::layer_mirrorY()
{

}

/*--------------------------
    image action slots
--------------------------*/

void KisView::add_new_image_tab()
{
    if(!m_pDoc->slotNewImage())
        kdDebug(0) << "Couldn't add image tab" << endl;
        
    m_pPainter->resize(m_pDoc->current()->width(), 
        m_pDoc->current()->height());
        
    m_pPainter->clearAll();
    slotUpdateImage();    
}


void KisView::remove_current_image_tab()
{
    if (m_pDoc->current())
	    m_pDoc->removeImage(m_pDoc->current());
    slotUpdateImage();            
}


void KisView::merge_all_layers()
{
    if (m_pDoc->current())
	    m_pDoc->current()->mergeAllLayers();
    slotUpdateImage();            
}


void KisView::merge_visible_layers()
{
    if (m_pDoc->current())
        m_pDoc->current()->mergeVisibleLayers();
    slotUpdateImage();            
}


void KisView::merge_linked_layers()
{
    if (m_pDoc->current())
	    m_pDoc->current()->mergeLinkedLayers();
    slotUpdateImage();            
}


/*------------------------------------
  Preferences and configuration slots
---------------------------------------*/

void KisView::showMenubar()
{
}


void KisView::showToolbar()
{
}


void KisView::showStatusbar()
{
}


void KisView::showSidebar()
{
    if (m_side_bar->isChecked())
    {
        m_pSideBar->show();
    }
    else
    {
        m_pSideBar->hide();
    } 
    
    // force resize to show scrollbars, etc
    resizeEvent(0L);
}


void KisView::floatSidebar()
{
    KFloatingDialog *f = (KFloatingDialog *)m_pSideBar;
    f->setDocked(!m_float_side_bar->isChecked());

    // force resize to show scrollbars, etc
    resizeEvent(0L);
}

/*
    leftSidebar -this does nothing except force a resize to show scrollbars
    Repositioning of sidebar is handled by resizeEvent() entirley
*/
void KisView::leftSidebar()
{
    resizeEvent(0L);
}

/*
    saveOptions - here we need to write entries to a congig
    file.
*/
void KisView::saveOptions()
{
}


/*
    preferences - the main Krayon preferences dialog - modeled
    after the konqueror prefs dialog - quite nice compound dialog
*/
void KisView::preferences()
{
    PreferencesDialog::editPreferences();
}


/*
    docWidth - simply returns the width of the document which is
    exactly the same as the width of the current image
*/
int KisView::docWidth()
{
    if (m_pDoc->current()) return m_pDoc->current()->width();
    else return 0;
}

/*
    docHeight - simply returns the width of the document which is
    exactly the same as the width of the current image
*/

int KisView::docHeight()
{
    if (m_pDoc->current()) return m_pDoc->current()->height();
    else return 0;
}


void KisView::slotSetPaintOffset()
{
    // dialog to set x and y paint offsets needed
    if(xPaintOffset() == 0)
    {
        m_xPaintOffset = 20;
        m_yPaintOffset = 20; 
    }
    else
    {
        m_xPaintOffset = 0;
        m_yPaintOffset = 0; 
    }
    
    showScrollBars();
}


int KisView::xPaintOffset()
{
    return m_xPaintOffset;
}


int KisView::yPaintOffset()
{
    return m_yPaintOffset;
}


void KisView::scrollTo( QPoint )
{

}


float KisView::zoomFactor()
{
    return m_zoomFactor;
}


void KisView::setZoomFactor( float zf )
{
    m_zoomFactor = zf;
}


void KisView::slotSetBrush(const KisBrush* b)
{
    m_pBrush = b;
    
    if (m_pBrushTool)
        m_pBrushTool->setBrush(b);
    if (m_pPenTool)
        m_pPenTool->setBrush(b);
    if (m_pAirBrushTool)
        m_pAirBrushTool->setBrush(b);
    if (m_pEraserTool)
        m_pEraserTool->setBrush(b);

    m_pSideBar->slotSetBrush(*b);
}


void KisView::slotSetKrayon(const KisKrayon* k)
{
    m_pKrayon = k;
    m_pSideBar->slotSetKrayon(*k);
}


void KisView::slotSetPattern(const KisPattern* p)
{
    m_pPattern = p;
    
    /* setPattern should actually be a part of the tool
    base class so all tools can use it */
    
    if (m_pStampTool)
        m_pStampTool->setPattern(p);

    m_pSideBar->slotSetPattern(*p);
}


/*
    The new foreground color should show up in the color selector 
    via signal sent to colorselector
*/

void KisView::slotSetFGColor(const KisColor& c)
{
    m_fg = c;
    emit fgColorChanged(c);
}

/*
    The new background color should show up in the color selector 
    via signal sent to colorselector
*/

void KisView::slotSetBGColor(const KisColor& c)
{
    m_bg = c;
    emit bgColorChanged(c);
}


void KisView::slotUndoRedoChanged( QString /*undo*/, QString /*redo*/ )
{
  // FIXME
#if 0
  m_undo->setEnabled( !undo.isEmpty() );
  m_redo->setEnabled( !redo.isEmpty() );
#endif  
}


void KisView::slotUndoRedoChanged( QStringList /*undo*/, QStringList /*redo*/ )
{
  // FIXME
#if 0
  if( undo.count() )
  {
    // enable undo action
    m_undo->setEnabled( true );
  }
  else
  {
    // disable undo action
    m_undo->setEnabled( false );
  }

  if( redo.count() )
  {
    // enable redo action
    m_redo->setEnabled( true );
  }
  else
  {
    // disable redo action
    m_redo->setEnabled( false );
  }
#endif
}

#include "kis_view.moc"

