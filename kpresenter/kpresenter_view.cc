/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

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
#include <qtextstream.h>
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
#include "page.h"
#include "webpresentation.h"
#include "footer_header.h"
#include "kptextobject.h"

#include <klocale.h>
#include <kcolordlg.h>
#include <kfontdialog.h>
#include <kglobal.h>
#include <kimgio.h>

#include <koPartSelectDia.h>
#include <koQueryTypes.h>
#include <koDocument.h>
#include <koPageLayoutDia.h>
#include <koRuler.h>
#include <koUIUtils.h>
#include <koIcons.h>
#include <koTemplateCreateDia.h>
#include <kcoloractions.h>
#include <kaction.h>

#include <stdlib.h>
#include <signal.h>

#include <X11/Xlib.h>
#include <kstddirs.h>

#include "KPresenterViewIface.h"

#define DEBUG

/*****************************************************************/
/* class KPresenterFrame					 */
/*****************************************************************/

/*======================= constructor ===========================*/
/* KPresenterFrame::KPresenterFrame( KPresenterView* _view, KPresenterChild* _child )
    : KoFrame( _view )
{
    m_pKPresenterView = _view;
    m_pKPresenterChild = _child;
    } */

/*****************************************************************/
/* class KPresenterView						 */
/*****************************************************************/

/*======================= constructor ===========================*/
KPresenterView::KPresenterView( KPresenterDoc* _doc, QWidget *_parent, const char *_name )
    : ContainerView( _doc, _parent, _name )
{
    dcop = 0;

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
    allowWebPres = true;

    m_pKPresenterDoc = _doc;
    m_bKPresenterModified = true;

    QObject::connect( m_pKPresenterDoc, SIGNAL( sig_KPresenterModified() ), this, SLOT( slotKPresenterModified() ) );

    createGUI();

    setKeyCompression( true );
}

/*=============================================================*/
DCOPObject* KPresenterView::dcopObject()
{
    if ( !dcop )
	dcop = new KPresenterViewIface( this );

    return dcop;
}

/*======================= destructor ============================*/
KPresenterView::~KPresenterView()
{
    // ######### Reggie: Why that ?
    page->setToolEditMode( TEM_MOUSE );
    delete dcop;
}

/*=========================== file print =======================*/
bool KPresenterView::printDlg()
{
    QPrinter prt;
    prt.setMinMax( 1, m_pKPresenterDoc->getPageNums() );
    bool makeLandscape = false;

    switch ( m_pKPresenterDoc->pageLayout().format ) {
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
    case PG_SCREEN: {
	warning( i18n( "You use the page layout SCREEN. I print it in DIN A4 LANDSCAPE!" ) );
	prt.setPageSize( QPrinter::A4 );
	makeLandscape = true;
    }	break;
    default: {
	warning( i18n( "The used page layout is not supported by QPrinter. I set it to DIN A4." ) );
	prt.setPageSize( QPrinter::A4 );
    } break;
    }

    switch ( m_pKPresenterDoc->pageLayout().orientation ) {
    case PG_PORTRAIT: prt.setOrientation( QPrinter::Portrait );
	break;
    case PG_LANDSCAPE: prt.setOrientation( QPrinter::Landscape );
	break;
    }

    float left_margin = 0.0;
    float top_margin = 0.0;

    if ( makeLandscape ) {
	prt.setOrientation( QPrinter::Landscape );
	left_margin = 28.5;
	top_margin = 15.0;
    }

    if ( prt.setup( this ) ) {
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
    if ( !page->kTxtObj() ) {
	page->setToolEditMode( TEM_MOUSE );
	m_pKPresenterDoc->copyObjs( xOffset, yOffset );
	m_pKPresenterDoc->deleteObjs();
    } else {
	page->kTxtObj()->cutRegion();
    }
}

/*===============================================================*/
void KPresenterView::editCopy()
{
    if ( !page->kTxtObj() ) {
	page->setToolEditMode( TEM_MOUSE );
	m_pKPresenterDoc->copyObjs( xOffset, yOffset );
    } else {
	page->kTxtObj()->copyRegion();
    }
}

/*===============================================================*/
void KPresenterView::editPaste()
{
    if ( !page->kTxtObj() ) {
	page->setToolEditMode( TEM_MOUSE );
	page->deSelectAllObj();
	m_pKPresenterDoc->pasteObjs( xOffset, yOffset );
    } else {
	page->kTxtObj()->paste();
    }
   setRanges();
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
    if ( !page->kTxtObj() ) {
	page->setToolEditMode( TEM_MOUSE );
	page->selectAllObj();
    } else {
	page->kTxtObj()->selectAll();
    }
}

/*===============================================================*/
void KPresenterView::editCopyPage()
{
    m_pKPresenterDoc->copyPage( getCurrPgNum() );
}

/*===============================================================*/
void KPresenterView::editDelPage()
{
    if ( delPageDia ) {
	QObject::disconnect( delPageDia, SIGNAL( deletePage( int, DelPageMode ) ),
			     this, SLOT( delPageOk( int, DelPageMode ) ) );
	delPageDia->close();
	delete delPageDia;
	delPageDia = 0;
    }

    if ( m_pKPresenterDoc->getPageNums() < 2 ) {
	QMessageBox::critical( this, i18n( "KPresenter Error" ),
			       i18n( "Every document has to have at least one page. Because this document \n"
				     "has not more that one page you can't delete this one." ),
			       i18n( "OK" ) );
    } else {
	delPageDia = new DelPageDia( this, "", m_pKPresenterDoc, getCurrPgNum() );
	delPageDia->setCaption( i18n( "KPresenter - Delete Page" ) );
	QObject::connect( delPageDia, SIGNAL( deletePage( int, DelPageMode ) ),
			  this, SLOT( delPageOk( int, DelPageMode ) ) );
	delPageDia->show();
    }
}

/*===============================================================*/
void KPresenterView::editFind()
{
    if ( searchDia ) {
	QObject::disconnect( searchDia, SIGNAL( doSearch( QString, bool, bool ) ),
			     this, SLOT( search( QString, bool, bool ) ) );
	searchDia->close();
	delete searchDia;
	searchDia = 0;
    }

    if ( page->kTxtObj() ) {
	searchDia = new KSearchDialog( this, "SearchDia" );
	searchDia->setMaximumSize( searchDia->width(), searchDia->height() );
	searchDia->setMinimumSize( searchDia->width(), searchDia->height() );
	QObject::connect( searchDia, SIGNAL( doSearch( QString, bool, bool ) ),
			  this, SLOT( search( QString, bool, bool ) ) );
	searchDia->show();

	searchFirst = true;
    }
}

/*===============================================================*/
void KPresenterView::editFindReplace()
{
    if ( replaceDia ) {
	QObject::disconnect( replaceDia, SIGNAL( doSearchReplace( QString, QString, bool, bool ) ),
			     this, SLOT( replace( QString, QString, bool, bool ) ) );
	QObject::disconnect( replaceDia, SIGNAL( doSearchReplaceAll( QString, QString, bool ) ),
			     this, SLOT( replaceAll( QString, QString, bool ) ) );
	replaceDia->close();
	delete replaceDia;
	replaceDia = 0;
    }

    if ( page->kTxtObj() ) {
	replaceDia = new KSearchReplaceDialog( this, "ReplaceDia" );
	replaceDia->setMaximumSize( replaceDia->width(), replaceDia->height() );
	replaceDia->setMinimumSize( replaceDia->width(), replaceDia->height() );
	QObject::connect( replaceDia, SIGNAL( doSearchReplace( QString, QString, bool, bool ) ),
			  this, SLOT( replace( QString, QString, bool, bool ) ) );
	QObject::connect( replaceDia, SIGNAL( doSearchReplaceAll( QString, QString, bool ) ),
			  this, SLOT( replaceAll( QString, QString, bool ) ) );
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
    // ############ TODO Torben
    /* assert( ( m_pKPresenterDoc != 0L ) );

    KPresenterShell *shell = new KPresenterShell;
    shell->show();
    shell->setDocument( m_pKPresenterDoc ); */
}

/*====================== insert a new page ======================*/
void KPresenterView::insertPage()
{
    if ( insPageDia ) {
	QObject::disconnect( insPageDia, SIGNAL( insertPage( int, InsPageMode, InsertPos ) ),
			     this, SLOT( insPageOk( int, InsPageMode, InsertPos ) ) );
	insPageDia->close();
	delete insPageDia;
	insPageDia = 0;
    }

    QString url = m_pKPresenterDoc->url();
    insPageDia = new InsPageDia( this, "", m_pKPresenterDoc, getCurrPgNum() );
    insPageDia->setCaption( i18n( "KPresenter - Insert Page" ) );
    QObject::connect( insPageDia, SIGNAL( insertPage( int, InsPageMode, InsertPos ) ),
		      this, SLOT( insPageOk( int, InsPageMode, InsertPos ) ) );
    insPageDia->show();
    m_pKPresenterDoc->setURL( url );
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

/*==============================================================*/
void KPresenterView::toolsMouse()
{
    if ( !( (KToggleAction*)actionToolsMouse )->isChecked() )
	return;
    page->setToolEditMode( TEM_MOUSE, FALSE );
    //page->deSelectAllObj();
}

/*=========================== insert line =======================*/
void KPresenterView::toolsLine()
{
    if ( !( (KToggleAction*)actionToolsLine )->isChecked() )
	return;
    page->setToolEditMode( INS_LINE, FALSE );
    page->deSelectAllObj();
}

/*===================== insert rectangle ========================*/
void KPresenterView::toolsRectangle()
{
    if ( !( (KToggleAction*)actionToolsRectangle )->isChecked() )
	return;
    page->deSelectAllObj();
    page->setToolEditMode( INS_RECT, FALSE );
}

/*===================== insert circle or ellipse ================*/
void KPresenterView::toolsCircleOrEllipse()
{
    if ( !( (KToggleAction*)actionToolsCircleOrEllipse )->isChecked() )
	return;
    page->deSelectAllObj();
    page->setToolEditMode( INS_ELLIPSE, FALSE );
}

/*==============================================================*/
void KPresenterView::toolsPie()
{
    if ( !( (KToggleAction*)actionToolsPie )->isChecked() )
	return;
    page->deSelectAllObj();
    page->setToolEditMode( INS_PIE, FALSE );
}

/*==============================================================*/
void KPresenterView::toolsDiagramm()
{
    if ( !( (KToggleAction*)actionToolsDiagramm )->isChecked() )
	return;
    page->deSelectAllObj();
    page->setToolEditMode( INS_DIAGRAMM, FALSE );

    // ####### Torben RepoId is no longer used
    QValueList<KoDocumentEntry>
	vec = KoDocumentEntry::query( "'IDL:KChart/DocumentFactory:1.0#KChart' in RepoIds", 1 );
    if ( vec.isEmpty() ) {
	cout << "Got no results" << endl;
	QMessageBox::critical( this, i18n( "Error" ),
			       i18n( "Sorry, no charting component registered" ), i18n( "Ok" ) );
	return;
    }

    cerr << "USING component " << vec[ 0 ].name.latin1() << endl;
    page->setPartEntry( vec[ 0 ] );
}

/*==============================================================*/
void KPresenterView::toolsTable()
{
    if ( !( (KToggleAction*)actionToolsTable )->isChecked() )
	return;
    page->deSelectAllObj();
    page->setToolEditMode( INS_TABLE, FALSE );

    // ####### Torben RepoId is no longer used
    QValueList<KoDocumentEntry>
	vec = KoDocumentEntry::query( "'IDL:KSpread/DocumentFactory:1.0#KSpread' in RepoIds", 1 );
    if ( vec.isEmpty() ) {
	cout << "Got no results" << endl;
	QMessageBox::critical( this, i18n( "Error" ),
			       i18n( "Sorry, no table component registered" ), i18n( "Ok" ) );
	return;
    }

    cerr << "USING component " << vec[ 0 ].name.latin1() << endl;
    page->setPartEntry( vec[ 0 ] );
}

/*==============================================================*/
void KPresenterView::toolsFormula()
{
    if ( !( (KToggleAction*)actionToolsFormula )->isChecked() )
	return;
    page->deSelectAllObj();
    page->setToolEditMode( INS_FORMULA, FALSE );

    // ####### Torben RepoId is no longer used
    QValueList<KoDocumentEntry>
	vec = KoDocumentEntry::query( "'IDL:KFormula/DocumentFactory:1.0#KFormula' in RepoIds", 1 );
    if ( vec.isEmpty() ) {
	cout << "Got no results" << endl;
	QMessageBox::critical( this, i18n( "Error" ),
			       i18n( "Sorry, no formula component registered" ), i18n( "Ok" ) );
	return;
    }

    cerr << "USING component " << vec[ 0 ].name.latin1() << endl;
    page->setPartEntry( vec[ 0 ] );
}

/*===================== insert a textobject =====================*/
void KPresenterView::toolsText()
{
    if ( !( (KToggleAction*)actionToolsText )->isChecked() )
	return;
    page->deSelectAllObj();
    page->setToolEditMode( INS_TEXT, FALSE );
}

/*===============================================================*/
void KPresenterView::toolsAutoform()
{
    if ( !( (KToggleAction*)actionToolsAutoform )->isChecked() )
	return;
    page->deSelectAllObj();
    page->setToolEditMode( TEM_MOUSE, FALSE );
    if ( afChoose ) {
	QObject::disconnect( afChoose, SIGNAL( formChosen( const QString & ) ),
			     this, SLOT( afChooseOk( const QString & ) ) );
	afChoose->close();
	delete afChoose;
	afChoose = 0;
    }
    afChoose = new AFChoose( this, i18n( "Autoform-Choose" ) );
    afChoose->resize( 400, 300 );
    afChoose->setCaption( i18n( "KPresenter - Insert an Autoform" ) );
    QObject::connect( afChoose, SIGNAL( formChosen( const QString & ) ),
		      this, SLOT( afChooseOk( const QString & ) ) );
    afChoose->show();
}

/*===============================================================*/
void KPresenterView::toolsObject()
{
    if ( !( (KToggleAction*)actionToolsObject )->isChecked() )
	return;
    page->deSelectAllObj();
    page->setToolEditMode( TEM_MOUSE, FALSE );

    KoDocumentEntry pe = KoPartSelectDia::selectPart();
    if ( pe.name.isEmpty() )
	return;

    page->setToolEditMode( INS_OBJECT );
    page->setPartEntry( pe );
}

/*===============================================================*/
void KPresenterView::extraPenBrush()
{
    if ( styleDia ) {
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
    if ( confPieDia ) {
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
    if ( confRectDia ) {
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
    if ( rotateDia ) {
	QObject::disconnect( rotateDia, SIGNAL( rotateDiaOk() ), this, SLOT( rotateOk() ) );
	rotateDia->close();
	delete rotateDia;
	rotateDia = 0;
    }

    if ( m_pKPresenterDoc->numSelected() > 0 ) {
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
    if ( shadowDia ) {
	QObject::disconnect( shadowDia, SIGNAL( shadowDiaOk() ), this, SLOT( shadowOk() ) );
	shadowDia->close();
	delete shadowDia;
	shadowDia = 0;
    }

    if ( m_pKPresenterDoc->numSelected() > 0 ) {
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
void KPresenterView::extraAlignObjs()
{
    page->setToolEditMode( TEM_MOUSE );
    QPoint pnt( QCursor::pos() );
    rb_oalign->popup( pnt );
}

/*===============================================================*/
void KPresenterView::extraBackground()
{
    if ( backDia ) {
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
			   m_pKPresenterDoc->getBackPixLastModified( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc->getBackClipFilename( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc->getBackClipLastModified( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc->getBackView( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc->getBackUnbalanced( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc->getBackXFactor( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc->getBackYFactor( getCurrPgNum() - 1 ),
			   m_pKPresenterDoc );
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

    if ( KoPageLayoutDia::pageLayout( pgLayout, hf, FORMAT_AND_BORDERS | USE_NEW_STUFF ) ) {
	PgLayoutCmd *pgLayoutCmd = new PgLayoutCmd( i18n( "Set Pagelayout" ),
						    pgLayout, oldLayout, this );
	pgLayoutCmd->execute();
	kPresenterDoc()->commands()->addCommand( pgLayoutCmd );
    }
}

/*===============================================================*/
void KPresenterView::extraOptions()
{
    if ( optionDia ) {
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
void KPresenterView::extraCreateTemplate()
{
    QPixmap pix( QSize( m_pKPresenterDoc->getPageSize( 0, 0, 0 ).width(), 
			m_pKPresenterDoc->getPageSize( 0, 0, 0 ).height() ) );
    pix.fill( Qt::white );
    int i = getCurrPgNum() - 1;
    page->drawPageInPix2( pix, i * m_pKPresenterDoc->getPageSize( 0, 0, 0 ).height(), i );

    QWMatrix m;
    m.scale( 60.0 / (float)pix.width(), 45.0 / (float)pix.height() );
    pix = pix.xForm( m );
    
    QString file = "/tmp/kpt";
    m_pKPresenterDoc->savePage( file, i );
    
    KoTemplateCreateDia::createTemplate( this, file, pix,
					 KPresenterFactory::global()->
					 dirs()->resourceDirs( "kpresenter_template" ),
					 "kpt" );
    system( QString( "rm %1" ).arg( file ).latin1() );
}

/*===============================================================*/
void KPresenterView::extraWebPres()
{
    if ( !allowWebPres )
	return;

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

/*===============================================================*/
void KPresenterView::extraGroup()
{
    page->setToolEditMode( TEM_MOUSE );
    m_pKPresenterDoc->groupObjects();
}

/*===============================================================*/
void KPresenterView::extraUnGroup()
{
    page->setToolEditMode( TEM_MOUSE );
    m_pKPresenterDoc->ungroupObjects();
}

/*========================== screen config pages ================*/
void KPresenterView::screenConfigPages()
{
    if ( pgConfDia ) {
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
//     if ( !presStructView ) {
	page->deSelectAllObj();
	page->setToolEditMode( TEM_MOUSE );

	presStructView = new KPPresStructView( this, "", kPresenterDoc(), this );
	presStructView->setCaption( i18n( "KPresenter - Presentation structure viewer" ) );
	QObject::connect( presStructView, SIGNAL( presStructViewClosed() ), this, SLOT( psvClosed() ) );
	presStructView->show();
//     }
}

/*===============================================================*/
void KPresenterView::screenAssignEffect()
{
    if ( effectDia ) {
	QObject::disconnect( effectDia, SIGNAL( effectDiaOk() ), this, SLOT( effectOk() ) );
	effectDia->close();
	delete effectDia;
	effectDia = 0;
    }

    page->setToolEditMode( TEM_MOUSE );

    QList<KPObject> objs;
    if ( page->canAssignEffect( objs ) ) {
	effectDia = new EffectDia( this, "Effect", objs, this );
	effectDia->setCaption( i18n( "KPresenter - Assign effects" ) );
	QObject::connect( effectDia, SIGNAL( effectDiaOk() ), this, SLOT( effectOk() ) );
	effectDia->show();
    }
}

/*========================== screen start =======================*/
void KPresenterView::screenStart()
{
    page->setToolEditMode( TEM_MOUSE );

    bool fullScreen = true; //m_rToolBarScreen->isButtonOn( m_idButtonScreen_Full );
    int curPg = getCurrPgNum();

    if ( page && !presStarted ) {
	// disable screensaver
	QString pidFile;
	pidFile = getenv( "HOME" );
	pidFile += "/.kss.pid";
	FILE *fp;
	if ( ( fp = fopen( pidFile, "r" ) ) != NULL ) {
	    fscanf( fp, "%d", &screensaver_pid );
	    fclose( fp );
	    kill( screensaver_pid, SIGSTOP );
	}

	page->deSelectAllObj();
	presStarted = true;
	int dw = QApplication::desktop()->width();
	int dh = QApplication::desktop()->height();
	if ( fullScreen ) {
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	
	    float _presFaktW = static_cast<float>( dw ) /
			       static_cast<float>( kPresenterDoc()->getPageSize( 0, 0, 0, 1.0, false ).width() ) >
			       1.0 ?
			       static_cast<float>( dw ) /
			       static_cast<float>( kPresenterDoc()->getPageSize( 0, 0, 0, 1.0, false ).width() )
			       : 1.0;
	    float _presFaktH = static_cast<float>( dh ) /
			       static_cast<float>( kPresenterDoc()->getPageSize( 0, 0, 0, 1.0, false ).height() ) >
			       1.0 ? static_cast<float>( dh ) /
			       static_cast<float>( kPresenterDoc()->getPageSize( 0, 0, 0, 1.0, false ).height() )
			       : 1.0;
	    float _presFakt = min(_presFaktW,_presFaktH);
	    page->setPresFakt( _presFakt );
	} else {
	    float _presFakt = 1.0;
	    page->setPresFakt( _presFakt );
	}

	_xOffset = xOffset;
	_yOffset = yOffset;
	xOffset = 0;
	yOffset = 0;

	if ( dw > kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).width() )
	    xOffset -= ( dw -
			 kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).width() ) / 2;
	if ( dh > kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() )
	    yOffset -= ( dh -
			 kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() ) / 2;

	vert->setEnabled( false );
	horz->setEnabled( false );
	m_bShowGUI = false;
	page->setBackgroundColor( black );
	oldSize = this->size();
	this->resize( page->size() );
	page->startScreenPresentation( fullScreen, curPg );

	yOffset = ( page->presPage() - 1 ) *
		  kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height();
	if ( dh > kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() )
	    yOffset -= ( dh -
			 kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() ) / 2;

	if ( fullScreen ) {
	    page->recreate( ( QWidget* )0L, WStyle_Customize | WStyle_NoBorder | WType_Popup,
			    QPoint( 0, 0 ), true );
	    page->topLevelWidget()->move( 0, 0 );
	    page->topLevelWidget()->resize( QApplication::desktop()->width(),
					    QApplication::desktop()->height() );
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    page->topLevelWidget()->setBackgroundColor( black );
	    page->setFocusPolicy( QWidget::StrongFocus );
	    page->setFocus();
	} else {
	    page->setBackgroundColor( black );
	    page->setFocusPolicy( QWidget::StrongFocus );
	    page->setFocus();
	}

	if ( !kPresenterDoc()->spManualSwitch() ) {
	    continuePres = true;
	    exitPres = false;
	    doAutomaticScreenPres();
	}
    }
}

/*========================== screen stop ========================*/
void KPresenterView::screenStop()
{
    if ( presStarted ) {
	continuePres = false;
	exitPres = true;
	if ( true ) { //m_rToolBarScreen->isButtonOn( m_idButtonScreen_Full ) )
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
	this->resize( oldSize );
	// #### Torben thinks this line is not needed, since the resize above does it already.
	resizeEvent( 0L );

	// start screensaver again
	QString pidFile;
	pidFile = getenv( "HOME" );
	pidFile += "/.kss.pid";
	FILE *fp;
	if ( ( fp = fopen( pidFile, "r" ) ) != NULL ) {
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
    if ( !presStarted )
	vert->setValue( 0 );
    else
	gotoPresPage( 1 );
}

/*========================== screen pevious =====================*/
void KPresenterView::screenPrev()
{
    if ( presStarted ) {
	if ( page->pPrev( true ) ) {
	    yOffset = ( page->presPage() - 1 ) *
		      kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height();
	    if ( page->height() > kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() )
		yOffset -= ( page->height() -
			     kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() ) / 2;
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    page->repaint( false );
	    page->setFocus();
	} else {
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    page->setFocus();
	}
    } else
	vert->setValue( yOffset - kPresenterDoc()->getPageSize( 0, 0, 0, 1.0, false ).height() );
}

/*========================== screen next ========================*/
void KPresenterView::screenNext()
{
    if ( presStarted ) {
	if ( page->pNext( true ) ) {
	    yOffset = ( page->presPage() - 1 ) *
		      kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height();
	    if ( page->height() > kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() )
		yOffset -= ( page->height() -
			     kPresenterDoc()->getPageSize( 0, 0, 0, page->presFakt(), false ).height() ) / 2;
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    page->setFocus();
	} else {
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    page->setFocus();
	}
    } else
	vert->setValue( yOffset + kPresenterDoc()->getPageSize( 0, 0, 0, 1.0, false ).height() );
}

/*========================== screen last ========================*/
void KPresenterView::screenLast()
{
    if ( !presStarted )
	vert->setValue(vert->maxValue());
    else
	gotoPresPage( getNumPresPages() );
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

/*===============================================================*/
void KPresenterView::sizeSelected()
{
    tbFont.setPointSize( ( (QFontSizeAction*)actionTextFontSize )->fontSize() );
    page->setTextFont( &tbFont );
}

/*===============================================================*/
void KPresenterView::fontSelected()
{
    tbFont.setFamily( ( (QFontAction*)actionTextFontFamily )->currentText() );
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
    if ( KColorDialog::getColor( tbColor ) ) {
	page->setTextColor( &tbColor );
	( (KColorAction*)actionTextColor )->blockSignals( TRUE );
	( (KColorAction*)actionTextColor )->setColor( tbColor );
	( (KColorAction*)actionTextColor )->blockSignals( FALSE );
    }
}

/*===============================================================*/
void KPresenterView::textAlignLeft()
{
    if ( !( (KToggleAction*)actionTextAlignLeft )->isChecked() )
	return;
    tbAlign = TxtParagraph::LEFT;
    page->setTextAlign( tbAlign );
}

/*===============================================================*/
void KPresenterView::textAlignCenter()
{
    if ( !( (KToggleAction*)actionTextAlignCenter )->isChecked() )
	return;
    tbAlign = TxtParagraph::CENTER;
    page->setTextAlign( TxtParagraph::CENTER );
}

/*===============================================================*/
void KPresenterView::textAlignRight()
{
    if ( !( (KToggleAction*)actionTextAlignRight )->isChecked() )
	return;
    tbAlign = TxtParagraph::RIGHT;
    page->setTextAlign( TxtParagraph::RIGHT );
}

/*===============================================================*/
void KPresenterView::mtextFont()
{
    QFont tmpFont = tbFont;

    if ( KFontDialog::getFont( tmpFont ) ) {
	fontChanged( &tmpFont );
	tbFont = tmpFont;
	page->setTextFont( &tbFont );
 	( (KFontAction*)actionTextFontFamily )->blockSignals( TRUE );
 	( (KFontAction*)actionTextFontFamily )->setFont( tbFont.family() );
 	( (KFontAction*)actionTextFontFamily )->blockSignals( FALSE );
 	( (KFontSizeAction*)actionTextFontSize )->blockSignals( TRUE );
 	( (KFontSizeAction*)actionTextFontSize )->setFontSize( tbFont.pointSize() );
 	( (KFontSizeAction*)actionTextFontSize )->blockSignals( FALSE );
    }
}

/*===============================================================*/
void KPresenterView::textEnumList()
{
    KTextObject *txtObj = page->kTxtObj();
    if ( !txtObj )
	txtObj = page->haveASelectedTextObj();
    if ( txtObj ) {
	int _type = txtObj->enumListType().type;
	QFont _font = txtObj->enumListType().font;
	QColor _color = txtObj->enumListType().color;
	QString _before = txtObj->enumListType().before;
	QString _after = txtObj->enumListType().after;
	int _start = txtObj->enumListType().start;

	if ( KEnumListDia::enumListDia( _type, _font, _color, _before, _after, _start, fontList ) ) {
	    KTextObject::EnumListType elt;
	    elt.type = _type;
	    elt.font = _font;
	    elt.color = _color;
	    elt.before = _before;
	    elt.after = _after;
	    elt.start = _start;
	    txtObj->setEnumListType( elt );
	}

	txtObj->setObjType( KTextObject::ENUM_LIST );

	if ( !page->kTxtObj() )
	    page->repaint( FALSE );
    }
}

/*===============================================================*/
void KPresenterView::textUnsortList()
{
    KTextObject *txtObj = page->kTxtObj();
    if ( !txtObj )
	txtObj = page->haveASelectedTextObj();
    if ( txtObj ) {
	QList<QFont> *_font = txtObj->unsortListType().font;
	QList<QColor> *_color = txtObj->unsortListType().color;
	QList<QChar> *_c = txtObj->unsortListType().chr;

	if ( KCharSelectDia::selectChar( _font, _color, _c ) ) {
	    KTextObject::UnsortListType ult;
	    ult.font = _font;
	    ult.color = _color;
	    ult.chr = _c;
	    txtObj->setUnsortListType( ult );
	}

	txtObj->setObjType( KTextObject::UNSORT_LIST );

	if ( !page->kTxtObj() )
	    page->repaint( FALSE );
    }
}

/*===============================================================*/
void KPresenterView::textNormalText()
{
    KTextObject *txtObj = page->kTxtObj();
    if ( !txtObj )
	txtObj = page->haveASelectedTextObj();
    if ( txtObj ) {
	txtObj->setObjType( KTextObject::PLAIN );
	if ( !page->kTxtObj() )
	    page->repaint( FALSE );
    }
}

/*===============================================================*/
void KPresenterView::textDepthPlus()
{
    KTextObject *txtObj = page->kTxtObj();
    if ( !txtObj )
	txtObj = page->haveASelectedTextObj();
    if ( txtObj ) {
	txtObj->incDepth();
	if ( !page->kTxtObj() )
	    page->repaint( FALSE );
    }
}

/*===============================================================*/
void KPresenterView::textDepthMinus()
{
    KTextObject *txtObj = page->kTxtObj();
    if ( !txtObj )
	txtObj = page->haveASelectedTextObj();
    if ( txtObj ) {
	txtObj->decDepth();
	if ( !page->kTxtObj() )
	    page->repaint( FALSE );
	
    }
}

/*===============================================================*/
void KPresenterView::textSpacing()
{
    KTextObject *obj = 0L;

    if ( page->kTxtObj() ) obj = page->kTxtObj();
    else if ( page->haveASelectedTextObj() ) obj = page->haveASelectedTextObj();

    if ( obj ) {
	if ( spacingDia ) {
	    QObject::disconnect( spacingDia, SIGNAL( spacingDiaOk() ), this, SLOT( spacingOk() ) );
	    spacingDia->close();
	    delete spacingDia;
	    spacingDia = 0;
	}

	spacingDia = new SpacingDia( this, obj->getLineSpacing(), obj->getDistBefore(),
				     obj->getDistAfter(), obj->getGap() );
	spacingDia->setMaximumSize( spacingDia->width(), spacingDia->height() );
	spacingDia->setMinimumSize( spacingDia->width(), spacingDia->height() );
	spacingDia->setCaption( i18n( "KPresenter - Spacings" ) );
	QObject::connect( spacingDia, SIGNAL( spacingDiaOk( int, int, int, int ) ),
			  this, SLOT( spacingOk( int, int, int, int ) ) );
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
void KPresenterView::penChosen( const QColor &c )
{
    bool fill = TRUE;

    if ( !m_pKPresenterDoc->setPenColor( c, fill ) ) {
	if ( fill )
	    pen.setColor( c );
	else
	    pen = NoPen;
    }
}

/*===============================================================*/
void KPresenterView::brushChosen( const QColor &c )
{
    bool fill = TRUE;

    if ( !m_pKPresenterDoc->setBrushColor( c, fill ) ) {
	if ( fill )
	    brush.setColor( c );
	else
	    brush = NoBrush;
    }
}

/*======================= align object left =====================*/
void KPresenterView::extraAlignObjLeft()
{
    kPresenterDoc()->alignObjsLeft();
}

/*======================= align object center h =================*/
void KPresenterView::extraAlignObjCenterH()
{
    kPresenterDoc()->alignObjsCenterH();
}

/*======================= align object right ====================*/
void KPresenterView::extraAlignObjRight()
{
    kPresenterDoc()->alignObjsRight();
}

/*======================= align object top ======================*/
void KPresenterView::extraAlignObjTop()
{
    kPresenterDoc()->alignObjsTop();
}

/*======================= align object center v =================*/
void KPresenterView::extraAlignObjCenterV()
{
    kPresenterDoc()->alignObjsCenterV();
}

/*======================= align object bottom ===================*/
void KPresenterView::extraAlignObjBottom()
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
    // setup page
    page = new Page( this, "Page", ( KPresenterView* )this );
    QObject::connect( page, SIGNAL( fontChanged( QFont* ) ),
		      this, SLOT( fontChanged( QFont* ) ) );
    QObject::connect( page, SIGNAL( colorChanged( QColor* ) ),
		      this, SLOT( colorChanged( QColor* ) ) );
    QObject::connect( page, SIGNAL( alignChanged( TxtParagraph::HorzAlign ) ),
		      this, SLOT( alignChanged( TxtParagraph::HorzAlign ) ) );

    // setup GUI
    setupActions();
    setupPopupMenus();
    setupScrollbars();
    setRanges();
    setupRulers();

    if ( m_pKPresenterDoc && page )
	QObject::connect( page, SIGNAL( stopPres() ), this, SLOT( stopPres() ) );


    resizeEvent( 0L );
}

/*=============================================================*/
void KPresenterView::initGui()
{
    tbColor = Qt::black;
    ( (KColorAction*)actionTextColor )->setColor( Qt::black );
    ( (KColorAction*)actionScreenPenColor )->setColor( Qt::red );
    ( (KSelectAction*)actionScreenPenWidth )->setCurrentItem( 2 );
    actionEditUndo->setEnabled( FALSE );
    actionEditRedo->setEnabled( FALSE );
}

/*====================== construct ==============================*/
void KPresenterView::setupActions()
{
    // -------------- Edit actions

    actionEditUndo = new KAction( i18n( "No Undo possible" ), KPBarIcon( "undo" ), ALT + Key_Z,
				  this, SLOT( editUndo() ),
				  actionCollection(), "edit_undo" );
    actionEditRedo = new KAction( i18n( "No Redo possible" ), KPBarIcon( "redo" ), 0,
				  this, SLOT( editRedo() ),
				  actionCollection(), "edit_redo" );
    actionEditCut = new KAction( i18n( "&Cut" ), KPBarIcon( "editcut" ), CTRL + Key_X,
				 this, SLOT( editCut() ),
				 actionCollection(), "edit_cut" );
    actionEditCopy = new KAction( i18n( "C&opy" ), KPBarIcon( "editcopy" ), CTRL + Key_C,
				  this, SLOT( editCopy() ),
				  actionCollection(), "edit_copy" );
    actionEditPaste = new KAction( i18n( "&Paste" ), KPBarIcon( "editpaste" ), CTRL + Key_V,
				   this, SLOT( editPaste() ),
				   actionCollection(), "edit_paste" );
    actionEditDelete = new KAction( i18n( "&Delete" ), KPBarIcon( "delete" ), CTRL + Key_Delete,
				    this, SLOT( editDelete() ),
				    actionCollection(), "edit_delete" );
    actionEditSelectAll = new KAction( i18n( "&Select All" ), CTRL + Key_A,
				       this, SLOT( editSelectAll() ),
				       actionCollection(), "edit_selectall" );
    actionEditCopyPage = new KAction( i18n( "Copy &Page to Clipboard" ), KPBarIcon( "newslide" ), 0,
				      this, SLOT( editCopyPage() ),
				      actionCollection(), "edit_copypage" );
    actionEditDelPage = new KAction( i18n( "Delete &Page..." ), KPBarIcon( "delslide"), 0,
				     this, SLOT( editDelPage() ),
				     actionCollection(), "edit_delpage" );
    actionEditFind = new KAction( i18n( "&Find..." ), CTRL + Key_S,
				  this, SLOT( editFind() ),
				  actionCollection(), "edit_find" );
    actionEditFindReplace = new KAction( i18n( "&Replace..." ), CTRL + Key_R,
					 this, SLOT( editFindReplace() ),
					 actionCollection(), "edit_findreplace" );
    actionEditHeaderFooter = new KAction( i18n( "&Header/Footer..." ), 0,
					  this, SLOT( editHeaderFooter() ),
					  actionCollection(), "edit_headerfooter" );

    // ---------------- view actions


    actionViewNewView = new KAction( i18n( "New View..." ), 0,
				     this, SLOT( newView() ),
				     actionCollection(), "view_newview" );

    // ---------------- insert actions

    actionInsertPage = new KAction( i18n( "&Page..." ), KPBarIcon( "newslide"), Key_F2,
				    this, SLOT( insertPage() ),
				    actionCollection(), "insert_page" );

    actionInsertPicture = new KAction( i18n( "P&icture..." ), KPBarIcon( "picture"), Key_F3,
				       this, SLOT( insertPicture() ),
				       actionCollection(), "insert_picture" );

    actionInsertClipart = new KAction( i18n( "&Clipart..." ), KPBarIcon( "clipart"), Key_F4,
				       this, SLOT( insertClipart() ),
				       actionCollection(), "insert_clipart" );

    // ----------------- tools actions

    actionToolsMouse = new KToggleAction( i18n( "&Mouse" ), KPBarIcon( "mouse" ), Key_F5,
					  this, SLOT( toolsMouse() ),
					  actionCollection(), "tools_mouse" );
    ( (KToggleAction*)actionToolsMouse )->setExclusiveGroup( "tools" );
    ( (KToggleAction*)actionToolsMouse )->setChecked( TRUE );

    actionToolsLine = new KToggleAction( i18n( "&Line" ), KPBarIcon( "line" ), Key_F6,
					 this, SLOT( toolsLine() ),
					 actionCollection(), "tools_line" );
    ( (KToggleAction*)actionToolsLine )->setExclusiveGroup( "tools" );

    actionToolsRectangle = new KToggleAction( i18n( "&Rectangle" ), KPBarIcon( "rectangle" ), Key_F7,
					      this, SLOT( toolsRectangle() ),
					      actionCollection(), "tools_rectangle" );
    ( (KToggleAction*)actionToolsRectangle )->setExclusiveGroup( "tools" );

    actionToolsCircleOrEllipse = new KToggleAction( i18n( "&Circle/Ellipse" ), KPBarIcon( "circle" ), Key_F8,
						    this, SLOT( toolsCircleOrEllipse() ),
						    actionCollection(), "tools_circle" );
    ( (KToggleAction*)actionToolsCircleOrEllipse )->setExclusiveGroup( "tools" );

    actionToolsPie = new KToggleAction( i18n( "&Pie/Arc/Chord" ), KPBarIcon( "pie" ), Key_F9,
					this, SLOT( toolsPie() ),
					actionCollection(), "tools_pie" );
    ( (KToggleAction*)actionToolsPie )->setExclusiveGroup( "tools" );

    actionToolsText = new KToggleAction( i18n( "&Text" ), KPBarIcon( "text" ), Key_F10,
					 this, SLOT( toolsText() ),
					 actionCollection(), "tools_text" );
    ( (KToggleAction*)actionToolsText )->setExclusiveGroup( "tools" );

    actionToolsAutoform = new KToggleAction( i18n( "&Autoform" ), KPBarIcon( "autoform" ), Key_F11,
					     this, SLOT( toolsAutoform() ),
					     actionCollection(), "tools_autoform" );
    ( (KToggleAction*)actionToolsAutoform )->setExclusiveGroup( "tools" );

    actionToolsDiagramm = new KToggleAction( i18n( "&Diagramm" ), KPBarIcon( "chart" ), Key_F12,
					   this, SLOT( toolsDiagramm() ),
					   actionCollection(), "tools_diagramm" );
    ( (KToggleAction*)actionToolsDiagramm )->setExclusiveGroup( "tools" );

    actionToolsTable = new KToggleAction( i18n( "T&able"), KPBarIcon( "table" ), CTRL + Key_F1,
					   this, SLOT( toolsTable() ),
					   actionCollection(), "tools_table" );
    ( (KToggleAction*)actionToolsTable )->setExclusiveGroup( "tools" );

    actionToolsObject = new KToggleAction( i18n( "&Object..." ), KPBarIcon( "parts" ), CTRL + Key_F2,
					   this, SLOT( toolsObject() ),
					   actionCollection(), "tools_object" );
    ( (KToggleAction*)actionToolsObject )->setExclusiveGroup( "tools" );

    // ----------------- text actions

    actionTextFont = new KAction( i18n( "&Font..." ), 0,
				   this, SLOT( mtextFont() ),
				   actionCollection(), "text_font" );

    actionTextFontSize = new KFontSizeAction( i18n( "Font Size" ), 0, this,
					      SLOT( sizeSelected() ),
					      actionCollection(), "text_fontsize" );

    actionTextFontFamily = new KFontAction( i18n( "Font Family" ), 0, this,
					    SLOT( fontSelected() ),
					      actionCollection(), "text_fontfamily" );

    actionTextBold = new KToggleAction( i18n( "&Bold" ), KPBarIcon( "bold" ), CTRL + Key_B,
					   this, SLOT( textBold() ),
					   actionCollection(), "text_bold" );

    actionTextItalic = new KToggleAction( i18n( "&Italic" ), KPBarIcon( "italic" ), CTRL + Key_I,
					   this, SLOT( textItalic() ),
					   actionCollection(), "text_italic" );

    actionTextUnderline = new KToggleAction( i18n( "&Underline" ), KPBarIcon( "underl" ), CTRL + Key_U,
					   this, SLOT( textUnderline() ),
					   actionCollection(), "text_underline" );

    actionTextColor = new KColorAction( i18n( "&Color..." ), KColorAction::TextColor, 0,
					this, SLOT( textColor() ),
					actionCollection(), "text_color" );

    actionTextAlignLeft = new KToggleAction( i18n( "Align &Left" ), KPBarIcon( "alignLeft" ), ALT + Key_L,
				       this, SLOT( textAlignLeft() ),
				       actionCollection(), "text_alignleft" );
    ( (KToggleAction*)actionTextAlignLeft )->setExclusiveGroup( "align" );
    ( (KToggleAction*)actionTextAlignLeft )->setChecked( TRUE );

    actionTextAlignCenter = new KToggleAction( i18n( "Align &Center" ), KPBarIcon( "alignCenter" ), ALT + Key_C,
					 this, SLOT( textAlignCenter() ),
					 actionCollection(), "text_aligncenter" );
    ( (KToggleAction*)actionTextAlignCenter )->setExclusiveGroup( "align" );

    actionTextAlignRight = new KToggleAction( i18n( "Align &Right" ), KPBarIcon( "alignRight" ), ALT + Key_R,
					this, SLOT( textAlignRight() ),
					actionCollection(), "text_alignright" );
    ( (KToggleAction*)actionTextAlignRight )->setExclusiveGroup( "align" );

    actionTextTypeEnumList = new KAction( i18n( "&Enumerated List" ), KPBarIcon( "enumList" ), 0,
					  this, SLOT( textEnumList() ),
					  actionCollection(), "text_enumList" );

    actionTextTypeUnsortList = new KAction( i18n( "&Unsorted List" ), KPBarIcon( "unsortedList" ), 0,
					    this, SLOT( textUnsortList() ),
					    actionCollection(), "text_unsortedList" );

    actionTextTypeNormalText = new KAction( i18n( "&Normal Text" ), KPBarIcon( "normalText" ), 0,
					    this, SLOT( textNormalText() ),
					    actionCollection(), "text_normalText" );

    actionTextDepthPlus = new KAction( i18n( "&Increase Depth" ), KPBarIcon( "depth+" ), CTRL + Key_Plus,
				       this, SLOT( textDepthPlus() ),
				       actionCollection(), "text_depthPlus" );

    actionTextDepthMinus = new KAction( i18n( "&Decrease Depth" ), KPBarIcon( "depth-" ), CTRL + Key_Minus,
					this, SLOT( textDepthMinus() ),
					actionCollection(), "text_depthMinus" );

    actionTextSpacing = new KAction( i18n( "&Spacing..." ), KPBarIcon( "spacing" ), 0,
				     this, SLOT( textSpacing() ),
				     actionCollection(), "text_spacing" );

    actionTextExtentCont2Height = new KAction( i18n( "Extend Contents to Object &Height" ), 0,
					       this, SLOT( textContentsToHeight() ),
					       actionCollection(), "text_con2hei" );

    actionTextExtendObj2Cont = new KAction( i18n( "&Extend Object to fit the Contents" ), 0,
					    this, SLOT( textObjectToContents() ),
					    actionCollection(), "text_obj2cont" );


    // ----------------- extra actions

    actionExtraPenBrush = new KAction( i18n( "&Pen and Brush..." ), KPBarIcon( "style" ), 0,
				       this, SLOT( extraPenBrush() ),
				       actionCollection(), "extra_penbrush" );

    actionExtraConfigPie = new KAction( i18n( "Configure Pie/&Arc/Chord..." ), KPBarIcon( "edit_pie" ), 0,
					this, SLOT( extraConfigPie() ),
				       actionCollection(), "extra_configpie" );

    actionExtraConfigRect = new KAction( i18n( "Configure &Rectangle..." ), KPBarIcon( "rectangle2" ), 0,
					this, SLOT( extraConfigRect() ),
				       actionCollection(), "extra_configrect" );

    actionExtraRaise = new KAction( i18n( "R&aise object(s)" ), KPBarIcon( "raise" ), CTRL + Key_R,
				    this, SLOT( extraRaise() ),
				    actionCollection(), "extra_raise" );

    actionExtraLower = new KAction( i18n( "&Lower object(s)" ), KPBarIcon( "lower" ), CTRL + Key_L,
				    this, SLOT( extraLower() ),
				    actionCollection(), "extra_lower" );

    actionExtraRotate = new KAction( i18n( "R&otate object(s)" ), KPBarIcon( "rotate" ), 0,
				     this, SLOT( extraRotate() ),
				     actionCollection(), "extra_rotate" );

    actionExtraShadow = new KAction( i18n( "&Shadow object(s)" ), KPBarIcon( "shadow" ), 0,
				     this, SLOT( extraShadow() ),
				     actionCollection(), "extra_shadow" );

    actionExtraAlignObjLeft = new KAction( i18n( "Align &Left" ), KPBarIcon( "aoleft" ), 0,
					   this, SLOT( extraAlignObjLeft() ),
					   actionCollection(), "extra_alignleft" );

    actionExtraAlignObjCenterH = new KAction( i18n( "Align Center (&horizontally)" ), KPBarIcon( "aocenterh" ), 0,
					      this, SLOT( extraAlignObjCenterH() ),
					      actionCollection(), "extra_aligncenterh" );

    actionExtraAlignObjRight = new KAction( i18n( "Align &Right" ), KPBarIcon( "aoright" ), 0,
					    this, SLOT( extraAlignObjRight() ),
					    actionCollection(), "extra_alignright" );

    actionExtraAlignObjTop = new KAction( i18n( "Align &Top" ), KPBarIcon( "aotop" ), 0,
					  this, SLOT( extraAlignObjTop() ),
					  actionCollection(), "extra_aligntop" );

    actionExtraAlignObjCenterV = new KAction( i18n( "Align Center (&vertically)" ), KPBarIcon( "aocenterv" ), 0,
					      this, SLOT( extraAlignObjCenterV() ),
					      actionCollection(), "extra_aligncenterv" );

    actionExtraAlignObjBottom = new KAction( i18n( "Align &Bottom" ), KPBarIcon( "aobottom" ), 0,
					     this, SLOT( extraAlignObjBottom() ),
					     actionCollection(), "extra_alignbottom" );

    actionExtraBackground = new KAction( i18n( "Page Bac&kground..." ), 0,
					 this, SLOT( extraBackground() ),
					 actionCollection(), "extra_background" );

    actionExtraLayout = new KAction( i18n( "Page &Layout..." ), 0,
				     this, SLOT( extraLayout() ),
				     actionCollection(), "extra_layout" );

    actionExtraOptions = new KAction( i18n( "Op&tions..." ), 0,
				      this, SLOT( extraOptions() ),
				      actionCollection(), "extra_options" );

    actionExtraWebPres = new KAction( i18n( "Create &Web-Presentation (HTML SLideshow)..." ),
				      KPBarIcon( "webpres" ), 0,
				      this, SLOT( extraWebPres() ),
				      actionCollection(), "extra_webpres" );

    actionExtraCreateTemplate = new KAction( i18n( "Create Template from current slide..." ), 0,
					     this, SLOT( extraCreateTemplate() ),
					     actionCollection(), "extra_template" );

    actionExtraAlignObjs = new KAction( i18n( "" ),
					KPBarIcon( "alignobjs" ), 0,
					this, SLOT( extraAlignObjs() ),
					actionCollection(), "extra_alignobjs" );

    actionExtraLineBegin = new KAction( i18n( "" ),
					KPBarIcon( "line_begin" ), 0,
					this, SLOT( extraLineBegin() ),
					actionCollection(), "extra_linebegin" );

    actionExtraLineEnd = new KAction( i18n( "" ),
				      KPBarIcon( "line_end" ), 0,
				      this, SLOT( extraLineEnd() ),
				      actionCollection(), "extra_lineend" );

    actionExtraGroup = new KAction( i18n( "&Group Objects" ),
				    KPBarIcon( "group" ), 0,
				    this, SLOT( extraGroup() ),
				    actionCollection(), "extra_group" );

    actionExtraGroup = new KAction( i18n( "&Ungroup Objects" ),
				    KPBarIcon( "ungroup" ), 0,
				    this, SLOT( extraUnGroup() ),
				    actionCollection(), "extra_ungroup" );

    // ----------------- screenpresentation actions

    actionScreenConfigPages = new KAction( i18n( "&Configure Pages..." ),
					   0,
					   this, SLOT( screenConfigPages() ),
					   actionCollection(), "screen_configpages" );

    actionScreenPresStructView = new KAction( i18n( "&Open Presentationstructor Editor..." ),
					   0,
					   this, SLOT( screenPresStructView() ),
					   actionCollection(), "screen_presstruct" );

    actionScreenAssignEffect = new KAction( i18n( "&Assign effect..." ),
					   KPBarIcon( "effect" ), 0,
					   this, SLOT( screenAssignEffect() ),
					   actionCollection(), "screen_assigneffect" );

    actionScreenStart = new KAction( i18n( "&Start" ),
				     KPBarIcon( "kpstart" ), 0,
				     this, SLOT( screenStart() ),
				     actionCollection(), "screen_start" );

    actionScreenFirst = new KAction( i18n( "&Go to Start" ),
				     KPBarIcon( "first" ), Key_Home,
				     this, SLOT( screenFirst() ),
				     actionCollection(), "screen_first" );

    actionScreenPrev = new KAction( i18n( "&Previous Step" ),
				     KPBarIcon( "kpprev" ), Key_Prior,
				     this, SLOT( screenPrev() ),
				     actionCollection(), "screen_prev" );

    actionScreenNext = new KAction( i18n( "&Next Step" ),
				     KPBarIcon( "kpnext" ), Key_Next,
				     this, SLOT( screenNext() ),
				     actionCollection(), "screen_next" );

    actionScreenLast = new KAction( i18n( "Go to End" ),
				     KPBarIcon( "last" ), Key_End,
				     this, SLOT( screenLast() ),
				     actionCollection(), "screen_last" );

    actionScreenSkip = new KAction( i18n( "Goto &Page..." ),
				     0,
				     this, SLOT( screenSkip() ),
				     actionCollection(), "screen_skip" );

    actionScreenPenColor = new KColorAction( i18n( "Pen &Color..." ), KColorAction::BackgroundColor, 0,
					     this, SLOT( screenPenColor() ),
					     actionCollection(), "screen_pencolor" );

    actionScreenPenWidth = new KSelectAction( i18n( "Pen &Width" ), 0,
					     actionCollection(), "screen_penwidth" );
    QStringList lst;
    lst << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "10";
    ( ( KSelectAction* )actionScreenPenWidth )->setItems( lst );
    connect( ( ( KSelectAction* )actionScreenPenWidth ), SIGNAL( activated( const QString & ) ),	
	     this, SLOT( screenPenWidth( const QString & ) ) );

    // ----------------- colorbar action

    QValueList<QColor> colorList;
    colorList << white << red << green << blue << cyan << magenta << yellow
	      << darkRed << darkGreen << darkBlue << darkCyan
	      << darkMagenta << darkYellow << white << lightGray
	      << gray << darkGray << black;

    actionColorBar = new KColorBarAction( i18n( "Colorbar" ), 0,
					  this,
					  SLOT( brushChosen( const QColor & ) ),
					  SLOT( penChosen( const QColor & ) ),
					  colorList,
					  actionCollection(), "colorbar" );



}

/*====================== construct ==============================*/
void KPresenterView::construct()
{
    if ( m_pKPresenterDoc == 0L && !m_bUnderConstruction ) return;

    assert( m_pKPresenterDoc != 0L );

    m_bUnderConstruction = false;

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
    QString fileName = locate( "autoforms",
			       fileInfo.dirPath( false ) + "/" + fileInfo.baseName() + ".atf",
			       KPresenterFactory::global() );

    page->deSelectAllObj();

    page->deSelectAllObj();
    page->setToolEditMode( INS_AUTOFORM );
    page->setAutoForm( fileName );
}

/*=========== take changes for style dialog =====================*/
void KPresenterView::styleOk()
{
    if ( !m_pKPresenterDoc->setPenBrush( styleDia->getPen(), styleDia->getBrush(), styleDia->getLineBegin(),
					 styleDia->getLineEnd(), styleDia->getFillType(),
					 styleDia->getGColor1(),
					 styleDia->getGColor2(), styleDia->getGType(),
					 styleDia->getGUnbalanced(),
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
					  kPresenterDoc()->spManualSwitch(),
					  kPresenterDoc()->spInfinitLoop(),
					  kPresenterDoc()->backgroundList()->
					  at( getCurrPgNum() - 1 )->getPageEffect(),
					  kPresenterDoc()->getPresSpeed(),
					  kPresenterDoc()->getPresentSlides(),
					  kPresenterDoc()->getSelectedSlides(),
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

    for ( int i = 0; i < static_cast<int>( kPresenterDoc()->objectList()->count() ); i++ ) {
	kpobject = kPresenterDoc()->objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    tmp = new RotateCmd::RotateValues;
	    tmp->angle = kpobject->getAngle();
	    _oldRotate.append( tmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() ) {
	RotateCmd *rotateCmd = new RotateCmd( i18n( "Change Rotation" ),
					      _oldRotate, _newAngle, _objects, kPresenterDoc() );
	kPresenterDoc()->commands()->addCommand( rotateCmd );
	rotateCmd->execute();
    } else {
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

    for ( int i = 0; i < static_cast<int>( kPresenterDoc()->objectList()->count() ); i++ ) {
	kpobject = kPresenterDoc()->objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    tmp = new ShadowCmd::ShadowValues;
	    tmp->shadowDirection = kpobject->getShadowDirection();
	    tmp->shadowDistance = kpobject->getShadowDistance();
	    tmp->shadowColor = kpobject->getShadowColor();
	    _oldShadow.append( tmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() ) {
	ShadowCmd *shadowCmd = new ShadowCmd( i18n( "Change Shadow" ),
					      _oldShadow, _newShadow, _objects, kPresenterDoc() );
	kPresenterDoc()->commands()->addCommand( shadowCmd );
	shadowCmd->execute();
    } else {
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
    if ( !m_pKPresenterDoc->setPieSettings( confPieDia->getType(),
					    confPieDia->getAngle(), confPieDia->getLength() ) ) {
	pieType = confPieDia->getType();
	pieAngle = confPieDia->getAngle();
	pieLength = confPieDia->getLength();
    }
}

/*================================================================*/
void KPresenterView::confRectOk()
{
    if ( !m_pKPresenterDoc->setRectSettings( confRectDia->getRndX(), confRectDia->getRndY() ) ) {
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
    if ( !presStarted ) {
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
    if ( !presStarted ) {
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
    if ( font->operator!=( tbFont ) ) {
	tbFont.setFamily( font->family() );
	tbFont.setBold( font->bold() );
	tbFont.setItalic( font->italic() );
	tbFont.setUnderline( font->underline() );
	tbFont.setPointSize( font->pointSize() );
	( (KToggleAction*) actionTextFontFamily )->blockSignals( TRUE );
 	( (KFontAction*) actionTextFontFamily )->setFont( tbFont.family() );
	( (KToggleAction*) actionTextFontFamily )->blockSignals( FALSE );
	( (KToggleAction*) actionTextFontSize )->blockSignals( TRUE );
 	( (KFontSizeAction*) actionTextFontSize )->setFontSize( tbFont.pointSize() );
	( (KToggleAction*) actionTextFontSize )->blockSignals( FALSE );
	( (KToggleAction*) actionTextBold )->blockSignals( TRUE );
	( (KToggleAction*) actionTextBold )->setChecked( tbFont.bold() );
	( (KToggleAction*) actionTextBold )->blockSignals( FALSE );
	( (KToggleAction*) actionTextItalic )->blockSignals( TRUE );
	( (KToggleAction*) actionTextItalic )->setChecked( tbFont.italic() );
	( (KToggleAction*) actionTextItalic )->blockSignals( FALSE );
	( (KToggleAction*) actionTextUnderline )->blockSignals( TRUE );
	( (KToggleAction*) actionTextUnderline )->setChecked( tbFont.underline() );
	( (KToggleAction*) actionTextUnderline )->blockSignals( FALSE );
    }
}

/*====================== color changed ==========================*/
void KPresenterView::colorChanged( QColor* color )
{
    if ( color->operator!=( tbColor ) ) {
	tbColor.setRgb( color->rgb() );
	( (KColorAction*) actionTextColor )->blockSignals( TRUE );
	( (KColorAction*) actionTextColor )->setColor( tbColor );
	( (KColorAction*) actionTextColor )->blockSignals( FALSE );
    }
}

/*====================== align changed ==========================*/
void KPresenterView::alignChanged( TxtParagraph::HorzAlign align )
{
    if ( align != tbAlign ) {
	tbAlign = align;
	switch ( tbAlign ) {
	case TxtParagraph::LEFT:
	    ( (KToggleAction*) actionTextAlignLeft )->blockSignals( TRUE );
	    ( (KToggleAction*)actionTextAlignLeft )->setChecked( TRUE );
	    ( (KToggleAction*) actionTextAlignLeft )->blockSignals( FALSE );
	    break;
	case TxtParagraph::CENTER:
	    ( (KToggleAction*) actionTextAlignCenter )->blockSignals( TRUE );
	    ( (KToggleAction*)actionTextAlignCenter )->setChecked( TRUE );
	    ( (KToggleAction*) actionTextAlignCenter )->blockSignals( FALSE );
	    break;
	case TxtParagraph::RIGHT:
	    ( (KToggleAction*) actionTextAlignRight )->blockSignals( TRUE );
	    ( (KToggleAction*)actionTextAlignRight )->setChecked( TRUE );
	    ( (KToggleAction*) actionTextAlignRight )->blockSignals( FALSE );
	    break;
	default: break;
	}
    }
}

/*======================== set pres pen width 1 =================*/
void KPresenterView::screenPenWidth( const QString &item )
{
    int i = item.toInt();
    QPen p = kPresenterDoc()->presPen();
    p.setWidth( i );
    kPresenterDoc()->setPresPen( p );
}

/*======================== set pres pen color ===================*/
void KPresenterView::screenPenColor()
{
    QColor c = kPresenterDoc()->presPen().color();
    if ( KColorDialog::getColor( c ) ) {
	QPen p = kPresenterDoc()->presPen();
	p.setColor( c );
	kPresenterDoc()->setPresPen( p );
	( (KColorAction*)actionScreenPenColor )->blockSignals( TRUE );
	( (KColorAction*)actionScreenPenColor )->setColor( c );
	( (KColorAction*)actionScreenPenColor )->blockSignals( FALSE );
    }
}

/*=========================== search =============================*/
void KPresenterView::search( QString text, bool sensitive, bool direction )
{
    if ( page->kTxtObj() ) {
	TxtCursor from, to;
	from.setKTextObject( page->kTxtObj() );
	to.setKTextObject( page->kTxtObj() );
	bool found = false;

	if ( !direction ) {
	    if ( searchFirst )
		found = page->kTxtObj()->searchFirst( text, &from, &to, sensitive );
	    else
		found = page->kTxtObj()->searchNext( text, &from, &to, sensitive );

	    if ( found )
		searchFirst = false;
	    else {
		searchFirst = false;
		page->kTxtObj()->setSearchIndexToBegin();
		QMessageBox::warning( this, i18n( "Warning" ),
				      i18n( "The search string '" + text + "' couldn't be found!" ), i18n("OK"));
	    }
	} else {
	    if ( searchFirst )
		found = page->kTxtObj()->searchFirstRev( text, &from, &to, sensitive );
	    else
		found = page->kTxtObj()->searchNextRev( text, &from, &to, sensitive );

	    if ( found )
		searchFirst = false;
	    else {
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
    if ( page->kTxtObj() ) {
	TxtCursor from, to;
	from.setKTextObject( page->kTxtObj() );
	to.setKTextObject( page->kTxtObj() );
	bool found = false;

	if ( !direction ) {
	    if ( searchFirst )
		found = page->kTxtObj()->replaceFirst( search, replace, &from, &to, sensitive );
	    else
		found = page->kTxtObj()->replaceNext( search, replace, &from, &to, sensitive );

	    if ( found )
		searchFirst = false;
	    else {
		searchFirst = false;
		page->kTxtObj()->setSearchIndexToBegin();
		QMessageBox::warning( this, i18n( "Warning" ),
				      i18n( "The search string '" + search + "' couldn't be found"
					    " and replaced with '" + replace + "'!" ), i18n("OK"));
	    }
	} else {
	    if ( searchFirst )
		found = page->kTxtObj()->replaceFirstRev( search, replace, &from, &to, sensitive );
	    else
		found = page->kTxtObj()->replaceNextRev( search, replace, &from, &to, sensitive );

	    if ( found )
		searchFirst = false;
	    else {
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
    if ( page->kTxtObj() ) {
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
    QString file = KFilePreviewDialog::getOpenFileName( fileInfo.dirPath( false ),
							i18n( "*.WMF *.wmf|Windows Metafiles" ),
							0L );

    if ( !file.isEmpty() ) m_pKPresenterDoc->changeClipart( file, xOffset, yOffset );
}

/*====================== resize event ===========================*/
void KPresenterView::resizeEvent( QResizeEvent *e )
{
    if ( !presStarted ) QWidget::resizeEvent( e );

    if ( m_bShowGUI ) {
	horz->show();
	vert->show();
	if ( h_ruler )
	    h_ruler->show();
	if ( v_ruler )
	    v_ruler->show();
	page->resize( this->width() - 36, this->height() - 36 );
	page->move( 20, 20 );
	vert->setGeometry( this->width() - 16, 0, 16, this->height() - 16 );
	horz->setGeometry( 0, this->height() - 16, this->width() - 16, 16 );
	if ( h_ruler )
	    h_ruler->setGeometry( 20, 0, page->width(), 20 );
	if ( v_ruler )
	    v_ruler->setGeometry( 0, 20, 20, page->height() );
	setRanges();
    } else {
	horz->hide();
	vert->hide();
	h_ruler->hide();
	v_ruler->hide();
	page->move( 0, 0 );
	page->resize( this->width(), this->height() );
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
    if ( e->key() == Key_Delete && !page->kTxtObj() )
	editDelete();
    else
	QApplication::sendEvent( page, e );
}

/*====================== do automatic screenpresentation ========*/
void KPresenterView::doAutomaticScreenPres()
{
    page->repaint( false );

    while ( continuePres && !exitPres )
	screenNext();

    if ( !exitPres && kPresenterDoc()->spInfinitLoop() ) {
	screenStop();
	screenStart();
    }

    screenStop();
}

/*========================= change undo =========================*/
void KPresenterView::changeUndo( QString _text, bool _enable )
{
    if ( _enable ) {
	actionEditUndo->setEnabled( TRUE );
	QString str;
	str.sprintf( i18n( "Undo: %s" ), _text.data() );
	actionEditUndo->setText( str );
    } else {
	actionEditUndo->setEnabled( FALSE );
	actionEditUndo->setText( i18n( "No Undo possible" ) );
    }
}

/*========================= change redo =========================*/
void KPresenterView::changeRedo( QString _text, bool _enable )
{
    if ( _enable ) {
	actionEditRedo->setEnabled( TRUE );
	QString str;
	str.sprintf( i18n( "Redo: %s" ), _text.data() );
	actionEditRedo->setText( str );
    } else {
	actionEditRedo->setEnabled( FALSE );
	actionEditRedo->setText( i18n( "No Redo possible" ) );
    }
}

/*======================== setup popup menus ===================*/
void KPresenterView::setupPopupMenus()
{
    QPixmap pixmap;

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
    if ( page && !presStarted ) page->resize( this->width()-16, this->height()-16 );
    vert->setGeometry( this->width()-16, 0, 16, this->height()-16 );
    horz->setGeometry( 0, this->height()-16, this->width()-16, 16 );
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

    QObject::connect( h_ruler, SIGNAL( unitChanged( QString ) ),
		      this, SLOT( unitChanged( QString ) ) );
    QObject::connect( h_ruler, SIGNAL( newPageLayout( KoPageLayout ) ),
		      this, SLOT( newPageLayout( KoPageLayout ) ) );
    QObject::connect( h_ruler, SIGNAL( openPageLayoutDia() ),
		      this, SLOT( openPageLayoutDia() ) );
    QObject::connect( v_ruler, SIGNAL( unitChanged( QString ) ),
		      this, SLOT( unitChanged( QString ) ) );
    QObject::connect( v_ruler, SIGNAL( newPageLayout( KoPageLayout ) ),
		      this, SLOT( newPageLayout( KoPageLayout ) ) );
    QObject::connect( v_ruler, SIGNAL( openPageLayoutDia() ),
		      this, SLOT( openPageLayoutDia() ) );

    switch ( m_pKPresenterDoc->pageLayout().unit ) {
    case PG_MM: {
	h_ruler->setUnit( "mm" );
	v_ruler->setUnit( "mm" );
    } break;
    case PG_PT: {
	h_ruler->setUnit( "pt" );
	v_ruler->setUnit( "pt" );
    } break;
    case PG_INCH: {
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
    if ( vert && horz && page && m_pKPresenterDoc ) {
	int range;

	vert->setSteps( 10, m_pKPresenterDoc->getPageSize( 0, xOffset, yOffset, 1.0, false ).height() + 20 );
	range = ( m_pKPresenterDoc->getPageSize( 0, xOffset, yOffset, 1.0, false ).height() ) *
		(int)m_pKPresenterDoc->getPageNums() - (int)page->height() + 16
		< 0 ? 0 :
	    ( m_pKPresenterDoc->getPageSize( 0, xOffset, yOffset, 1.0, false ).height() ) *
		m_pKPresenterDoc->getPageNums() - page->height() + 16;
	vert->setRange( 0, range );
	horz->setSteps( 10, m_pKPresenterDoc->getPageSize( 0, xOffset, yOffset, 1.0, false ).width() +
			16 - page->width() );
	range = m_pKPresenterDoc->getPageSize( 0, xOffset, yOffset, 1.0, false ).width() +
		16 - page->width() < 0 ? 0 :
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
    switch ( toolEditMode ) {
    case TEM_MOUSE:
	( (KToggleAction*)actionToolsMouse )->setChecked( TRUE );
	break;
    case INS_LINE:
	( (KToggleAction*)actionToolsLine )->setChecked( TRUE );
	break;
    case INS_RECT:
	( (KToggleAction*)actionToolsRectangle )->setChecked( TRUE );
	break;
    case INS_ELLIPSE:
	( (KToggleAction*)actionToolsCircleOrEllipse )->setChecked( TRUE );
	break;
    case INS_PIE:
	( (KToggleAction*)actionToolsPie )->setChecked( TRUE );
	break;
    case INS_OBJECT:
	( (KToggleAction*)actionToolsObject )->setChecked( TRUE );
	break;
    case INS_DIAGRAMM:
	( (KToggleAction*)actionToolsDiagramm )->setChecked( TRUE );
	break;
    case INS_TABLE:
	( (KToggleAction*)actionToolsTable )->setChecked( TRUE );
	break;
    case INS_FORMULA:
	( (KToggleAction*)actionToolsFormula )->setChecked( TRUE );
	break;
    case INS_TEXT:
	( (KToggleAction*)actionToolsText )->setChecked( TRUE );
	break;
    case INS_AUTOFORM:
	( (KToggleAction*)actionToolsAutoform )->setChecked( TRUE );
	break;
    }
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

    bool have_installed = kapp->kdeFonts( lst );

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

/*================================================================*/
void KPresenterView::enableWebPres()
{
}

/*================================================================*/
bool KPresenterView::doubleClickActivation() const
{
    return TRUE;
}

/*================================================================*/
QWidget* KPresenterView::canvas()
{
    return page;
}

/*================================================================*/
int KPresenterView::canvasXOffset() const
{
    return -getDiffX();
}

/*================================================================*/
int KPresenterView::canvasYOffset() const
{
    return -getDiffY();
}

/*================================================================*/
int KPresenterView::getCurrentPresPage()
{
    if ( !presStarted )
	return -1;

    return page->presPage();
}

/*================================================================*/
int KPresenterView::getCurrentPresStep()
{
    if ( !presStarted )
	return -1;

    return page->presStep();
}

/*================================================================*/
int KPresenterView::getPresStepsOfPage()
{
    if ( !presStarted )
	return -1;

    return page->numPresSteps();
}

/*================================================================*/
int KPresenterView::getNumPresPages()
{
    if ( !presStarted )
	return -1;

    return page->numPresPages();
}

/*================================================================*/
float KPresenterView::getCurrentFaktor()
{
    if ( !presStarted )
	return 1.0;

    return page->presFakt();
}

/*================================================================*/
bool KPresenterView::gotoPresPage( int pg )
{
    if ( !presStarted )
	return FALSE;

    page->gotoPage( pg );
    return TRUE;
}
