/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998			  */
/* Version: 0.1.0						  */
/* Author: Reginald Stadlbauer					  */
/* E-Mail: reggie@kde.org					  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* written for KDE (http://www.kde.org)				  */
/* KPresenter is under GNU GPL					  */
/******************************************************************/
/* Module: KPresenter View					  */
/******************************************************************/

#include <qprinter.h>
#include <qpainter.h>
#include <qstring.h>
#include <qmsgbox.h>
#include <qkeycode.h>
#include <qscrbar.h>
#include <qevent.h>
#include <qpopmenu.h>
#include <qcursor.h>
#include <qfileinf.h>
#include "backdia.h"
#include "autoformEdit/afchoose.h"
#include "styledia.h"
#include "ktextobject.h"
#include "kcharselectdia.h"
#include "kenumlistdia.h"
#include "optiondia.h"
#include "pgconfdia.h"
#include "effectdia.h"
#include "rotatedia.h"
#include "ksearchdialogs.h"
#include "shadowdia.h"
#include "presstructview.h"
#include "delpagedia.h"
#include "inspagedia.h"
#include "setbackcmd.h"
#include "pgconfcmd.h"
#include "confpiedia.h"
#include "confrectdia.h"
#include "spacingdia.h"
#include "pglayoutcmd.h"
#include "shadowcmd.h"
#include "rotatecmd.h"
#include "kppartobject.h"

#include <kfiledialog.h>

#include "kpresenter_view.h"
#include "kpresenter_view.moc"
#include "kpresenter_shell.h"
#include "page.h"
#include "webpresentation.h"
#include "footer_header.h"
#include "kptextobject.h"

#include <klocale.h>
#include <kcolordlg.h>
#include <kfontdialog.h>
#include <kglobal.h>

#include <opUIUtils.h>
#include <opToolBarIf.h>

#include <koPartSelectDia.h>
#include <koQueryTypes.h>
#include <koDocument.h>
#include <koAboutDia.h>
#include <koPageLayoutDia.h>
#include <koRuler.h>
#include <kimgio.h>

#include <stdlib.h>
#include <signal.h>

#include <X11/Xlib.h>
#include <kstddirs.h>

#define DEBUG

/*****************************************************************/
/* class KPresenterFrame					 */
/*****************************************************************/

/*======================= constructor ===========================*/
KPresenterFrame::KPresenterFrame( KPresenterView* _view, KPresenterChild* _child )
    : KoFrame( _view )
{
    m_pKPresenterView = _view;
    m_pKPresenterChild = _child;
}

/*****************************************************************/
/* class KPresenterView						 */
/*****************************************************************/

/*======================= constructor ===========================*/
KPresenterView::KPresenterView( QWidget *_parent, const char *_name, KPresenterDoc* _doc )
    : QWidget( _parent, _name ), KoViewIf( _doc ), OPViewIf( _doc ), KPresenter::KPresenterView_skel()
{
    setWidget( this );

    shell = 0L;

    OPPartIf::setFocusPolicy( OpenParts::Part::ClickFocus );

    m_pKPresenterDoc = 0L;
    m_bKPresenterModified = false;
    m_bUnderConstruction = true;

    // init
    backDia = 0;
    afChoose = 0;
    styleDia = 0;
    optionDia = 0;
    pgConfDia = 0;
    effectDia = 0;
    rotateDia = 0;
    searchDia = 0;
    replaceDia = 0;
    shadowDia = 0;
    presStructView = 0;
    delPageDia = 0;
    insPageDia = 0;
    confPieDia = 0;
    confRectDia = 0;
    spacingDia = 0;
    xOffset = 0;
    yOffset = 0;
    v_ruler = 0;
    h_ruler = 0;
    pen = QPen( black, 1, SolidLine );
    brush = QBrush( white, SolidPattern );
    lineBegin = L_NORMAL;
    lineEnd = L_NORMAL;
    gColor1 = red;
    gColor2 = green;
    gType = BCT_GHORZ;
    gUnbalanced = FALSE;
    gXFactor = 100;
    gYFactor = 100;
    fillType = FT_BRUSH;
    pieType = PT_PIE;
    pieLength = 90 * 16;
    pieAngle = 45 * 16;
    setMouseTracking( true );
    m_bShowGUI = true;
    m_bRectSelection = false;
    presStarted = false;
    searchFirst = true;
    continuePres = false;
    exitPres = false;
    rndX = 0;
    rndY = 0;
    m_vColorBar = 0;
    allowWebPres = true;

    m_pKPresenterDoc = _doc;
    m_bKPresenterModified = true;

    QObject::connect( m_pKPresenterDoc, SIGNAL( sig_KPresenterModified() ), this, SLOT( slotKPresenterModified() ) );
    QObject::connect( m_pKPresenterDoc, SIGNAL( sig_insertObject( KPresenterChild*, KPPartObject* ) ),
		      this, SLOT( slotInsertObject( KPresenterChild*, KPPartObject* ) ) );
    QObject::connect( m_pKPresenterDoc, SIGNAL( sig_updateChildGeometry( KPresenterChild* ) ),
		      this, SLOT( slotUpdateChildGeometry( KPresenterChild* ) ) );

    createGUI();

    setKeyCompression( true );
}

/*======================= init ============================*/
void KPresenterView::init()
{
    /****
     * Menu
     ****/

    cerr << "Registering menu as " << id() << endl;

    OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
    if ( !CORBA::is_nil( menu_bar_manager ) )
	menu_bar_manager->registerClient( id(), this );
    else
	cerr << "Did not get a menu bar manager" << endl;

  /****
   * Toolbar
   ****/

    OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
    if ( !CORBA::is_nil( tool_bar_manager ) )
	tool_bar_manager->registerClient( id(), this );
    else
	cerr << "Did not get a tool bar manager" << endl;

    KPObject *kpobject;
    for ( unsigned int i = 0; i < m_pKPresenterDoc->objectList()->count(); i++ )
    {
	kpobject = m_pKPresenterDoc->objectList()->at( i );
	if ( kpobject->getType() == OT_PART )
	    slotInsertObject( dynamic_cast<KPPartObject*>( kpobject )->getChild(), dynamic_cast<KPPartObject*>( kpobject ) );
    }
}

/*======================= destructor ============================*/
KPresenterView::~KPresenterView()
{
    sdeb( "KPresenterView::~KPresenterView()\n" );
    page->setToolEditMode( TEM_MOUSE );
    cleanUp();
    edeb( "...KPresenterView::~KPresenterView() %i\n", _refcnt() );
}

/*======================= clean up ==============================*/
void KPresenterView::cleanUp()
{
    if ( m_bIsClean ) return;

    OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
    if ( !CORBA::is_nil( menu_bar_manager ) )
	menu_bar_manager->unregisterClient( id() );

    OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
    if ( !CORBA::is_nil( tool_bar_manager ) )
	tool_bar_manager->unregisterClient( id() );

    m_pKPresenterDoc->removeView( this );

    KoViewIf::cleanUp();
}

/*=========================== file print =======================*/
CORBA::Boolean KPresenterView::printDlg()
{
    QPrinter prt;
    prt.setMinMax( 1, m_pKPresenterDoc->getPageNums() );
    bool makeLandscape = false;

    switch ( m_pKPresenterDoc->pageLayout().format )
    {
    case PG_DIN_A3: prt.setPageSize( QPrinter::A3 );
	break;
    case PG_DIN_A4: prt.setPageSize( QPrinter::A4 );
	break;
    case PG_DIN_A5: prt.setPageSize( QPrinter::A5 );
	break;
    case PG_US_LETTER: prt.setPageSize( QPrinter::Letter );
	break;
    case PG_US_LEGAL: prt.setPageSize( QPrinter::Legal );
	break;
    case PG_US_EXECUTIVE: prt.setPageSize( QPrinter::Executive );
	break;
    case PG_DIN_B5: prt.setPageSize( QPrinter::B5 );
	break;
    case PG_SCREEN:
    {
	warning( i18n( "You use the page layout SCREEN. I print it in DIN A4 LANDSCAPE!" ) );
	prt.setPageSize( QPrinter::A4 );
	makeLandscape = true;
    }	break;
    default:
    {
	warning( i18n( "The used page layout is not supported by QPrinter. I set it to DIN A4." ) );
	prt.setPageSize( QPrinter::A4 );
    } break;
    }

    switch ( m_pKPresenterDoc->pageLayout().orientation )
    {
    case PG_PORTRAIT: prt.setOrientation( QPrinter::Portrait );
	break;
    case PG_LANDSCAPE: prt.setOrientation( QPrinter::Landscape );
	break;
    }

    float left_margin = 0.0;
    float top_margin = 0.0;

    if ( makeLandscape )
    {
	prt.setOrientation( QPrinter::Landscape );
	left_margin = 28.5;
	top_margin = 15.0;
    }

    if ( prt.setup( this ) )
    {
	page->deSelectAllObj();
	QPainter painter;
	painter.begin( &prt );
	page->print( &painter, &prt, left_margin, top_margin );
	painter.end();
    }
    return true;
}

/*===============================================================*/
void KPresenterView::editUndo()
{
    page->setToolEditMode( TEM_MOUSE );
    m_pKPresenterDoc->commands()->undo();
}

/*===============================================================*/
void KPresenterView::editRedo()
{
    page->setToolEditMode( TEM_MOUSE );
    m_pKPresenterDoc->commands()->redo();
}

/*===============================================================*/
void KPresenterView::editCut()
{
    page->setToolEditMode( TEM_MOUSE );
    m_pKPresenterDoc->copyObjs( xOffset, yOffset );
    m_pKPresenterDoc->deleteObjs();
}

/*===============================================================*/
void KPresenterView::editCopy()
{
    page->setToolEditMode( TEM_MOUSE );
    m_pKPresenterDoc->copyObjs( xOffset, yOffset );
}

/*===============================================================*/
void KPresenterView::editPaste()
{
    page->setToolEditMode( TEM_MOUSE );
    page->deSelectAllObj();
    m_pKPresenterDoc->pasteObjs( xOffset, yOffset );
}

/*===============================================================*/
void KPresenterView::editDelete()
{
    page->setToolEditMode( TEM_MOUSE );
    m_pKPresenterDoc->deleteObjs();
}

/*===============================================================*/
void KPresenterView::editSelectAll()
{
    page->setToolEditMode( TEM_MOUSE );
    page->selectAllObj();
}

/*===============================================================*/
void KPresenterView::editCopyPage()
{
}

/*===============================================================*/
void KPresenterView::editDelPage()
{
    if ( delPageDia )
    {
	QObject::disconnect( delPageDia, SIGNAL( deletePage( int, DelPageMode ) ), this, SLOT( delPageOk( int, DelPageMode ) ) );
	delPageDia->close();
	delete delPageDia;
	delPageDia = 0;
    }

    if ( m_pKPresenterDoc->getPageNums() < 2 )
    {
	QMessageBox::critical( this, i18n( "KPresenter Error" ),
			       i18n( "Every document has to have at least one page. Because this document \n"
				     "has not more that one page you can't delete this one." ),
			       i18n( "OK" ) );
    }
    else
    {
	delPageDia = new DelPageDia( this, "", m_pKPresenterDoc, getCurrPgNum() );
	delPageDia->setCaption( i18n( "KPresenter - Delete Page" ) );
	QObject::connect( delPageDia, SIGNAL( deletePage( int, DelPageMode ) ), this, SLOT( delPageOk( int, DelPageMode ) ) );
	delPageDia->show();
    }
}

/*===============================================================*/
void KPresenterView::editFind()
{
    if ( searchDia )
    {
	QObject::disconnect( searchDia, SIGNAL( doSearch( QString, bool, bool ) ), this, SLOT( search( QString, bool, bool ) ) );
	searchDia->close();
	delete searchDia;
	searchDia = 0;
    }

    if ( page->kTxtObj() )
    {
	searchDia = new KSearchDialog( this, "SearchDia" );
	searchDia->setMaximumSize( searchDia->width(), searchDia->height() );
	searchDia->setMinimumSize( searchDia->width(), searchDia->height() );
	QObject::connect( searchDia, SIGNAL( doSearch( QString, bool, bool ) ), this, SLOT( search( QString, bool, bool ) ) );
	searchDia->show();

	searchFirst = true;
    }
}

/*===============================================================*/
void KPresenterView::editFindReplace()
{
    if ( replaceDia )
    {
	QObject::disconnect( replaceDia, SIGNAL( doSearchReplace( QString, QString, bool, bool ) ), this, SLOT( replace( QString, QString, bool, bool ) ) );
	QObject::disconnect( replaceDia, SIGNAL( doSearchReplaceAll( QString, QString, bool ) ), this, SLOT( replaceAll( QString, QString, bool ) ) );
	replaceDia->close();
	delete replaceDia;
	replaceDia = 0;
    }

    if ( page->kTxtObj() )
    {
	replaceDia = new KSearchReplaceDialog( this, "ReplaceDia" );
	replaceDia->setMaximumSize( replaceDia->width(), replaceDia->height() );
	replaceDia->setMinimumSize( replaceDia->width(), replaceDia->height() );
	QObject::connect( replaceDia, SIGNAL( doSearchReplace( QString, QString, bool, bool ) ), this, SLOT( replace( QString, QString, bool, bool ) ) );
	QObject::connect( replaceDia, SIGNAL( doSearchReplaceAll( QString, QString, bool ) ), this, SLOT( replaceAll( QString, QString, bool ) ) );
	replaceDia->show();

	searchFirst = true;
    }
}

/*===============================================================*/
void KPresenterView::editHeaderFooter()
{
    page->setToolEditMode( TEM_MOUSE );
    m_pKPresenterDoc->getHeaderFooterEdit()->show();
}

/*========================= view new view =======================*/
void KPresenterView::newView()
{
    assert( ( m_pKPresenterDoc != 0L ) );

    KPresenterShell *shell = new KPresenterShell;
    shell->show();
    shell->setDocument( m_pKPresenterDoc );
}

/*====================== insert a new page ======================*/
void KPresenterView::insertPage()
{
    if ( insPageDia )
    {
	QObject::disconnect( insPageDia, SIGNAL( insertPage( int, InsPageMode, InsertPos ) ), this, SLOT( insPageOk( int, InsPageMode, InsertPos ) ) );
	insPageDia->close();
	delete insPageDia;
	insPageDia = 0;
    }

    QString url = m_pKPresenterDoc->url();
    insPageDia = new InsPageDia( this, "", m_pKPresenterDoc, getCurrPgNum() );
    insPageDia->setCaption( i18n( "KPresenter - Insert Page" ) );
    QObject::connect( insPageDia, SIGNAL( insertPage( int, InsPageMode, InsertPos ) ), this, SLOT( insPageOk( int, InsPageMode, InsertPos ) ) );
    insPageDia->show();
    m_pKPresenterDoc->setURL( url );
}

/*==============================================================*/
void KPresenterView::toolsMouse()
{
    page->setToolEditMode( TEM_MOUSE );
    page->deSelectAllObj();
}

/*====================== insert a picture =======================*/
void KPresenterView::insertPicture()
{
    page->setToolEditMode( TEM_MOUSE );
    page->deSelectAllObj();

    QString file = KFilePreviewDialog::getOpenFileName( QString::null,
							KImageIO::pattern( KImageIO::Reading ), 0);

    QCursor c = page->cursor();
    page->setCursor( waitCursor );
    if ( !file.isEmpty() ) m_pKPresenterDoc->insertPicture( file.data(), xOffset, yOffset );
    page->setCursor( c );
}

/*====================== insert a clipart =======================*/
void KPresenterView::insertClipart()
{
    page->setToolEditMode( TEM_MOUSE );
    page->deSelectAllObj();
    QString file = KFilePreviewDialog::getOpenFileName( QString::null, i18n( "*.WMF *.wmf|Windows Metafiles" ), 0L );
    if ( !file.isEmpty() ) m_pKPresenterDoc->insertClipart( file.data(), xOffset, yOffset );
}

/*=========================== insert line =======================*/
void KPresenterView::toolsLine()
{
    page->setToolEditMode( INS_LINE );
    page->deSelectAllObj();
}

/*===================== insert rectangle ========================*/
void KPresenterView::toolsRectangle()
{
    page->deSelectAllObj();
    page->setToolEditMode( INS_RECT );
}

/*===================== insert circle or ellipse ================*/
void KPresenterView::toolsCircleOrEllipse()
{
    page->deSelectAllObj();
    page->setToolEditMode( INS_ELLIPSE );
}

/*==============================================================*/
void KPresenterView::toolsPie()
{
    page->deSelectAllObj();
    page->setToolEditMode( INS_PIE );
}

/*==============================================================*/
void KPresenterView::toolsDiagramm()
{
    page->deSelectAllObj();
    page->setToolEditMode( INS_DIAGRAMM );

    QValueList<KoDocumentEntry> vec = KoDocumentEntry::query( "'IDL:KDiagramm/DocumentFactory:1.0#KDiagramm' in RepoIds", 1 );
    if ( vec.isEmpty() )
    {
	cout << "Got no results" << endl;
	QMessageBox::critical( this, i18n( "Error" ), i18n( "Sorry, no charting component registered" ), i18n( "Ok" ) );
	return;
    }

    cerr << "USING component " << vec[ 0 ].name.latin1() << endl;
    page->setPartEntry( vec[ 0 ] );
}

/*==============================================================*/
void KPresenterView::toolsTable()
{
    page->deSelectAllObj();
    page->setToolEditMode( INS_TABLE );

    QValueList<KoDocumentEntry> vec = KoDocumentEntry::query( "'IDL:KSpread/DocumentFactory:1.0#KSpread' in RepoIds", 1 );
    if ( vec.isEmpty() )
    {
	cout << "Got no results" << endl;
	QMessageBox::critical( this, i18n( "Error" ), i18n( "Sorry, no table component registered" ), i18n( "Ok" ) );
	return;
    }

    cerr << "USING component " << vec[ 0 ].name.latin1() << endl;
    page->setPartEntry( vec[ 0 ] );
}

/*==============================================================*/
void KPresenterView::toolsFormula()
{
    page->deSelectAllObj();
    page->setToolEditMode( INS_FORMULA );

    QValueList<KoDocumentEntry> vec = KoDocumentEntry::query( "'IDL:KFormula/DocumentFactory:1.0#KFormula' in RepoIds", 1 );
    if ( vec.isEmpty() )
    {
	cout << "Got no results" << endl;
	QMessageBox::critical( this, i18n( "Error" ), i18n( "Sorry, no formula component registered" ), i18n( "Ok" ) );
	return;
    }

    cerr << "USING component " << vec[ 0 ].name.latin1() << endl;
    page->setPartEntry( vec[ 0 ] );
}

/*===================== insert a textobject =====================*/
void KPresenterView::toolsText()
{
    page->deSelectAllObj();
    page->setToolEditMode( INS_TEXT );
}

/*===============================================================*/
void KPresenterView::toolsAutoform()
{
    page->deSelectAllObj();
    page->setToolEditMode( TEM_MOUSE );
    if ( afChoose )
    {
	QObject::disconnect( afChoose, SIGNAL( formChosen( const QString & ) ), this, SLOT( afChooseOk( const QString & ) ) );
	afChoose->close();
	delete afChoose;
	afChoose = 0;
    }
    afChoose = new AFChoose( this, i18n( "Autoform-Choose" ) );
    afChoose->resize( 400, 300 );
    afChoose->setCaption( i18n( "KPresenter - Insert an Autoform" ) );
    QObject::connect( afChoose, SIGNAL( formChosen( const QString & ) ), this, SLOT( afChooseOk( const QString & ) ) );
    afChoose->show();
}

/*===============================================================*/
void KPresenterView::toolsObject()
{
    page->deSelectAllObj();
    page->setToolEditMode( TEM_MOUSE );

    KoDocumentEntry pe = KoPartSelectDia::selectPart();
    if ( pe.name.isEmpty() )
	return;

    page->setToolEditMode( INS_OBJECT );
    page->setPartEntry( pe );
}

/*===============================================================*/
void KPresenterView::extraPenBrush()
{
    if ( styleDia )
    {
	QObject::disconnect( styleDia, SIGNAL( styleOk() ), this, SLOT( styleOk() ) );
	styleDia->close();
	delete styleDia;
	styleDia = 0;
    }
    styleDia = new StyleDia( this, "StyleDia", m_pKPresenterDoc->getPenBrushFlags() );
    styleDia->setPen( m_pKPresenterDoc->getPen( pen ) );
    styleDia->setBrush( m_pKPresenterDoc->getBrush( brush ) );
    styleDia->setLineBegin( m_pKPresenterDoc->getLineBegin( lineBegin ) );
    styleDia->setLineEnd( m_pKPresenterDoc->getLineEnd( lineEnd ) );
    styleDia->setFillType( m_pKPresenterDoc->getFillType( fillType ) );
    styleDia->setGradient( m_pKPresenterDoc->getGColor1( gColor1 ),
			   m_pKPresenterDoc->getGColor2( gColor2 ),
			   m_pKPresenterDoc->getGType( gType ),
			   m_pKPresenterDoc->getGUnbalanced( gUnbalanced ),
			   m_pKPresenterDoc->getGXFactor( gXFactor ),
			   m_pKPresenterDoc->getGYFactor( gYFactor ) );
    styleDia->setCaption( i18n( "KPresenter - Pen and Brush" ) );
    QObject::connect( styleDia, SIGNAL( styleOk() ), this, SLOT( styleOk() ) );
    page->setToolEditMode( TEM_MOUSE );
    styleDia->show();
}

/*===============================================================*/
void KPresenterView::extraConfigPie()
{
    if ( confPieDia )
    {
	QObject::disconnect( confPieDia, SIGNAL( confPieDiaOk() ), this, SLOT( confPieOk() ) );
	confPieDia->close();
	delete confPieDia;
	confPieDia = 0;
    }

    confPieDia = new ConfPieDia( this, "ConfPageDia" );
    confPieDia->setMaximumSize( confPieDia->width(), confPieDia->height() );
    confPieDia->setMinimumSize( confPieDia->width(), confPieDia->height() );
    confPieDia->setType( m_pKPresenterDoc->getPieType( pieType ) );
    confPieDia->setAngle( m_pKPresenterDoc->getPieAngle( pieAngle ) );
    confPieDia->setLength( m_pKPresenterDoc->getPieLength( pieLength ) );
    confPieDia->setPenBrush( m_pKPresenterDoc->getPen( pen ), m_pKPresenterDoc->getBrush( brush ) );
    confPieDia->setCaption( i18n( "KPresenter - Configure Pie/Arc/Chord" ) );
    QObject::connect( confPieDia, SIGNAL( confPieDiaOk() ), this, SLOT( confPieOk() ) );
    page->setToolEditMode( TEM_MOUSE );
    confPieDia->show();
}

/*===============================================================*/
void KPresenterView::extraConfigRect()
{
    if ( confRectDia )
    {
	QObject::disconnect( confRectDia, SIGNAL( confRectDiaOk() ), this, SLOT( confRectOk() ) );
	confRectDia->close();
	delete confRectDia;
	confRectDia = 0;
    }

    confRectDia = new ConfRectDia( this, "ConfRectDia" );
    confRectDia->setMaximumSize( confRectDia->width(), confRectDia->height() );
    confRectDia->setMinimumSize( confRectDia->width(), confRectDia->height() );
    confRectDia->setRnds( m_pKPresenterDoc->getRndX( rndX ), m_pKPresenterDoc->getRndY( rndY ) );
    confRectDia->setCaption( i18n( "KPresenter - Configure Rectangle" ) );
    QObject::connect( confRectDia, SIGNAL( confRectDiaOk() ), this, SLOT( confRectOk() ) );
    page->setToolEditMode( TEM_MOUSE );
    confRectDia->show();
}

/*===============================================================*/
void KPresenterView::extraRaise()
{
    page->setToolEditMode( TEM_MOUSE );
    m_pKPresenterDoc->raiseObjs( xOffset, yOffset );
}

/*===============================================================*/
void KPresenterView::extraLower()
{
    page->setToolEditMode( TEM_MOUSE );
    m_pKPresenterDoc->lowerObjs( xOffset, yOffset );
}

/*===============================================================*/
void KPresenterView::extraRotate()
{
    if ( rotateDia )
    {
	QObject::disconnect( rotateDia, SIGNAL( rotateDiaOk() ), this, SLOT( rotateOk() ) );
	rotateDia->close();
	delete rotateDia;
	rotateDia = 0;
    }

    if ( m_pKPresenterDoc->numSelected() > 0 )
    {
	rotateDia = new RotateDia( this, "Rotate" );
	rotateDia->setMaximumSize( rotateDia->width(), rotateDia->height() );
	rotateDia->setMinimumSize( rotateDia->width(), rotateDia->height() );
	rotateDia->setCaption( i18n( "KPresenter - Rotate" ) );
	QObject::connect( rotateDia, SIGNAL( rotateDiaOk() ), this, SLOT( rotateOk() ) );
	rotateDia->setAngle( m_pKPresenterDoc->getSelectedObj()->getAngle() );
	page->setToolEditMode( TEM_MOUSE );
	rotateDia->show();
    }
}

/*===============================================================*/
void KPresenterView::extraShadow()
{
    if ( shadowDia )
    {
	QObject::disconnect( shadowDia, SIGNAL( shadowDiaOk() ), this, SLOT( shadowOk() ) );
	shadowDia->close();
	delete shadowDia;
	shadowDia = 0;
    }

    if ( m_pKPresenterDoc->numSelected() > 0 )
    {
	shadowDia = new ShadowDia( this, "Shadow" );
	shadowDia->setMaximumSize( shadowDia->width(), shadowDia->height() );
	shadowDia->setMinimumSize( shadowDia->width(), shadowDia->height() );
	shadowDia->setCaption( i18n( "KPresenter - Shadow" ) );
	QObject::connect( shadowDia, SIGNAL( shadowDiaOk() ), this, SLOT( shadowOk() ) );
	shadowDia->setShadowDirection( m_pKPresenterDoc->getSelectedObj()->getShadowDirection() );
	shadowDia->setShadowDistance( m_pKPresenterDoc->getSelectedObj()->getShadowDistance() );
	shadowDia->setShadowColor( m_pKPresenterDoc->getSelectedObj()->getShadowColor() );
	page->setToolEditMode( TEM_MOUSE );
	shadowDia->show();
    }
}

/*===============================================================*/
void KPresenterView::extraAlignObj()
{
    page->setToolEditMode( TEM_MOUSE );

    QPoint pnt( QCursor::pos() );

    rb_oalign->popup( pnt );
}

/*===============================================================*/
void KPresenterView::extraBackground()
{
    if ( backDia )
    {
	QObject::disconnect( backDia, SIGNAL( backOk( bool ) ), this, SLOT( backOk( bool ) ) );
	backDia->close();
	delete backDia;
	backDia = 0;
    }
    backDia = new BackDia( this, "InfoDia", m_pKPresenterDoc->getBackType( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc->getBackColor1( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc->getBackColor2( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc->getBackColorType( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc->getBackPixFilename( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc->getBackClipFilename( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc->getBackView( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc->getBackUnbalanced( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc->getBackXFactor( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc->getBackYFactor( getCurrPgNum() - 1 ) );
    backDia->setMaximumSize( backDia->width(), backDia->height() );
    backDia->setMinimumSize( backDia->width(), backDia->height() );
    backDia->setCaption( i18n( "KPresenter - Page Background" ) );
    QObject::connect( backDia, SIGNAL( backOk( bool ) ), this, SLOT( backOk( bool ) ) );
    backDia->show();
}

/*===============================================================*/
void KPresenterView::extraLayout()
{
    KoPageLayout pgLayout = m_pKPresenterDoc->pageLayout();
    KoPageLayout oldLayout = m_pKPresenterDoc->pageLayout();
    KoHeadFoot hf;

    if ( KoPageLayoutDia::pageLayout( pgLayout, hf, FORMAT_AND_BORDERS | USE_NEW_STUFF ) )
    {
	PgLayoutCmd *pgLayoutCmd = new PgLayoutCmd( i18n( "Set Pagelayout" ), pgLayout, oldLayout, this );
	pgLayoutCmd->execute();
	kPresenterDoc()->commands()->addCommand( pgLayoutCmd );
    }
}

/*===============================================================*/
void KPresenterView::extraOptions()
{
    if ( optionDia )
    {
	QObject::disconnect( optionDia, SIGNAL( applyButtonPressed() ), this, SLOT( optionOk() ) );
	optionDia->close();
	delete optionDia;
	optionDia = 0;
    }
    optionDia = new OptionDia( this, "OptionDia" );
    optionDia->setCaption( i18n( "KPresenter - Options" ) );
    QObject::connect( optionDia, SIGNAL( applyButtonPressed() ), this, SLOT( optionOk() ) );
    optionDia->setRastX( kPresenterDoc()->getRastX() );
    optionDia->setRastY( kPresenterDoc()->getRastY() );
    optionDia->setBackCol( kPresenterDoc()->getTxtBackCol() );
    optionDia->show();
}

/*===============================================================*/
void KPresenterView::extraWebPres()
{
    if ( !allowWebPres )
	return;

    m_vToolBarExtra->setItemEnabled( ID_TOOL_WEBPRES, false );
    m_vMenuExtra->setItemEnabled( m_idMenuExtra_WepPres, false );

    QString config = QString::null;
    if ( QMessageBox::information( this, i18n( "Create Web-Presentation" ),
				   i18n( "Do you want to load a configuration which should be used for this\n"
					 "Web-Presentation, which you have already saved earlier?" ),
				   i18n( "&Yes" ), i18n( "&No" ), QString::null, 1, 1 ) == 0 )
	config = KFileDialog::getOpenFileName( QString::null, "*.kpweb|KPresenter Web-Presentation" );

    KPWebPresentationWizard::createWebPresentation( config, m_pKPresenterDoc, this );
}

/*===============================================================*/
void KPresenterView::extraLineBegin()
{
    page->setToolEditMode( TEM_MOUSE );

    QPoint pnt( QCursor::pos() );

    rb_lbegin->popup( pnt );
}

/*===============================================================*/
void KPresenterView::extraLineEnd()
{
    page->setToolEditMode( TEM_MOUSE );

    QPoint pnt( QCursor::pos() );

    rb_lend->popup( pnt );
}

/*========================== screen config pages ================*/
void KPresenterView::screenConfigPages()
{
    if ( pgConfDia )
    {
	QObject::disconnect( pgConfDia, SIGNAL( pgConfDiaOk() ), this, SLOT( pgConfOk() ) );
	pgConfDia->close();
	delete pgConfDia;
	pgConfDia = 0;
    }
    pgConfDia = new PgConfDia( this, kPresenterDoc(), "PageConfig", kPresenterDoc()->spInfinitLoop(),
			       kPresenterDoc()->spManualSwitch(), getCurrPgNum(),
			       kPresenterDoc()->backgroundList()->at( getCurrPgNum() - 1 )->getPageEffect(),
			       kPresenterDoc()->getPresSpeed(), kPresenterDoc()->getPresentSlides(),
			       kPresenterDoc()->getSelectedSlides() );
    pgConfDia->setCaption( i18n( "KPresenter - Page Configuration for Screenpresentations" ) );
    QObject::connect( pgConfDia, SIGNAL( pgConfDiaOk() ), this, SLOT( pgConfOk() ) );
    pgConfDia->show();
}

/*========================== screen presStructView  =============*/
void KPresenterView::screenPresStructView()
{
    if ( !presStructView )
    {
	page->deSelectAllObj();
	page->setToolEditMode( TEM_MOUSE );

	presStructView = new KPPresStructView( this, "", kPresenterDoc(), this );
	presStructView->setCaption( i18n( "KPresenter - Presentation structure viewer" ) );
	QObject::connect( presStructView, SIGNAL( presStructViewClosed() ), this, SLOT( psvClosed() ) );
	presStructView->show();
    }
}

/*===============================================================*/
void KPresenterView::screenAssignEffect()
{
    int _pNum, _oNum;

    if ( effectDia )
    {
	QObject::disconnect( effectDia, SIGNAL( effectDiaOk() ), this, SLOT( effectOk() ) );
	effectDia->close();
	delete effectDia;
	effectDia = 0;
    }

    if ( page->canAssignEffect( _pNum, _oNum ) && _pNum >= 1 )
    {
	effectDia = new EffectDia( this, "Effect", _pNum, _oNum, ( KPresenterView* )this );
	effectDia->setCaption( i18n( "KPresenter - Assign effects" ) );
	QObject::connect( effectDia, SIGNAL( effectDiaOk() ), this, SLOT( effectOk() ) );
	effectDia->show();
	page->deSelectAllObj();
	page->setToolEditMode( TEM_MOUSE );
	page->selectObj( _oNum );
    }
    else
	QMessageBox::critical( this, i18n( "KPresenter Error" ),
			       i18n( "I can't assign an effect. You have to select EXACTLY one object!" ),
			       i18n( "OK" ) );
}

/*========================== screen start =======================*/
void KPresenterView::screenStart()
{
    bool fullScreen = true; //m_rToolBarScreen->isButtonOn( m_idButtonScreen_Full );
    int curPg = getCurrPgNum();

    if ( page && !presStarted )
    {
	// disable screensaver
	QString pidFile;
	pidFile = getenv( "HOME" );
	pidFile += "/.kss.pid";
	FILE *fp;
	if ( ( fp = fopen( pidFile, "r" ) ) != NULL )
	{
	    fscanf( fp, "%d", &screensaver_pid );
	    fclose( fp );
	    kill( screensaver_pid, SIGSTOP );
	}

	page->deSelectAllObj();
	presStarted = true;
	if ( fullScreen )
	{
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );

	    float _presFaktW = static_cast<float>( page->width() ) /
			       static_cast<float>( kPresenterDoc()->getPageSize( 0, 0, 0, 1.0, false ).width() ) > 1.0 ?
			       static_cast<float>( page->width() ) / static_cast<float>( kPresenterDoc()->getPageSize( 0, 0, 0, 1.0, false ).width() ) : 1.0;
	    float _presFaktH = static_cast<float>( page->height() ) /
			       static_cast<float>( kPresenterDoc()->getPageSize( 0, 0, 0, 1.0, false ).height() ) >
			       1.0 ? static_cast<float>( page->height() ) / static_cast<float>( kPresenterDoc()->getPageSize( 0, 0, 0, 1.0, false ).height() ) : 1.0;
	    float _presFakt = min(_presFaktW,_presFaktH);
	    page->setPresFakt( _presFakt );
	}
	else
	{
	    float _presFakt = 1.0;
	    page->setPresFakt( _presFakt );
	}

	_xOffset = xOffset;
	_yOffset = yOffset;
	xOffset = 0;
	yOffset = 0;

	if ( page->width() > kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).width() )
	    xOffset -= ( page->width() - kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).width() ) / 2;
	if ( page->height() > kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() )
	    yOffset -= ( page->height() - kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() ) / 2;

	vert->setEnabled( false );
	horz->setEnabled( false );
	m_bShowGUI = false;
	page->setBackgroundColor( black );
	oldSize = widget()->size();
	widget()->resize( page->size() );
	setSize( page->size().width(), page->size().height() );
	page->startScreenPresentation( fullScreen, curPg );

	yOffset = ( page->presPage() - 1 ) * kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height();
	if ( page->height() > kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() )
	    yOffset -= ( page->height() - kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() ) / 2;

	if ( fullScreen )
	{
	    page->recreate( ( QWidget* )0L, WStyle_Customize | WStyle_NoBorder | WType_Popup, QPoint( 0, 0 ), true );
	    page->topLevelWidget()->move( 0, 0 );
	    page->topLevelWidget()->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    page->topLevelWidget()->setBackgroundColor( black );
	    page->setFocusPolicy( QWidget::StrongFocus );
	    page->setFocus();
	}
	else
	{
	    page->setBackgroundColor( black );
	    page->setFocusPolicy( QWidget::StrongFocus );
	    page->setFocus();
	}

	if ( !kPresenterDoc()->spManualSwitch() )
	{
	    continuePres = true;
	    exitPres = false;
	    doAutomaticScreenPres();
	}
    }
}

/*========================== screen stop ========================*/
void KPresenterView::screenStop()
{
    if ( presStarted )
    {
	continuePres = false;
	exitPres = true;
	if ( true ) //m_rToolBarScreen->isButtonOn( m_idButtonScreen_Full ) )
	{
	    page->close( false );
	    page->recreate( ( QWidget* )this, 0, QPoint( 0, 0 ), true );
	    page->lower();
	}
	xOffset = _xOffset;
	yOffset = _yOffset;
	page->stopScreenPresentation();
	presStarted = false;
	vert->setEnabled( true );
	horz->setEnabled( true );
	m_bShowGUI = true;
	page->setMouseTracking( true );
	page->setBackgroundColor( white );
	setSize( oldSize.width(), oldSize.height() );
	widget()->resize( oldSize );
	resizeEvent( 0L );

	// start screensaver again
	QString pidFile;
	pidFile = getenv( "HOME" );
	pidFile += "/.kss.pid";
	FILE *fp;
	if ( ( fp = fopen( pidFile, "r" ) ) != NULL )
	{
	    fscanf( fp, "%d", &screensaver_pid );
	    fclose( fp );
	    kill( screensaver_pid, SIGCONT );
	}
    }
}

/*========================== screen pause =======================*/
void KPresenterView::screenPause()
{
}

/*========================== screen first =======================*/
void KPresenterView::screenFirst()
{
    vert->setValue( 0 );
}

/*========================== screen pevious =====================*/
void KPresenterView::screenPrev()
{
    if ( presStarted )
    {
	if ( page->pPrev( true ) )
	{
	    yOffset = ( page->presPage() - 1 ) * kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height();
	    if ( page->height() > kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() )
		yOffset -= ( page->height() - kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() ) / 2;
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    page->repaint( false );
	    page->setFocus();
	}
	else
	{
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    page->setFocus();
	}
    }
    else
	vert->setValue( yOffset - kPresenterDoc()->getPageSize( 0, 0, 0, 1.0, false ).height() );
}

/*========================== screen next ========================*/
void KPresenterView::screenNext()
{
    if ( presStarted )
    {
	if ( page->pNext( true ) )
	{
	    yOffset = ( page->presPage() - 1 ) * kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height();
	    if ( page->height() > kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() )
		yOffset -= ( page->height() - kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() ) / 2;
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    page->setFocus();
	}
	else
	{
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    page->setFocus();
	}
    }
    else
	vert->setValue( yOffset + kPresenterDoc()->getPageSize( 0, 0, 0, 1.0, false ).height() );
}

/*========================== screen last ========================*/
void KPresenterView::screenLast()
{
    vert->setValue(vert->maxValue());
}

/*========================== screen skip =======================*/
void KPresenterView::screenSkip()
{
}

/*========================== screen full screen ================*/
void KPresenterView::screenFullScreen()
{
    qWarning( "Screenpresentations only work in FULLSCREEN mode at the moment!" );
}

/*========================== screen pen/marker =================*/
void KPresenterView::screenPen()
{
    QPoint pnt( QCursor::pos() );

    rb_pen->popup( pnt );
}

/*======================= help contents ========================*/
void KPresenterView::helpContents()
{
    KoAboutDia::about( KoAboutDia::KOffice, "0.0.1" );
}

/*===============================================================*/
void KPresenterView::sizeSelected( const CORBA::WChar *size )
{
    tbFont.setPointSize( C2Q( size ).toInt() );
    page->setTextFont( &tbFont );
}

/*===============================================================*/
void KPresenterView::fontSelected( const CORBA::WChar *font )
{
    tbFont.setFamily( C2Q( font ) );
    page->setTextFont( &tbFont );
}

/*===============================================================*/
void KPresenterView::textBold()
{
    tbFont.setBold( !tbFont.bold() );
    page->setTextFont( &tbFont );
}

/*===============================================================*/
void KPresenterView::textItalic()
{
    tbFont.setItalic( !tbFont.italic() );
    page->setTextFont( &tbFont );
}

/*===============================================================*/
void KPresenterView::textUnderline()
{
    tbFont.setUnderline( !tbFont.underline() );
    page->setTextFont( &tbFont );
}

/*===============================================================*/
void KPresenterView::textColor()
{
    if ( KColorDialog::getColor( tbColor ) )
    {
	OpenPartsUI::Pixmap pix;
	pix.data = CORBA::string_dup( colorToPixString( tbColor ) );
	m_vToolBarText->setButtonPixmap( ID_TEXT_COLOR, pix );
	page->setTextColor( &tbColor );
    }
}

/*===============================================================*/
void KPresenterView::textAlignLeft()
{
    tbAlign = TxtParagraph::LEFT;
    page->setTextAlign( tbAlign );

    m_vToolBarText->setButton( ID_ALEFT, true );
    m_vToolBarText->setButton( ID_ACENTER, false );
    m_vToolBarText->setButton( ID_ARIGHT, false );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Left, true );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Center, false );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Right, false );
}

/*===============================================================*/
void KPresenterView::textAlignCenter()
{
    tbAlign = TxtParagraph::CENTER;
    page->setTextAlign( TxtParagraph::CENTER );

    m_vToolBarText->setButton( ID_ALEFT, false );
    m_vToolBarText->setButton( ID_ACENTER, true );
    m_vToolBarText->setButton( ID_ARIGHT, false );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Left, false );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Center, true );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Right, false );
}

/*===============================================================*/
void KPresenterView::textAlignRight()
{
    tbAlign = TxtParagraph::RIGHT;
    page->setTextAlign( TxtParagraph::RIGHT );

    m_vToolBarText->setButton( ID_ALEFT, false );
    m_vToolBarText->setButton( ID_ACENTER, false );
    m_vToolBarText->setButton( ID_ARIGHT, true );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Left, false );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Center, false );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Right, true );
}

/*===============================================================*/
void KPresenterView::mtextAlignLeft()
{
    tbAlign = TxtParagraph::LEFT;
    page->setTextAlign( tbAlign );

    m_vToolBarText->setButton( ID_ALEFT, true );
    m_vToolBarText->setButton( ID_ACENTER, false );
    m_vToolBarText->setButton( ID_ARIGHT, false );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Left, true );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Center, false );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Right, false );
}

/*===============================================================*/
void KPresenterView::mtextAlignCenter()
{
    tbAlign = TxtParagraph::CENTER;
    page->setTextAlign( TxtParagraph::CENTER );

    m_vToolBarText->setButton( ID_ALEFT, false );
    m_vToolBarText->setButton( ID_ACENTER, true );
    m_vToolBarText->setButton( ID_ARIGHT, false );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Left, false );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Center, true );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Right, false );
}

/*===============================================================*/
void KPresenterView::mtextAlignRight()
{
    tbAlign = TxtParagraph::RIGHT;
    page->setTextAlign( TxtParagraph::RIGHT );

    m_vToolBarText->setButton( ID_ALEFT, false );
    m_vToolBarText->setButton( ID_ACENTER, false );
    m_vToolBarText->setButton( ID_ARIGHT, true );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Left, false );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Center, false );
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Right, true );
}

/*===============================================================*/
void KPresenterView::mtextFont()
{
    QFont tmpFont = tbFont;

    if ( KFontDialog::getFont( tmpFont ) )
    {
	fontChanged( &tmpFont );
	tbFont = tmpFont;
	page->setTextFont( &tbFont );
    }
}

/*===============================================================*/
void KPresenterView::textEnumList()
{
    if ( page->kTxtObj() )
    {
	int _type = page->kTxtObj()->enumListType().type;
	QFont _font = page->kTxtObj()->enumListType().font;
	QColor _color = page->kTxtObj()->enumListType().color;
	QString _before = page->kTxtObj()->enumListType().before;
	QString _after = page->kTxtObj()->enumListType().after;
	int _start = page->kTxtObj()->enumListType().start;

	if ( KEnumListDia::enumListDia( _type, _font, _color, _before, _after, _start, fontList ) )
	{
	    KTextObject::EnumListType elt;
	    elt.type = _type;
	    elt.font = _font;
	    elt.color = _color;
	    elt.before = _before;
	    elt.after = _after;
	    elt.start = _start;
	    page->kTxtObj()->setEnumListType( elt );
	}

	page->kTxtObj()->setObjType( KTextObject::ENUM_LIST );
    }
}

/*===============================================================*/
void KPresenterView::textUnsortList()
{
    if ( page->kTxtObj() )
    {
	QList<QFont> *_font = page->kTxtObj()->unsortListType().font;
	QList<QColor> *_color = page->kTxtObj()->unsortListType().color;
	QList<QChar> *_c = page->kTxtObj()->unsortListType().chr;

	if ( KCharSelectDia::selectChar( _font, _color, _c ) )
	{
	    KTextObject::UnsortListType ult;
	    ult.font = _font;
	    ult.color = _color;
	    ult.chr = _c;
	    page->kTxtObj()->setUnsortListType( ult );
	}

	page->kTxtObj()->setObjType( KTextObject::UNSORT_LIST );
    }
}

/*===============================================================*/
void KPresenterView::textNormalText()
{
    if ( page->kTxtObj() ) page->kTxtObj()->setObjType( KTextObject::PLAIN );
}

/*===============================================================*/
void KPresenterView::textDepthPlus()
{
    if ( page->kTxtObj() ) page->kTxtObj()->incDepth();
}

/*===============================================================*/
void KPresenterView::textDepthMinus()
{
    if ( page->kTxtObj() ) page->kTxtObj()->decDepth();
}

/*===============================================================*/
void KPresenterView::textSpacing()
{
    KTextObject *obj = 0L;

    if ( page->kTxtObj() ) obj = page->kTxtObj();
    else if ( page->haveASelectedTextObj() ) obj = page->haveASelectedTextObj();

    if ( obj )
    {
	if ( spacingDia )
	{
	    QObject::disconnect( spacingDia, SIGNAL( spacingDiaOk() ), this, SLOT( spacingOk() ) );
	    spacingDia->close();
	    delete spacingDia;
	    spacingDia = 0;
	}

	spacingDia = new SpacingDia( this, obj->getLineSpacing(), obj->getDistBefore(), obj->getDistAfter(), obj->getGap() );
	spacingDia->setMaximumSize( spacingDia->width(), spacingDia->height() );
	spacingDia->setMinimumSize( spacingDia->width(), spacingDia->height() );
	spacingDia->setCaption( i18n( "KPresenter - Spacings" ) );
	QObject::connect( spacingDia, SIGNAL( spacingDiaOk( int, int, int, int ) ), this, SLOT( spacingOk( int, int, int, int ) ) );
	spacingDia->show();
    }
}

/*===============================================================*/
void KPresenterView::textContentsToHeight()
{
    KTextObject *txtObj = 0L;

    if ( page->kTxtObj() )
	txtObj = page->kTxtObj();
    else if ( page->haveASelectedTextObj() )
	txtObj = page->haveASelectedTextObj();

    if ( txtObj )
	txtObj->extendContents2Height();

    if ( page->haveASelectedTextObj() )
	m_pKPresenterDoc->repaint( false );
}

/*===============================================================*/
void KPresenterView::textObjectToContents()
{
    KPTextObject *txtObj = 0L;

    if ( page->kpTxtObj() )
	txtObj = page->kpTxtObj();
    else if ( page->haveASelectedKPTextObj() )
	txtObj = page->haveASelectedKPTextObj();

    if ( txtObj )
	txtObj->extendObject2Contents( this );

    if ( page->haveASelectedKPTextObj() )
	m_pKPresenterDoc->repaint( false );
}

/*===============================================================*/
void KPresenterView::setPenColor( CORBA::Long id )
{
    CORBA::Short red, green, blue;
    CORBA::Boolean fill;

    if( !m_vColorBar->getRgbColor( id, red, green, blue, fill ) )
	return;

    if ( !m_pKPresenterDoc->setPenColor( QColor( red, green, blue ), fill ) )
    {
	if ( fill )
	    pen.setColor( QColor( red, green, blue ) );
	else
	    pen = NoPen;
    }
}

/*===============================================================*/
void KPresenterView::setFillColor( CORBA::Long id )
{
    CORBA::Short red, green, blue;
    CORBA::Boolean fill;

    if( !m_vColorBar->getRgbColor( id, red, green, blue, fill ) )
	return;

    if ( !m_pKPresenterDoc->setBrushColor( QColor( red, green, blue ), fill ) )
    {
	if ( fill )
	    brush.setColor( QColor( red, green, blue ) );
	else
	    brush = NoBrush;
    }
}

/*======================= align object left =====================*/
void KPresenterView::extraAlignObjLeftidl()
{
    kPresenterDoc()->alignObjsLeft();
}

/*======================= align object center h =================*/
void KPresenterView::extraAlignObjCenterHidl()
{
    kPresenterDoc()->alignObjsCenterH();
}

/*======================= align object right ====================*/
void KPresenterView::extraAlignObjRightidl()
{
    kPresenterDoc()->alignObjsRight();
}

/*======================= align object top ======================*/
void KPresenterView::extraAlignObjTopidl()
{
    kPresenterDoc()->alignObjsTop();
}

/*======================= align object center v =================*/
void KPresenterView::extraAlignObjCenterVidl()
{
    kPresenterDoc()->alignObjsCenterV();
}

/*======================= align object bottom ===================*/
void KPresenterView::extraAlignObjBottomidl()
{
    kPresenterDoc()->alignObjsBottom();
}

/*===============================================================*/
void KPresenterView::extraLineBeginNormal()
{
    if ( !m_pKPresenterDoc->setLineBegin( L_NORMAL ) )
	lineBegin = L_NORMAL;
}

/*===============================================================*/
void KPresenterView::extraLineBeginArrow()
{
    if ( !m_pKPresenterDoc->setLineBegin( L_ARROW ) )
	lineBegin = L_ARROW;
}

/*===============================================================*/
void KPresenterView::extraLineBeginRect()
{
    if ( !m_pKPresenterDoc->setLineBegin( L_SQUARE ) )
	lineBegin = L_SQUARE;
}

/*===============================================================*/
void KPresenterView::extraLineBeginCircle()
{
    if ( !m_pKPresenterDoc->setLineBegin( L_CIRCLE ) )
	lineBegin = L_CIRCLE;
}

/*===============================================================*/
void KPresenterView::extraLineEndNormal()
{
    if ( !m_pKPresenterDoc->setLineEnd( L_NORMAL ) )
	lineEnd = L_NORMAL;
}

/*===============================================================*/
void KPresenterView::extraLineEndArrow()
{
    if ( !m_pKPresenterDoc->setLineEnd( L_ARROW ) )
	lineEnd = L_ARROW;
}

/*===============================================================*/
void KPresenterView::extraLineEndRect()
{
    if ( !m_pKPresenterDoc->setLineEnd( L_SQUARE ) )
	lineEnd = L_SQUARE;
}

/*===============================================================*/
void KPresenterView::extraLineEndCircle()
{
    if ( !m_pKPresenterDoc->setLineEnd( L_CIRCLE ) )
	lineEnd = L_CIRCLE;
}

/*===============================================================*/
void KPresenterView::newPageLayout( KoPageLayout _layout )
{
    KoPageLayout oldLayout = m_pKPresenterDoc->pageLayout();

    PgLayoutCmd *pgLayoutCmd = new PgLayoutCmd( i18n( "Set Pagelayout" ), _layout, oldLayout, this );
    pgLayoutCmd->execute();
    kPresenterDoc()->commands()->addCommand( pgLayoutCmd );
}

/*======================== create GUI ==========================*/
void KPresenterView::createGUI()
{
    sdeb( "void KPresenterView::createGUI() %i | %i\n", refCount(), _refcnt() );

    // setup page
    page = new Page( this, "Page", ( KPresenterView* )this );
    QObject::connect( page, SIGNAL( fontChanged( QFont* ) ), this, SLOT( fontChanged( QFont* ) ) );
    QObject::connect( page, SIGNAL( colorChanged( QColor* ) ), this, SLOT( colorChanged( QColor* ) ) );
    QObject::connect( page, SIGNAL( alignChanged( TxtParagraph::HorzAlign ) ), this, SLOT( alignChanged( TxtParagraph::HorzAlign ) ) );

    // setup GUI
    setupPopupMenus();
    setupScrollbars();
    setRanges();
    setupRulers();

    if ( m_pKPresenterDoc && page )
	QObject::connect( page, SIGNAL( stopPres() ), this, SLOT( stopPres() ) );

    resizeEvent( 0L );
}

/*====================== construct ==============================*/
void KPresenterView::construct()
{
    if ( m_pKPresenterDoc == 0L && !m_bUnderConstruction ) return;

    assert( m_pKPresenterDoc != 0L );

    m_bUnderConstruction = false;

    KPObject *kpobject;
    for ( unsigned int i = 0; i < m_pKPresenterDoc->objectList()->count(); i++ )
    {
	kpobject = m_pKPresenterDoc->objectList()->at( i );
	if ( kpobject->getType() == OT_PART )
	    slotInsertObject( dynamic_cast<KPPartObject*>( kpobject )->getChild(), dynamic_cast<KPPartObject*>( kpobject ) );
    }

    // We are now in sync with the document
    m_bKPresenterModified = false;

    resizeEvent( 0L );
}

/*======================== document modified ===================*/
void KPresenterView::slotKPresenterModified()
{
    m_bKPresenterModified = true;
    update();
}

/*======================= insert object ========================*/
void KPresenterView::slotInsertObject( KPresenterChild *_child, KPPartObject *_kppo )
{
    OpenParts::View_var v;

    try
    {
	v = _child->createView( m_vKoMainWindow );
    }
    catch ( OpenParts::Document::MultipleViewsNotSupported &_ex )
    {
	// HACK
	printf( "void KPresenterView::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )\n" );
	printf( "Could not create view\n" );
	exit( 1 );
    }

    if ( CORBA::is_nil( v ) )
    {
	printf( "void KPresenterView::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )\n" );
	printf( "return value is 0L\n" );
	exit( 1 );
    }

    KOffice::View_var kv = KOffice::View::_narrow( v );
    kv->setMode( KOffice::View::ChildMode );
    assert( !CORBA::is_nil( kv ) );
    _kppo->setView( kv );
}

/*========================== update child geometry =============*/
void KPresenterView::slotUpdateChildGeometry( KPresenterChild * /*_child*/ )
{
}

/*======================= slot geometry end ====================*/
void KPresenterView::slotGeometryEnd( KoFrame* /*_frame*/ )
{
}

/*==================== slot move end ===========================*/
void KPresenterView::slotMoveEnd( KoFrame* /*_frame*/ )
{
}

/*=========== take changes for backgr dialog =====================*/
void KPresenterView::backOk( bool takeGlobal )
{
    SetBackCmd *setBackCmd = new SetBackCmd( i18n( "Set Background" ), backDia->getBackColor1(),
					     backDia->getBackColor2(), backDia->getBackColorType(),
					     backDia->getBackUnbalanced(),
					     backDia->getBackXFactor(), backDia->getBackYFactor(),
					     backDia->getBackPixFilename(), backDia->getBackClipFilename(),
					     backDia->getBackView(), backDia->getBackType(),
					     m_pKPresenterDoc->getBackColor1( getCurrPgNum() - 1 ),
					     m_pKPresenterDoc->getBackColor2( getCurrPgNum() - 1 ),
					     m_pKPresenterDoc->getBackColorType( getCurrPgNum() - 1 ),
					     m_pKPresenterDoc->getBackUnbalanced( getCurrPgNum() - 1 ),
					     m_pKPresenterDoc->getBackXFactor( getCurrPgNum() - 1 ),
					     m_pKPresenterDoc->getBackYFactor( getCurrPgNum() - 1 ),
					     m_pKPresenterDoc->getBackPixFilename( getCurrPgNum() - 1 ),
					     m_pKPresenterDoc->getBackClipFilename( getCurrPgNum() - 1 ),
					     m_pKPresenterDoc->getBackView( getCurrPgNum() - 1 ),
					     m_pKPresenterDoc->getBackType( getCurrPgNum() - 1 ),
					     takeGlobal, getCurrPgNum(), m_pKPresenterDoc );
    setBackCmd->execute();
    m_pKPresenterDoc->commands()->addCommand( setBackCmd );
}

/*================== autoform chosen =============================*/
void KPresenterView::afChooseOk( const QString & c )
{
    QFileInfo fileInfo( c );
    QString fileName = locate( "autoforms", fileInfo.dirPath( false ) + "/" + fileInfo.baseName() + ".atf" );

    page->deSelectAllObj();

    page->deSelectAllObj();
    page->setToolEditMode( INS_AUTOFORM );
    page->setAutoForm( fileName );
}

/*=========== take changes for style dialog =====================*/
void KPresenterView::styleOk()
{
    if ( !m_pKPresenterDoc->setPenBrush( styleDia->getPen(), styleDia->getBrush(), styleDia->getLineBegin(),
					 styleDia->getLineEnd(), styleDia->getFillType(), styleDia->getGColor1(),
					 styleDia->getGColor2(), styleDia->getGType(), styleDia->getGUnbalanced(),
					 styleDia->getGXFactor(), styleDia->getGYFactor() ) ) {
	pen = styleDia->getPen();
	brush = styleDia->getBrush();
	lineBegin = styleDia->getLineBegin();
	lineEnd = styleDia->getLineEnd();
	fillType = styleDia->getFillType();
	gColor1 = styleDia->getGColor1();
	gColor2 = styleDia->getGColor2();
	gType = styleDia->getGType();
	gUnbalanced = styleDia->getGUnbalanced();
	gXFactor = styleDia->getGXFactor();
	gYFactor = styleDia->getGYFactor();
    }
}

/*=========== take changes for option dialog ====================*/
void KPresenterView::optionOk()
{
    if ( optionDia->getRastX() < 1 )
	optionDia->setRastX( 1 );
    if ( optionDia->getRastY() < 1 )
	optionDia->setRastY( 1 );
    kPresenterDoc()->setRasters( optionDia->getRastX(), optionDia->getRastY(), false );

    kPresenterDoc()->setTxtBackCol( optionDia->getBackCol() );

    kPresenterDoc()->replaceObjs();
    kPresenterDoc()->repaint( false );
}

/*=================== page configuration ok ======================*/
void KPresenterView::pgConfOk()
{
    PgConfCmd *pgConfCmd = new PgConfCmd( i18n( "Configure Page for Screenpresentations" ),
					  pgConfDia->getManualSwitch(), pgConfDia->getInfinitLoop(),
					  pgConfDia->getPageEffect(), pgConfDia->getPresSpeed(),
					  pgConfDia->getPresentSlides(), pgConfDia->getSelectedSlides(),
					  kPresenterDoc()->spManualSwitch(), kPresenterDoc()->spInfinitLoop(),
					  kPresenterDoc()->backgroundList()->at( getCurrPgNum() - 1 )->getPageEffect(),
					  kPresenterDoc()->getPresSpeed(),
					  kPresenterDoc()->getPresentSlides(), kPresenterDoc()->getSelectedSlides(),
					  kPresenterDoc(), getCurrPgNum() - 1 );
    pgConfCmd->execute();
    kPresenterDoc()->commands()->addCommand( pgConfCmd );
}

/*=================== effect dialog ok ===========================*/
void KPresenterView::effectOk()
{
}

/*=================== rotate dialog ok ===========================*/
void KPresenterView::rotateOk()
{
    KPObject *kpobject = 0;
    QList<KPObject> _objects;
    QList<RotateCmd::RotateValues> _oldRotate;
    float _newAngle;
    RotateCmd::RotateValues *tmp;

    _objects.setAutoDelete( false );
    _oldRotate.setAutoDelete( false );

    _newAngle = rotateDia->getAngle();

    for ( int i = 0; i < static_cast<int>( kPresenterDoc()->objectList()->count() ); i++ )
    {
	kpobject = kPresenterDoc()->objectList()->at( i );
	if ( kpobject->isSelected() )
	{
	    tmp = new RotateCmd::RotateValues;
	    tmp->angle = kpobject->getAngle();
	    _oldRotate.append( tmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() )
    {
	RotateCmd *rotateCmd = new RotateCmd( i18n( "Change Rotation" ), _oldRotate, _newAngle, _objects, kPresenterDoc() );
	kPresenterDoc()->commands()->addCommand( rotateCmd );
	rotateCmd->execute();
    }
    else
    {
	_oldRotate.setAutoDelete( true );
	_oldRotate.clear();
    }
}

/*=================== shadow dialog ok ==========================*/
void KPresenterView::shadowOk()
{
    KPObject *kpobject = 0;
    QList<KPObject> _objects;
    QList<ShadowCmd::ShadowValues> _oldShadow;
    ShadowCmd::ShadowValues _newShadow, *tmp;

    _objects.setAutoDelete( false );
    _oldShadow.setAutoDelete( false );

    _newShadow.shadowDirection = shadowDia->getShadowDirection();
    _newShadow.shadowDistance = shadowDia->getShadowDistance();
    _newShadow.shadowColor = shadowDia->getShadowColor();

    for ( int i = 0; i < static_cast<int>( kPresenterDoc()->objectList()->count() ); i++ )
    {
	kpobject = kPresenterDoc()->objectList()->at( i );
	if ( kpobject->isSelected() )
	{
	    tmp = new ShadowCmd::ShadowValues;
	    tmp->shadowDirection = kpobject->getShadowDirection();
	    tmp->shadowDistance = kpobject->getShadowDistance();
	    tmp->shadowColor = kpobject->getShadowColor();
	    _oldShadow.append( tmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() )
    {
	ShadowCmd *shadowCmd = new ShadowCmd( i18n( "Change Shadow" ), _oldShadow, _newShadow, _objects, kPresenterDoc() );
	kPresenterDoc()->commands()->addCommand( shadowCmd );
	shadowCmd->execute();
    }
    else
    {
	_oldShadow.setAutoDelete( true );
	_oldShadow.clear();
    }
}

/*================================================================*/
void KPresenterView::psvClosed()
{
    QObject::disconnect( presStructView, SIGNAL( presStructViewClosed() ), this, SLOT( psvClosed() ) );
    presStructView = 0;
}

/*================================================================*/
void KPresenterView::delPageOk( int _page, DelPageMode _delPageMode )
{
    m_pKPresenterDoc->deletePage( _page, _delPageMode );
    setRanges();
}


/*================================================================*/
void KPresenterView::insPageOk( int _page, InsPageMode _insPageMode, InsertPos _insPos )
{
    m_pKPresenterDoc->insertPage( _page, _insPageMode, _insPos );
    setRanges();
}

/*================================================================*/
void KPresenterView::confPieOk()
{
    if ( !m_pKPresenterDoc->setPieSettings( confPieDia->getType(), confPieDia->getAngle(), confPieDia->getLength() ) )
    {
	pieType = confPieDia->getType();
	pieAngle = confPieDia->getAngle();
	pieLength = confPieDia->getLength();
    }
}

/*================================================================*/
void KPresenterView::confRectOk()
{
    if ( !m_pKPresenterDoc->setRectSettings( confRectDia->getRndX(), confRectDia->getRndY() ) )
    {
	rndX = confRectDia->getRndX();
	rndY = confRectDia->getRndY();
    }
}

/*================================================================*/
void KPresenterView::spacingOk( int _lineSpacing, int _distBefore, int _distAfter, int _gap )
{
    if ( page->kTxtObj() ) {
	page->kTxtObj()->setLineSpacing( _lineSpacing );
	page->kTxtObj()->setDistBefore( _distBefore );
	page->kTxtObj()->setDistAfter( _distAfter );
	page->kTxtObj()->setGap( _gap );
    } else if ( page->haveASelectedTextObj() ) {
	KTextObject *obj = page->haveASelectedTextObj();
	obj->setAllLineSpacing( _lineSpacing );
	obj->setAllDistBefore( _distBefore );
	obj->setAllDistAfter( _distAfter );
	obj->setGap( _gap );
	repaint( false );
    }
}

/*================================================================*/
unsigned int KPresenterView::getCurrPgNum()
{
    if ( vert->value() == vert->minValue() )
	return 1;
    else if ( vert->value() == vert->maxValue() )
	return m_pKPresenterDoc->getPageNums();
    else
	return static_cast<int>( vert->value() + page->height() / 2 ) /
	    kPresenterDoc()->getPageSize( 0, 0, 0, 1.0, false ).height() + 1;
}

/*================== scroll horizontal ===========================*/
void KPresenterView::scrollH( int _value )
{
    if ( !presStarted )
    {
	int xo = xOffset;

	xOffset = _value;
	page->scroll( xo - _value, 0 );
	if ( h_ruler )
	    h_ruler->setOffset( xOffset, 0 );
    }
}

/*===================== scroll vertical ==========================*/
void KPresenterView::scrollV( int _value )
{
    if ( !presStarted )
    {
	int yo = yOffset;

	yOffset = _value;
	page->scroll( 0, yo - _value );

	if ( v_ruler )
	    v_ruler->setOffset( 0, -kPresenterDoc()->getPageSize( getCurrPgNum() - 1, xOffset, yOffset, 1.0, false ).y() );
    }
}

/*====================== font changed ===========================*/
void KPresenterView::fontChanged( QFont* font )
{
    if ( font->operator!=( tbFont ) )
    {
	tbFont.setFamily( font->family() );
	tbFont.setBold( font->bold() );
	tbFont.setItalic( font->italic() );
	tbFont.setUnderline( font->underline() );
	tbFont.setPointSize( font->pointSize() );
	m_vToolBarText->setButton( ID_BOLD, tbFont.bold() );
	m_vToolBarText->setButton( ID_ITALIC, tbFont.italic() );
	m_vToolBarText->setButton( ID_UNDERLINE, tbFont.underline() );
	QValueList<QString>::Iterator it = fontList.begin();
	QValueList<QString>::Iterator it2 = fontList.find( tbFont.family().lower() );
	if ( it2 != fontList.end() )
	{
	    int pos = 0;
	    for ( ; it != it2; ++it, ++pos );
	    m_vToolBarText->setCurrentComboItem( ID_FONT_LIST, pos );
	}
	m_vToolBarText->setCurrentComboItem( ID_FONT_SIZE, tbFont.pointSize()-4 );
    }
}

/*====================== color changed ==========================*/
void KPresenterView::colorChanged( QColor* color )
{
    if ( color->operator!=( tbColor ) )
    {
	OpenPartsUI::Pixmap pix;
	pix.data = CORBA::string_dup( colorToPixString( color->rgb() ) );

	tbColor.setRgb( color->rgb() );
	m_vToolBarText->setButtonPixmap( ID_TEXT_COLOR, pix );
    }
}

/*====================== align changed ==========================*/
void KPresenterView::alignChanged( TxtParagraph::HorzAlign align )
{
    if ( align != tbAlign )
    {
	tbAlign = align;
	m_vToolBarText->setButton( ID_ALEFT, false );
	m_vToolBarText->setButton( ID_ARIGHT, false );
	m_vToolBarText->setButton( ID_ACENTER, false );
	m_vMenuText->setItemChecked( m_idMenuText_TAlign_Left, false );
	m_vMenuText->setItemChecked( m_idMenuText_TAlign_Center, false );
	m_vMenuText->setItemChecked( m_idMenuText_TAlign_Right, false );
	switch ( tbAlign )
	{
	case TxtParagraph::LEFT:
	{
	    m_vToolBarText->setButton( ID_ALEFT, true );
	    m_vMenuText->setItemChecked( m_idMenuText_TAlign_Left, true );
	} break;
	case TxtParagraph::CENTER:
	{
	    m_vToolBarText->setButton( ID_ACENTER, true );
	    m_vMenuText->setItemChecked( m_idMenuText_TAlign_Center, true );
	} break;
	case TxtParagraph::RIGHT:
	{
	    m_vToolBarText->setButton( ID_ARIGHT, true );
	    m_vMenuText->setItemChecked( m_idMenuText_TAlign_Right, true );
	} break;
	default: break;
	}
    }
}

/*======================== set pres pen width 1 =================*/
void KPresenterView::presPen1()
{
    QPen p = kPresenterDoc()->presPen();
    p.setWidth( 1 );
    kPresenterDoc()->setPresPen( p );
    rb_pen_width->setItemChecked( W1, true );
    rb_pen_width->setItemChecked( W2, false );
    rb_pen_width->setItemChecked( W3, false );
    rb_pen_width->setItemChecked( W4, false );
    rb_pen_width->setItemChecked( W5, false );
    rb_pen_width->setItemChecked( W6, false );
    rb_pen_width->setItemChecked( W7, false );
    rb_pen_width->setItemChecked( W8, false );
    rb_pen_width->setItemChecked( W9, false );
    rb_pen_width->setItemChecked( W10, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW1, true );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW2, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW3, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW4, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW5, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW6, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW7, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW8, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW9, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW10, false );
}

/*======================== set pres pen width 2 =================*/
void KPresenterView::presPen2()
{
    QPen p = kPresenterDoc()->presPen();
    p.setWidth( 2 );
    kPresenterDoc()->setPresPen( p );
    rb_pen_width->setItemChecked( W1, false );
    rb_pen_width->setItemChecked( W2, true );
    rb_pen_width->setItemChecked( W3, false );
    rb_pen_width->setItemChecked( W4, false );
    rb_pen_width->setItemChecked( W5, false );
    rb_pen_width->setItemChecked( W6, false );
    rb_pen_width->setItemChecked( W7, false );
    rb_pen_width->setItemChecked( W8, false );
    rb_pen_width->setItemChecked( W9, false );
    rb_pen_width->setItemChecked( W10, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW1, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW2, true );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW3, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW4, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW5, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW6, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW7, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW8, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW9, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW10, false );
}

/*======================== set pres pen width 3 =================*/
void KPresenterView::presPen3()
{
    QPen p = kPresenterDoc()->presPen();
    p.setWidth( 3 );
    kPresenterDoc()->setPresPen( p );
    rb_pen_width->setItemChecked( W1, false );
    rb_pen_width->setItemChecked( W2, false );
    rb_pen_width->setItemChecked( W3, true );
    rb_pen_width->setItemChecked( W4, false );
    rb_pen_width->setItemChecked( W5, false );
    rb_pen_width->setItemChecked( W6, false );
    rb_pen_width->setItemChecked( W7, false );
    rb_pen_width->setItemChecked( W8, false );
    rb_pen_width->setItemChecked( W9, false );
    rb_pen_width->setItemChecked( W10, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW1, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW2, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW3, true );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW4, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW5, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW6, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW7, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW8, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW9, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW10, false );
}

/*======================== set pres pen width 4 =================*/
void KPresenterView::presPen4()
{
    QPen p = kPresenterDoc()->presPen();
    p.setWidth( 4 );
    kPresenterDoc()->setPresPen( p );
    rb_pen_width->setItemChecked( W1, false );
    rb_pen_width->setItemChecked( W2, false );
    rb_pen_width->setItemChecked( W3, false );
    rb_pen_width->setItemChecked( W4, true );
    rb_pen_width->setItemChecked( W5, false );
    rb_pen_width->setItemChecked( W6, false );
    rb_pen_width->setItemChecked( W7, false );
    rb_pen_width->setItemChecked( W8, false );
    rb_pen_width->setItemChecked( W9, false );
    rb_pen_width->setItemChecked( W10, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW1, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW2, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW3, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW4, true );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW5, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW6, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW7, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW8, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW9, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW10, false );
}

/*======================== set pres pen width 5 =================*/
void KPresenterView::presPen5()
{
    QPen p = kPresenterDoc()->presPen();
    p.setWidth( 5 );
    kPresenterDoc()->setPresPen( p );
    rb_pen_width->setItemChecked( W1, false );
    rb_pen_width->setItemChecked( W2, false );
    rb_pen_width->setItemChecked( W3, false );
    rb_pen_width->setItemChecked( W4, false );
    rb_pen_width->setItemChecked( W5, true );
    rb_pen_width->setItemChecked( W6, false );
    rb_pen_width->setItemChecked( W7, false );
    rb_pen_width->setItemChecked( W8, false );
    rb_pen_width->setItemChecked( W9, false );
    rb_pen_width->setItemChecked( W10, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW1, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW2, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW3, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW4, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW5, true );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW6, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW7, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW8, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW9, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW10, false );
}

/*======================== set pres pen width 6 =================*/
void KPresenterView::presPen6()
{
    QPen p = kPresenterDoc()->presPen();
    p.setWidth( 6 );
    kPresenterDoc()->setPresPen( p );
    rb_pen_width->setItemChecked( W1, false );
    rb_pen_width->setItemChecked( W2, false );
    rb_pen_width->setItemChecked( W3, false );
    rb_pen_width->setItemChecked( W4, false );
    rb_pen_width->setItemChecked( W5, false );
    rb_pen_width->setItemChecked( W6, true );
    rb_pen_width->setItemChecked( W7, false );
    rb_pen_width->setItemChecked( W8, false );
    rb_pen_width->setItemChecked( W9, false );
    rb_pen_width->setItemChecked( W10, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW1, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW2, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW3, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW4, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW5, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW6, true );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW7, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW8, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW9, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW10, false );
}

/*======================== set pres pen width 7 =================*/
void KPresenterView::presPen7()
{
    QPen p = kPresenterDoc()->presPen();
    p.setWidth( 7 );
    kPresenterDoc()->setPresPen( p );
    rb_pen_width->setItemChecked( W1, false );
    rb_pen_width->setItemChecked( W2, false );
    rb_pen_width->setItemChecked( W3, false );
    rb_pen_width->setItemChecked( W4, false );
    rb_pen_width->setItemChecked( W5, false );
    rb_pen_width->setItemChecked( W6, false );
    rb_pen_width->setItemChecked( W7, true );
    rb_pen_width->setItemChecked( W8, false );
    rb_pen_width->setItemChecked( W9, false );
    rb_pen_width->setItemChecked( W10, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW1, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW2, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW3, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW4, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW5, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW6, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW7, true );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW8, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW9, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW10, false );
}

/*======================== set pres pen width 8 =================*/
void KPresenterView::presPen8()
{
    QPen p = kPresenterDoc()->presPen();
    p.setWidth( 8 );
    kPresenterDoc()->setPresPen( p );
    rb_pen_width->setItemChecked( W1, false );
    rb_pen_width->setItemChecked( W2, false );
    rb_pen_width->setItemChecked( W3, false );
    rb_pen_width->setItemChecked( W4, false );
    rb_pen_width->setItemChecked( W5, false );
    rb_pen_width->setItemChecked( W6, false );
    rb_pen_width->setItemChecked( W7, false );
    rb_pen_width->setItemChecked( W8, true );
    rb_pen_width->setItemChecked( W9, false );
    rb_pen_width->setItemChecked( W10, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW1, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW2, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW3, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW4, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW5, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW6, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW7, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW8, true );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW9, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW10, false );
}

/*======================== set pres pen width 9 =================*/
void KPresenterView::presPen9()
{
    QPen p = kPresenterDoc()->presPen();
    p.setWidth( 9 );
    kPresenterDoc()->setPresPen( p );
    rb_pen_width->setItemChecked( W1, false );
    rb_pen_width->setItemChecked( W2, false );
    rb_pen_width->setItemChecked( W3, false );
    rb_pen_width->setItemChecked( W4, false );
    rb_pen_width->setItemChecked( W5, false );
    rb_pen_width->setItemChecked( W6, false );
    rb_pen_width->setItemChecked( W7, false );
    rb_pen_width->setItemChecked( W8, false );
    rb_pen_width->setItemChecked( W9, true );
    rb_pen_width->setItemChecked( W10, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW1, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW2, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW3, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW4, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW5, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW6, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW7, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW8, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW9, true );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW10, false );
}

/*======================== set pres pen width 10 ================*/
void KPresenterView::presPen10()
{
    QPen p = kPresenterDoc()->presPen();
    p.setWidth( 10 );
    kPresenterDoc()->setPresPen( p );
    rb_pen_width->setItemChecked( W1, false );
    rb_pen_width->setItemChecked( W2, false );
    rb_pen_width->setItemChecked( W3, false );
    rb_pen_width->setItemChecked( W4, false );
    rb_pen_width->setItemChecked( W5, false );
    rb_pen_width->setItemChecked( W6, false );
    rb_pen_width->setItemChecked( W7, false );
    rb_pen_width->setItemChecked( W8, false );
    rb_pen_width->setItemChecked( W9, false );
    rb_pen_width->setItemChecked( W10, true );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW1, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW2, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW3, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW4, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW5, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW6, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW7, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW8, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW9, false );
    m_vMenuScreen->setItemChecked( m_idMenuScreen_PenW10, true );
}

/*======================== set pres pen color ===================*/
void KPresenterView::presPenColor()
{
    QColor c = kPresenterDoc()->presPen().color();

    if ( KColorDialog::getColor( c ) )
    {
	QPen p = kPresenterDoc()->presPen();
	p.setColor( c );
	kPresenterDoc()->setPresPen( p );
	QPixmap pix( 16, 16 );
	pix.fill( c );
	rb_pen->changeItem( pix, i18n( "Pen color..." ), P_COL );
    }
}

/*======================== set pres pen width 1 =================*/
void KPresenterView::presPen1idl()
{
    presPen1();
}

/*======================== set pres pen width 2 =================*/
void KPresenterView::presPen2idl()
{
    presPen2();
}

/*======================== set pres pen width 3 =================*/
void KPresenterView::presPen3idl()
{
    presPen3();
}

/*======================== set pres pen width 4 =================*/
void KPresenterView::presPen4idl()
{
    presPen4();
}

/*======================== set pres pen width 5 =================*/
void KPresenterView::presPen5idl()
{
    presPen5();
}

/*======================== set pres pen width 6 =================*/
void KPresenterView::presPen6idl()
{
    presPen6();
}

/*======================== set pres pen width 7 =================*/
void KPresenterView::presPen7idl()
{
    presPen7();
}

/*======================== set pres pen width 8 =================*/
void KPresenterView::presPen8idl()
{
    presPen8();
}

/*======================== set pres pen width 9 =================*/
void KPresenterView::presPen9idl()
{
    presPen9();
}

/*======================== set pres pen width 10 ================*/
void KPresenterView::presPen10idl()
{
    presPen10();
}

/*======================== set pres pen color ===================*/
void KPresenterView::presPenColoridl()
{
    presPenColor();
}

/*=========================== search =============================*/
void KPresenterView::search( QString text, bool sensitive, bool direction )
{
    if ( page->kTxtObj() )
    {
	TxtCursor from, to;
	from.setKTextObject( page->kTxtObj() );
	to.setKTextObject( page->kTxtObj() );
	bool found = false;

	if ( !direction )
	{
	    if ( searchFirst )
		found = page->kTxtObj()->searchFirst( text, &from, &to, sensitive );
	    else
		found = page->kTxtObj()->searchNext( text, &from, &to, sensitive );

	    if ( found )
		searchFirst = false;
	    else
	    {
		searchFirst = false;
		page->kTxtObj()->setSearchIndexToBegin();
		QMessageBox::warning( this, i18n( "Warning" ),
				      i18n( "The search string '" + text + "' couldn't be found!" ), i18n("OK"));
	    }
	}
	else
	{
	    if ( searchFirst )
		found = page->kTxtObj()->searchFirstRev( text, &from, &to, sensitive );
	    else
		found = page->kTxtObj()->searchNextRev( text, &from, &to, sensitive );

	    if ( found )
		searchFirst = false;
	    else
	    {
		searchFirst = false;
		page->kTxtObj()->setSearchIndexToEnd();
		QMessageBox::warning( this, i18n( "Warning" ),
				      i18n( "The search string '" + text + "' couldn't be found!" ), i18n("OK"));
	    }
	}
    }
}

/*=========================== search and replace =================*/
void KPresenterView::replace( QString search, QString replace, bool sensitive, bool direction )
{
    if ( page->kTxtObj() )
    {
	TxtCursor from, to;
	from.setKTextObject( page->kTxtObj() );
	to.setKTextObject( page->kTxtObj() );
	bool found = false;

	if ( !direction )
	{
	    if ( searchFirst )
		found = page->kTxtObj()->replaceFirst( search, replace, &from, &to, sensitive );
	    else
		found = page->kTxtObj()->replaceNext( search, replace, &from, &to, sensitive );

	    if ( found )
		searchFirst = false;
	    else
	    {
		searchFirst = false;
		page->kTxtObj()->setSearchIndexToBegin();
		QMessageBox::warning( this, i18n( "Warning" ),
				      i18n( "The search string '" + search + "' couldn't be found"
					    " and replaced with '" + replace + "'!" ), i18n("OK"));
	    }
	}
	else
	{
	    if ( searchFirst )
		found = page->kTxtObj()->replaceFirstRev( search, replace, &from, &to, sensitive );
	    else
		found = page->kTxtObj()->replaceNextRev( search, replace, &from, &to, sensitive );

	    if ( found )
		searchFirst = false;
	    else
	    {
		searchFirst = false;
		page->kTxtObj()->setSearchIndexToEnd();
		QMessageBox::warning( this, i18n( "Warning" ),
				      i18n( "The search string '" + search + "' couldn't be found"
					    " and replaced with '" + replace + "'!" ), i18n("OK"));
	    }
	}
    }
}

/*=========================== search and replace all =============*/
void KPresenterView::replaceAll( QString search, QString replace, bool sensitive )
{
    if ( page->kTxtObj() )
    {
	TxtCursor from, to;
	from.setKTextObject( page->kTxtObj() );
	to.setKTextObject( page->kTxtObj() );
	bool found = true;

	page->kTxtObj()->setSearchIndexToBegin();

	while ( found )
	    found = page->kTxtObj()->replaceNext( search, replace, &from, &to, sensitive );
    }
}

/*====================== paint event ============================*/
void KPresenterView::repaint( bool erase )
{
    QWidget::repaint( erase );
    page->repaint( erase );
}

/*====================== paint event ============================*/
void KPresenterView::repaint( unsigned int x, unsigned int y, unsigned int w,
			      unsigned int h, bool erase )
{
    QWidget::repaint( x, y, w, h, erase );
    page->repaint( x, y, w, h, erase );
}

/*====================== paint event ============================*/
void KPresenterView::repaint( QRect r, bool erase )
{
    QWidget::repaint( r, erase );
    page->repaint( r, erase );
}

/*====================== change pciture =========================*/
void KPresenterView::changePicture( unsigned int, const QString & filename )
{
    QFileInfo fileInfo( filename );

    QString file = KFilePreviewDialog::getOpenFileName( QString::null,
							KImageIO::pattern(KImageIO::Reading), 0);

    if ( !file.isEmpty() ) m_pKPresenterDoc->changePicture( file, xOffset, yOffset );
}

/*====================== change clipart =========================*/
void KPresenterView::changeClipart( unsigned int, QString filename )
{
    QFileInfo fileInfo( filename );
    QString file = KFilePreviewDialog::getOpenFileName( fileInfo.dirPath( false ), i18n( "*.WMF *.wmf|Windows Metafiles" ),
							0L );

    if ( !file.isEmpty() ) m_pKPresenterDoc->changeClipart( file, xOffset, yOffset );
}

/*====================== resize event ===========================*/
void KPresenterView::resizeEvent( QResizeEvent *e )
{
    if ( !presStarted ) QWidget::resizeEvent( e );

    if ( ( KoViewIf::hasFocus() || mode() == KOffice::View::RootMode ) && m_bShowGUI )
    {
	horz->show();
	vert->show();
	if ( h_ruler )
	    h_ruler->show();
	if ( v_ruler )
	    v_ruler->show();
	page->resize( widget()->width() - 36, widget()->height() - 36 );
	page->move( 20, 20 );
	vert->setGeometry( widget()->width() - 16, 0, 16, widget()->height() - 16 );
	horz->setGeometry( 0, widget()->height() - 16, widget()->width() - 16, 16 );
	if ( h_ruler )
	    h_ruler->setGeometry( 20, 0, page->width(), 20 );
	if ( v_ruler )
	    v_ruler->setGeometry( 0, 20, 20, page->height() );
	setRanges();
    }
    else
    {
	horz->hide();
	vert->hide();
	h_ruler->hide();
	v_ruler->hide();
	page->move( 0, 0 );
	page->resize( widget()->width(), widget()->height() );
    }
}

/*===============================================================*/
void KPresenterView::dragEnterEvent( QDragEnterEvent *e )
{
    QApplication::sendEvent( page, e );
}

/*===============================================================*/
void KPresenterView::dragMoveEvent( QDragMoveEvent *e )
{
    QApplication::sendEvent( page, e );
}

/*===============================================================*/
void KPresenterView::dragLeaveEvent( QDragLeaveEvent *e )
{
    QApplication::sendEvent( page, e );
}

/*===============================================================*/
void KPresenterView::dropEvent( QDropEvent *e )
{
    QApplication::sendEvent( page, e );
}

/*======================= key press event =======================*/
void KPresenterView::keyPressEvent( QKeyEvent *e )
{
    QApplication::sendEvent( page, e );
}

/*====================== do automatic screenpresentation ========*/
void KPresenterView::doAutomaticScreenPres()
{
    page->repaint( false );

    while ( continuePres && !exitPres )
	screenNext();

    if ( !exitPres && kPresenterDoc()->spInfinitLoop() )
    {
	screenStop();
	screenStart();
    }

    screenStop();
}

/*========================= change undo =========================*/
void KPresenterView::changeUndo( QString _text, bool _enable )
{
    CORBA::WString_var text;
    if ( _enable )
    {
	m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, true );
	QString str;
	str.sprintf( i18n( "Undo: %s" ), _text.data() );
	text = Q2C( str );
	m_vMenuEdit->changeItemText( text, m_idMenuEdit_Undo );
	m_vToolBarEdit->setItemEnabled( ID_UNDO, true );
    }
    else
    {
	text = Q2C( i18n( "No Undo possible" ) );
	m_vMenuEdit->changeItemText( text, m_idMenuEdit_Undo );
	m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, false );
	m_vToolBarEdit->setItemEnabled( ID_UNDO, false );
    }
}

/*========================= change redo =========================*/
void KPresenterView::changeRedo( QString _text, bool _enable )
{
    CORBA::WString_var text;
    if ( _enable )
    {
	m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, true );
	QString str;
	str.sprintf( i18n( "Redo: %s" ), _text.data() );
	text = Q2C( str );
	m_vMenuEdit->changeItemText( text, m_idMenuEdit_Redo );
	m_vToolBarEdit->setItemEnabled( ID_REDO, true );
    }
    else
    {
	text = Q2C( i18n( "No Redo possible" ) );
	m_vMenuEdit->changeItemText( text, m_idMenuEdit_Redo );
	m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, false );
	m_vToolBarEdit->setItemEnabled( ID_REDO, false );
    }
}

/*================ color of pres-pen changed ====================*/
void KPresenterView::presColorChanged()
{
    QPen p = kPresenterDoc()->presPen();
    QColor c = p.color();
    QPixmap pix( 16, 16 );
    pix.fill( c );
    rb_pen->changeItem( pix, i18n( "Pen color..." ), P_COL );
}

/*======================= event handler ============================*/
bool KPresenterView::event( const char* _event, const CORBA::Any& _value )
{
    EVENT_MAPPER( _event, _value );

    MAPPING( OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_ptr, mappingCreateMenubar );
    MAPPING( OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_ptr, mappingCreateToolbar );

    END_EVENT_MAPPER;

    return false;
}

/*======================= setup menu ============================*/
bool KPresenterView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )
{
    if ( CORBA::is_nil( _menubar ) )
    {
	m_vMenuEdit = 0L;
	m_vMenuView = 0L;
	m_vMenuInsert = 0L;
	m_vMenuExtra = 0L;
	m_vMenuText = 0L;
	m_vMenuTools = 0L;
	m_vMenuScreen = 0L;
	m_vMenuHelp = 0L;
	return true;
    }

    CORBA::WString_var text;

    // MENU Edit
    text = Q2C( i18n( "&Edit" ) );
    _menubar->insertMenu( text, m_vMenuEdit, -1, -1 );

    OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap( BarIcon( "undo" ) );
    text = Q2C( i18n( "No Undo possible" ) );
    m_idMenuEdit_Undo = m_vMenuEdit->insertItem6( pix, text, this, "editUndo", CTRL + Key_Z, -1, -1 );
    m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, false );

    pix = OPUIUtils::convertPixmap( BarIcon( "redo" ) );
    text = Q2C( i18n( "No Redo possible" ) );
    m_idMenuEdit_Redo = m_vMenuEdit->insertItem6( pix, text, this, "editRedo", 0, -1, -1 );
    m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, false );
    m_vMenuEdit->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "editcut" ) );
    text = Q2C( i18n( "&Cut" ) );
    m_idMenuEdit_Cut = m_vMenuEdit->insertItem6( pix, text, this, "editCut", CTRL + Key_X, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "editcopy" ) );
    text = Q2C( i18n( "&Copy" ) );
    m_idMenuEdit_Copy = m_vMenuEdit->insertItem6( pix, text, this, "editCopy", CTRL + Key_C, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "editpaste" ) );
    text = Q2C( i18n( "&Paste" ) );
    m_idMenuEdit_Paste = m_vMenuEdit->insertItem6( pix, text, this, "editPaste", CTRL + Key_V, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "delete" ) );
    text = Q2C( i18n( "&Delete" ) );
    m_idMenuEdit_Delete = m_vMenuEdit->insertItem6( pix, text, this, "editDelete", CTRL + Key_Delete, -1, -1 );

    m_vMenuEdit->insertSeparator( -1 );

    text = Q2C( i18n( "&Select all" ) );
    m_idMenuEdit_SelectAll = m_vMenuEdit->insertItem( text, this, "editSelectAll", CTRL + Key_A );

    m_vMenuEdit->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "newslide" ) );
    text = Q2C( i18n( "&Copy Page..." ) );
    m_idMenuEdit_CopyPage = m_vMenuEdit->insertItem6( pix, text, this, "editCopyPage", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "delslide" ) );
    text = Q2C( i18n( "&Delete Page..." ) );
    m_idMenuEdit_DelPage = m_vMenuEdit->insertItem6( pix, text, this, "editDelPage", 0, -1, -1 );

    m_vMenuEdit->insertSeparator( -1 );

    text = Q2C( i18n( "&Find..." ) );
    m_idMenuEdit_Find = m_vMenuEdit->insertItem( text, this, "editFind", CTRL + Key_F );

    text = Q2C( i18n( "&Replace..." ) );
    m_idMenuEdit_FindReplace = m_vMenuEdit->insertItem( text, this, "editFindReplace", CTRL + Key_R );

    m_vMenuEdit->insertSeparator( -1 );

    text = Q2C( i18n( "Edit &Header/Footer..." ) );
    m_idMenuEdit_HeaderFooter = m_vMenuEdit->insertItem( text, this, "editHeaderFooter", 0 );

    // MENU View
    text = Q2C( i18n( "&View" ) );
    _menubar->insertMenu( text, m_vMenuView, -1, -1 );

    text = Q2C( i18n( "&New View" ) );
    m_idMenuView_NewView = m_vMenuView->insertItem( text, this, "newView", 0 );


    // MENU Insert
    text = Q2C( i18n( "&Insert" ) );
    _menubar->insertMenu( text, m_vMenuInsert, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "newslide" ) );
    text = Q2C( i18n( "Pa&ge..." ) );
    m_idMenuInsert_Page = m_vMenuInsert->insertItem6( pix, text, this, "insertPage", 0, -1, -1 );
    m_vMenuInsert->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "picture" ) );
    text = Q2C( i18n( "&Picture..." ) );
    m_idMenuInsert_Picture = m_vMenuInsert->insertItem6( pix, text, this, "insertPicture", Key_F2, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "clipart" ) );
    text = Q2C( i18n( "&Clipart..." ) );
    m_idMenuInsert_Clipart = m_vMenuInsert->insertItem6( pix, text, this, "insertClipart", Key_F3, -1, -1 );

    m_vMenuInsert->setCheckable( true );

    // MENU Tools
    text = Q2C( i18n( "&Tools" ) );
    _menubar->insertMenu( text, m_vMenuTools, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "mouse" ) );
    text = Q2C( i18n( "&Mouse" ) );
    m_idMenuTools_Mouse = m_vMenuTools->insertItem6( pix, text, this, "toolsMouse", Key_F4, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "line" ) );
    text = Q2C( i18n( "&Line" ) );
    m_idMenuTools_Line = m_vMenuTools->insertItem6( pix, text, this, "toolsLine", Key_F5, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "rectangle" ) );
    text = Q2C( i18n( "&Rectangle" ) );
    m_idMenuTools_Rectangle = m_vMenuTools->insertItem6( pix, text, this, "toolsRectangle", Key_F6, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "circle" ) );
    text = Q2C( i18n( "C&ircle or Ellipse" ) );
    m_idMenuTools_Circle = m_vMenuTools->insertItem6( pix, text, this, "toolsCircleOrEllipse", Key_F7, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "pie" ) );
    text = Q2C( i18n( "Pie/&Arc/Chord" ) );
    m_idMenuTools_Pie = m_vMenuTools->insertItem6( pix, text, this, "toolsPie", Key_F8, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "text" ) );
    text = Q2C( i18n( "&Text" ) );
    m_idMenuTools_Text = m_vMenuTools->insertItem6( pix, text, this, "toolsText", Key_F9, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "autoform" ) );
    text = Q2C( i18n( "&Autoform..." ) );
    m_idMenuTools_Autoform = m_vMenuTools->insertItem6( pix, text, this, "toolsAutoform", Key_F10, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "chart" ) );
    text = Q2C( i18n( "&Diagramm" ) );
    m_idMenuTools_Diagramm = m_vMenuTools->insertItem6( pix, text, this, "toolsDiagramm", Key_F11, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "table" ) );
    text = Q2C( i18n( "&Table" ) );
    m_idMenuTools_Table = m_vMenuTools->insertItem6( pix, text, this, "toolsTable", Key_F12, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "formula" ) );
    text = Q2C( i18n( "&Formula" ) );
    m_idMenuTools_Formula = m_vMenuTools->insertItem6( pix, text, this, "toolsFormula", ALT + Key_F1, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "parts" ) );
    text = Q2C( i18n( "&Object..." ) );
    m_idMenuTools_Part = m_vMenuTools->insertItem6( pix, text, this, "toolsObject", ALT + Key_F2, -1, -1 );

    m_vMenuTools->setCheckable( true );

    // MENU Text
    text = Q2C( i18n( "T&ext" ) );
    _menubar->insertMenu( text, m_vMenuText, -1, -1 );

    text = Q2C( i18n( "&Font..." ) );
    m_idMenuText_TFont = m_vMenuText->insertItem( text, this, "mtextFont", 0 );

    text = Q2C( i18n( "Text &Color..." ) );
    m_idMenuText_TColor = m_vMenuText->insertItem( text, this, "textColor", 0 );

    text = Q2C( i18n( "Text &Alignment" ) );
    m_vMenuText->insertItem8( text, m_vMenuText_TAlign, -1, -1 );
    m_vMenuText_TAlign->setCheckable( true );

    pix = OPUIUtils::convertPixmap( BarIcon( "alignLeft" ) );
    text = Q2C( i18n( "Align &Left" ) );
    m_idMenuText_TAlign_Left = m_vMenuText_TAlign->insertItem6( pix, text, this, "mtextAlignLeft", 0, -1, -1 );
    // m_vMenuText_TAlign->setCheckable( m_idMenuText_TAlign_Left, true );

    pix = OPUIUtils::convertPixmap( BarIcon( "alignCenter" ) );
    text = Q2C( i18n( "Align &Center" ) );
    m_idMenuText_TAlign_Center = m_vMenuText_TAlign->insertItem6( pix, text, this, "mtextAlignCenter", 0, -1, -1 );
    // m_vMenuText_TAlign->setCheckable( m_idMenuText_TAlign_Center, true );

    pix = OPUIUtils::convertPixmap( BarIcon( "alignRight" ) );
    text = Q2C( i18n( "Align &Right" ) );
    m_idMenuText_TAlign_Right = m_vMenuText_TAlign->insertItem6( pix, text, this, "mtextAlignRight", 0, -1, -1 );
    // m_vMenuText_TAlign->setCheckable( m_idMenuText_TAlign_Right, true );

    text = Q2C( i18n( "Text &Type" ) );
    m_vMenuText->insertItem8( text, m_vMenuText_TType, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "enumList" ) );
    text = Q2C( i18n( "&Enumerated List" ) );
    m_idMenuText_TType_EnumList = m_vMenuText_TType->insertItem6( pix, text, this, "textEnumList", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "unsortedList" ) );
    text = Q2C( i18n( "&Unsorted List" ) );
    m_idMenuText_TType_UnsortList = m_vMenuText_TType->insertItem6( pix, text, this, "textUnsortList", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "normalText" ) );
    text = Q2C( i18n( "&Normal Text" ) );
    m_idMenuText_TType_NormalText = m_vMenuText_TType->insertItem6( pix, text, this, "textNormalText", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "depth+" ) );
    text = Q2C( i18n( "&Increase Depth" ) );
    m_idMenuText_TDepthPlus = m_vMenuText->insertItem6( pix, text, this, "textDepthPlus", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "depth-" ) );
    text = Q2C( i18n( "&Decrease Depth" ) );
    m_idMenuText_TDepthMinus = m_vMenuText->insertItem6( pix, text, this, "textDepthMinus", 0, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "spacing" ) );
    text = Q2C( i18n( "Paragraph &Spacing..." ) );
    m_idMenuText_TSpacing = m_vMenuText->insertItem6( pix, text, this, "textSpacing", 0, -1, -1 );

    text = Q2C( i18n( "&Extend Contents to Object Height" ) );
    m_idMenuText_TExtentCont2Height = m_vMenuText->insertItem( text, this, "textContentsToHeight", 0 );

    text = Q2C( i18n( "&Resize Object to fit the Contents" ) );
    m_idMenuText_TExtentObj2Cont = m_vMenuText->insertItem( text, this, "textObjToCont", 0 );


    // MENU Extra
    text = Q2C( i18n( "&Extra" ) );
    _menubar->insertMenu( text, m_vMenuExtra, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "style" ) );
    text = Q2C( i18n( "&Pen and Brush..." ) );
    m_idMenuExtra_PenBrush = m_vMenuExtra->insertItem6( pix, text, this, "extraPenBrush", ALT + Key_P, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "edit_pie" ) );
    text = Q2C( i18n( "&Configure Pie/Arc/Chord..." ) );
    m_idMenuExtra_Pie = m_vMenuExtra->insertItem6( pix, text, this, "extraConfigPie", ALT + Key_A, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "rectangle2" ) );
    text = Q2C( i18n( "C&onfigure Rectangle..." ) );
    m_idMenuExtra_Rect = m_vMenuExtra->insertItem6( pix, text, this, "extraConfigRect", ALT + Key_R, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "raise" ) );
    text = Q2C( i18n( "&Raise object( s )" ) );
    m_idMenuExtra_Raise = m_vMenuExtra->insertItem6( pix, text, this, "extraRaise", CTRL + Key_Plus, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "lower" ) );
    text = Q2C( i18n( "&Lower object( s )" ) );
    m_idMenuExtra_Lower = m_vMenuExtra->insertItem6( pix, text, this, "extraLower", CTRL + Key_Minus, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "rotate" ) );
    text = Q2C( i18n( "Rot&ate object( s )..." ) );
    m_idMenuExtra_Rotate = m_vMenuExtra->insertItem6( pix, text, this, "extraRotate", ALT + Key_R, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "shadow" ) );
    text = Q2C( i18n( "&Shadow object( s )..." ) );
    m_idMenuExtra_Shadow = m_vMenuExtra->insertItem6( pix, text, this, "extraShadow", ALT + Key_S, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "alignobjs" ) );
    text = Q2C( i18n( "Text &Alignment" ) );
    m_vMenuExtra->insertItem12( pix, text, m_vMenuExtra_AlignObj, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "aoleft" ) );
    text = Q2C( i18n( "Align &Left" ) );
    m_idMenuExtra_AlignObj_Left = m_vMenuExtra_AlignObj->insertItem6( pix, text, this, "extraAlignObjLeftidl", 0, -1, -1 );

    m_vMenuExtra_AlignObj->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "aocenterh" ) );
    text = Q2C( i18n( "Align Center ( &horizontal )" ) );
    m_idMenuExtra_AlignObj_CenterH = m_vMenuExtra_AlignObj->insertItem6( pix, text, this, "extraAlignObjCenterHidl", 0, -1, -1 );
    m_vMenuExtra_AlignObj->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "aoright" ) );
    text = Q2C( i18n( "Align &Right" ) );
    m_idMenuExtra_AlignObj_Right = m_vMenuExtra_AlignObj->insertItem6( pix, text, this, "extraAlignObjRightidl", 0, -1, -1 );
    m_vMenuExtra_AlignObj->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "aotop" ) );
    text = Q2C( i18n( "Align &Top" ) );
    m_idMenuExtra_AlignObj_Top = m_vMenuExtra_AlignObj->insertItem6( pix, text, this, "extraAlignObjTopidl", 0, -1, -1 );
    m_vMenuExtra_AlignObj->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "aocenterv" ) );
    text = Q2C( i18n( "Align Center ( &vertical )" ) );
    m_idMenuExtra_AlignObj_CenterV = m_vMenuExtra_AlignObj->insertItem6( pix, text, this, "extraAlignObjCenterVidl", 0, -1, -1 );
    m_vMenuExtra_AlignObj->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "aobottom" ) );
    text = Q2C( i18n( "Align &Bottom" ) );
    m_idMenuExtra_AlignObj_Bottom = m_vMenuExtra_AlignObj->insertItem6( pix, text, this, "extraAlignObjBottomidl", 0, -1, -1 );

    m_vMenuExtra->insertSeparator( -1 );

    text = Q2C( i18n( "Page &Background..." ) );
    m_idMenuExtra_Background = m_vMenuExtra->insertItem( text, this, "extraBackground", 0 );

    text = Q2C( i18n( "Pa&ge Layout..." ) );
    m_idMenuExtra_Layout = m_vMenuExtra->insertItem( text, this, "extraLayout", 0 );

    m_vMenuExtra->insertSeparator( -1 );

    text = Q2C( i18n( "&Options..." ) );
    m_idMenuExtra_Options = m_vMenuExtra->insertItem( text, this, "extraOptions", 0 );

    m_vMenuExtra->insertSeparator( -1 );

    text = Q2C( i18n( "Create Web-Presentation ( &HTML Slideshow )..." ) );
    pix = OPUIUtils::convertPixmap( BarIcon( "webpres" ) );
    m_idMenuExtra_WepPres = m_vMenuExtra->insertItem6( pix, text, this, "extraWebPres", 0, -1, -1 );

    // MENU Screenpresentation
    text = Q2C( i18n( "&Screen Presentations" ) );
    _menubar->insertMenu( text, m_vMenuScreen, -1, -1 );

    text = Q2C( i18n( "&Configure pages..." ) );
    m_idMenuScreen_ConfigPage = m_vMenuScreen->insertItem( text, this, "screenConfigPages", 0 );
    text = Q2C( i18n( "&Open presentation structure viewer..." ) );
    m_idMenuScreen_PresStructView = m_vMenuScreen->insertItem( text, this, "screenPresStructView", 0 );

    pix = OPUIUtils::convertPixmap( BarIcon( "effect" ) );
    text = Q2C( i18n( "&Assign effect..." ) );
    m_idMenuScreen_AssignEffect = m_vMenuScreen->insertItem6( pix, text, this, "screenAssignEffect", 0, -1, -1 );
    m_vMenuScreen->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "kpstart" ) );
    text = Q2C( i18n( "&Start" ) );
    m_idMenuScreen_Start = m_vMenuScreen->insertItem6( pix, text, this, "screenStart", CTRL + Key_G, -1, -1 );

    //	     pix = OPUIUtils::convertPixmap( BarIcon( "stop" ) );
    //	     m_idMenuScreen_Stop = m_vMenuScreen->insertItem6( pix,
    //				i18n( "St&op" ), m_idMenuScreen,
    //				this, "screenStop" );
    //	     pix = OPUIUtils::convertPixmap( BarIcon( "pause" ) );
    //	     m_idMenuScreen_Pause = m_vMenuScreen->insertItem6( pix,
    //				 i18n( "Pa&use" ), m_idMenuScreen,
    //				 this, "screenPause" );
    m_vMenuScreen->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "first" ) );
    text = Q2C( i18n( "&Go to start" ) );
    m_idMenuScreen_First = m_vMenuScreen->insertItem6( pix, text, this, "screenFirst", Key_Home, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "kpprev" ) );
    text = Q2C( i18n( "&Previous step" ) );
    m_idMenuScreen_Prev = m_vMenuScreen->insertItem6( pix, text, this, "screenPrev", Key_Prior, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "kpnext" ) );
    text = Q2C( i18n( "&Next step" ) );
    m_idMenuScreen_Next = m_vMenuScreen->insertItem6( pix, text, this, "screenNext", Key_Next, -1, -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "last" ) );
    text = Q2C( i18n( "&Go to end" ) );
    m_idMenuScreen_Last = m_vMenuScreen->insertItem6( pix, text, this, "screenLast", Key_End, -1, -1 );

//   m_idMenuScreen_Skip = m_vMenuScreen->insertItem( i18n( "Goto &page" ), m_idMenuScreen,
//			   this, "screenSkip" );

    m_vMenuScreen->insertSeparator( -1 );

    pix = OPUIUtils::convertPixmap( BarIcon( "pen" ) );
    text = Q2C( i18n( "&Choose Pen" ) );
    m_vMenuScreen->insertItem12( pix, text, m_vMenuScreen_Pen, -1, -1 );
    m_vMenuScreen_Pen->setCheckable( true );

    text = Q2C( i18n( "Pen width" ) );
    m_vMenuScreen_Pen->insertItem8( text , m_vMenuScreen_PenWidth, -1, -1 );
    m_vMenuScreen_PenWidth->setCheckable( true );

    text = Q2C( i18n( "Pen color" ) );
    m_idMenuScreen_PenColor = m_vMenuScreen_Pen->insertItem( text, this, "presPenColoridl", 0 );

    text = Q2C( i18n( "1" ) );
    m_idMenuScreen_PenW1 = m_vMenuScreen_PenWidth->insertItem( text, this, "presPen1idl", 0 );

    text = Q2C( i18n( "2" ) );
    m_idMenuScreen_PenW2 = m_vMenuScreen_PenWidth->insertItem( text, this, "presPen2idl", 0 );

    text = Q2C( i18n( "3" ) );
    m_idMenuScreen_PenW3 = m_vMenuScreen_PenWidth->insertItem( text, this, "presPen3idl", 0 );

    text = Q2C( i18n( "4" ) );
    m_idMenuScreen_PenW4 = m_vMenuScreen_PenWidth->insertItem( text, this, "presPen4idl", 0 );

    text = Q2C( i18n( "5" ) );
    m_idMenuScreen_PenW5 = m_vMenuScreen_PenWidth->insertItem( text, this, "presPen5idl", 0 );

    text = Q2C( i18n( "6" ) );
    m_idMenuScreen_PenW6 = m_vMenuScreen_PenWidth->insertItem( text, this, "presPen6idl", 0 );

    text = Q2C( i18n( "7" ) );
    m_idMenuScreen_PenW7 = m_vMenuScreen_PenWidth->insertItem( text, this, "presPen7idl", 0 );

    text = Q2C( i18n( "8" ) );
    m_idMenuScreen_PenW8 = m_vMenuScreen_PenWidth->insertItem( text, this, "presPen8idl", 0 );

    text = Q2C( i18n( "9" ) );
    m_idMenuScreen_PenW9 = m_vMenuScreen_PenWidth->insertItem( text, this, "presPen9idl", 0 );

    text = Q2C( i18n( "10" ) );
    m_idMenuScreen_PenW10 = m_vMenuScreen_PenWidth->insertItem( text, this, "presPen10idl", 0 );

    // MENU Help
    m_vMenuHelp = _menubar->helpMenu();
    if ( CORBA::is_nil( m_vMenuHelp ) )
    {
	_menubar->insertSeparator( -1 );
	text = Q2C( i18n( "&Help" ) );
	_menubar->setHelpMenu( _menubar->insertMenu( text, m_vMenuHelp, -1, -1 ) );
    }

    text = Q2C( i18n( "&Contents" ) );
    m_idMenuHelp_Contents = m_vMenuHelp->insertItem( text, this, "helpContents", 0 );

    // Torben: Reggie, Check/uncheck all checked menu items to their actual value in this function
    m_vMenuText_TAlign->setItemChecked( m_idMenuText_TAlign_Left, true );
    m_vMenuScreen_PenWidth->setItemChecked( m_idMenuScreen_PenW3, true );

    return true;
}

/*======================== setup popup menus ===================*/
void KPresenterView::setupPopupMenus()
{
    QPixmap pixmap;

    // create right button pen menu
    rb_pen_width = new QPopupMenu();
    CHECK_PTR( rb_pen_width );
    W1 = rb_pen_width->insertItem( "1", this, SLOT( presPen1() ) );
    W2 = rb_pen_width->insertItem( "2", this, SLOT( presPen2() ) );
    W3 = rb_pen_width->insertItem( "3", this, SLOT( presPen3() ) );
    W4 = rb_pen_width->insertItem( "4", this, SLOT( presPen4() ) );
    W5 = rb_pen_width->insertItem( "5", this, SLOT( presPen5() ) );
    W6 = rb_pen_width->insertItem( "6", this, SLOT( presPen6() ) );
    W7 = rb_pen_width->insertItem( "7", this, SLOT( presPen7() ) );
    W8 = rb_pen_width->insertItem( "8", this, SLOT( presPen8() ) );
    W9 = rb_pen_width->insertItem( "9", this, SLOT( presPen9() ) );
    W10 = rb_pen_width->insertItem( "10", this, SLOT( presPen10() ) );
    rb_pen_width->setCheckable( true );
    rb_pen_width->setMouseTracking( true );
    rb_pen_width->setItemChecked( W3, true );

    rb_pen = new QPopupMenu();
    CHECK_PTR( rb_pen );
    rb_pen->insertItem( i18n( "Pen width" ), rb_pen_width );
    QPixmap pix( 16, 16 );
    pix.fill( red );
    P_COL = rb_pen->insertItem( pix, i18n( "Pen color..." ), this, SLOT( presPenColor() ) );
    rb_pen->setMouseTracking( true );
    rb_pen->setCheckable( false );

    // create right button object align menu
    rb_oalign = new QPopupMenu();
    CHECK_PTR( rb_oalign );
    rb_oalign->insertItem( BarIcon("aoleft" ), this, SLOT( extraAlignObjLeft() ) );
    rb_oalign->insertSeparator( -1 );
    rb_oalign->insertItem( BarIcon("aocenterh" ), this, SLOT( extraAlignObjCenterH() ) );
    rb_oalign->insertSeparator( -1 );
    rb_oalign->insertItem( BarIcon("aoright" ), this, SLOT( extraAlignObjRight() ) );
    rb_oalign->insertSeparator( -1 );
    rb_oalign->insertItem(BarIcon("aotop" ) , this, SLOT( extraAlignObjTop() ) );
    rb_oalign->insertSeparator( -1 );
    rb_oalign->insertItem( BarIcon("aocenterv" ), this, SLOT( extraAlignObjCenterV() ) );
    rb_oalign->insertSeparator( -1 );
    rb_oalign->insertItem( BarIcon("aobottom" ), this, SLOT( extraAlignObjBottom() ) );
    rb_oalign->setMouseTracking( true );
    rb_oalign->setCheckable( false );

    // create right button line begin
    rb_lbegin = new QPopupMenu();
    CHECK_PTR( rb_lbegin );
    rb_lbegin->insertItem( BarIcon("line_normal_begin" ), this, SLOT( extraLineBeginNormal() ) );
    rb_lbegin->insertSeparator( -1 );
    rb_lbegin->insertItem(BarIcon("line_arrow_begin" ), this, SLOT( extraLineBeginArrow() ) );
    rb_lbegin->insertSeparator( -1 );
    rb_lbegin->insertItem( BarIcon("line_rect_begin" ), this, SLOT( extraLineBeginRect() ) );
    rb_lbegin->insertSeparator( -1 );
    rb_lbegin->insertItem(BarIcon("line_circle_begin" ), this, SLOT( extraLineBeginCircle() ) );
    rb_lbegin->setMouseTracking( true );
    rb_lbegin->setCheckable( false );

    // create right button line end
    rb_lend = new QPopupMenu();
    CHECK_PTR( rb_lend );
    rb_lend->insertItem(BarIcon("line_normal_end" ), this, SLOT( extraLineEndNormal() ) );
    rb_lend->insertSeparator( -1 );
    rb_lend->insertItem(BarIcon("line_arrow_end" ), this, SLOT( extraLineEndArrow() ) );
    rb_lend->insertSeparator( -1 );
    rb_lend->insertItem(BarIcon("line_rect_end" ), this, SLOT( extraLineEndRect() ) );
    rb_lend->insertSeparator( -1 );
    rb_lend->insertItem(BarIcon("line_circle_end" ), this, SLOT( extraLineEndCircle() ) );
    rb_lend->setMouseTracking( true );
    rb_lend->setCheckable( false );
}

/*======================= setup toolbar ===================*/
bool KPresenterView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
    if ( CORBA::is_nil( _factory ) )
    {
	m_vToolBarEdit = 0L;
	m_vToolBarInsert = 0L;
	m_vToolBarText = 0L;
	m_vToolBarExtra = 0L;
	m_vToolBarScreen = 0L;
	return true;
    }

    cerr << "bool KPresenterView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )" << endl;

    CORBA::WString_var toolTip;

    /****************
     * Edit
     ***************/
    m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarEdit->setFullWidth( false );

    // undo
    OpenPartsUI::Pixmap_var pix = OPUIUtils::convertPixmap( BarIcon( "undo" ) );
    toolTip = Q2C( i18n( "Undo" ) );
    m_idButtonEdit_Undo = m_vToolBarEdit->insertButton2( pix, ID_UNDO, SIGNAL( clicked() ), this, "editUndo", true, toolTip, -1 );
    m_vToolBarEdit->setItemEnabled( ID_UNDO, false );

    // redo
    pix = OPUIUtils::convertPixmap( BarIcon( "redo" ) );
    toolTip = Q2C( i18n( "Redo" ) );
    m_idButtonEdit_Redo = m_vToolBarEdit->insertButton2( pix, ID_REDO, SIGNAL( clicked() ), this, "editRedo", true, toolTip, -1 );
    m_vToolBarEdit->setItemEnabled( ID_REDO, false );

    m_vToolBarEdit->insertSeparator( -1 );

    // cut
    pix = OPUIUtils::convertPixmap( BarIcon( "editcut" ) );
    toolTip = Q2C( i18n( "Cut" ) );
    m_idButtonEdit_Cut = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editCut", true, toolTip, -1 );

    // copy
    pix = OPUIUtils::convertPixmap( BarIcon( "editcopy" ) );
    toolTip = Q2C( i18n( "Copy" ) );
    m_idButtonEdit_Copy = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editCopy", true, toolTip, -1 );

    // paste
    pix = OPUIUtils::convertPixmap( BarIcon( "editpaste" ) );
    toolTip = Q2C( i18n( "Paste" ) );
    m_idButtonEdit_Paste = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editPaste", true, toolTip, -1 );

    m_vToolBarEdit->insertSeparator( -1 );

    // delete
    pix = OPUIUtils::convertPixmap( BarIcon( "delete" ) );
    toolTip = Q2C( i18n( "Delete" ) );
    m_idButtonEdit_Delete = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editDelete", true, toolTip, -1 );

    m_vToolBarEdit->enable( OpenPartsUI::Show );

    // Insert
    m_vToolBarInsert = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarInsert->setFullWidth( false );

    // page
    pix = OPUIUtils::convertPixmap( BarIcon( "newslide" ) );
    toolTip = Q2C( i18n( "Insert Page" ) );
    m_idButtonInsert_Page = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertPage", true, toolTip, -1 );
    m_vToolBarInsert->insertSeparator( -1 );

    // picture
    pix = OPUIUtils::convertPixmap( BarIcon( "picture" ) );
    toolTip = Q2C( i18n( "Insert Picture" ) );
    m_idButtonInsert_Picture = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertPicture", true, toolTip, -1 );

    // clipart
    pix = OPUIUtils::convertPixmap( BarIcon( "clipart" ) );
    toolTip = Q2C( i18n( "Insert Clipart" ) );
    m_idButtonInsert_Clipart = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertClipart", true, toolTip, -1 );

    m_vToolBarInsert->enable( OpenPartsUI::Show );

    /******************
     * Tools
     *****************/
    m_vToolBarTools = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarTools->setFullWidth( false );

    // mouse
    pix = OPUIUtils::convertPixmap( BarIcon( "mouse" ) );
    toolTip = Q2C( i18n( "Mouse Tool" ) );
    m_idButtonTools_Mouse = m_vToolBarTools->insertButton2( pix, ID_TOOL_MOUSE, SIGNAL( clicked() ), this, "toolsMouse",
							    true, toolTip, -1 );

    m_vToolBarTools->setToggle( ID_TOOL_MOUSE, true );
    m_vToolBarTools->setButton( ID_TOOL_MOUSE, true );

    // line
    pix = OPUIUtils::convertPixmap( BarIcon( "line" ) );
    toolTip = Q2C( i18n( "Create Line" ) );
    m_idButtonTools_Line = m_vToolBarTools->insertButton2( pix, ID_TOOL_LINE, SIGNAL( clicked() ), this, "toolsLine",
							   true, toolTip, -1 );

    m_vToolBarTools->setToggle( ID_TOOL_LINE, true );
    m_vToolBarTools->setButton( ID_TOOL_LINE, false );

    // rectangle
    pix = OPUIUtils::convertPixmap( BarIcon( "rectangle" ) );
    toolTip = Q2C( i18n( "Create Rectangle" ) );
    m_idButtonTools_Rectangle = m_vToolBarTools->insertButton2( pix, ID_TOOL_RECT, SIGNAL( clicked() ), this, "toolsRectangle",
								true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_RECT, true );
    m_vToolBarTools->setButton( ID_TOOL_RECT, false );

    // circle or ellipse
    pix = OPUIUtils::convertPixmap( BarIcon( "circle" ) );
    toolTip = Q2C( i18n( "Create Circle or Ellipse" ) );
    m_idButtonTools_Circle = m_vToolBarTools->insertButton2( pix, ID_TOOL_ELLIPSE, SIGNAL( clicked() ), this, "toolsCircleOrEllipse",
							     true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_ELLIPSE, true );
    m_vToolBarTools->setButton( ID_TOOL_ELLIPSE, false );

    // circle or ellipse
    pix = OPUIUtils::convertPixmap( BarIcon( "pie" ) );
    toolTip = Q2C( i18n( "Create Pie/Arc/Chord" ) );
    m_idButtonTools_Pie = m_vToolBarTools->insertButton2( pix, ID_TOOL_PIE, SIGNAL( clicked() ), this, "toolsPie",
							  true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_PIE, true );
    m_vToolBarTools->setButton( ID_TOOL_PIE, false );

    // text
    pix = OPUIUtils::convertPixmap( BarIcon( "text" ) );
    toolTip = Q2C( i18n( "Create Text" ) );
    m_idButtonTools_Text = m_vToolBarTools->insertButton2( pix, ID_TOOL_TEXT, SIGNAL( clicked() ), this, "toolsText",
							   true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_TEXT, true );
    m_vToolBarTools->setButton( ID_TOOL_TEXT, false );

    // autoform
    pix = OPUIUtils::convertPixmap( BarIcon( "autoform" ) );
    toolTip = Q2C( i18n( "Create Autoform" ) );
    m_idButtonTools_Autoform = m_vToolBarTools->insertButton2( pix, ID_TOOL_AUTOFORM, SIGNAL( clicked() ), this, "toolsAutoform",
							       true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_AUTOFORM, true );
    m_vToolBarTools->setButton( ID_TOOL_AUTOFORM, false );

    // diagramm
    pix = OPUIUtils::convertPixmap( BarIcon( "chart" ) );
    toolTip = Q2C( i18n( "Create Diagramm" ) );
    m_idButtonTools_Diagramm = m_vToolBarTools->insertButton2( pix, ID_TOOL_DIAGRAMM, SIGNAL( clicked() ), this, "toolsDiagramm",
							       true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_DIAGRAMM, true );
    m_vToolBarTools->setButton( ID_TOOL_DIAGRAMM, false );

    // table
    pix = OPUIUtils::convertPixmap( BarIcon( "table" ) );
    toolTip = Q2C( i18n( "Create Table" ) );
    m_idButtonTools_Table = m_vToolBarTools->insertButton2( pix, ID_TOOL_TABLE, SIGNAL( clicked() ), this, "toolsTable",
							    true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_TABLE, true );
    m_vToolBarTools->setButton( ID_TOOL_TABLE, false );

    // formula
    pix = OPUIUtils::convertPixmap( BarIcon( "formula" ) );
    toolTip = Q2C( i18n( "Create Formula" ) );
    m_idButtonTools_Formula = m_vToolBarTools->insertButton2( pix, ID_TOOL_FORMULA, SIGNAL( clicked() ), this, "toolsFormula",
							      true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_FORMULA, true );
    m_vToolBarTools->setButton( ID_TOOL_FORMULA, false );

    // parts
    pix = OPUIUtils::convertPixmap( BarIcon( "parts" ) );
    toolTip = Q2C( i18n( "Create Object" ) );
    m_idButtonTools_Part = m_vToolBarTools->insertButton2( pix, ID_TOOL_OBJECT, SIGNAL( clicked() ), this, "toolsObject",
							   true, toolTip, -1 );
    m_vToolBarTools->setToggle( ID_TOOL_OBJECT, true );
    m_vToolBarTools->setButton( ID_TOOL_OBJECT, false );

    m_vToolBarTools->enable( OpenPartsUI::Show );

    /****************
     * Text
     *************/
    m_vToolBarText = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarText->setFullWidth( false );

    // size combobox
    OpenPartsUI::StrList sizelist;
    sizelist.length( 97 );
    for( int i = 4; i <= 100 ; i++ )
    {
	char buffer[ 10 ];
	sprintf( buffer, "%i", i );
	sizelist[ i-4 ] = CORBA::string_dup( buffer );
    }
    toolTip = Q2C( i18n( "Font Size" ) );
    m_idComboText_FontSize = m_vToolBarText->insertCombo( sizelist, ID_FONT_SIZE, true, SIGNAL( activated( const QString & ) ),
							  this, "sizeSelected", true,
							  toolTip, 50, -1, OpenPartsUI::AtBottom );
    m_vToolBarText->setCurrentComboItem( ID_FONT_SIZE, 16 );
    tbFont.setPointSize( 20 );

    // fonts combobox
    getFonts( fontList );
    OpenPartsUI::StrList fonts;
    fonts.length( fontList.count() );
    for( unsigned int i = 0; i < fontList.count(); i++ )
	fonts[ i ] = CORBA::string_dup( fontList[ i ] );
    toolTip = Q2C( i18n( "Font List" ) );
    m_idComboText_FontList = m_vToolBarText->insertCombo( fonts, ID_FONT_LIST, true, SIGNAL( activated( const QString & ) ), this,
							  "fontSelected", true, toolTip,
							  200, -1, OpenPartsUI::AtBottom );
    tbFont.setFamily( fontList[ 0 ] );
    m_vToolBarText->setCurrentComboItem( ID_FONT_LIST, 0 );

    m_vToolBarText->insertSeparator( -1 );

    // bold
    pix = OPUIUtils::convertPixmap( BarIcon( "bold" ) );
    toolTip = Q2C( i18n( "Bold" ) );
    m_idButtonText_Bold = m_vToolBarText->insertButton2( pix, ID_BOLD, SIGNAL( clicked() ), this, "textBold", true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_BOLD, true );
    m_vToolBarText->setButton( ID_BOLD, false );
    tbFont.setBold( false );

    // italic
    pix = OPUIUtils::convertPixmap( BarIcon( "italic" ) );
    toolTip = Q2C( i18n( "Italic" ) );
    m_idButtonText_Italic = m_vToolBarText->insertButton2( pix, ID_ITALIC, SIGNAL( clicked() ), this, "textItalic", true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ITALIC, true );
    m_vToolBarText->setButton( ID_ITALIC, false );
    tbFont.setItalic( false );

    // underline
    pix = OPUIUtils::convertPixmap( BarIcon( "underl" ) );
    toolTip = Q2C( i18n( "Underline" ) );
    m_idButtonText_Underline = m_vToolBarText->insertButton2( pix, ID_UNDERLINE, SIGNAL( clicked() ), this, "textUnderline",
							      true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_UNDERLINE, true );
    m_vToolBarText->setButton( ID_UNDERLINE, false );
    tbFont.setUnderline( false );

    // color
    OpenPartsUI::Pixmap* p = new OpenPartsUI::Pixmap;
    p->data = CORBA::string_dup( colorToPixString( black ) );
    pix = p;
    toolTip = Q2C( i18n( "Color" ) );
    m_idButtonText_Color = m_vToolBarText->insertButton2( pix, ID_TEXT_COLOR, SIGNAL( clicked() ), this, "textColor", true, toolTip, -1 );
    tbColor = black;

    m_vToolBarText->insertSeparator( -1 );

    // align left
    pix = OPUIUtils::convertPixmap( BarIcon( "alignLeft" ) );
    toolTip = Q2C( i18n( "Align Left" ) );
    m_idButtonText_ALeft = m_vToolBarText->insertButton2( pix, ID_ALEFT, SIGNAL( clicked() ), this, "textAlignLeft",
							  true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ALEFT, true );
    m_vToolBarText->setButton( ID_ALEFT, true );
    tbAlign = TxtParagraph::LEFT;

    // align center
    pix = OPUIUtils::convertPixmap( BarIcon( "alignCenter" ) );
    toolTip = Q2C( i18n( "Align Center" ) );
    m_idButtonText_ACenter = m_vToolBarText->insertButton2( pix, ID_ACENTER, SIGNAL( clicked() ), this, "textAlignCenter",
							    true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ACENTER, true );
    m_vToolBarText->setButton( ID_ACENTER, false );

    // align right
    pix = OPUIUtils::convertPixmap( BarIcon( "alignRight" ) );
    toolTip = Q2C( i18n( "Align Right" ) );
    m_idButtonText_ARight = m_vToolBarText->insertButton2( pix, ID_ARIGHT, SIGNAL( clicked() ), this, "textAlignRight",
							   true, toolTip, -1 );
    m_vToolBarText->setToggle( ID_ARIGHT, true );
    m_vToolBarText->setButton( ID_ARIGHT, false );

    m_vToolBarText->insertSeparator( -1 );

    // enum list
    pix = OPUIUtils::convertPixmap( BarIcon( "enumList" ) );
    toolTip = Q2C( i18n( "Enumerated List" ) );
    m_idButtonText_EnumList = m_vToolBarText->insertButton2( pix, 1, SIGNAL( clicked() ), this, "textEnumList",
							     true, toolTip, -1 );

    // unsorted list
    pix = OPUIUtils::convertPixmap( BarIcon( "unsortedList" ) );
    toolTip = Q2C( i18n( "Unsorted List" ) );
    m_idButtonText_UnsortList = m_vToolBarText->insertButton2( pix, 1, SIGNAL( clicked() ), this, "textUnsortList", true, toolTip, -1 );

    // normal text
    pix = OPUIUtils::convertPixmap( BarIcon( "normalText" ) );
    toolTip = Q2C( i18n( "Normal Text" ) );
    m_idButtonText_NormalText = m_vToolBarText->insertButton2( pix, 1, SIGNAL( clicked() ), this, "textNormalText", true, toolTip, -1 );

    m_vToolBarText->enable( OpenPartsUI::Show );

    // depth plus
    pix = OPUIUtils::convertPixmap( BarIcon( "depth+" ) );
    toolTip = Q2C( i18n( "Increase Depth" ) );
    m_idButtonText_DepthPlus = m_vToolBarText->insertButton2( pix, 1, SIGNAL( clicked() ), this, "textDepthPlus", true, toolTip, -1 );

    // depth minus
    pix = OPUIUtils::convertPixmap( BarIcon( "depth-" ) );
    toolTip = Q2C( i18n( "Decrease Depth" ) );
    m_idButtonText_DepthPlus = m_vToolBarText->insertButton2( pix, 1, SIGNAL( clicked() ), this, "textDepthMinus", true, toolTip, -1 );

    // spacing
    pix = OPUIUtils::convertPixmap( BarIcon( "spacing" ) );
    toolTip = Q2C( i18n( "Paragraph Spacing" ) );
    m_idButtonText_Spacing = m_vToolBarText->insertButton2( pix, 1, SIGNAL( clicked() ), this, "textSpacing", true, toolTip, -1 );

    m_vToolBarText->enable( OpenPartsUI::Show );

    /****************
     * Extra
     ************/
    m_vToolBarExtra = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarExtra->setFullWidth( false );

    // pen and brush
    pix = OPUIUtils::convertPixmap( BarIcon( "style" ) );
    toolTip = Q2C( i18n( "Pen & Brush" ) );
    m_idButtonExtra_Style = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraPenBrush", true, toolTip, -1 );
    m_vToolBarExtra->insertSeparator( -1 );

    // pie
    pix = OPUIUtils::convertPixmap( BarIcon( "edit_pie" ) );
    toolTip = Q2C( i18n( "Configure Pie" ) );
    m_idButtonExtra_Pie = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraConfigPie", true, toolTip, -1 );

    // rect
    pix = OPUIUtils::convertPixmap( BarIcon( "rectangle2" ) );
    toolTip = Q2C( i18n( "Configure Rectangle" ) );
    m_idButtonExtra_Rect = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraConfigRect", true,
							   toolTip, -1 );
    m_vToolBarExtra->insertSeparator( -1 );

    // raise
    pix = OPUIUtils::convertPixmap( BarIcon( "raise" ) );
    toolTip = Q2C( i18n( "Raise object" ) );
    m_idButtonExtra_Raise = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraRaise", true, toolTip, -1 );

    // lower
    pix = OPUIUtils::convertPixmap( BarIcon( "lower" ) );
    toolTip = Q2C( i18n( "Lower object" ) );
    m_idButtonExtra_Lower = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraLower", true, toolTip, -1 );
    m_vToolBarExtra->insertSeparator( -1 );

    // rotate
    pix = OPUIUtils::convertPixmap( BarIcon( "rotate" ) );
    toolTip = Q2C( i18n( "Rotate object" ) );
    m_idButtonExtra_Rotate = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraRotate", true, toolTip, -1 );

    // shadow
    pix = OPUIUtils::convertPixmap( BarIcon( "shadow" ) );
    toolTip = Q2C( i18n( "Shadow object" ) );
    m_idButtonExtra_Shadow = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraShadow", true, toolTip, -1 );
    m_vToolBarExtra->insertSeparator( -1 );

    // align
    pix = OPUIUtils::convertPixmap( BarIcon( "alignobjs" ) );
    toolTip = Q2C( i18n( "Align object" ) );
    m_idButtonExtra_Align = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraAlignObj", true, toolTip, -1 );
    m_vToolBarExtra->insertSeparator( -1 );

    // line begin
    pix = OPUIUtils::convertPixmap( BarIcon( "line_begin" ) );
    toolTip = Q2C( i18n( "Line Begin" ) );
    m_idButtonExtra_LineBegin = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraLineBegin",
								true, toolTip, -1 );
    // line end
    pix = OPUIUtils::convertPixmap( BarIcon( "line_end" ) );
    toolTip = Q2C( i18n( "Line End" ) );
    m_idButtonExtra_LineEnd = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraLineEnd",
							      true, toolTip, -1 );

    m_vToolBarExtra->insertSeparator( -1 );

    // web presentation
    pix = OPUIUtils::convertPixmap( BarIcon( "webpres" ) );
    toolTip = Q2C( i18n( "Create HTML Slide show" ) );
    m_idButtonExtra_WebPres = m_vToolBarExtra->insertButton2( pix, ID_TOOL_WEBPRES, SIGNAL( clicked() ), this, "extraWebPres",
							      true, toolTip, -1 );

    m_vToolBarExtra->enable( OpenPartsUI::Show );

    /*************
     * Screen
     **********/
    m_vToolBarScreen = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
    m_vToolBarScreen->setFullWidth( false );

    // start
    pix = OPUIUtils::convertPixmap( BarIcon( "kpstart" ) );
    toolTip = Q2C( i18n( "Start" ) );
    m_idButtonScreen_Start = m_vToolBarScreen->insertButton2( pix, 1, SIGNAL( clicked() ), this, "screenStart", true, toolTip, -1 );
    m_vToolBarScreen->insertSeparator( -1 );

    // first
    pix = OPUIUtils::convertPixmap( BarIcon( "first" ) );
    toolTip = Q2C( i18n( "First" ) );
    m_idButtonScreen_First = m_vToolBarScreen->insertButton2( pix, 1, SIGNAL( clicked() ), this, "screenFirst", true, toolTip, -1 );

    // previous
    pix = OPUIUtils::convertPixmap( BarIcon( "kpprev" ) );
    toolTip = Q2C( i18n( "Previous" ) );
    m_idButtonScreen_Prev = m_vToolBarScreen->insertButton2( pix, 1, SIGNAL( clicked() ), this, "screenPrev", true, toolTip, -1 );

    // next
    pix = OPUIUtils::convertPixmap( BarIcon( "kpnext" ) );
    toolTip = Q2C( i18n( "Next" ) );
    m_idButtonScreen_Next = m_vToolBarScreen->insertButton2( pix, 1, SIGNAL( clicked() ), this, "screenNext", true, toolTip, -1 );

    // last
    pix = OPUIUtils::convertPixmap( BarIcon( "last" ) );
    toolTip = Q2C( i18n( "Last" ) );
    m_idButtonScreen_Last = m_vToolBarScreen->insertButton2( pix, 1, SIGNAL( clicked() ), this, "screenLast", true, toolTip, -1 );
    m_vToolBarScreen->insertSeparator( -1 );

    // effect
    pix = OPUIUtils::convertPixmap( BarIcon( "effect" ) );
    toolTip = Q2C( i18n( "Assign Effect" ) );
    m_idButtonScreen_Effect = m_vToolBarScreen->insertButton2( pix, 1, SIGNAL( clicked() ), this, "screenAssignEffect", true, toolTip, -1 );
    m_vToolBarScreen->insertSeparator( -1 );

    // pen
    pix = OPUIUtils::convertPixmap( BarIcon( "pen" ) );
    toolTip = Q2C( i18n( "choose Pen" ) );
    m_idButtonScreen_Pen = m_vToolBarScreen->insertButton2( pix, 1, SIGNAL( clicked() ), this, "screenPen", true, toolTip, -1 );

    m_vToolBarScreen->enable( OpenPartsUI::Show );

    setTool( TEM_MOUSE );

    /*
     * ColorBar
     */

    m_vColorBar = _factory->createColorBar( OpenPartsUI::ToolBarFactory::Transient );
    m_vColorBar->setFullWidth( false );

    int i = 0;
    const QColor cpalette[] = {white, red, green, blue, cyan, magenta, yellow,
			       darkRed, darkGreen, darkBlue, darkCyan,
			       darkMagenta, darkYellow, white, lightGray,
			       gray, darkGray, black};

    for ( i = 0; i < 18; i++ )
	m_vColorBar->insertRgbColor( i, cpalette[ i ].red(),
				     cpalette[ i ].green(),
				     cpalette[ i ].blue(),
				     i != 0 );

    m_vColorBar->addConnection( SIGNAL( fgColorSelected( int ) ), this,
				"setPenColor" );
    m_vColorBar->addConnection( SIGNAL( bgColorSelected( int ) ), this,
				"setFillColor" );

    m_vColorBar->setBarPos( OpenPartsUI::Right );
    m_vColorBar->enable( OpenPartsUI::Show );

    m_vColorBar->enable( OpenPartsUI::Hide );
    m_vColorBar->setBarPos( OpenPartsUI::Floating );
    m_vColorBar->setBarPos( OpenPartsUI::Right );
    m_vColorBar->enable( OpenPartsUI::Show );

    return true;
}

/*======================= setup scrollbars =====================*/
void KPresenterView::setupScrollbars()
{
    vert = new QScrollBar( QScrollBar::Vertical, this );
    horz = new QScrollBar( QScrollBar::Horizontal, this );
    vert->show();
    horz->show();
    QObject::connect( vert, SIGNAL( valueChanged( int ) ), this, SLOT( scrollV( int ) ) );
    QObject::connect( horz, SIGNAL( valueChanged( int ) ), this, SLOT( scrollH( int ) ) );
    vert->setValue(vert->maxValue());
    horz->setValue(horz->maxValue());
    vert->setValue(vert->minValue());
    horz->setValue(horz->minValue());
    if ( page && !presStarted ) page->resize( widget()->width()-16, widget()->height()-16 );
    vert->setGeometry( widget()->width()-16, 0, 16, widget()->height()-16 );
    horz->setGeometry( 0, widget()->height()-16, widget()->width()-16, 16 );
}

/*==============================================================*/
void KPresenterView::setupRulers()
{
    h_ruler = new KoRuler( this, page, KoRuler::HORIZONTAL, kPresenterDoc()->pageLayout(), 0 );
    v_ruler = new KoRuler( this, page, KoRuler::VERTICAL, kPresenterDoc()->pageLayout(), 0 );
    page->resize( page->width() - 20, page->height() - 20 );
    page->move( 20, 20 );
    h_ruler->setGeometry( 20, 0, page->width(), 20 );
    v_ruler->setGeometry( 0, 20, 20, page->height() );

    QObject::connect( h_ruler, SIGNAL( unitChanged( QString ) ), this, SLOT( unitChanged( QString ) ) );
    QObject::connect( h_ruler, SIGNAL( newPageLayout( KoPageLayout ) ), this, SLOT( newPageLayout( KoPageLayout ) ) );
    QObject::connect( h_ruler, SIGNAL( openPageLayoutDia() ), this, SLOT( openPageLayoutDia() ) );
    QObject::connect( v_ruler, SIGNAL( unitChanged( QString ) ), this, SLOT( unitChanged( QString ) ) );
    QObject::connect( v_ruler, SIGNAL( newPageLayout( KoPageLayout ) ), this, SLOT( newPageLayout( KoPageLayout ) ) );
    QObject::connect( v_ruler, SIGNAL( openPageLayoutDia() ), this, SLOT( openPageLayoutDia() ) );

    switch ( m_pKPresenterDoc->pageLayout().unit )
    {
    case PG_MM:
    {
	h_ruler->setUnit( "mm" );
	v_ruler->setUnit( "mm" );
    } break;
    case PG_PT:
    {
	h_ruler->setUnit( "pt" );
	v_ruler->setUnit( "pt" );
    } break;
    case PG_INCH:
    {
	h_ruler->setUnit( "inch" );
	v_ruler->setUnit( "inch" );
    } break;
    }
}

/*==============================================================*/
void KPresenterView::unitChanged( QString u )
{
    if ( u == "mm" )
	m_pKPresenterDoc->setUnit( PG_MM, u );
    else if ( u == "pt" )
	m_pKPresenterDoc->setUnit( PG_PT, u );
    else if ( u == "inch" )
	m_pKPresenterDoc->setUnit( PG_INCH, u );
}

/*===================== set ranges of scrollbars ===============*/
void KPresenterView::setRanges()
{
    if ( vert && horz && page && m_pKPresenterDoc )
    {
	int range;

	vert->setSteps( 10, m_pKPresenterDoc->getPageSize( 0, xOffset, yOffset, 1.0, false ).height() + 20 );
	range = ( m_pKPresenterDoc->getPageSize( 0, xOffset, yOffset, 1.0, false ).height() ) *
		(int)m_pKPresenterDoc->getPageNums() - (int)page->height() + 16
		< 0 ? 0 :
	    ( m_pKPresenterDoc->getPageSize( 0, xOffset, yOffset, 1.0, false ).height() ) *
		m_pKPresenterDoc->getPageNums() - page->height() + 16;
	vert->setRange( 0, range );
	horz->setSteps( 10, m_pKPresenterDoc->getPageSize( 0, xOffset, yOffset, 1.0, false ).width() + 16 - page->width() );
	range = m_pKPresenterDoc->getPageSize( 0, xOffset, yOffset, 1.0, false ).width() + 16 - page->width() < 0 ? 0 :
	    m_pKPresenterDoc->getPageSize( 0, xOffset, yOffset, 1.0, false ).width() + 16 - page->width();
	horz->setRange( 0, range );
    }
}

/*==============================================================*/
void KPresenterView::skipToPage( int _num )
{
    if ( _num < 0 || _num > static_cast<int>( m_pKPresenterDoc->getPageNums() ) - 1 )
	return;

    vert->setValue( kPresenterDoc()->getPageSize( _num, 0, 0, 1.0, false ).y() );
    repaint( false );
}

/*==============================================================*/
void KPresenterView::makeRectVisible( QRect _rect )
{
    horz->setValue( _rect.x() );
    vert->setValue( _rect.y() );
}

/*==============================================================*/
void KPresenterView::restartPresStructView()
{
    QObject::disconnect( presStructView, SIGNAL( presStructViewClosed() ), this, SLOT( psvClosed() ) );
    presStructView->close();
    delete presStructView;
    presStructView = 0;
    page->deSelectAllObj();

    presStructView = new KPPresStructView( this, "", kPresenterDoc(), this );
    presStructView->setCaption( i18n( "KPresenter - Presentation structure viewer" ) );
    QObject::connect( presStructView, SIGNAL( presStructViewClosed() ), this, SLOT( psvClosed() ) );
    presStructView->show();
}

/*==============================================================*/
void KPresenterView::setTool( ToolEditMode toolEditMode )
{
    if ( !m_vToolBarTools || !m_vMenuTools )
	return;

    m_vToolBarTools->setButton( ID_TOOL_MOUSE, false );
    m_vToolBarTools->setButton( ID_TOOL_LINE, false );
    m_vToolBarTools->setButton( ID_TOOL_RECT, false );
    m_vToolBarTools->setButton( ID_TOOL_ELLIPSE, false );
    m_vToolBarTools->setButton( ID_TOOL_PIE, false );
    m_vToolBarTools->setButton( ID_TOOL_TEXT, false );
    m_vToolBarTools->setButton( ID_TOOL_AUTOFORM, false );
    m_vToolBarTools->setButton( ID_TOOL_TABLE, false );
    m_vToolBarTools->setButton( ID_TOOL_DIAGRAMM, false );
    m_vToolBarTools->setButton( ID_TOOL_FORMULA, false );
    m_vToolBarTools->setButton( ID_TOOL_OBJECT, false );

    m_vMenuTools->setItemChecked( m_idMenuTools_Mouse, false );
    m_vMenuTools->setItemChecked( m_idMenuTools_Line, false );
    m_vMenuTools->setItemChecked( m_idMenuTools_Rectangle, false );
    m_vMenuTools->setItemChecked( m_idMenuTools_Circle, false );
    m_vMenuTools->setItemChecked( m_idMenuTools_Pie, false );
    m_vMenuTools->setItemChecked( m_idMenuTools_Text, false );
    m_vMenuTools->setItemChecked( m_idMenuTools_Autoform, false );
    m_vMenuTools->setItemChecked( m_idMenuTools_Table, false );
    m_vMenuTools->setItemChecked( m_idMenuTools_Diagramm, false );
    m_vMenuTools->setItemChecked( m_idMenuTools_Formula, false );
    m_vMenuTools->setItemChecked( m_idMenuTools_Part, false );

    switch ( toolEditMode )
    {
    case TEM_MOUSE:
    {
	m_vMenuTools->setItemChecked( m_idMenuTools_Mouse, true );
	m_vToolBarTools->setButton( ID_TOOL_MOUSE, true );
    } break;
    case INS_LINE:
    {
	m_vMenuTools->setItemChecked( m_idMenuTools_Line, true );
	m_vToolBarTools->setButton( ID_TOOL_LINE, true );
    } break;
    case INS_RECT:
    {
	m_vMenuTools->setItemChecked( m_idMenuTools_Rectangle, true );
	m_vToolBarTools->setButton( ID_TOOL_RECT, true );
    } break;
    case INS_ELLIPSE:
    {
	m_vMenuTools->setItemChecked( m_idMenuTools_Circle, true );
	m_vToolBarTools->setButton( ID_TOOL_ELLIPSE, true );
    } break;
    case INS_PIE:
    {
	m_vMenuTools->setItemChecked( m_idMenuTools_Pie, true );
	m_vToolBarTools->setButton( ID_TOOL_PIE, true );
    } break;
    case INS_OBJECT:
    {
	m_vMenuTools->setItemChecked( m_idMenuTools_Part, true );
	m_vToolBarTools->setButton( ID_TOOL_OBJECT, true );
    } break;
    case INS_DIAGRAMM:
    {
	m_vMenuTools->setItemChecked( m_idMenuTools_Diagramm, true );
	m_vToolBarTools->setButton( ID_TOOL_DIAGRAMM, true );
    } break;
    case INS_TABLE:
    {
	m_vMenuTools->setItemChecked( m_idMenuTools_Table, true );
	m_vToolBarTools->setButton( ID_TOOL_TABLE, true );
    } break;
    case INS_FORMULA:
    {
	m_vMenuTools->setItemChecked( m_idMenuTools_Formula, true );
	m_vToolBarTools->setButton( ID_TOOL_FORMULA, true );
    } break;
    case INS_TEXT:
    {
	m_vMenuTools->setItemChecked( m_idMenuTools_Text, true );
	m_vToolBarTools->setButton( ID_TOOL_TEXT, true );
    } break;
    case INS_AUTOFORM:
    {
	m_vMenuTools->setItemChecked( m_idMenuTools_Autoform, true );
	m_vToolBarTools->setButton( ID_TOOL_AUTOFORM, true );
    } break;
    }
}

/*============== create a pixmapstring from a color ============*/
QString KPresenterView::colorToPixString( QColor c )
{
    int r, g, b;
    QString pix;
    QString line;

    c.rgb( &r, &g, &b );

    pix = "/* XPM */\n";

    pix += "static char * text_xpm[] = {\n";

    line.sprintf( "%c 20 20 1 1 %c,\n", 34, 34 );
    pix += line.copy();

    line.sprintf( "%c c #%02X%02X%02X %c,\n", 34, r, g, b, 34 );
    pix += line.copy();

    line.sprintf( "%c                    %c,\n", 34, 34 );
    for ( unsigned int i = 1; i <= 20; i++ )
	pix += line.copy();

    line.sprintf( "%c                    %c};\n", 34, 34 );
    pix += line.copy();

    return QString( pix );
}

/*===================== load not KDE installed fonts =============*/
void KPresenterView::getFonts( QStringList &lst )
{
    int numFonts;
    Display *kde_display;
    char** fontNames;
    char** fontNames_copy;
    QString qfontname;

    kde_display = kapp->getDisplay();

    bool have_installed = kapp->getKDEFonts( lst );

    if ( have_installed )
	return;

    fontNames = XListFonts( kde_display, "*", 32767, &numFonts );
    fontNames_copy = fontNames;

    for( int i = 0; i < numFonts; i++ ){

	if ( **fontNames != '-' )
	{
	    fontNames ++;
	    continue;
	};

	qfontname = "";
	qfontname = *fontNames;
	int dash = qfontname.find ( '-', 1, TRUE );

	if ( dash == -1 )
	{
	    fontNames ++;
	    continue;
	}

	int dash_two = qfontname.find ( '-', dash + 1 , TRUE );

	if ( dash == -1 )
	{
	    fontNames ++;
	    continue;
	}


	qfontname = qfontname.mid( dash +1, dash_two - dash -1 );

	if ( !qfontname.contains( "open look", TRUE ) )
	{
	    if( qfontname != "nil" ){
		if( lst.find( qfontname ) == lst.end() )
		    lst.append( qfontname );
	    }
	}

	fontNames ++;

    }

    XFreeFontNames( fontNames_copy );
}

/*================================================================*/
void KPresenterView::setRulerMouseShow( bool _show )
{
    v_ruler->showMousePos( _show );
    h_ruler->showMousePos( _show );
}

/*================================================================*/
void KPresenterView::setRulerMousePos( int mx, int my )
{
    v_ruler->setMousePos( mx, my );
    h_ruler->setMousePos( mx, my );
}
