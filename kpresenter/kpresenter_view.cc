/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kprinter.h>
#include <kaccel.h>
#include <qpainter.h>
#include <qscrollbar.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <assert.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qdir.h>
#include <qclipboard.h>
#include <qradiobutton.h>
#include <qdragobject.h>

#include <backdia.h>
#include <autoformEdit/afchoose.h>
#include <styledia.h>
#include <pgconfdia.h>
#include <effectdia.h>
#include <rotatedia.h>
#include <shadowdia.h>
#include <presstructview.h>


#include <confpiedia.h>
#include <confrectdia.h>
#include <confpolygondia.h>
#include <kppartobject.h>
#include <textdialog.h>
#include <sidebar.h>
#include <notebar.h>
#include <insertpagedia.h>
#include <koPictureFilePreview.h>

#include <dcopclient.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kstdaction.h>
#include <kapplication.h>
#include <kspelldlg.h>
#include <kio/netaccess.h>

#include <kpresenter_view.h>
#include <webpresentation.h>
#include <footer_header.h>
#include <kptextobject.h>

#include <klocale.h>
#include <kcolordialog.h>
#include <kconfig.h>
#include <kfontdialog.h>
#include <kglobal.h>
#include <kimageio.h>
#include <kparts/event.h>
#include <kdebug.h>
#include <ktempfile.h>
#include <kcolorbutton.h>

#include <koPartSelectDia.h>
#include <koQueryTrader.h>
#include <koPageLayoutDia.h>
#include <koRuler.h>
#include <koTemplateCreateDia.h>
#include <kcoloractions.h>
#include <tkcoloractions.h>
#include <kaction.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <koPartSelectAction.h>
#include <kozoomhandler.h>

#include <stdlib.h>
#include <signal.h>

#include <kstandarddirs.h>

#include <KPresenterViewIface.h>
#include <kpresenter_dlg_config.h>

#include <qrichtext_p.h>
#include <kotextobject.h>
#include <kprcommand.h>
#include <koFontDia.h>
#include <koCharSelectDia.h>
#include <koInsertLink.h>
#include <koAutoFormatDia.h>
#include <koparagcounter.h>
#include <koParagDia.h>
#include <koVariable.h>
#include <koVariableDlgs.h>

#include <kspell.h>
#include <kstatusbar.h>
#include "kprtextdocument.h"

#include <koChangeCaseDia.h>
#include <qregexp.h>

#include <koSearchDia.h>
#include "searchdia.h"
#include "kprvariable.h"
#include "kprcanvas.h"
#include <qpaintdevicemetrics.h>

#define DEBUG

static const char *pageup_xpm[] = {
"    14    14        2            1",
". c #000000",
"# c none",
"##############",
"##############",
"######..######",
"#####....#####",
"####......####",
"###........###",
"##############",
"######..######",
"#####....#####",
"####......####",
"###........###",
"##############",
"##############",
"##############"
};

static const char *pagedown_xpm[] = {
"    14    14        2            1",
". c #000000",
"# c none",
"##############",
"##############",
"##############",
"###........###",
"####......####",
"#####....#####",
"######..######",
"##############",
"###........###",
"####......####",
"#####....#####",
"######..######",
"##############",
"##############"
};

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
    : KoView( _doc, _parent, _name )
{

    setInstance( KPresenterFactory::global() );
    setXMLFile( "kpresenter.rc" );

    dcop = 0;
    dcopObject(); // build it

    m_pKPresenterDoc = 0L;

    // init
    backDia = 0;
    afChoose = 0;
    styleDia = 0;
    pgConfDia = 0;
    rotateDia = 0;
    shadowDia = 0;
    presStructView = 0;
    confPieDia = 0;
    confRectDia = 0;
    confPolygonDia = 0;
    v_ruler = 0;
    h_ruler = 0;
    pen = QPen( black, 1, SolidLine );
    brush = QBrush( white, SolidPattern );
    lineBegin = L_NORMAL;
    lineEnd = L_NORMAL;
    gColor1 = red;
    gColor2 = green;
    gType = BCT_GHORZ;
    gUnbalanced = false;
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
    continuePres = false;
    exitPres = false;
    rndX = 0;
    rndY = 0;
    allowWebPres = true;
    currPg = 0;
    sidebar = 0;
    notebar = 0;
    splitter = 0;
    pageBase = 0;
    sticky = FALSE;
    m_canvas = 0L;
    m_spell.kspell = 0;
    automaticScreenPresFirstTimer = true;
    m_actionList.setAutoDelete( true );
    checkConcavePolygon = false;
    cornersValue = 3;
    sharpnessValue = 0;

    m_searchEntry = 0L;
    m_replaceEntry = 0L;
    m_findReplace = 0L;
    m_searchPage=-1;

    m_pKPresenterDoc = _doc;

    createGUI();

    setKeyCompression( true );
    m_specialCharDlg=0L;

    if ( shell() )
    {
        changeNbOfRecentFiles( m_pKPresenterDoc->maxRecentFiles() );
    }

    connect(this, SIGNAL(embeddImage(const QString &)), SLOT(insertPicture(const QString &)));
    connect( m_pKPresenterDoc, SIGNAL( sig_refreshMenuCustomVariable()),
             this, SLOT( refreshCustomMenu()));

    // Cut and copy are directly connected to the selectionChanged signal
    if ( m_pKPresenterDoc->isReadWrite() )
    {
        connect( m_canvas, SIGNAL(selectionChanged(bool)),
                 actionEditCut, SLOT(setEnabled(bool)) );
    }
    else
    {
        actionEditCut->setEnabled( false );
    }
    connect( m_canvas, SIGNAL(selectionChanged(bool)),
             actionEditCopy, SLOT(setEnabled(bool)) );

    connect (m_canvas, SIGNAL(selectionChanged(bool)),
             actionChangeCase, SLOT(setEnabled(bool)));

    connect( h_ruler, SIGNAL( tabListChanged( const KoTabulatorList & ) ), this,
             SLOT( tabListChanged( const KoTabulatorList & ) ) );

    //statusbar stuff
    connect( m_pKPresenterDoc, SIGNAL( pageNumChanged() ), this, SLOT( pageNumChanged()) );
    connect( this, SIGNAL( currentPageChanged(int) ), this, SLOT( pageNumChanged()) );
    connect( m_canvas, SIGNAL( objectSelectedChanged() ), this, SLOT( updateObjectStatusBarItem() ));
    connect (m_pKPresenterDoc, SIGNAL(sig_updateRuler()),this, SLOT( slotUpdateRuler()));
    connect (m_pKPresenterDoc, SIGNAL(sig_updateRuler()),this, SLOT( slotUpdateScrollBarRanges()));
    connect (m_pKPresenterDoc, SIGNAL(sig_updateMenuBar()),this, SLOT(updateSideBarMenu()));

    //change table active.
    connect( m_pKPresenterDoc, SIGNAL( sig_changeActivePage( KPrPage* ) ), m_canvas, SLOT( slotSetActivePage( KPrPage* ) ) );

    KStatusBar * sb = statusBar();
    m_sbPageLabel = 0L;
    if ( sb ) // No statusbar in e.g. konqueror
    {
        m_sbPageLabel = new KStatusBarLabel( QString::null, 0, sb );
        addStatusBarItem( m_sbPageLabel, 0 );
    }
    m_sbObjectLabel = 0L; // Only added when objects are selected

    setAcceptDrops( TRUE );
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
    //close header/footer dialogbox when kpresenter is embedded
    //into other programm, and it's desactivated
    if(m_pKPresenterDoc->getHeaderFooterEdit())
        m_pKPresenterDoc->getHeaderFooterEdit()->slotCloseDia();
    if(sidebar) {
        KConfig *config=KGlobal::config();
        config->setGroup("Global");
        config->writeEntry("Sidebar", sidebar->isVisible());
    }
    if(notebar) {
        KConfig *config=KGlobal::config();
        config->setGroup("Global");
        config->writeEntry("Notebar", notebar->isVisible());
    }

    if(m_spell.kspell)
    {
        KPTextObject * objtxt = 0L;
        if(m_spell.spellCurrTextObjNum !=-1)
        {
            objtxt =m_spell.textObject.at( m_spell.spellCurrTextObjNum ) ;
            Q_ASSERT( objtxt );
            if ( objtxt )
                objtxt->removeHighlight();
        }
        delete m_spell.kspell;
    }

    delete presStructView;
    delete rb_oalign;
    delete rb_lbegin;
    delete rb_lend;
    delete dcop;
    delete m_canvas; // it's a child widget, but it emits a signal on destruction
    delete m_specialCharDlg;
    delete m_sbPageLabel;
    delete notebar;
    delete m_searchEntry;
    delete m_replaceEntry;
    delete m_specialCharDlg;
}

/*=========================== file print =======================*/
void KPresenterView::setupPrinter( KPrinter &prt )
{
    m_pKPresenterDoc->recalcVariables(  VT_TIME );
    m_pKPresenterDoc->recalcVariables(  VT_DATE );
    prt.setMinMax( 1, m_pKPresenterDoc->getPageNums() );
    prt.setFromTo( 1, m_pKPresenterDoc->getPageNums() );
    prt.setOption( "kde-range", m_pKPresenterDoc->selectedForPrinting() );
    prt.setPageSelection(KPrinter::ApplicationSide);
    KoFormat pageFormat = m_pKPresenterDoc->pageLayout().format;
    prt.setPageSize( static_cast<KPrinter::PageSize>( KoPageFormat::printerPageSize( pageFormat ) ) );

    if ( m_pKPresenterDoc->pageLayout().orientation == PG_LANDSCAPE || pageFormat == PG_SCREEN )
        prt.setOrientation( KPrinter::Landscape );
    else
        prt.setOrientation( KPrinter::Portrait );
}

void KPresenterView::unZoomDocument(int &dpiX,int &dpiY)
{
    // ### HACK: disable zooming-when-printing if embedded parts are used.
    // No koffice app supports zooming in paintContent currently.
    // Disable in ALL cases now
    bool doZoom=false;
    dpiX = doZoom ? 300 : QPaintDevice::x11AppDpiX();
    dpiY = doZoom ? 300 : QPaintDevice::x11AppDpiY();
    m_pKPresenterDoc->zoomHandler()->setZoomAndResolution( 100, dpiX, dpiY );
    m_pKPresenterDoc->newZoomAndResolution( false, true /* for printing*/ );
}

void KPresenterView::zoomDocument(int zoom)
{
    m_pKPresenterDoc->zoomHandler()->setZoomAndResolution( zoom, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY() );
    m_pKPresenterDoc->newZoomAndResolution( false, false );
    updateRuler();
}

void KPresenterView::print( KPrinter &prt )
{
    float left_margin = 0.0;
    float top_margin = 0.0;
    int dpiX=0;
    int dpiY=0;
    int oldZoom = m_pKPresenterDoc->zoomHandler()->zoom();
    QPaintDeviceMetrics metrics( &prt );
    unZoomDocument(dpiX,dpiY);
    if ( m_pKPresenterDoc->pageLayout().format == PG_SCREEN )
    {
        left_margin = 28.5;
        top_margin = 15.0;
    }

    m_canvas->deSelectAllObj();
    QPainter painter;
    painter.begin( &prt );
    kdDebug() << "KPresenterView::print scaling by " << (double)metrics.logicalDpiX() / (double)dpiX
                   << "," << (double)metrics.logicalDpiY() / (double)dpiY << endl;
    painter.scale( (double)metrics.logicalDpiX() / (double)dpiX,
                   (double)metrics.logicalDpiY() / (double)dpiY );

    m_canvas->print( &painter, &prt, left_margin, top_margin );
    painter.end();

    zoomDocument(oldZoom);
    kdDebug() << "KPresenterView::print zoom&res reset" << endl;
}

/*===============================================================*/
void KPresenterView::editCut()
{
    if ( !m_canvas->currentTextObjectView() ) {
	m_canvas->setToolEditMode( TEM_MOUSE );
	m_canvas->activePage()->copyObjs();
	m_canvas->activePage()->deleteObjs();
    } else {
	m_canvas->currentTextObjectView()->cut();
    }
}

/*===============================================================*/
void KPresenterView::editCopy()
{
    if ( !m_canvas->currentTextObjectView() ) {
	m_canvas->setToolEditMode( TEM_MOUSE );
	m_canvas->activePage()->copyObjs();
    } else {
	m_canvas->currentTextObjectView()->copy();
    }
}

/*===============================================================*/
void KPresenterView::editPaste()
{
    if ( !m_canvas->currentTextObjectView() ) {
        m_canvas->setToolEditMode( TEM_MOUSE );
        m_canvas->deSelectAllObj();
        QMimeSource *data = QApplication::clipboard()->data();
        QString clip_str = QString::fromUtf8( data->encodedData("application/x-kpresenter-selection") );
        if ( data->provides( "text/uri-list" ) )
        {
            m_pKPresenterDoc->pastePage( data, currPg );
        }
        else if ( data->provides( "application/x-kpresenter-selection" ) )
        {
            m_canvas->activePage()->pasteObjs( data->encodedData("application/x-kpresenter-selection"));

            m_canvas->setMouseSelectedObject(true);
            emit objectSelectedChanged();
        }
        else if (QImageDrag::canDecode (data)) {
            m_canvas->setToolEditMode( TEM_MOUSE );
            m_canvas->deSelectAllObj();

            QImage pix;
            QImageDrag::decode( data, pix );

            KTempFile tmpFile;
            tmpFile.setAutoDelete(true);

            if( tmpFile.status() != 0 ) {
                return;
            }
            tmpFile.close();

            pix.save( tmpFile.name(), "PNG" );
            QCursor c = cursor();
            setCursor( waitCursor );
            m_canvas->activePage()->insertPicture( tmpFile.name() );
            setCursor( c );
        }
    } else {
        m_canvas->currentTextObjectView()->paste();
    }
}

/*===============================================================*/
void KPresenterView::editDelete()
{
    m_canvas->setToolEditMode( TEM_MOUSE );
    m_canvas->activePage()->deleteObjs();
}

/*===============================================================*/
void KPresenterView::editSelectAll()
{
    KPTextView *edit=m_canvas->currentTextObjectView();
    if ( !edit ) {
	m_canvas->setToolEditMode( TEM_MOUSE );
	m_canvas->selectAllObj();
    }
    else
        edit->selectAll();
}

/*===============================================================*/
void KPresenterView::editCopyPage()
{
    m_pKPresenterDoc->copyPageToClipboard( currPg );
}

/*===============================================================*/
void KPresenterView::editDuplicatePage()
{
    m_pKPresenterDoc->copyPage( currPg, currPg+1 );
    setRanges();
    skipToPage( currPg+1 ); // go to the new page
    updateSideBarMenu();
}

void KPresenterView::updateSideBarMenu()
{
    actionEditDelPage->setEnabled( m_pKPresenterDoc->getPageNums() > 1 );
}

/*===============================================================*/
void KPresenterView::editDelPage()
{
    if ( KMessageBox::questionYesNo( this,
                                     i18n( "Do you want to remove the current page?") )
         != KMessageBox::Yes )
        return;
    m_canvas->exitEditMode();
    m_pKPresenterDoc->deletePage( currPg );
    setRanges();
    currPg = QMIN( currPg, (int)m_pKPresenterDoc->getPageNums() - 1 );
    skipToPage( currPg );
    updateSideBarMenu();
}

/*===============================================================*/
void KPresenterView::editHeaderFooter()
{
    m_canvas->setToolEditMode( TEM_MOUSE );
    m_pKPresenterDoc->getHeaderFooterEdit()->show();
}

/*====================== insert a new page ======================*/
void KPresenterView::insertPage()
{
    InsertPageDia dia( this, 0, TRUE );
    QString templ = QDir::homeDirPath();
    templ += "/.default.kpr";
    if ( !QFile::exists( templ ) ) {
	dia.radioDifferent->setChecked( TRUE );
	dia.radioDefault->setEnabled( FALSE );
    }
    if ( dia.exec() != QDialog::Accepted )
	return;
    InsertPos pos = (InsertPos)dia.locationCombo->currentItem();
    int pg = m_pKPresenterDoc->insertNewPage( i18n("Insert new page"),currPg, pos, dia.radioDifferent->isChecked(), QString::null );
    setRanges();
    if ( pg != -1 )
	skipToPage( pg );
    updateSideBarMenu();
}

/*====================== insert a picture =======================*/
void KPresenterView::insertPicture()
{
    m_canvas->setToolEditMode( INS_PICTURE );
    m_canvas->deSelectAllObj();

    //url = KFileDialog::getImageOpenURL(); lukas: put this back in KDE 3.0

    KFileDialog fd( QString::null, KImageIO::pattern( KImageIO::Reading ), 0, 0, true );
    fd.setCaption( i18n( "Insert Picture" ) );
    fd.setPreviewWidget( new KoPictureFilePreview( &fd ) );

    KURL url;
    if ( fd.exec() == QDialog::Accepted )
        url = fd.selectedURL();

    if( url.isEmpty() || !url.isValid() )
        return;

    QString file;
    if ( !KIO::NetAccess::download( url, file ) )
        return;

    if ( !file.isEmpty() )
        m_canvas->activePage()->setInsPictureFile( file );
}

/*====================== insert a picture (w/o filedialog) =======================*/
void KPresenterView::insertPicture(const QString &file)
{
    m_canvas->setToolEditMode( INS_PICTURE );
    m_canvas->deSelectAllObj();

    if ( !file.isEmpty() )
        m_canvas->activePage()->setInsPictureFile( file );
}

/*====================== insert a clipart =======================*/
void KPresenterView::insertClipart()
{
    m_canvas->setToolEditMode( INS_CLIPART );
    m_canvas->deSelectAllObj();

    KFileDialog fd( QString::null, KoPictureFilePreview::clipartPattern(), 0, 0, true );
    fd.setCaption( i18n( "Insert Clipart" ) );
    fd.setPreviewWidget( new KoPictureFilePreview( &fd ) );

    KURL url;
    if ( fd.exec() == QDialog::Accepted )
        url = fd.selectedURL();

    if( url.isEmpty() || !url.isValid() )
        return;

    QString file;
    if ( !KIO::NetAccess::download( url, file ) )
        return;

    if ( !file.isEmpty() )
        m_canvas->activePage()->setInsClipartFile( file );
}

/*==============================================================*/
void KPresenterView::toolsMouse()
{
    if ( actionToolsMouse ->isChecked() )
        m_canvas->setToolEditMode( TEM_MOUSE, false );
    else
        actionToolsMouse->setChecked(true);
    //m_canvas->deSelectAllObj();
}

/*=========================== insert line =======================*/
void KPresenterView::toolsLine()
{
    if ( actionToolsLine->isChecked() )
    {
        m_canvas->setToolEditMode( INS_LINE, false );
        m_canvas->deSelectAllObj();
    }
    else
        actionToolsLine->setChecked(true);
}

/*===================== insert rectangle ========================*/
void KPresenterView::toolsRectangle()
{
    if ( actionToolsRectangle->isChecked() )
    {
        m_canvas->deSelectAllObj();
        m_canvas->setToolEditMode( INS_RECT, false );
    }
    else
        actionToolsRectangle->setChecked(true);
}

/*===================== insert circle or ellipse ================*/
void KPresenterView::toolsCircleOrEllipse()
{
    if ( actionToolsCircleOrEllipse->isChecked() )
    {
        m_canvas->deSelectAllObj();
        m_canvas->setToolEditMode( INS_ELLIPSE, false );
    }
    else
        actionToolsCircleOrEllipse->setChecked(true);
}

/*==============================================================*/
void KPresenterView::toolsPie()
{
    if ( actionToolsPie->isChecked() )
    {
        m_canvas->deSelectAllObj();
        m_canvas->setToolEditMode( INS_PIE, false );
    }
    else
        actionToolsPie->setChecked(true);
}

/*==============================================================*/
void KPresenterView::toolsDiagramm()
{
    if ( actionToolsDiagramm->isChecked() )
    {
        m_canvas->deSelectAllObj();
        m_canvas->setToolEditMode( INS_DIAGRAMM, false );

        KoDocumentEntry entry = KoDocumentEntry::queryByMimeType( "application/x-kchart" );
        if (entry.isEmpty())
        {
            KMessageBox::sorry( this, i18n( "Sorry, no chart component registered" ) );
            m_canvas->setToolEditMode( TEM_MOUSE );
        }
        else
            m_canvas->setPartEntry( entry );
    }
    else
        actionToolsDiagramm->setChecked(true);
}

/*==============================================================*/
void KPresenterView::toolsTable()
{
    if ( actionToolsTable->isChecked() )
    {
        m_canvas->deSelectAllObj();
        m_canvas->setToolEditMode( INS_TABLE, false );

        KoDocumentEntry entry = KoDocumentEntry::queryByMimeType( "application/x-kspread" );
        if (entry.isEmpty())
        {
            KMessageBox::sorry( this, i18n( "Sorry, no table component registered" ) );
            m_canvas->setToolEditMode( TEM_MOUSE );
        }
        else
            m_canvas->setPartEntry( entry );
    }
    else
        actionToolsTable->setChecked(true);
}

/*==============================================================*/
void KPresenterView::toolsFormula()
{
    if ( actionToolsFormula->isChecked() )
    {
        m_canvas->deSelectAllObj();
        m_canvas->setToolEditMode( INS_FORMULA, false );

        KoDocumentEntry entry = KoDocumentEntry::queryByMimeType( "application/x-kformula" );
        if (entry.isEmpty())
        {
            KMessageBox::sorry( this, i18n( "Sorry, no formula component registered" ) );
            m_canvas->setToolEditMode( TEM_MOUSE );
        }
        else
            m_canvas->setPartEntry( entry );
    }
    else
        actionToolsFormula->setChecked(true);
}

/*===================== insert a textobject =====================*/
void KPresenterView::toolsText()
{
    if ( actionToolsText->isChecked() )
    {
        m_canvas->deSelectAllObj();
        m_canvas->setToolEditMode( INS_TEXT, false );
    }
    else
        actionToolsText->setChecked(true);
}

/*===============================================================*/
void KPresenterView::toolsAutoform()
{
    if ( actionToolsAutoform->isChecked() )
    {
        m_canvas->deSelectAllObj();
        m_canvas->setToolEditMode( TEM_MOUSE, false );
        if ( afChoose ) {
            delete afChoose;
            afChoose = 0;
        }
        afChoose = new AFChoose( this, i18n( "Autoform-Choose" ) );
        afChoose->resize( 400, 300 );
        afChoose->setCaption( i18n( "KPresenter - Insert an Autoform" ) );

        QObject::connect( afChoose, SIGNAL( formChosen( const QString & ) ),
                          this, SLOT( afChooseOk( const QString & ) ) );
        QObject::connect( afChoose, SIGNAL( afchooseCanceled()),
                          this,SLOT(slotAfchooseCanceled()));
        afChoose->exec();

        QObject::disconnect( afChoose, SIGNAL( formChosen( const QString & ) ),
                             this, SLOT( afChooseOk( const QString & ) ) );
        delete afChoose;
        afChoose = 0;
    }
    else
        actionToolsAutoform->setChecked(true);
}

/*===============================================================*/
void KPresenterView::toolsObject()
{
    KoDocumentEntry pe = actionToolsObject->documentEntry();
    if ( pe.isEmpty() ) {
        m_canvas->setToolEditMode( TEM_MOUSE );
        return;
    }

    m_canvas->setToolEditMode( INS_OBJECT );
    m_canvas->setPartEntry( pe );
}

/*==================== insert freehand line =====================*/
void KPresenterView::toolsFreehand()
{
    if ( actionToolsFreehand->isChecked() ) {
        m_canvas->setToolEditMode( INS_FREEHAND, false );
        m_canvas->deSelectAllObj();
    }
    else
        actionToolsFreehand->setChecked(true);
}

/*====================== insert polyline =======================*/
void KPresenterView::toolsPolyline()
{
    if ( actionToolsPolyline->isChecked() ) {
        m_canvas->setToolEditMode( INS_POLYLINE, false );
        m_canvas->deSelectAllObj();
    }
    else
        actionToolsPolyline->setChecked(true);
}

/*================ insert quadric bezier curve =================*/
void KPresenterView::toolsQuadricBezierCurve()
{
    if ( actionToolsQuadricBezierCurve->isChecked() ) {
        m_canvas->setToolEditMode( INS_QUADRICBEZIERCURVE, false );
        m_canvas->deSelectAllObj();
    }
    else
        actionToolsQuadricBezierCurve->setChecked(true);
}

/*================= insert cubic bezier curve ==================*/
void KPresenterView::toolsCubicBezierCurve()
{
    if ( actionToolsCubicBezierCurve->isChecked() ) {
        m_canvas->setToolEditMode( INS_CUBICBEZIERCURVE, false );
        m_canvas->deSelectAllObj();
    }
    else
        actionToolsCubicBezierCurve->setChecked(true);
}

/*============== insert convex or concave polygon ===============*/
void KPresenterView::toolsConvexOrConcavePolygon()
{
    if ( actionToolsConvexOrConcavePolygon->isChecked() ) {
        m_canvas->setToolEditMode( INS_POLYGON, false );
        m_canvas->deSelectAllObj();
    }
    else
        actionToolsConvexOrConcavePolygon->setChecked(true);
}

/*===============================================================*/
void KPresenterView::extraPenBrush()
{
    if ( styleDia ) {
        delete styleDia;
        styleDia = 0;
    }

    styleDia = new StyleDia( this, "StyleDia", m_canvas->activePage()->getPenBrushFlags() );
    styleDia->setPen( m_canvas->activePage()->getPen( pen ) );
    styleDia->setBrush( m_canvas->activePage()->getBrush( brush ) );
    styleDia->setLineBegin( m_canvas->activePage()->getLineBegin( lineBegin ) );
    styleDia->setLineEnd( m_canvas->activePage()->getLineEnd( lineEnd ) );
    styleDia->setFillType( m_canvas->activePage()->getFillType( fillType ) );
    styleDia->setGradient( m_canvas->activePage()->getGColor1( gColor1 ),
			   m_canvas->activePage()->getGColor2( gColor2 ),
			   m_canvas->activePage()->getGType( gType ),
			   m_canvas->activePage()->getGUnbalanced( gUnbalanced ),
			   m_canvas->activePage()->getGXFactor( gXFactor ),
			   m_canvas->activePage()->getGYFactor( gYFactor ) );
    styleDia->setSticky( m_canvas->activePage()->getSticky( sticky ) );
    styleDia->setCaption( i18n( "KPresenter - Pen and Brush" ) );
    QObject::connect( styleDia, SIGNAL( styleOk() ), this, SLOT( styleOk() ) );
    m_canvas->setToolEditMode( TEM_MOUSE );
    styleDia->exec();

    QObject::disconnect( styleDia, SIGNAL( styleOk() ), this, SLOT( styleOk() ) );
    delete styleDia;
    styleDia = 0;
}

/*===============================================================*/
void KPresenterView::extraConfigPie()
{
    if ( confPieDia ) {
        delete confPieDia;
        confPieDia = 0;
    }

    confPieDia = new ConfPieDia( this, "ConfPageDia" );
    confPieDia->setMaximumSize( confPieDia->width(), confPieDia->height() );
    confPieDia->setMinimumSize( confPieDia->width(), confPieDia->height() );
    confPieDia->setType( m_canvas->activePage()->getPieType( pieType ) );
    confPieDia->setAngle( m_canvas->activePage()->getPieAngle( pieAngle ) );
    confPieDia->setLength( m_canvas->activePage()->getPieLength( pieLength ) );
    confPieDia->setPenBrush( m_canvas->activePage()->getPen( pen ), m_canvas->activePage()->getBrush( brush ) );
    confPieDia->setCaption( i18n( "KPresenter - Configure Pie/Arc/Chord" ) );
    QObject::connect( confPieDia, SIGNAL( confPieDiaOk() ), this, SLOT( confPieOk() ) );
    m_canvas->setToolEditMode( TEM_MOUSE );
    confPieDia->exec();

    QObject::disconnect( confPieDia, SIGNAL( confPieDiaOk() ), this, SLOT( confPieOk() ) );
    delete confPieDia;
    confPieDia = 0;
}

/*===============================================================*/
void KPresenterView::extraConfigRect()
{
    if ( confRectDia ) {
        delete confRectDia;
        confRectDia = 0;
    }

    confRectDia = new ConfRectDia( this, "ConfRectDia" );
    confRectDia->setMaximumSize( confRectDia->width(), confRectDia->height() );
    confRectDia->setMinimumSize( confRectDia->width(), confRectDia->height() );
    confRectDia->setRnds( m_canvas->activePage()->getRndX( rndX ), m_canvas->activePage()->getRndY( rndY ) );
    confRectDia->setCaption( i18n( "KPresenter - Configure Rectangle" ) );
    QObject::connect( confRectDia, SIGNAL( confRectDiaOk() ), this, SLOT( confRectOk() ) );
    m_canvas->setToolEditMode( TEM_MOUSE );
    confRectDia->exec();

    QObject::disconnect( confRectDia, SIGNAL( confRectDiaOk() ), this, SLOT( confRectOk() ) );
    delete confRectDia;
    confRectDia = 0;
}

/*===============================================================*/
void KPresenterView::extraConfigPolygon()
{
    bool _checkConcavePolygon;
    int _cornersValue;
    int _sharpnessValue;

    if ( !m_canvas->activePage()->getPolygonSettings( &_checkConcavePolygon, &_cornersValue, &_sharpnessValue ) ) {
        _checkConcavePolygon = checkConcavePolygon;
        _cornersValue = cornersValue;
        _sharpnessValue = sharpnessValue;
    }

    if ( confPolygonDia ) {
        delete confPolygonDia;
        confPolygonDia = 0;
    }

    confPolygonDia = new ConfPolygonDia( this, "ConfPolygonDia", _checkConcavePolygon, _cornersValue, _sharpnessValue );
    confPolygonDia->setMaximumSize( confPolygonDia->width(), confPolygonDia->height() );
    confPolygonDia->setMinimumSize( confPolygonDia->width(), confPolygonDia->height() );
    confPolygonDia->setCaption( i18n( "KPresenter - Configure Polygon" ) );

    QObject::connect( confPolygonDia, SIGNAL( confPolygonDiaOk() ), this, SLOT( confPolygonOk() ) );

    m_canvas->setToolEditMode( TEM_MOUSE );

    confPolygonDia->exec();

    QObject::disconnect( confPolygonDia, SIGNAL( confPolygonDiaOk() ), this, SLOT( confPolygonOk() ) );
    delete confPolygonDia;
    confPolygonDia = 0;
}

/*===============================================================*/
void KPresenterView::extraRaise()
{
    m_canvas->setToolEditMode( TEM_MOUSE );
    m_canvas->activePage()->raiseObjs();
}

/*===============================================================*/
void KPresenterView::extraLower()
{
    m_canvas->setToolEditMode( TEM_MOUSE );
    m_canvas->activePage()->lowerObjs();
}

/*===============================================================*/
void KPresenterView::extraRotate()
{
    if ( rotateDia ) {
	delete rotateDia;
	rotateDia = 0;
    }

    if ( m_canvas->activePage()->numSelected() > 0 ) {
	rotateDia = new RotateDia( this, "Rotate" );
	rotateDia->setMaximumSize( rotateDia->width(), rotateDia->height() );
	rotateDia->setMinimumSize( rotateDia->width(), rotateDia->height() );
	rotateDia->setCaption( i18n( "KPresenter - Rotate" ) );
	QObject::connect( rotateDia, SIGNAL( rotateDiaOk() ), this, SLOT( rotateOk() ) );
	rotateDia->setAngle( m_canvas->activePage()->getSelectedObj()->getAngle() );
	m_canvas->setToolEditMode( TEM_MOUSE );
	rotateDia->exec();

        QObject::disconnect( rotateDia, SIGNAL( rotateDiaOk() ), this, SLOT( rotateOk() ) );
        delete rotateDia;
        rotateDia = 0;
    }
}

/*===============================================================*/
void KPresenterView::extraShadow()
{
    if ( shadowDia ) {
	delete shadowDia;
	shadowDia = 0;
    }

    if ( m_canvas->activePage()->numSelected() > 0 ) {
	shadowDia = new ShadowDia( this, "Shadow" );
	shadowDia->setMaximumSize( shadowDia->width(), shadowDia->height() );
	shadowDia->setMinimumSize( shadowDia->width(), shadowDia->height() );
	shadowDia->setCaption( i18n( "KPresenter - Shadow" ) );
	QObject::connect( shadowDia, SIGNAL( shadowDiaOk() ), this, SLOT( shadowOk() ) );
        KPObject *object=m_canvas->activePage()->getSelectedObj();
	shadowDia->setShadowDirection( object->getShadowDirection() );
	shadowDia->setShadowDistance( object->getShadowDistance() );
	shadowDia->setShadowColor( object->getShadowColor() );
	m_canvas->setToolEditMode( TEM_MOUSE );
	shadowDia->exec();

        QObject::disconnect( shadowDia, SIGNAL( shadowDiaOk() ), this, SLOT( shadowOk() ) );
        delete shadowDia;
	shadowDia = 0;
    }
}

/*===============================================================*/
void KPresenterView::extraAlignObjs()
{
    m_canvas->setToolEditMode( TEM_MOUSE );
    QPoint pnt( QCursor::pos() );
    rb_oalign->popup( pnt );
}

/*===============================================================*/
void KPresenterView::extraBackground()
{
    if ( backDia ) {
	delete backDia;
	backDia = 0;
    }
    backDia = new BackDia( this, "InfoDia", m_canvas->activePage()->getBackType(  ),
			   m_canvas->activePage()->getBackColor1(  ),
			   m_canvas->activePage()->getBackColor2(  ),
			   m_canvas->activePage()->getBackColorType(  ),
			   m_canvas->activePage()->getBackPixKey(  ).filename(),
                           m_canvas->activePage()->getBackPixKey(  ).lastModified(),
			   m_canvas->activePage()->getBackClipKey().filename(),
			   m_canvas->activePage()->getBackClipKey().lastModified(),
			   m_canvas->activePage()->getBackView(),
			   m_canvas->activePage()->getBackUnbalanced(),
			   m_canvas->activePage()->getBackXFactor(),
			   m_canvas->activePage()->getBackYFactor( ),
			   m_canvas->activePage() );
    backDia->setCaption( i18n( "KPresenter - Page Background" ) );
    QObject::connect( backDia, SIGNAL( backOk( bool ) ), this, SLOT( backOk( bool ) ) );
    backDia->exec();

    QObject::disconnect( backDia, SIGNAL( backOk( bool ) ), this, SLOT( backOk( bool ) ) );
    delete backDia;
    backDia = 0;
}

/*===============================================================*/
void KPresenterView::extraLayout()
{
    KoPageLayout pgLayout = m_pKPresenterDoc->pageLayout();
    KoPageLayout oldLayout = pgLayout;
    KoHeadFoot hf;
    KoUnit::Unit oldUnit = m_pKPresenterDoc->getUnit();
    KoUnit::Unit unit = oldUnit;

    if ( KoPageLayoutDia::pageLayout( pgLayout, hf, FORMAT_AND_BORDERS, unit ) ) {
	PgLayoutCmd *pgLayoutCmd = new PgLayoutCmd( i18n( "Set Page Layout" ),
						    pgLayout, oldLayout, unit, oldUnit, this );
	pgLayoutCmd->execute();
	kPresenterDoc()->addCommand( pgLayoutCmd );
        updateRuler();
    }
}

/*===============================================================*/
void KPresenterView::extraConfigure()
{
    KPConfig configDia( this );
    configDia.exec();
}
/*===============================================================*/
void KPresenterView::extraCreateTemplate()
{
    QRect r=m_pKPresenterDoc->pageList().at(0)->getZoomPageRect();
    QPixmap pix( QSize( r.width(),
			r.height() ) );
    pix.fill( Qt::white );
    int i = getCurrPgNum() - 1;
    //todo fix me
    m_canvas->drawPageInPix2( pix, i );

    QWMatrix m;
    m.scale( 60.0 / (float)pix.width(), 45.0 / (float)pix.height() );
    pix = pix.xForm( m );

    KTempFile tempFile( QString::null, ".kpt" );
    tempFile.setAutoDelete( true );
    m_pKPresenterDoc->savePage( tempFile.name(), i );

    KoTemplateCreateDia::createTemplate( "kpresenter_template", KPresenterFactory::global(),
					 tempFile.name(), pix, this);
    KPresenterFactory::global()->dirs()->addResourceType("kpresenter_template",
							 KStandardDirs::kde_default( "data" ) +
							 "kpresenter/templates/");
}

void KPresenterView::extraDefaultTemplate()
{
    QString file = QDir::homeDirPath();
    file += "/.default.kpr";\
    m_pKPresenterDoc->savePage( file, currPg );
}

/*===============================================================*/
void KPresenterView::extraWebPres()
{
    if ( !allowWebPres )
	return;

    KURL url;
    QString config = QString::null;
    if ( KMessageBox::questionYesNo( this,
	   i18n( "Do you want to load a previously saved configuration"
		 " which will be used for this HTML Presentation?" ),
	   i18n( "Create HTML Presentation" ) ) == KMessageBox::Yes )
    {
	url = KFileDialog::getOpenURL( QString::null, i18n("*.kpweb|KPresenter HTML Presentation (*.kpweb)") );

	if( url.isEmpty() )
	  return;

	if( !url.isLocalFile() )
	{
	  KMessageBox::sorry( this, i18n( "Only local files are currently supported." ) );
	  return;
	}

	config = url.path();
    }

    KPWebPresentationWizard::createWebPresentation( config, m_pKPresenterDoc, this );
}

/*===============================================================*/
void KPresenterView::extraLineBegin()
{
    m_canvas->setToolEditMode( TEM_MOUSE );
    QPoint pnt( QCursor::pos() );
    rb_lbegin->popup( pnt );
}

/*===============================================================*/
void KPresenterView::extraLineEnd()
{
    m_canvas->setToolEditMode( TEM_MOUSE );
    QPoint pnt( QCursor::pos() );
    rb_lend->popup( pnt );
}

/*===============================================================*/
void KPresenterView::extraGroup()
{
    m_canvas->setToolEditMode( TEM_MOUSE );
    m_canvas->activePage()->groupObjects();
    objectSelectedChanged();
}

/*===============================================================*/
void KPresenterView::extraUnGroup()
{
    m_canvas->setToolEditMode( TEM_MOUSE );
    m_canvas->activePage()->ungroupObjects();
    objectSelectedChanged();
}

/*===============================================================*/
void KPresenterView::extraPenStyle()
{
    m_canvas->setToolEditMode( TEM_MOUSE );
    QPoint pnt( QCursor::pos() );
    rb_pstyle->popup( pnt );
}

/*===============================================================*/
void KPresenterView::extraPenWidth()
{
    m_canvas->setToolEditMode( TEM_MOUSE );
    QPoint pnt( QCursor::pos() );
    rb_pwidth->popup( pnt );
}


/*========================== screen config pages ================*/
void KPresenterView::screenConfigPages()
{
    if ( pgConfDia ) {
        delete pgConfDia;
        pgConfDia = 0;
    }

    pgConfDia = new PgConfDia( this, "PageConfig", kPresenterDoc()->spInfinitLoop(),
			       kPresenterDoc()->spManualSwitch(), getCurrPgNum(),
			       m_canvas->activePage()->getPageEffect(),
			       kPresenterDoc()->getPresSpeed(),
			       m_canvas->activePage()->getPageTimer(),
			       m_canvas->activePage()->getPageSoundEffect(),
			       m_canvas->activePage()->getPageSoundFileName() );
    pgConfDia->setCaption( i18n( "KPresenter - Page Configuration for Screen Presentations" ) );
    QObject::connect( pgConfDia, SIGNAL( pgConfDiaOk() ), this, SLOT( pgConfOk() ) );
    pgConfDia->exec();

    QObject::disconnect( pgConfDia, SIGNAL( pgConfDiaOk() ), this, SLOT( pgConfOk() ) );
    delete pgConfDia;
    pgConfDia = 0;
}

/*========================== screen presStructView  =============*/
void KPresenterView::screenPresStructView()
{
    if ( presStructView ) {
        delete presStructView;
        presStructView = 0;
    }
    m_canvas->setToolEditMode( TEM_MOUSE );
    m_canvas->deSelectAllObj();

    presStructView = new KPPresStructView( this, "", kPresenterDoc(), this );
    presStructView->setCaption( i18n( "KPresenter - Presentation Structure Viewer" ) );
    QObject::connect( presStructView, SIGNAL( presStructViewClosed() ), this, SLOT( psvClosed() ) );
    presStructView->exec();
    delete presStructView;
    presStructView = 0;
}

/*===============================================================*/
void KPresenterView::screenAssignEffect()
{
    m_canvas->setToolEditMode( TEM_MOUSE );

    QPtrList<KPObject> objs;
    if ( m_canvas->canAssignEffect( objs ) ) {
        EffectDia *effectDia = new EffectDia( this, "Effect", objs, this );
	effectDia->setCaption( i18n( "KPresenter - Assign Effects" ) );
	if(effectDia->exec())
            effectOk();
        delete effectDia;
    }
}

/*========================== screen start =======================*/
void KPresenterView::screenStart()
{
    startScreenPres( -1 ); // all selected pages
}

void KPresenterView::screenViewPage()
{
    startScreenPres( getCurrPgNum() ); // current page only
}

void KPresenterView::startScreenPres( int pgNum /*1-based*/ )
{
    m_canvas->setToolEditMode( TEM_MOUSE );

    if ( m_canvas && !presStarted ) {
        QByteArray data;
        QByteArray replyData;
	QCString replyType;
	m_screenSaverWasEnabled = false;
        // is screensaver enabled?
        if (kapp->dcopClient()->call("kdesktop", "KScreensaverIface", "isEnabled()", data, replyType, replyData)
	    && replyType=="bool")
	{
            QDataStream replyArg(replyData, IO_ReadOnly);
	    replyArg >> m_screenSaverWasEnabled;
	    if ( m_screenSaverWasEnabled )
	    {
                // disable screensaver
                QDataStream arg(data, IO_WriteOnly);
                arg << false;
                if (!kapp->dcopClient()->send("kdesktop", "KScreensaverIface", "enable(bool)", data))
                    kdWarning(33001) << "Couldn't disable screensaver (using dcop to kdesktop)!" << endl;
		else
                    kdDebug(33001) << "Screensaver successfully disabled" << endl;
	    }
	}

        m_canvas->deSelectAllObj();
        presStarted = true;
        int deskw = QApplication::desktop()->width();
        int deskh = QApplication::desktop()->height();
        QRect pgRect = kPresenterDoc()->pageList().at(0)->getZoomPageRect();

        float _presFaktW = static_cast<float>( deskw ) / static_cast<float>( pgRect.width() );
        float _presFaktH = static_cast<float>( deskh ) / static_cast<float>( pgRect.height() );
        float _presFakt = QMIN(_presFaktW,_presFaktH);
        kdDebug(33001) << "KPresenterView::startScreenPres page->setPresFakt " << _presFakt << endl;

        //xOffsetSaved = xOffset;
        //yOffsetSaved = yOffset;
        //xOffset = 0;
        //yOffset = 0;

        // Center the slide in the screen, if it's smaller...
        pgRect = kPresenterDoc()->pageList().at(0)->getZoomPageRect();
        kdDebug(33001) << "                                pgRect: " << pgRect.x() << "," << pgRect.y()
                  << " " << pgRect.width() << "x" << pgRect.height() << endl;
        /*if ( deskw > pgRect.width() )
            xOffset -= ( deskw - pgRect.width() ) / 2;
        if ( deskh > pgRect.height() )
            yOffset -= ( deskh - pgRect.height() ) / 2;*/

        vert->setEnabled( false );
        horz->setEnabled( false );
        m_bShowGUI = false;
        m_canvas->reparent( ( QWidget* )0L, 0, QPoint( 0, 0 ), FALSE );
        m_canvas->showFullScreen();
        m_canvas->setFocusPolicy( QWidget::StrongFocus );
        m_canvas->startScreenPresentation( _presFakt, pgNum );

        actionScreenStart->setEnabled( false );
        actionScreenViewPage->setEnabled( false );

        if ( !kPresenterDoc()->spManualSwitch() && pgNum == -1 ) {
            continuePres = true;
            exitPres = false;
            m_canvas->repaint( false );

            if ( automaticScreenPresFirstTimer ) {
                connect( &automaticScreenPresTimer, SIGNAL( timeout() ), SLOT( doAutomaticScreenPres() ) );
                automaticScreenPresTime.start();
                automaticScreenPresWaitTime = 0;
                setCurrentTimer( 1 );
                automaticScreenPresTimer.start( currentTimer );
                automaticScreenPresFirstTimer = false;
            }
            else
                autoScreenPresReStartTimer();
        }
    }
}

/*========================== screen stop ========================*/
void KPresenterView::screenStop()
{
    if ( presStarted ) {
        continuePres = false;
        exitPres = true;
        m_canvas->setNextPageTimer( true );
        m_canvas->stopSound();
        m_canvas->showNormal();
        m_canvas->hide();
        m_canvas->reparent( pageBase, 0, QPoint( 0, 0 ), true );
        m_canvas->lower();
        //xOffset = xOffsetSaved;
        //yOffset = yOffsetSaved;
        m_canvas->stopScreenPresentation();
        presStarted = false;
        vert->setEnabled( true );
        horz->setEnabled( true );
        m_bShowGUI = true;
        m_canvas->setMouseTracking( true );
        m_canvas->setBackgroundColor( white );

	if ( m_screenSaverWasEnabled )
	{
            // start screensaver again
            QByteArray data;
            QDataStream arg(data, IO_WriteOnly);
            arg << true;
            if (!kapp->dcopClient()->send("kdesktop", "KScreensaverIface", "enable(bool)", data))
                kdWarning(33001) << "Couldn't re-enabled screensaver (using dcop to kdesktop)" << endl;
	}

        actionScreenStart->setEnabled( true );
        actionScreenViewPage->setEnabled( true );
        pageBase->resizeEvent( 0 );
    }
}

/*========================== screen pause =======================*/
void KPresenterView::screenPause()
{
}

/*========================== screen first =======================*/
void KPresenterView::screenFirst()
{
    if ( m_canvas->currentTextObjectView() )
#if 0
	m_canvas->currentTextObjectView()->home();
#else
    ;
#endif
    else {
	if ( !presStarted ) {
            skipToPage( 0 );
	} else {
	    gotoPresPage( 1 );
	}
    }
}

/*========================== screen previous =====================*/
void KPresenterView::screenPrev()
{
    if ( m_canvas->currentTextObjectView() )
        return;

    if ( presStarted ) {
        if ( !kPresenterDoc()->spManualSwitch() ) {
            setCurrentTimer( 1 );
            m_canvas->setNextPageTimer( true );
        }
	if ( m_canvas->pPrev( true ) ) {
#if 0 // TODO currentPage-- instead
            QRect pgRect = kPresenterDoc()->getPageRect( 0, 0, 0, m_canvas->presFakt(), false );
	    yOffset = ( m_canvas->presPage() - 1 ) * pgRect.height();
	    if ( m_canvas->height() > pgRect.height() )
		yOffset -= ( m_canvas->height() - pgRect.height() ) / 2;
#endif
	    m_canvas->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    m_canvas->repaint( false );
	    m_canvas->setFocus();
	} else {
	    m_canvas->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    m_canvas->setFocus();
	}
    } else {
	prevPage();
    }
}

/*========================== screen next ========================*/
void KPresenterView::screenNext()
{
    if ( m_canvas->currentTextObjectView() )
        return;
    if ( presStarted ) {
	if ( m_canvas->pNext( true ) ) {
#if 0 // TODO currentPage-- instead
            QRect pgRect = kPresenterDoc()->getPageRect( 0, 0, 0, m_canvas->presFakt(), false );
	    yOffset = ( m_canvas->presPage() - 1 ) * pgRect.height();
	    if ( m_canvas->height() > pgRect.height() )
		yOffset -= ( m_canvas->height() - pgRect.height() ) / 2;
#endif
	    m_canvas->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    m_canvas->setFocus();

            if ( !kPresenterDoc()->spManualSwitch() ) {
                setCurrentTimer( 1 );
                m_canvas->setNextPageTimer( true );
            }
	} else {
	    m_canvas->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    m_canvas->setFocus();
	}
    } else {
	nextPage();
    }
}

/*========================== screen last ========================*/
void KPresenterView::screenLast()
{
    if ( m_canvas->currentTextObjectView() )
#if 0
	m_canvas->currentTextObjectView()->end();
#else
    ;
#endif
    else {
	if ( !presStarted )
        {
	    skipToPage( m_pKPresenterDoc->getPageNums() - 1 );
	}
        else
        {

	    gotoPresPage( getNumPresPages() );
	}
    }
}

/*========================== screen skip =======================*/
void KPresenterView::screenSkip()
{
}

/*===============================================================*/
void KPresenterView::sizeSelected()
{
    tbFont.setPointSize( actionTextFontSize ->fontSize() );
    m_canvas->setTextPointSize( tbFont.pointSize() );
    m_canvas->setFocus();
    kdDebug(33001) << "sizeSelected() " << tbFont.pointSize() << endl;
}

/*===============================================================*/
void KPresenterView::fontSelected()
{
    tbFont.setFamily( actionTextFontFamily->currentText() );
    m_canvas->setTextFamily( tbFont.family() );
    m_canvas->setFocus();
    kdDebug(33001) << "fontSelected() " << tbFont.family() << endl;
}

void KPresenterView::textBold()
{
    bool b=actionTextBold->isChecked();
    tbFont.setBold( b );
    m_canvas->setTextBold(b );
}


void KPresenterView::textStrikeOut()
{
    bool b=actionFormatStrikeOut->isChecked();
    tbFont.setUnderline( b );
    m_canvas->setTextStrikeOut( b );
}

void KPresenterView::textItalic()
{
    bool b=actionTextItalic->isChecked();
    tbFont.setItalic( b );
    m_canvas->setTextItalic( b );
}

void KPresenterView::textUnderline()
{
    bool b=actionTextUnderline->isChecked();
    tbFont.setUnderline(b );
    m_canvas->setTextUnderline( b );
}

void KPresenterView::textColor()
{
    tbColor = actionTextColor->color();
    m_canvas->setTextColor( tbColor );
}

void KPresenterView::textAlignLeft()
{
    if ( actionTextAlignLeft->isChecked() )
    {
        tbAlign = Qt::AlignLeft;
        m_canvas->setTextAlign( tbAlign );
    }
    else
        actionTextAlignLeft->setChecked(true);
}

void KPresenterView::textAlignCenter()
{
    if ( actionTextAlignCenter->isChecked() )
    {
        tbAlign = Qt::AlignCenter;
        m_canvas->setTextAlign( Qt::AlignCenter );
    }
    else
         actionTextAlignCenter->setChecked(true);
}

void KPresenterView::textAlignRight()
{
    if ( actionTextAlignRight->isChecked() )
    {
        tbAlign = Qt::AlignRight;
        m_canvas->setTextAlign( Qt::AlignRight );
    }
    else
        actionTextAlignRight->setChecked(true);

}

void KPresenterView::textAlignBlock()
{
    if ( actionTextAlignBlock->isChecked() )
    {
        tbAlign = Qt::AlignJustify;
        m_canvas->setTextAlign(Qt::AlignJustify);
    }
    else
        actionTextAlignBlock->setChecked(true);

}

void KPresenterView::textInsertPageNum()
{
#if 0 // note: also the action is disabled now (Werner)
    if ( m_canvas->currentTextObjectView() )
	m_canvas->currentTextObjectView()->insertPageNum();
#endif
}

void KPresenterView::mtextFont()
{
    KoTextFormatInterface* textIface = m_canvas->applicableTextInterfaces().first();

    QColor col;
    if (textIface)
        col = textIface->textBackgroundColor();
    col = col.isValid() ? col : QApplication::palette().color( QPalette::Active, QColorGroup::Base );
    KoFontDia *fontDia = new KoFontDia( this, "", textIface->textFont(),
                                        actionFormatSub->isChecked(), actionFormatSuper->isChecked(),
                                        textIface->textColor(), col );
    fontDia->exec();
    int flags = fontDia->changedFlags();
    kdDebug() << "changedFlags = " << flags << endl;
    if ( flags )
    {
        // The "change all the format" call
        m_canvas->setFont(fontDia->getNewFont(),
                      fontDia->getSubScript(), fontDia->getSuperScript(),
                      fontDia->color(), fontDia->backGroundColor(),
                      flags);
    }

    delete fontDia;
}

/*===============================================================*/
void KPresenterView::textEnumList()
{
    KoParagCounter c;
    if ( actionTextTypeEnumList->isChecked() )
    {
        if(actionTextTypeUnsortList->isChecked())
            actionTextTypeUnsortList->setChecked(false);
        c.setNumbering( KoParagCounter::NUM_LIST );
        c.setStyle( KoParagCounter::STYLE_NUM );
    }
    else
    {
        c.setNumbering( KoParagCounter::NUM_NONE );
    }
    m_canvas->setTextCounter(c);
}

/*===============================================================*/
void KPresenterView::textUnsortList()
{
    KoParagCounter c;
    c.setPrefix( QString::null );
    c.setSuffix( QString::null );
    if ( actionTextTypeUnsortList->isChecked() )
    {
        if(actionTextTypeEnumList->isChecked())
            actionTextTypeEnumList->setChecked(false);
        c.setNumbering( KoParagCounter::NUM_LIST );
        c.setStyle( KoParagCounter::STYLE_DISCBULLET );
    }
    else
        c.setNumbering( KoParagCounter::NUM_NONE );
    m_canvas->setTextCounter(c);
}

/*===============================================================*/
void KPresenterView::textDepthPlus()
{
    m_canvas->setTextDepthPlus();
}

/*===============================================================*/
void KPresenterView::textDepthMinus()
{
    m_canvas->setTextDepthMinus();
}

/*===============================================================*/
void KPresenterView::textSettings()
{
#if 0
    KPTextObject *txtObj = m_canvas->currentTextObjectView()->kpTextObject();
    if ( !txtObj )
	txtObj = m_canvas->selectedTextObj();
    if ( txtObj ) {
	TextDialog dlg( this, 0, TRUE );
/*	dlg.comboBullet1->setCurrentItem( (int)txtObj->document()->textSettings().bulletType[0] );
	dlg.comboBullet2->setCurrentItem( (int)txtObj->document()->textSettings().bulletType[1] );
	dlg.comboBullet3->setCurrentItem( (int)txtObj->document()->textSettings().bulletType[2] );
	dlg.comboBullet4->setCurrentItem( (int)txtObj->document()->textSettings().bulletType[3] );
	dlg.colorBullet1->setColor( txtObj->document()->textSettings().bulletColor[0] );
	dlg.colorBullet2->setColor( txtObj->document()->textSettings().bulletColor[1] );
	dlg.colorBullet3->setColor( txtObj->document()->textSettings().bulletColor[2] );
	dlg.colorBullet4->setColor( txtObj->document()->textSettings().bulletColor[3] ); */
	dlg.spinLineSpacing->setValue( txtObj->document()->textSettings().lineSpacing );
	dlg.spinParagSpacing->setValue( txtObj->document()->textSettings().paragSpacing );
	dlg.spinMargin->setValue( txtObj->document()->textSettings().margin );
	if ( dlg.exec() == QDialog::Accepted ) {
	    KTextEditDocument::TextSettings s;
	    /*s.bulletType[0] = (KTextEditDocument::Bullet)dlg.comboBullet1->currentItem();
	    s.bulletType[1] = (KTextEditDocument::Bullet)dlg.comboBullet2->currentItem();
	    s.bulletType[2] = (KTextEditDocument::Bullet)dlg.comboBullet3->currentItem();
	    s.bulletType[3] = (KTextEditDocument::Bullet)dlg.comboBullet4->currentItem();
	    s.bulletColor[0] = dlg.colorBullet1->color();
	    s.bulletColor[1] = dlg.colorBullet2->color();
	    s.bulletColor[2] = dlg.colorBullet3->color();
	    s.bulletColor[3] = dlg.colorBullet4->color();*/
	    s.lineSpacing = dlg.spinLineSpacing->value();
	    s.paragSpacing = dlg.spinParagSpacing->value();
	    s.margin = dlg.spinMargin->value();
	    txtObj->document()->setTextSettings( s );
	    if ( !m_canvas->currentTextObjectView() )
		m_canvas->repaint( false );
	    else
		txtObj->repaint( FALSE );
	}
    }
#endif
}

/*===============================================================*/
void KPresenterView::textContentsToHeight()
{
#if 0
    KPTextObject *txtObj = m_canvas->currentTextObjectView();
    if ( !txtObj )
	txtObj = m_canvas->selectedTextObj();

    if ( txtObj )
	txtObj->extendContents2Height();

    if ( m_canvas->selectedTextObj() )
	m_pKPresenterDoc->repaint( false );
    else if ( txtObj )
	txtObj->repaint( FALSE );
#endif
}

/*===============================================================*/
void KPresenterView::textObjectToContents()
{
#if 0
    KPTextObject *txtObj = m_canvas->currentTextObjectView();
    if ( !txtObj )
	txtObj = m_canvas->selectedTextObj();

    if ( txtObj )
	txtObj->extendObject2Contents( this );

    if ( m_canvas->selectedKPTextObj() )
	m_pKPresenterDoc->repaint( false );
    else if ( txtObj )
    {
	txtObj->textObject()->repaint( FALSE );
    }
#endif
}

/*===============================================================*/
void KPresenterView::penChosen()
{
    QColor c = actionPenColor->color();
    if ( !m_canvas->currentTextObjectView() ) {
	bool fill = true;

	if ( !m_canvas->activePage()->setPenColor( c, fill ) ) {
	    if ( fill )
		pen.setColor( c );
	    else
		pen = NoPen;
	}
    } else {
	tbColor = c;
	m_canvas->setTextColor( tbColor );
    }
}

/*===============================================================*/
void KPresenterView::brushChosen()
{
    QColor c = actionBrushColor->color();
    KPTextView *edit=m_canvas->currentTextObjectView();
    if ( !edit )
    {
	bool fill = true;

	if ( !m_canvas->activePage()->setBrushColor( c, fill ) )
        {
	    if ( fill )
		brush.setColor( c );
	    else
		brush = NoBrush;
	}
    }
    else
    {
	tbColor = c;
        m_canvas->setTextBackgroundColor(c);
    }
}

/*======================= align object left =====================*/
void KPresenterView::extraAlignObjLeft()
{
    m_canvas->activePage()->alignObjsLeft();
}

/*======================= align object center h =================*/
void KPresenterView::extraAlignObjCenterH()
{
    m_canvas->activePage()->alignObjsCenterH();
}

/*======================= align object right ====================*/
void KPresenterView::extraAlignObjRight()
{
    m_canvas->activePage()->alignObjsRight();
}

/*======================= align object top ======================*/
void KPresenterView::extraAlignObjTop()
{
    m_canvas->activePage()->alignObjsTop();
}

/*======================= align object center v =================*/
void KPresenterView::extraAlignObjCenterV()
{
    m_canvas->activePage()->alignObjsCenterV();
}

/*======================= align object bottom ===================*/
void KPresenterView::extraAlignObjBottom()
{
    m_canvas->activePage()->alignObjsBottom();
}

/*===============================================================*/
void KPresenterView::extraLineBeginNormal()
{
    if ( !m_canvas->activePage()->setLineBegin( L_NORMAL ) )
	lineBegin = L_NORMAL;
}

/*===============================================================*/
void KPresenterView::extraLineBeginArrow()
{
    if ( !m_canvas->activePage()->setLineBegin( L_ARROW ) )
	lineBegin = L_ARROW;
}

/*===============================================================*/
void KPresenterView::extraLineBeginRect()
{
    if ( !m_canvas->activePage()->setLineBegin( L_SQUARE ) )
	lineBegin = L_SQUARE;
}

/*===============================================================*/
void KPresenterView::extraLineBeginCircle()
{
    if ( !m_canvas->activePage()->setLineBegin( L_CIRCLE ) )
	lineBegin = L_CIRCLE;
}

/*===============================================================*/
void KPresenterView::extraLineEndNormal()
{
    if ( !m_canvas->activePage()->setLineEnd( L_NORMAL ) )
	lineEnd = L_NORMAL;
}

/*===============================================================*/
void KPresenterView::extraLineEndArrow()
{
    if ( !m_canvas->activePage()->setLineEnd( L_ARROW ) )
	lineEnd = L_ARROW;
}

/*===============================================================*/
void KPresenterView::extraLineEndRect()
{
    if ( !m_canvas->activePage()->setLineEnd( L_SQUARE ) )
	lineEnd = L_SQUARE;
}

/*===============================================================*/
void KPresenterView::extraLineEndCircle()
{
    if ( !m_canvas->activePage()->setLineEnd( L_CIRCLE ) )
	lineEnd = L_CIRCLE;
}

/*===============================================================*/
void KPresenterView::extraPenStyleSolid()
{
    setExtraPenStyle( Qt::SolidLine );
}

/*===============================================================*/
void KPresenterView::extraPenStyleDash()
{
    setExtraPenStyle( Qt::DashLine );
}

/*===============================================================*/
void KPresenterView::extraPenStyleDot()
{
    setExtraPenStyle( Qt::DotLine );
}

/*===============================================================*/
void KPresenterView::extraPenStyleDashDot()
{
    setExtraPenStyle( Qt::DashDotLine );
}

/*===============================================================*/
void KPresenterView::extraPenStyleDashDotDot()
{
    setExtraPenStyle( Qt::DashDotDotLine );
}

/*===============================================================*/
void KPresenterView::extraPenStyleNoPen()
{
    setExtraPenStyle( Qt::NoPen );
}

/*===============================================================*/
void KPresenterView::setExtraPenStyle( Qt::PenStyle style )
{
    KPrPage *doc = m_canvas->activePage();
    QPen e_pen = QPen( (doc->getPen( pen )).color(), (doc->getPen( pen )).width(), style );
    doc->setPenBrush( e_pen,
                      doc->getBrush( brush ), doc->getLineBegin( lineBegin ),
                      doc->getLineEnd( lineEnd ), doc->getFillType( fillType ),
                      doc->getGColor1( gColor1 ),
                      doc->getGColor2( gColor2 ), doc->getGType( gType ),
                      doc->getGUnbalanced( gUnbalanced ),
                      doc->getGXFactor( gXFactor ), doc->getGYFactor( gYFactor ),
                      doc->getSticky( sticky ) );
}

/*===============================================================*/
void KPresenterView::extraPenWidth1()
{
    setExtraPenWidth( 1 );
}

/*===============================================================*/
void KPresenterView::extraPenWidth2()
{
    setExtraPenWidth( 2 );
}

/*===============================================================*/
void KPresenterView::extraPenWidth3()
{
    setExtraPenWidth( 3 );
}

/*===============================================================*/
void KPresenterView::extraPenWidth4()
{
    setExtraPenWidth( 4 );
}

/*===============================================================*/
void KPresenterView::extraPenWidth5()
{
    setExtraPenWidth( 5 );
}

/*===============================================================*/
void KPresenterView::extraPenWidth6()
{
    setExtraPenWidth( 6 );
}

/*===============================================================*/
void KPresenterView::extraPenWidth7()
{
    setExtraPenWidth( 7 );
}

/*===============================================================*/
void KPresenterView::extraPenWidth8()
{
    setExtraPenWidth( 8 );
}

/*===============================================================*/
void KPresenterView::extraPenWidth9()
{
    setExtraPenWidth( 9 );
}

/*===============================================================*/
void KPresenterView::extraPenWidth10()
{
    setExtraPenWidth( 10 );
}

/*===============================================================*/
void KPresenterView::setExtraPenWidth( unsigned int width )
{
    KPrPage *page=m_canvas->activePage();
    QPen e_pen = QPen( (page->getPen( pen )).color(), width,
                       (page->getPen( pen )).style() );
    page->setPenBrush( e_pen,
                      page->getBrush( brush ), page->getLineBegin( lineBegin ),
                      page->getLineEnd( lineEnd ), page->getFillType( fillType ),
                      page->getGColor1( gColor1 ),
                      page->getGColor2( gColor2 ), page->getGType( gType ),
                      page->getGUnbalanced( gUnbalanced ),
                      page->getGXFactor( gXFactor ), page->getGYFactor( gYFactor ),
                      page->getSticky( sticky ) );
}

/*===============================================================*/
void KPresenterView::newPageLayout( KoPageLayout _layout )
{
    KoPageLayout oldLayout = m_pKPresenterDoc->pageLayout();
    KoUnit::Unit unit = m_pKPresenterDoc->getUnit(); // unchanged

    PgLayoutCmd *pgLayoutCmd = new PgLayoutCmd( i18n( "Set Page Layout" ), _layout, oldLayout, unit, unit, this );
    pgLayoutCmd->execute();
    kPresenterDoc()->addCommand( pgLayoutCmd );
    updateRuler();
}


void KPresenterView::updateRuler()
{
    //update koruler
    QRect r=m_canvas->activePage()->getZoomPageRect();
    getHRuler()->setFrameStartEnd( r.left() , r.right()/*+m_canvas->diffx()*/ );
    getVRuler()->setFrameStartEnd( r.top() , r.bottom()/*+m_canvas->diffy()*/ );
}

/*======================== create GUI ==========================*/
void KPresenterView::createGUI()
{
    splitter = new QSplitter( this );

    if ( !m_pKPresenterDoc->isEmbedded()
         && !m_pKPresenterDoc->isSingleViewMode() ) // No sidebar if the document is embedded
    {
        sidebar = new SideBar( splitter, m_pKPresenterDoc, this );
        connect( sidebar, SIGNAL( movePage( int, int ) ),
	         m_pKPresenterDoc, SLOT( movePage( int, int ) ) );
        connect( sidebar, SIGNAL( selectPage( int, bool ) ),
	         m_pKPresenterDoc, SLOT( selectPage( int, bool ) ) );
        connect( sidebar, SIGNAL( showPage( int ) ),
                 this, SLOT( skipToPage( int ) ) );
        // This sucks when resizing the window
        //splitter->setResizeMode( sidebar, QSplitter::FollowSizeHint );
        //splitter->setResizeMode( pageBase, QSplitter::Stretch );
        splitter->setResizeMode( sidebar, QSplitter::KeepSize );
    }

    QSplitter *splitterVertical = new QSplitter( QSplitter::Vertical, splitter );

    // setup page
    pageBase = new PageBase( splitterVertical, this );
    pageBase->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

    m_canvas=new KPrCanvas( pageBase, "Canvas", (KPresenterView*)this );

    QObject::connect( m_canvas, SIGNAL( fontChanged( const QFont & ) ),
		      this, SLOT( fontChanged( const QFont & ) ) );
    QObject::connect( m_canvas, SIGNAL( colorChanged( const QColor & ) ),
		      this, SLOT( colorChanged( const QColor & ) ) );
    QObject::connect( m_canvas, SIGNAL( alignChanged( int ) ),
		      this, SLOT( alignChanged( int ) ) );
    QObject::connect( m_canvas, SIGNAL( updateSideBarItem( int ) ),
                      this, SLOT( updateSideBarItem( int ) ) );
    QObject::connect( m_canvas, SIGNAL( objectSelectedChanged()),
                      this, SLOT( objectSelectedChanged()));
    QObject::connect( m_canvas, SIGNAL( sigMouseWheelEvent( QWheelEvent* ) ),
                      this, SLOT( getPageMouseWheelEvent( QWheelEvent* ) ) );


    // setup notebar.
    if ( !m_pKPresenterDoc->isEmbedded()
         && !m_pKPresenterDoc->isSingleViewMode() ) // No notebar if the document is embedded
    {
        notebar = new NoteBar( splitterVertical, m_pKPresenterDoc, this );

        QValueList<int> tmpList;
        tmpList << 100 << 10;
        splitterVertical->setSizes( tmpList );
    }

    // setup GUI
    setupActions();
    setupPopupMenus();
    setupScrollbars();
    setRanges();
    setupRulers();

    if ( m_pKPresenterDoc && m_canvas )
	QObject::connect( m_canvas, SIGNAL( stopPres() ), this, SLOT( stopPres() ) );

    if ( sidebar )
    {
        sidebar->outline()->setCurrentItem( sidebar->outline()->firstChild() );
        sidebar->outline()->setSelected( sidebar->outline()->firstChild(), TRUE );
        KConfig *config=KGlobal::config();
        config->setGroup("Global");
        if(!config->readBoolEntry("Sidebar", true)) {
            sidebar->hide();
            actionViewShowSideBar->setChecked(false);
        }
    }

    if ( notebar )
    {
        KConfig *config=KGlobal::config();
        config->setGroup("Global");
        if(!config->readBoolEntry("Notebar", true)) {
            notebar->hide();
            actionViewShowNoteBar->setChecked(false);
        }
    }
        KPrPage *initPage=m_pKPresenterDoc->initialActivePage();
    if (initPage)
        skipToPage(m_pKPresenterDoc->pageList().findRef( initPage));
    else
        //activate first page
        skipToPage(m_pKPresenterDoc->pageList().findRef(m_pKPresenterDoc->pageList().at(0)));
}

/*=============================================================*/
void KPresenterView::initGui()
{
    tbColor = Qt::black;
    actionTextColor->setCurrentColor( Qt::black );
    actionBrushColor->setCurrentColor( Qt::white );
    actionPenColor->setCurrentColor( Qt::black );
    actionScreenPenColor->setColor( Qt::red );
    actionScreenPenWidth->setCurrentItem( 2 );
    updateSideBarMenu();
    objectSelectedChanged();
    refreshPageButton();

    KStatusBar * sb = statusBar();
    if ( sb )
        sb->show();
    showZoom( zoomHandler()->zoom() );
    m_pKPresenterDoc->updateZoomRuler();
    updatePageInfo();
}

void KPresenterView::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
    if ( ev->activated() )
        initGui();

    KoView::guiActivateEvent( ev );
}

/*====================== construct ==============================*/
void KPresenterView::setupActions()
{
    actionEditCut = KStdAction::cut( this, SLOT( editCut() ), actionCollection(), "edit_cut" );
    actionEditCopy = KStdAction::copy( this, SLOT( editCopy() ), actionCollection(), "edit_copy" );
    actionEditPaste = KStdAction::paste( this, SLOT( editPaste() ), actionCollection(), "edit_paste" );
    connect( m_pKPresenterDoc, SIGNAL( enablePaste( bool ) ),
             actionEditPaste, SLOT( setEnabled( bool ) ) );
    m_pKPresenterDoc->clipboardDataChanged(); // set paste's initial state

    actionEditDelete = new KAction( i18n( "&Delete" ), "editdelete", CTRL + Key_Delete,
				    this, SLOT( editDelete() ),
				    actionCollection(), "edit_delete" );
    actionEditSelectAll = KStdAction::selectAll( this, SLOT( editSelectAll() ), actionCollection(), "edit_selectall" );
    /*actionEditCopyPage = */new KAction( i18n( "Copy Page" ), "editcopy",
                                          0, this, SLOT( editCopyPage() ),
                                          actionCollection(), "edit_copypage" );
    actionEditDuplicatePage = new KAction( i18n( "Duplicate Page" ), "newslide",
					   0, this, SLOT( editDuplicatePage() ),
					   actionCollection(), "edit_duplicatepage" );
    actionEditDelPage = new KAction( i18n( "Delete &Page..." ), "delslide", 0,
				     this, SLOT( editDelPage() ),
				     actionCollection(), "edit_delpage" );

    actionEditSearch=KStdAction::find( this, SLOT( editFind() ), actionCollection(), "edit_find" );
    actionEditReplace=KStdAction::replace( this, SLOT( editReplace() ), actionCollection(), "edit_replace" );

    actionEditHeaderFooter = new KAction( i18n( "&Header/Footer..." ), 0,
					  this, SLOT( editHeaderFooter() ),
					  actionCollection(), "edit_headerfooter" );

    // ---------------- View actions

    if ( !m_pKPresenterDoc->isEmbedded() )
    {
        actionViewShowSideBar = new KToggleAction( i18n("Show Sidebar"), 0,
                                                   this, SLOT( viewShowSideBar() ),
                                                   actionCollection(), "view_showsidebar" );
        actionViewShowSideBar->setChecked(true);

        actionViewShowNoteBar = new KToggleAction( i18n("Show Notebar"), 0,
                                                   this, SLOT( viewShowNoteBar() ),
                                                   actionCollection(), "view_shownotebar" );
        actionViewShowNoteBar->setChecked(true);
    }

    // ---------------- insert actions

    actionInsertPage = new KAction( i18n( "&Page..." ), "newslide", Key_F2,
				    this, SLOT( insertPage() ),
				    actionCollection(), "insert_page" );

    actionInsertPicture = new KAction( i18n( "P&icture..." ), "frame_image", Key_F3,
				       this, SLOT( insertPicture() ),
				       actionCollection(), "insert_picture" );

    actionInsertClipart = new KAction( i18n( "&Clipart..." ), "clipart", Key_F4,
				       this, SLOT( insertClipart() ),
				       actionCollection(), "insert_clipart" );

    // ----------------- tools actions

    actionToolsMouse = new KToggleAction( i18n( "&Mouse" ), "frame_edit", Key_F5,
					  this, SLOT( toolsMouse() ),
					  actionCollection(), "tools_mouse" );
    actionToolsMouse->setExclusiveGroup( "tools" );
    actionToolsMouse->setChecked( true );

    actionToolsLine = new KToggleAction( i18n( "&Line" ), "line", Key_F6,
					 this, SLOT( toolsLine() ),
					 actionCollection(), "tools_line" );
    actionToolsLine->setExclusiveGroup( "tools" );

    actionToolsRectangle = new KToggleAction( i18n( "&Rectangle" ), "rectangle",
					      Key_F7, this, SLOT( toolsRectangle() ),
					      actionCollection(), "tools_rectangle" );
    actionToolsRectangle->setExclusiveGroup( "tools" );

    actionToolsCircleOrEllipse = new KToggleAction( i18n( "&Circle/Ellipse" ),
						    "circle", Key_F8,
						    this, SLOT( toolsCircleOrEllipse() ),
						    actionCollection(), "tools_circle" );
    actionToolsCircleOrEllipse->setExclusiveGroup( "tools" );

    actionToolsPie = new KToggleAction( i18n( "&Pie/Arc/Chord" ), "pie", Key_F9,
					this, SLOT( toolsPie() ),
					actionCollection(), "tools_pie" );
    actionToolsPie->setExclusiveGroup( "tools" );

    actionToolsText = new KToggleAction( i18n( "&Text" ), "frame_text", Key_F10,
					 this, SLOT( toolsText() ),
					 actionCollection(), "tools_text" );
    actionToolsText->setExclusiveGroup( "tools" );

    actionToolsAutoform = new KToggleAction( i18n( "&Autoform" ), "autoform",
					     Key_F11, this, SLOT( toolsAutoform() ),
					     actionCollection(), "tools_autoform" );
    actionToolsAutoform->setExclusiveGroup( "tools" );

    actionToolsDiagramm = new KToggleAction( i18n( "&Diagram" ), "frame_chart", Key_F12,
					   this, SLOT( toolsDiagramm() ),
					   actionCollection(), "tools_diagramm" );
    actionToolsDiagramm->setExclusiveGroup( "tools" );

    actionToolsTable = new KToggleAction( i18n( "Ta&ble"), "frame_spreadsheet", 0,
					   this, SLOT( toolsTable() ),
					   actionCollection(), "tools_table" );
    actionToolsTable->setExclusiveGroup( "tools" );

    actionToolsObject = new KoPartSelectAction( i18n( "&Object..." ), "frame_query",
                                                    this, SLOT( toolsObject() ),
                                                    actionCollection(), "tools_object" );

    actionToolsFreehand = new KToggleAction( i18n( "&Freehand" ), "freehand", 0,
                                             this, SLOT( toolsFreehand() ),
                                             actionCollection(), "tools_freehand" );
    actionToolsFreehand->setExclusiveGroup( "tools" );

    actionToolsPolyline = new KToggleAction( i18n( "Po&lyline" ), "polyline", 0,
                                             this, SLOT( toolsPolyline() ),
                                             actionCollection(), "tools_polyline" );
    actionToolsPolyline->setExclusiveGroup( "tools" );

    actionToolsQuadricBezierCurve = new KToggleAction( i18n( "&Quadric Bezier Curve" ), "quadricbeziercurve", 0,
                                                       this, SLOT( toolsQuadricBezierCurve() ),
                                                       actionCollection(), "tools_quadricbeziercurve" );
    actionToolsQuadricBezierCurve->setExclusiveGroup( "tools" );

    actionToolsCubicBezierCurve = new KToggleAction( i18n( "C&ubic Bezier Curve" ), "cubicbeziercurve", 0,
                                                     this, SLOT( toolsCubicBezierCurve() ),
                                                     actionCollection(), "tools_cubicbeziercurve" );
    actionToolsCubicBezierCurve->setExclusiveGroup( "tools" );

    actionToolsConvexOrConcavePolygon = new KToggleAction( i18n( "Co&nvex/Concave Polygon" ), "polygon", 0,
                                                           this, SLOT( toolsConvexOrConcavePolygon() ),
                                                           actionCollection(), "tools_polygon" );
    actionToolsConvexOrConcavePolygon->setExclusiveGroup( "tools" );

    // ----------------- text actions

    actionTextFont = new KAction( i18n( "&Font..." ), 0,
				   this, SLOT( mtextFont() ),
				   actionCollection(), "text_font" );

    actionTextFontSize = new KFontSizeAction( i18n( "Font Size" ), 0,
					      actionCollection(), "text_fontsize" );
    connect(actionTextFontSize, SIGNAL( activated( const QString & ) ),
	     this, SLOT( sizeSelected() ) );

    actionTextFontFamily = new KFontAction( i18n( "Font Family" ), 0,
					      actionCollection(), "text_fontfamily" );
    connect( actionTextFontFamily , SIGNAL( activated( const QString & ) ),
	     this, SLOT( fontSelected() ) );

    actionTextBold = new KToggleAction( i18n( "&Bold" ), "text_bold", CTRL + Key_B,
					   this, SLOT( textBold() ),
					   actionCollection(), "text_bold" );

    actionTextItalic = new KToggleAction( i18n( "&Italic" ), "text_italic", CTRL + Key_I,
					   this, SLOT( textItalic() ),
					   actionCollection(), "text_italic" );

    actionTextUnderline = new KToggleAction( i18n( "&Underline" ), "text_under", CTRL + Key_U,
					   this, SLOT( textUnderline() ),
					   actionCollection(), "text_underline" );

    actionFormatStrikeOut = new KToggleAction( i18n( "&Strike out" ), "text_strike", 0 ,
                                               this, SLOT( textStrikeOut() ),
                                               actionCollection(), "format_strike" );

    actionTextColor = new TKSelectColorAction( i18n( "&Color" ), TKSelectColorAction::TextColor,
                                               actionCollection(), "text_color" ,true);
    connect( actionTextColor, SIGNAL( activated() ), SLOT( textColor() ) );
    actionTextColor->setDefaultColor(QColor());


    actionTextAlignLeft = new KToggleAction( i18n( "Align &Left" ), "text_left", ALT + Key_L,
				       this, SLOT( textAlignLeft() ),
				       actionCollection(), "text_alignleft" );
    actionTextAlignLeft->setExclusiveGroup( "align" );
    actionTextAlignLeft->setChecked( true );

    actionTextAlignCenter = new KToggleAction( i18n( "Align &Center" ), "text_center", ALT + Key_C,
					 this, SLOT( textAlignCenter() ),
					 actionCollection(), "text_aligncenter" );
    actionTextAlignCenter->setExclusiveGroup( "align" );

    actionTextAlignRight = new KToggleAction( i18n( "Align &Right" ), "text_right", ALT + Key_R,
					this, SLOT( textAlignRight() ),
					actionCollection(), "text_alignright" );
    actionTextAlignRight->setExclusiveGroup( "align" );

    actionTextAlignBlock = new KToggleAction( i18n( "Align &Block" ), "text_block", CTRL + Key_J,
                                        this, SLOT( textAlignBlock() ),
                                        actionCollection(), "text_alignblock" );
    actionTextAlignBlock->setExclusiveGroup( "align" );


    actionTextTypeEnumList = new KToggleAction( i18n( "&Enumerated List" ), "enum_list", 0,
					  this, SLOT( textEnumList() ),
					  actionCollection(), "text_enumList" );

    actionTextTypeUnsortList = new KToggleAction( i18n( "&Unsorted List" ), "unsorted_list",
					    0, this, SLOT( textUnsortList() ),
					    actionCollection(), "text_unsortedList" );

    actionTextDepthPlus = new KAction( i18n( "&Increase Depth" ), "format_increaseindent",
				       CTRL + Key_Plus, this, SLOT( textDepthPlus() ),
				       actionCollection(), "text_depthPlus" );

    actionTextDepthMinus = new KAction( i18n( "&Decrease Depth" ), "format_decreaseindent",
					CTRL + Key_Minus, this, SLOT( textDepthMinus() ),
					actionCollection(), "text_depthMinus" );

    actionTextSettings = new KAction( i18n( "&Settings..." ), "configure", 0,
				      this, SLOT( textSettings() ),
				      actionCollection(), "text_settings" );

    actionTextExtentCont2Height = new KAction( i18n( "Extend Contents to Object &Height" ), 0,
					       this, SLOT( textContentsToHeight() ),
					       actionCollection(), "text_con2hei" );

    actionTextExtendObj2Cont = new KAction( i18n( "&Extend Object to fit the Contents" ), 0,
					    this, SLOT( textObjectToContents() ),
					    actionCollection(), "text_obj2cont" );

#if 0 // note: Don't forget to add the functionality before enabling again :)
    actionTextInsertPageNum = new KAction( i18n( "&Insert Page Number" ), "pgnum", 0,
					   this, SLOT( textInsertPageNum() ),
					   actionCollection(), "text_inspgnum" );
#endif

    // ----------------- format actions

    actionExtraPenBrush = new KAction( i18n( "&Pen and Brush..." ), "penbrush", 0,
				       this, SLOT( extraPenBrush() ),
				       actionCollection(), "extra_penbrush" );

    actionExtraConfigPie = new KAction( i18n( "Configure Pie/&Arc/Chord..." ),
					"edit_pie", 0,
					this, SLOT( extraConfigPie() ),
					actionCollection(), "extra_configpie" );

    actionExtraConfigRect = new KAction( i18n( "Configure &Rectangle..." ),
					 "rectangle2", 0,
					 this, SLOT( extraConfigRect() ),
					 actionCollection(), "extra_configrect" );

    actionExtraConfigPolygon = new KAction( i18n( "Configure Po&lygon..." ),
                                            "edit_polygon", 0,
                                            this, SLOT( extraConfigPolygon() ),
                                            actionCollection(), "extra_configpolygon" );

    actionExtraRaise = new KAction( i18n( "Ra&ise object(s)" ), "raise",
				    CTRL +SHIFT+ Key_R, this, SLOT( extraRaise() ),
				    actionCollection(), "extra_raise" );

    actionExtraLower = new KAction( i18n( "&Lower object(s)" ), "lower", CTRL +SHIFT+ Key_L,
				    this, SLOT( extraLower() ),
				    actionCollection(), "extra_lower" );

    actionExtraRotate = new KAction( i18n( "R&otate object(s)..." ), "rotate", 0,
				     this, SLOT( extraRotate() ),
				     actionCollection(), "extra_rotate" );

    actionExtraShadow = new KAction( i18n( "&Shadow object(s)..." ), "shadow", 0,
				     this, SLOT( extraShadow() ),
				     actionCollection(), "extra_shadow" );

    actionExtraAlignObjLeft = new KAction( i18n( "Align &Left" ), "aoleft", 0,
					   this, SLOT( extraAlignObjLeft() ),
					   actionCollection(), "extra_alignleft" );

    actionExtraAlignObjCenterH = new KAction( i18n( "Align Center (&horizontally)" ),
					      "aocenterh", 0,
					      this, SLOT( extraAlignObjCenterH() ),
					      actionCollection(), "extra_aligncenterh" );

    actionExtraAlignObjRight = new KAction( i18n( "Align &Right" ), "aoright", 0,
					    this, SLOT( extraAlignObjRight() ),
					    actionCollection(), "extra_alignright" );

    actionExtraAlignObjTop = new KAction( i18n( "Align &Top" ), "aotop", 0,
					  this, SLOT( extraAlignObjTop() ),
					  actionCollection(), "extra_aligntop" );

    actionExtraAlignObjCenterV = new KAction( i18n( "Align Center (&vertically)" ),
					      "aocenterv", 0,
					      this, SLOT( extraAlignObjCenterV() ),
					      actionCollection(), "extra_aligncenterv" );

    actionExtraAlignObjBottom = new KAction( i18n( "Align &Bottom" ), "aobottom", 0,
					     this, SLOT( extraAlignObjBottom() ),
					     actionCollection(), "extra_alignbottom" );


    actionExtraBackground = new KAction( i18n( "Page Bac&kground..." ), "background", 0,
					 this, SLOT( extraBackground() ),
					 actionCollection(), "extra_background" );

    actionExtraLayout = new KAction( i18n( "Page La&yout..." ), 0,
				     this, SLOT( extraLayout() ),
				     actionCollection(), "extra_layout" );

    actionExtraConfigure = new KAction( i18n( "Configure KPresenter..." ),
					"configure", 0,
					this, SLOT( extraConfigure() ),
					actionCollection(), "extra_configure" );

    actionExtraWebPres = new KAction( i18n( "Create &HTML Slideshow..." ),
				      "webpres", 0,
				      this, SLOT( extraWebPres() ),
				      actionCollection(), "extra_webpres" );

    actionExtraCreateTemplate = new KAction( i18n( "Create Template From Current Slide..." ), 0,
					     this, SLOT( extraCreateTemplate() ),
					     actionCollection(), "extra_template" );

    actionExtraDefaultTemplate = new KAction( i18n( "Use Current Slide As Default Template" ), 0,
					     this, SLOT( extraDefaultTemplate() ),
					     actionCollection(), "extra_defaulttemplate" );

    actionExtraAlignObjs = new KAction( i18n("Align O&bjects"), "alignobjs", 0,
					this, SLOT( extraAlignObjs() ),
					actionCollection(), "extra_alignobjs" );

    actionExtraLineBegin = new KAction( i18n("Line Begin"), "line_begin", 0,
					this, SLOT( extraLineBegin() ),
					actionCollection(), "extra_linebegin" );

    actionExtraLineEnd = new KAction( i18n("Line End"), "line_end", 0,
				      this, SLOT( extraLineEnd() ),
				      actionCollection(), "extra_lineend" );

    actionExtraPenStyle = new KAction( i18n("Pen Style"), "pen_style", 0,
					this, SLOT( extraPenStyle() ),
					actionCollection(), "extra_penstyle" );

    actionExtraPenWidth = new KAction( i18n("Pen Width"), "pen_width", 0,
					this, SLOT( extraPenWidth() ),
					actionCollection(), "extra_penwidth" );

    actionExtraGroup = new KAction( i18n( "&Group Objects" ), "group", 0,
				    this, SLOT( extraGroup() ),
				    actionCollection(), "extra_group" );

    actionExtraUnGroup = new KAction( i18n( "&Ungroup Objects" ),
				    "ungroup", 0,
				    this, SLOT( extraUnGroup() ),
				    actionCollection(), "extra_ungroup" );

    // ----------------- screenpresentation actions

    actionScreenConfigPages = new KAction( i18n( "&Configure Pages..." ),
					   0,
					   this, SLOT( screenConfigPages() ),
					   actionCollection(), "screen_configpages" );

    actionScreenPresStructView = new KAction( i18n( "&Open Presentation Structure Editor..." ),
					   0,
					   this, SLOT( screenPresStructView() ),
					   actionCollection(), "screen_presstruct" );

    actionScreenAssignEffect = new KAction( i18n( "&Assign effect..." ),
					   "effect", 0,
					   this, SLOT( screenAssignEffect() ),
					   actionCollection(), "screen_assigneffect");

    actionScreenStart = new KAction( i18n( "&Start" ),
				     "1rightarrow", 0,
				     this, SLOT( screenStart() ),
				     actionCollection(), "screen_start" );

    actionScreenViewPage = new KAction( i18n( "&View current page" ),
				     "viewmag", 0,
				     this, SLOT( screenViewPage() ),
				     actionCollection(), "screen_viewpage" );

    actionScreenFirst = new KAction( i18n( "&Go to Start" ),
				     "start", Key_Home,
				     this, SLOT( screenFirst() ),
				     actionCollection(), "screen_first" );

    actionScreenPrev = new KAction( i18n( "&Previous Step" ),
				     "back", Key_Prior,
				     this, SLOT( screenPrev() ),
				     actionCollection(), "screen_prev" );

    actionScreenNext = new KAction( i18n( "&Next Step" ),
				     "forward", Key_Next,
				     this, SLOT( screenNext() ),
				     actionCollection(), "screen_next" );

    actionScreenLast = new KAction( i18n( "Go to &End" ),
				     "finish", Key_End,
				     this, SLOT( screenLast() ),
				     actionCollection(), "screen_last" );

    actionScreenSkip = new KAction( i18n( "Goto &Page..." ),
				     "goto", 0,
				     this, SLOT( screenSkip() ),
				     actionCollection(), "screen_skip" );

    actionScreenPenColor = new KColorAction( i18n( "Pen Colo&r..." ), KColorAction::BackgroundColor, 0,
					     this, SLOT( screenPenColor() ),
					     actionCollection(), "screen_pencolor" );

    actionScreenPenWidth = new KSelectAction( i18n( "Pen &Width" ), 0,
					     actionCollection(), "screen_penwidth" );
    QStringList lst;
    lst << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "10";
    actionScreenPenWidth->setItems( lst );
    connect( actionScreenPenWidth, SIGNAL( activated( const QString & ) ),
	     this, SLOT( screenPenWidth( const QString & ) ) );

     // ----------------- colorbar(Brush and Pen) action

     actionBrushColor = new TKSelectColorAction( i18n( "Brush Color" ), TKSelectColorAction::FillColor,
                                                 actionCollection(), "brush_color" ,true);
     connect( actionBrushColor, SIGNAL( activated() ), SLOT( brushChosen() ) );
     actionBrushColor->setDefaultColor(QColor());

     actionPenColor = new TKSelectColorAction( i18n( "Pen Color" ), TKSelectColorAction::LineColor,
                                               actionCollection(), "pen_color" );
     connect( actionPenColor, SIGNAL( activated() ), SLOT( penChosen() ) );
     actionPenColor->setDefaultColor(QColor());
    actionExtendObjectHeight = new KAction( i18n( "&Extend Contents to Object Height" ),0, this, SLOT( textContentsToHeight() ), actionCollection(), "extendobjectheight" );

    actionResizeTextObject = new KAction( i18n( "&Resize Object to fit the Contents" ),0, this, SLOT( textObjectToContents() ), actionCollection(), "resizetextobject" );

    actionObjectProperties = new KAction( i18n( "&Properties..." ), "penbrush", 0,
				       this, SLOT( extraPenBrush() ),
				       actionCollection(), "object_properties" );
    actionChangeClipart =new KAction( i18n( "&Change Clipart..." ), "clipart", 0,
				       this, SLOT( extraChangeClip() ),
				       actionCollection(), "change_clipart" );
    actionRenamePage=new KAction(i18n( "&Rename Page" ),0,this,
                     SLOT( renamePageTitle() ),
                     actionCollection(), "rename_page" );

    actionPic640x480=new KAction(i18n( "640x480" ),0,this,
                     SLOT( picViewOrig640x480() ),
                     actionCollection(), "pic_640_480" );

    actionPic800x600=new KAction(i18n( "800x600" ),0,this,
                     SLOT( picViewOrig800x600() ),
                     actionCollection(), "pic_800_600" );

    actionPic1024x768=new KAction(i18n( "1024x768" ),0,this,
                     SLOT( picViewOrig1024x768() ),
                     actionCollection(), "pic_1024_768" );

    actionPic1280x1024=new KAction(i18n( "1280x1024" ),0,this,
                     SLOT( picViewOrig1280x1024() ),
                     actionCollection(), "pic_1280_1024" );

    actionPic1600x1200=new KAction(i18n( "1600x1200" ),0,this,
                     SLOT( picViewOrig1600x1200() ),
                     actionCollection(), "pic_1600_1200" );

    actionChangePic=new KAction( i18n( "&Change Picture..." ),"frame_image"
                                 ,0,this,SLOT( chPic() ),
                     actionCollection(), "change_picture" );

    actionFormatSuper = new KToggleAction( i18n( "Superscript" ), "super", 0,
                                              this, SLOT( textSuperScript() ),
                                              actionCollection(), "format_super" );
    actionFormatSuper->setExclusiveGroup( "valign" );
    actionFormatSub = new KToggleAction( i18n( "Subscript" ), "sub", 0,
                                              this, SLOT( textSubScript() ),
                                              actionCollection(), "format_sub" );
    actionFormatSub->setExclusiveGroup( "valign" );


    actionInsertSpecialChar = new KAction( i18n( "Sp&ecial Character..." ), "char",
                        ALT + SHIFT + Key_C,
                        this, SLOT( insertSpecialChar() ),
                        actionCollection(), "insert_specialchar" );

    actionInsertLink = new KAction( i18n( "Insert link" ), 0,
                                    this, SLOT( insertLink() ),
                                    actionCollection(), "insert_link" );

    // ------------------- Actions with a key binding and no GUI item
    KAction* actSoftHyphen = new KAction( i18n( "Insert soft hyphen" ), CTRL+Key_Minus,
                                          this, SLOT( slotSoftHyphen() ), actionCollection(), "soft_hyphen" );
    // Necessary for the actions that are not plugged anywhere
    KAccel * accel = new KAccel( this );
    actSoftHyphen->plugAccel( accel );

#if 0
    //code from page.cc
    //not implemented
    picResizeMenu->insertSeparator();
    picResizeMenu->insertItem( i18n( "Enter Custom Factor..." ), this, SLOT( picViewOrigFactor() ) );
#endif
    (void) new KAction( i18n( "&Autocorrection..." ), 0,
                        this, SLOT( extraAutoFormat() ),
                        actionCollection(), "extra_autocorrection" );
    actionExtraSpellCheck = KStdAction::spelling( this, SLOT( extraSpelling() ), actionCollection(), "extra_spellcheck" );

    actionFormatParag = new KAction( i18n( "&Paragraph..." ), ALT + CTRL + Key_P,
                                     this, SLOT( formatParagraph() ),
                                     actionCollection(), "format_paragraph" );

    actionFormatDefault=new KAction( i18n( "Default Format" ), 0,
                                          this, SLOT( textDefaultFormat() ),
                                          actionCollection(), "text_default" );

    actionOpenLink = new KAction( i18n( "Open link" ), 0,
                                     this, SLOT( openLink() ),
                                     actionCollection(), "open_link" );

    actionChangeLink=new KAction( i18n("Change Link"), 0,
                                  this,SLOT(changeLink()),
                                  actionCollection(), "change_link");


    m_variableDefMap.clear();
    actionInsertVariable = new KActionMenu( i18n( "&Variable" ),
                                            actionCollection(), "insert_variable" );
    // The last argument is only needed if a submenu is to be created
    addVariableActions( VT_FIELD, KoFieldVariable::actionTexts(), actionInsertVariable, i18n("&Property") );
    addVariableActions( VT_DATE, KoDateVariable::actionTexts(), actionInsertVariable, i18n("&Date") );
    addVariableActions( VT_TIME, KoTimeVariable::actionTexts(), actionInsertVariable, i18n("&Time") );

    actionInsertCustom = new KActionMenu( i18n( "&Custom" ),
                                            actionCollection(), "insert_custom" );
    actionInsertVariable->insert(actionInsertCustom);
    refreshCustomMenu();

    addVariableActions( VT_PGNUM, KoPgNumVariable::actionTexts(), actionInsertVariable, QString::null );

    actionInsertVariable->popupMenu()->insertSeparator();
    actionRefreshAllVariable = new KAction( i18n( "&Refresh all variables" ), 0,
                                    this, SLOT( refreshAllVariable() ),
                                    actionCollection(), "refresh_all_variable" );
    actionInsertVariable->insert(actionRefreshAllVariable);

    actionIncreaseFontSize = new KAction( i18n("Increase font size"),"fontsizeup", 0,
                                  this, SLOT( increaseFontSize() ),
                                  actionCollection(), "increaseFontSize" );

    actionDecreaseFontSize = new KAction( i18n("Decrease font size"),"fontsizedown", 0,
                                  this, SLOT( decreaseFontSize() ),
                                  actionCollection(), "decreaseFontSize" );

    actionChangeCase=new KAction( i18n( "Change case..." ), 0,
                                  this, SLOT( changeCaseOfText() ),
                                  actionCollection(), "change_case" );

    actionViewZoom = new KSelectAction( i18n( "Zoom" ), "viewmag", 0,
                                        actionCollection(), "view_zoom" );
    connect( actionViewZoom, SIGNAL( activated( const QString & ) ),
             this, SLOT( viewZoom( const QString & ) ) );
    actionViewZoom->setEditable(true);
    actionViewZoom->setComboWidth( 50 );
    changeZoomMenu( );

}

void KPresenterView::textSubScript()
{
    m_canvas->setTextSubScript(actionFormatSub->isChecked());
}

void KPresenterView::textSuperScript()
{
    m_canvas->setTextSuperScript(actionFormatSuper->isChecked());
}

void KPresenterView::decreaseFontSize()
{
    m_canvas->setDecreaseFontSize();
}

void KPresenterView::increaseFontSize()
{
    m_canvas->setIncreaseFontSize();
}

void KPresenterView::objectSelectedChanged()
{
    bool state=m_canvas->isOneObjectSelected();
    actionScreenAssignEffect->setEnabled(state);
    actionExtraRotate->setEnabled(state);
    actionExtraAlignObjs->setEnabled(state);
    actionExtraGroup->setEnabled(state && m_canvas->activePage()->numSelected()>1);
    actionExtraUnGroup->setEnabled(state);
    actionExtraAlignObjLeft->setEnabled(state);
    actionExtraAlignObjCenterH->setEnabled(state);
    actionExtraAlignObjRight->setEnabled(state);
    actionExtraAlignObjTop->setEnabled(state);
    actionExtraAlignObjCenterV->setEnabled(state);
    actionExtraAlignObjBottom->setEnabled(state);
    actionEditDelete->setEnabled(state);
    actionExtraRaise->setEnabled(state && m_canvas->activePage()->numSelected()==1);
    actionExtraLower->setEnabled(state && m_canvas->activePage()->numSelected()==1);
    //actionBrushColor->setEnabled(state);
    actionPenColor->setEnabled(state);
    //actionExtraPenStyle->setEnabled(state);
    //actionExtraPenWidth->setEnabled(state);

    bool isText=!m_canvas->applicableTextInterfaces().isEmpty();
    actionTextFont->setEnabled(isText);
    actionTextFontSize->setEnabled(isText);
    actionTextFontFamily->setEnabled(isText);
    actionTextColor->setEnabled(isText);
    actionTextAlignLeft->setEnabled(isText);
    actionTextAlignCenter->setEnabled(isText);
    actionTextAlignRight->setEnabled(isText);
    actionTextAlignBlock->setEnabled(isText);
    actionTextTypeUnsortList->setEnabled(isText);
    actionTextTypeEnumList->setEnabled(isText);
    actionTextDepthPlus->setEnabled(isText);
    actionFormatDefault->setEnabled(isText);
    actionTextDepthMinus->setEnabled(isText);

    actionTextSettings->setEnabled(isText);
    actionTextExtentCont2Height->setEnabled(isText);
    actionTextExtendObj2Cont->setEnabled(isText);
    actionTextBold->setEnabled(isText);
    actionTextItalic->setEnabled(isText);
    actionTextUnderline->setEnabled(isText);
    actionFormatStrikeOut->setEnabled(isText);
    actionFormatSuper->setEnabled(isText);
    actionFormatSub->setEnabled(isText);
    actionIncreaseFontSize->setEnabled(isText);
    actionDecreaseFontSize->setEnabled(isText);
    if ( !m_canvas->applicableTextInterfaces().isEmpty() )
    {
        KoTextFormat format =*(m_canvas->applicableTextInterfaces().first()->currentFormat());
        //unzoom font
        format.setPointSize( (int)KoTextZoomHandler::layoutUnitPtToPt( format.font().pointSize() ) );
        showFormat( format );
        const KoParagLayout * paragLayout=m_canvas->applicableTextInterfaces().first()->currentParagLayoutFormat();
        if(paragLayout->counter)
        {
            KoParagCounter counter=*(paragLayout->counter);
            showCounter( counter );
        }
        else
        {
            actionTextTypeUnsortList->setChecked( false );
            actionTextTypeEnumList->setChecked( false );
        }
        alignChanged(  paragLayout->alignment );
    }

    KPTextView *edit=m_canvas->currentTextObjectView();
    bool val=edit && isText;
    actionInsertSpecialChar->setEnabled(val);
    actionInsertLink->setEnabled(val);
    actionFormatParag->setEnabled(val);
    actionInsertVariable->setEnabled(val);
    if(edit)
    {
        double leftMargin =edit->currentParagLayout().margins[QStyleSheetItem::MarginLeft];
        actionTextDepthMinus->setEnabled( val && leftMargin>0);
    }

    if(!edit)
    {
        actionEditCopy->setEnabled(state);
        actionEditCut->setEnabled(state);
        actionChangeCase->setEnabled( val);
    }

    actionExtraShadow->setEnabled(!m_canvas->haveASelectedPictureObj());

    state=m_canvas->oneObjectTextExist();
    actionEditSearch->setEnabled(state);
    actionEditReplace->setEnabled(state);

    slotUpdateRuler();

}

/*=========== take changes for backgr dialog =====================*/
void KPresenterView::backOk( bool takeGlobal )
{
    SetBackCmd *setBackCmd = new SetBackCmd( i18n( "Set Background" ), backDia->getBackColor1(),
					     backDia->getBackColor2(), backDia->getBackColorType(),
					     backDia->getBackUnbalanced(),
					     backDia->getBackXFactor(), backDia->getBackYFactor(),
					     KPImageKey( backDia->getBackPixFilename(),
                                                         backDia->getBackPixLastModified() ),
                                             KPClipartKey( backDia->getBackClipFilename(),
                                                           backDia->getBackClipLastModified() ),
					     backDia->getBackView(), backDia->getBackType(),
					     m_canvas->activePage()->getBackColor1(  ),
					     m_canvas->activePage()->getBackColor2(  ),
					     m_canvas->activePage()->getBackColorType(  ),
					     m_canvas->activePage()->getBackUnbalanced( ),
					     m_canvas->activePage()->getBackXFactor(  ),
					     m_canvas->activePage()->getBackYFactor(  ),
					     m_canvas->activePage()->getBackPixKey(  ),
					     m_canvas->activePage()->getBackClipKey( ),
					     m_canvas->activePage()->getBackView(  ),
					     m_canvas->activePage()->getBackType(  ),
					     takeGlobal, m_pKPresenterDoc,m_canvas->activePage() );
    setBackCmd->execute();
    m_pKPresenterDoc->addCommand( setBackCmd );
}

/*================== autoform chosen =============================*/
void KPresenterView::afChooseOk( const QString & c )
{
    QFileInfo fileInfo( c );
    QString fileName = locate( "autoforms",
			       fileInfo.dirPath( false ) + "/" + fileInfo.baseName() + ".atf",
			       KPresenterFactory::global() );

    m_canvas->deSelectAllObj();
    m_canvas->setToolEditMode( INS_AUTOFORM );
    m_canvas->setAutoForm( fileName );
}

void KPresenterView::slotAfchooseCanceled()
{
    setTool( TEM_MOUSE );
}

/*=========== take changes for style dialog =====================*/
void KPresenterView::styleOk()
{
    if ( !m_canvas->activePage()->setPenBrush( styleDia->getPen(), styleDia->getBrush(), styleDia->getLineBegin(),
					 styleDia->getLineEnd(), styleDia->getFillType(),
					 styleDia->getGColor1(),
					 styleDia->getGColor2(), styleDia->getGType(),
					 styleDia->getGUnbalanced(),
					 styleDia->getGXFactor(), styleDia->getGYFactor(),
					 styleDia->isSticky() ) ) {
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
	sticky = styleDia->isSticky();
    }
    else {
        actionBrushColor->setCurrentColor( (styleDia->getBrush()).color() );
        actionPenColor->setCurrentColor( (styleDia->getPen()).color() );
    }
}

/*=================== page configuration ok ======================*/
void KPresenterView::pgConfOk()
{
    PgConfCmd *pgConfCmd = new PgConfCmd( i18n( "Configure Page for Screen Presentations" ),
					  pgConfDia->getManualSwitch(), pgConfDia->getInfinitLoop(),
					  pgConfDia->getPageEffect(), pgConfDia->getPresSpeed(),
					  pgConfDia->getPageTimer(),
					  pgConfDia->getPageSoundEffect(),
					  pgConfDia->getPageSoundFileName(),
					  kPresenterDoc()->spManualSwitch(),
					  kPresenterDoc()->spInfinitLoop(),
					  m_canvas->activePage()->getPageEffect(),
					  kPresenterDoc()->getPresSpeed(),
					  m_canvas->activePage()->getPageTimer(),
					  m_canvas->activePage()->getPageSoundEffect(),
					  m_canvas->activePage()->getPageSoundFileName(),
					  kPresenterDoc(), m_canvas->activePage() );
    pgConfCmd->execute();
    kPresenterDoc()->addCommand( pgConfCmd );
}

/*=================== effect dialog ok ===========================*/
void KPresenterView::effectOk()
{
}

/*=================== rotate dialog ok ===========================*/
void KPresenterView::rotateOk()
{
    bool newAngle=false;
    QPtrList<KPObject> _objects;
    QPtrList<RotateCmd::RotateValues> _oldRotate;
    float _newAngle;
    RotateCmd::RotateValues *tmp;

    _objects.setAutoDelete( false );
    _oldRotate.setAutoDelete( false );

    _newAngle = rotateDia->getAngle();

    QPtrListIterator<KPObject> it( m_canvas->getObjectList() );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() ) {
	    tmp = new RotateCmd::RotateValues;
	    tmp->angle =it.current()->getAngle();

            if(!newAngle &&tmp->angle!= _newAngle)
                newAngle=true;

	    _oldRotate.append( tmp );
	    _objects.append( it.current() );
	}
    }

    if ( !_objects.isEmpty() && newAngle ) {
	RotateCmd *rotateCmd = new RotateCmd( i18n( "Change Rotation" ),
					      _oldRotate, _newAngle, _objects, kPresenterDoc() );
	kPresenterDoc()->addCommand( rotateCmd );
	rotateCmd->execute();
    } else {
	_oldRotate.setAutoDelete( true );
	_oldRotate.clear();
    }
}

/*=================== shadow dialog ok ==========================*/
void KPresenterView::shadowOk()
{
    bool newShadow=false;
    QPtrList<KPObject> _objects;
    QPtrList<ShadowCmd::ShadowValues> _oldShadow;
    ShadowCmd::ShadowValues _newShadow, *tmp;

    _objects.setAutoDelete( false );
    _oldShadow.setAutoDelete( false );

    _newShadow.shadowDirection = shadowDia->getShadowDirection();
    _newShadow.shadowDistance = shadowDia->getShadowDistance();
    _newShadow.shadowColor = shadowDia->getShadowColor();

    QPtrListIterator<KPObject> it( m_canvas->getObjectList() );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() ) {
	    tmp = new ShadowCmd::ShadowValues;
	    tmp->shadowDirection = it.current()->getShadowDirection();
	    tmp->shadowDistance =it.current()->getShadowDistance();
	    tmp->shadowColor = it.current()->getShadowColor();

            if(!newShadow &&( tmp->shadowDirection!=_newShadow.shadowDirection
               || tmp->shadowDistance!=_newShadow.shadowDistance
               || tmp->shadowColor!=_newShadow.shadowColor))
                newShadow=true;

	    _oldShadow.append( tmp );
	    _objects.append( it.current() );
	}
    }

    if ( !_objects.isEmpty() && newShadow ) {
	ShadowCmd *shadowCmd = new ShadowCmd( i18n( "Change Shadow" ),
					      _oldShadow, _newShadow, _objects, kPresenterDoc() );
	kPresenterDoc()->addCommand( shadowCmd );
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
void KPresenterView::confPieOk()
{
    if ( !m_canvas->activePage()->setPieSettings( confPieDia->getType(),
					    confPieDia->getAngle(), confPieDia->getLength() ) ) {
	pieType = confPieDia->getType();
	pieAngle = confPieDia->getAngle();
	pieLength = confPieDia->getLength();
    }
    updateObjectStatusBarItem();  //the type might have changed
}

/*================================================================*/
void KPresenterView::confRectOk()
{
    if ( !m_canvas->activePage()->setRectSettings( confRectDia->getRndX(), confRectDia->getRndY() ) ) {
	rndX = confRectDia->getRndX();
	rndY = confRectDia->getRndY();
    }
}

/*================================================================*/
void KPresenterView::confPolygonOk()
{
    if ( !m_canvas->activePage()->setPolygonSettings( confPolygonDia->getCheckConcavePolygon(),
                                                confPolygonDia->getCornersValue(),
                                                confPolygonDia->getSharpnessValue() ) ) {
        checkConcavePolygon = confPolygonDia->getCheckConcavePolygon();
        cornersValue = confPolygonDia->getCornersValue();
        sharpnessValue = confPolygonDia->getSharpnessValue();
    }
}

/*================================================================*/
unsigned int KPresenterView::getCurrPgNum() const
{
    return currPg + 1;
}

/*================== scroll horizontal ===========================*/
void KPresenterView::scrollH( int value )
{
    if ( !presStarted ) {
	m_canvas->scrollX( value );
	if ( h_ruler )
	    h_ruler->setOffset( value, 0 );
    }
}

/*===================== scroll vertical ==========================*/
void KPresenterView::scrollV( int value )
{
    if ( !presStarted ) {
        m_canvas->scrollY( value );
	if ( v_ruler )
	    v_ruler->setOffset( 0, value );
    }
}

/*====================== font changed ===========================*/
void KPresenterView::fontChanged( const QFont &font )
{
    tbFont.setFamily( font.family() );
    tbFont.setBold( font.bold() );
    tbFont.setItalic( font.italic() );
    tbFont.setUnderline( font.underline() );
    tbFont.setPointSize( font.pointSize() );

    actionTextFontFamily->setFont( tbFont.family() );
    actionTextFontSize->setFontSize( tbFont.pointSize() );
    actionTextBold->setChecked( tbFont.bold() );
    actionTextItalic->setChecked( tbFont.italic() );
    actionTextUnderline->setChecked( tbFont.underline() );
}

/*====================== color changed ==========================*/
void KPresenterView::colorChanged( const QColor &color )
{
  tbColor = color;
  actionTextColor->setEnabled( true );
  actionTextColor->setCurrentColor( tbColor );
}

/*====================== align changed ==========================*/
void KPresenterView::alignChanged( int align )
{
    if ( align != tbAlign ) {
	tbAlign = align;
	if ( ( align & AlignLeft ) == AlignLeft ) {
	    actionTextAlignLeft->blockSignals( true );
	    actionTextAlignLeft->setChecked( true );
	    actionTextAlignLeft->blockSignals( false );
	} else if ( ( align & AlignHCenter ) == AlignHCenter ||
		    ( align & AlignCenter ) == AlignCenter ) {
	    actionTextAlignCenter->blockSignals( true );
	    actionTextAlignCenter->setChecked( true );
	    actionTextAlignCenter->blockSignals( false );
	} else if ( ( align & AlignRight ) == AlignRight ) {
	    actionTextAlignRight->blockSignals( true );
	    actionTextAlignRight->setChecked( true );
	    actionTextAlignRight->blockSignals( false );
	} else if ( (align & AlignJustify ) == AlignJustify ) {
	    actionTextAlignBlock->blockSignals( true );
	    actionTextAlignBlock->setChecked( true );
	    actionTextAlignBlock->blockSignals( false );
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
	actionScreenPenColor->blockSignals( true );
	actionScreenPenColor->setColor( c );
	actionScreenPenColor->blockSignals( false );
    }
}

#if 0
/*====================== paint event ============================*/
void KPresenterView::repaint( bool erase )
{
    QWidget::repaint( erase );
    m_canvas->repaint( erase );
}

/*====================== paint event ============================*/
void KPresenterView::repaint( unsigned int x, unsigned int y, unsigned int w,
			      unsigned int h, bool erase )
{
    QWidget::repaint( x, y, w, h, erase );
    m_canvas->repaint( x, y, w, h, erase );
}

/*====================== paint event ============================*/
void KPresenterView::repaint( QRect r, bool erase )
{
    QWidget::repaint( r, erase );
    m_canvas->repaint( r, erase );
}
#endif

/*====================== change pciture =========================*/
void KPresenterView::changePicture( const QString & filename )
{
    //url = KFileDialog::getImageOpenURL(); lukas: put this back in KDE 3.0

    KFileDialog fd( filename, KImageIO::pattern(KImageIO::Reading), 0, 0, true );
    fd.setCaption(i18n("Select new Picture"));
    fd.setPreviewWidget( new KoPictureFilePreview( &fd ) );

    KURL url;
    if ( fd.exec() == QDialog::Accepted )
      url = fd.selectedURL();

    if( url.isEmpty() || !url.isValid())
      return;

    QString file;
    if (!KIO::NetAccess::download( url, file ))
        return;

    if ( !file.isEmpty() )
      m_canvas->activePage()->changePicture( file );
}

/*====================== change clipart =========================*/
void KPresenterView::changeClipart( const QString & filename )
{
    KFileDialog fd( filename, KoPictureFilePreview::clipartPattern(), 0, 0, true );
    fd.setCaption(i18n("Select new Clipart"));
    fd.setPreviewWidget( new KoPictureFilePreview( &fd ) );

    KURL url;
    if ( fd.exec() == QDialog::Accepted )
        url = fd.selectedURL();

    if( url.isEmpty() || !url.isValid() )
        return;

    QString file;
    if (!KIO::NetAccess::download( url, file ))
        return;

    if ( !file.isEmpty() )
        m_canvas->activePage()->changeClipart( file );
}

/*====================== resize event ===========================*/
void KPresenterView::resizeEvent( QResizeEvent *e )
{
    if ( !presStarted )
	QWidget::resizeEvent( e );

    QSize s = e ? e->size() : size();
    splitter->setGeometry( 0, 0, s.width(), s.height() );
}

void KPresenterView::reorganize()
{
    if (m_bShowGUI ) {

        horz->show();
        vert->show();
	pgNext->show();
	pgPrev->show();

        if(kPresenterDoc()->showRuler())
        {
            m_canvas->move( 20, 20 );
            if ( h_ruler )
            {
                h_ruler->show();
                h_ruler->setGeometry( 20, 0, m_canvas->width(), 20 );
            }
            if (v_ruler )
            {
                v_ruler->show();
                v_ruler->setGeometry( 0, 20, 20, m_canvas->height() );
            }
            if(getTabChooser())
            {
                getTabChooser()->setGeometry(0,0,20,20);
                getTabChooser()->show();
            }
        }
        else
        {
            m_canvas->move( 0, 0 );
            if ( h_ruler )
                h_ruler->hide();
            if ( v_ruler )
                v_ruler->hide();
            getTabChooser()->hide();
        }
	setRanges();
    }
    else
    {
	horz->hide();
	vert->hide();
	pgNext->hide();
	pgPrev->hide();
	h_ruler->hide();
	v_ruler->hide();
        getTabChooser()->hide();
	m_canvas->move( 0, 0 );
    }
}

void PageBase::resizeEvent( QResizeEvent *e )
{
    if ( !view->presStarted )
	QWidget::resizeEvent( e );

    QSize s = e ? e->size() : size();

    if ( view->m_bShowGUI ) {

        view->m_canvas->resize( s.width() - 36, s.height() - 36 );
	view->vert->setGeometry( s.width() - 16, 0, 16, s.height() - 32 );
	view->pgPrev->setGeometry( s.width() - 15, s.height() - 32, 15, 16 );
	view->pgNext->setGeometry( s.width() - 15, s.height() - 16, 15, 16 );
	view->horz->setGeometry( 0, s.height() - 16, s.width() - 16, 16 );
    }
    else
    {
	view->m_canvas->move( 0, 0 );
	view->m_canvas->resize( s.width(), s.height() );
    }
    view->reorganize();
}

/*===============================================================*/
void KPresenterView::dragEnterEvent( QDragEnterEvent *e )
{
    QApplication::sendEvent( m_canvas, e );
}

/*===============================================================*/
void KPresenterView::dragMoveEvent( QDragMoveEvent *e )
{
    QApplication::sendEvent( m_canvas, e );
}

/*===============================================================*/
void KPresenterView::dragLeaveEvent( QDragLeaveEvent *e )
{
    QApplication::sendEvent( m_canvas, e );
}

/*===============================================================*/
void KPresenterView::dropEvent( QDropEvent *e )
{
    QApplication::sendEvent( m_canvas, e );
}

/*===============================================================*/
void KPresenterView::getPageMouseWheelEvent( QWheelEvent *e )
{
  QApplication::sendEvent( vert, e );
}

/*======================= key press event =======================*/
void KPresenterView::keyPressEvent( QKeyEvent *e )
{
    if ( e->key() == Key_Delete && !m_canvas->currentTextObjectView() )
	editDelete();
    else
	QApplication::sendEvent( m_canvas, e );
}

/*====================== do automatic screenpresentation ========*/
void KPresenterView::doAutomaticScreenPres()
{
    if ( exitPres ) // A user pushed Escape key or clicked "Exit presentation" menu.
        return;
    else if ( !continuePres && kPresenterDoc()->spInfinitLoop() ) {
        continuePres = true;
        m_canvas->gotoPage( 1 ); // return to first page.
        setCurrentTimer( 1 );
        m_canvas->setNextPageTimer( true );
    }
    else if ( !continuePres ) {
        screenStop();
        return;
    }
    else
        screenNext();
}

void KPresenterView::updateReadWrite( bool readwrite )
{
#ifdef __GNUC__
#warning TODO
#endif
    if ( !readwrite )
    {
        QValueList<KAction *> actions = actionCollection()->actions();
        QValueList<KAction *>::ConstIterator aIt = actions.begin();
        QValueList<KAction *>::ConstIterator aEnd = actions.end();
        for (; aIt != aEnd; ++aIt )
            (*aIt)->setEnabled( readwrite );
        refreshPageButton();
        actionViewZoom->setEnabled( true );
    }
}

/*======================== setup popup menus ===================*/
void KPresenterView::setupPopupMenus()
{
    QPixmap pixmap;

    // create right button object align menu
    rb_oalign = new QPopupMenu();
    Q_CHECK_PTR( rb_oalign );
    rb_oalign->insertItem( KPBarIcon("aoleft" ), this, SLOT( extraAlignObjLeft() ) );
    rb_oalign->insertSeparator( -1 );
    rb_oalign->insertItem( KPBarIcon("aocenterh" ), this, SLOT( extraAlignObjCenterH() ) );
    rb_oalign->insertSeparator( -1 );
    rb_oalign->insertItem( KPBarIcon("aoright" ), this, SLOT( extraAlignObjRight() ) );
    rb_oalign->insertSeparator( -1 );
    rb_oalign->insertItem( KPBarIcon("aotop" ) , this, SLOT( extraAlignObjTop() ) );
    rb_oalign->insertSeparator( -1 );
    rb_oalign->insertItem( KPBarIcon("aocenterv" ), this, SLOT( extraAlignObjCenterV() ) );
    rb_oalign->insertSeparator( -1 );
    rb_oalign->insertItem( KPBarIcon("aobottom" ), this, SLOT( extraAlignObjBottom() ) );
    rb_oalign->setMouseTracking( true );
    rb_oalign->setCheckable( false );

    // create right button line begin
    rb_lbegin = new QPopupMenu();
    Q_CHECK_PTR( rb_lbegin );
    rb_lbegin->insertItem( KPBarIcon("line_normal_begin" ), this, SLOT( extraLineBeginNormal() ) );
    rb_lbegin->insertSeparator( -1 );
    rb_lbegin->insertItem( KPBarIcon("line_arrow_begin" ), this, SLOT( extraLineBeginArrow() ) );
    rb_lbegin->insertSeparator( -1 );
    rb_lbegin->insertItem( KPBarIcon("line_rect_begin" ), this, SLOT( extraLineBeginRect() ) );
    rb_lbegin->insertSeparator( -1 );
    rb_lbegin->insertItem( KPBarIcon("line_circle_begin" ), this, SLOT( extraLineBeginCircle() ) );
    rb_lbegin->setMouseTracking( true );
    rb_lbegin->setCheckable( false );

    // create right button line end
    rb_lend = new QPopupMenu();
    Q_CHECK_PTR( rb_lend );
    rb_lend->insertItem( KPBarIcon("line_normal_end" ), this, SLOT( extraLineEndNormal() ) );
    rb_lend->insertSeparator( -1 );
    rb_lend->insertItem( KPBarIcon("line_arrow_end" ), this, SLOT( extraLineEndArrow() ) );
    rb_lend->insertSeparator( -1 );
    rb_lend->insertItem( KPBarIcon("line_rect_end" ), this, SLOT( extraLineEndRect() ) );
    rb_lend->insertSeparator( -1 );
    rb_lend->insertItem( KPBarIcon("line_circle_end" ), this, SLOT( extraLineEndCircle() ) );
    rb_lend->setMouseTracking( true );
    rb_lend->setCheckable( false );

    // create right button pen style
    rb_pstyle = new QPopupMenu();
    Q_CHECK_PTR( rb_pstyle );
    rb_pstyle->insertItem( KPBarIcon( "pen_style_solid" ), this, SLOT( extraPenStyleSolid() ) );
    rb_pstyle->insertSeparator( -1 );
    rb_pstyle->insertItem( KPBarIcon( "pen_style_dash" ), this, SLOT( extraPenStyleDash() ) );
    rb_pstyle->insertSeparator( -1 );
    rb_pstyle->insertItem( KPBarIcon( "pen_style_dot" ), this, SLOT( extraPenStyleDot() ) );
    rb_pstyle->insertSeparator( -1 );
    rb_pstyle->insertItem( KPBarIcon( "pen_style_dashdot" ), this, SLOT( extraPenStyleDashDot() ) );
    rb_pstyle->insertSeparator( -1 );
    rb_pstyle->insertItem( KPBarIcon( "pen_style_dashdotdot" ), this, SLOT( extraPenStyleDashDotDot() ) );
    rb_pstyle->insertSeparator( -1 );
    rb_pstyle->insertItem( KPBarIcon( "pen_style_nopen" ), this, SLOT( extraPenStyleNoPen() ) );
    rb_pstyle->setMouseTracking( true );
    rb_pstyle->setCheckable( false );

    // create right button pen width
    rb_pwidth = new QPopupMenu();
    Q_CHECK_PTR( rb_pwidth );
    rb_pwidth->insertItem( KPBarIcon( "pen_width1" ), this, SLOT( extraPenWidth1() ) );
    rb_pwidth->insertSeparator( -1 );
    rb_pwidth->insertItem( KPBarIcon( "pen_width2" ), this, SLOT( extraPenWidth2() ) );
    rb_pwidth->insertSeparator( -1 );
    rb_pwidth->insertItem( KPBarIcon( "pen_width3" ), this, SLOT( extraPenWidth3() ) );
    rb_pwidth->insertSeparator( -1 );
    rb_pwidth->insertItem( KPBarIcon( "pen_width4" ), this, SLOT( extraPenWidth4() ) );
    rb_pwidth->insertSeparator( -1 );
    rb_pwidth->insertItem( KPBarIcon( "pen_width5" ), this, SLOT( extraPenWidth5() ) );
    rb_pwidth->insertSeparator( -1 );
    rb_pwidth->insertItem( KPBarIcon( "pen_width6" ), this, SLOT( extraPenWidth6() ) );
    rb_pwidth->insertSeparator( -1 );
    rb_pwidth->insertItem( KPBarIcon( "pen_width7" ), this, SLOT( extraPenWidth7() ) );
    rb_pwidth->insertSeparator( -1 );
    rb_pwidth->insertItem( KPBarIcon( "pen_width8" ), this, SLOT( extraPenWidth8() ) );
    rb_pwidth->insertSeparator( -1 );
    rb_pwidth->insertItem( KPBarIcon( "pen_width9" ), this, SLOT( extraPenWidth9() ) );
    rb_pwidth->insertSeparator( -1 );
    rb_pwidth->insertItem( KPBarIcon( "pen_width10" ), this, SLOT( extraPenWidth10() ) );
    rb_pwidth->setMouseTracking( true );
    rb_pwidth->setCheckable( false );
}

/*======================= setup scrollbars =====================*/
void KPresenterView::setupScrollbars()
{
    vert = new QScrollBar( QScrollBar::Vertical, pageBase );
    horz = new QScrollBar( QScrollBar::Horizontal, pageBase );
    vert->show();
    horz->show();
    QObject::connect( vert, SIGNAL( valueChanged( int ) ), this, SLOT( scrollV( int ) ) );
    QObject::connect( horz, SIGNAL( valueChanged( int ) ), this, SLOT( scrollH( int ) ) );
    vert->setValue(vert->maxValue());
    horz->setValue(horz->maxValue());
    vert->setValue(vert->minValue());
    horz->setValue(horz->minValue());
    pgNext = new QToolButton( pageBase );
    pgNext->setPixmap( QPixmap( pagedown_xpm ) );
    pgNext->setAutoRepeat( TRUE );
    QToolTip::add( pgNext, i18n( "Next Page" ) );
    connect( pgNext, SIGNAL( clicked() ), this, SLOT( nextPage() ) );
    pgPrev = new QToolButton( pageBase );
    pgPrev->setPixmap( QPixmap( pageup_xpm ) );
    pgPrev->setAutoRepeat( TRUE );
    QToolTip::add( pgPrev, i18n( "Previous Page" ) );
    connect( pgPrev, SIGNAL( clicked() ), this, SLOT( prevPage() ) );
}

/*==============================================================*/
void KPresenterView::setupRulers()
{
    tabChooser = new KoTabChooser( pageBase, KoTabChooser::TAB_ALL );
    tabChooser->setReadWrite(kPresenterDoc()->isReadWrite());

    h_ruler = new KoRuler( pageBase, m_canvas, Qt::Horizontal, kPresenterDoc()->pageLayout(), KoRuler::F_INDENTS | KoRuler::F_TABS, kPresenterDoc()->getUnit(), tabChooser );
    h_ruler->changeFlags(0);
    h_ruler->setReadWrite(kPresenterDoc()->isReadWrite());
    v_ruler = new KoRuler( pageBase, m_canvas, Qt::Vertical, kPresenterDoc()->pageLayout(), 0, kPresenterDoc()->getUnit() );
    v_ruler->setReadWrite(kPresenterDoc()->isReadWrite());
    m_canvas->resize( m_canvas->width() - 20, m_canvas->height() - 20 );
    m_canvas->move( 20, 20 );
    h_ruler->setGeometry( 20, 0, m_canvas->width(), 20 );
    v_ruler->setGeometry( 0, 20, 20, m_canvas->height() );

    QObject::connect( h_ruler, SIGNAL( unitChanged( QString ) ),
		      this, SLOT( unitChanged( QString ) ) );
    QObject::connect( h_ruler, SIGNAL( newPageLayout( KoPageLayout ) ),
		      this, SLOT( newPageLayout( KoPageLayout ) ) );

    connect( h_ruler, SIGNAL( doubleClicked() ), this,
             SLOT( slotHRulerDoubleClicked() ) );
    connect( h_ruler, SIGNAL( doubleClicked(double) ), this,
             SLOT( slotHRulerDoubleClicked(double) ) );

    QObject::connect( v_ruler, SIGNAL( unitChanged( QString ) ),
		      this, SLOT( unitChanged( QString ) ) );
    QObject::connect( v_ruler, SIGNAL( newPageLayout( KoPageLayout ) ),
		      this, SLOT( newPageLayout( KoPageLayout ) ) );
    QObject::connect( v_ruler, SIGNAL( doubleClicked() ),
		      this, SLOT( openPageLayoutDia() ) );

    connect( h_ruler, SIGNAL( newLeftIndent( double ) ), this, SLOT( newLeftIndent( double ) ) );
    connect( h_ruler, SIGNAL( newFirstIndent( double ) ), this, SLOT( newFirstIndent( double ) ) );
    connect( h_ruler, SIGNAL( newRightIndent( double ) ), this, SLOT( newRightIndent( double ) ) );

    h_ruler->setUnit( m_pKPresenterDoc->getUnit() );
    v_ruler->setUnit( m_pKPresenterDoc->getUnit() );
}

/*==============================================================*/
void KPresenterView::unitChanged( QString u )
{
    m_pKPresenterDoc->setUnit(KoUnit::unit( u ) );
}

/*===================== set ranges of scrollbars ===============*/
void KPresenterView::setRanges()
{
    if ( vert && horz && m_canvas && m_pKPresenterDoc ) {
	vert->setSteps( 10, m_canvas->height() );
	vert->setRange( 0, QMAX( 0, m_canvas->activePage()->getZoomPageRect().height()  - m_canvas->height() ) );
	horz->setSteps( 10, m_canvas->activePage()->getZoomPageRect().width() +
			16 - m_canvas->width() );
	int range = m_canvas->activePage()->getZoomPageRect().width() +
		16 - m_canvas->width() < 0 ? 0 :
	    m_canvas->activePage()->getZoomPageRect().width() + 16 - m_canvas->width();
	horz->setRange( 0, range );
    }
}

/*==============================================================*/
void KPresenterView::skipToPage( int num )
{
    if ( num < 0 || num > static_cast<int>( m_pKPresenterDoc->getPageNums() ) - 1 || m_pKPresenterDoc->isEmbedded() || !m_canvas )
	return;
    m_canvas->exitEditMode();
    vert->setValue( 0 );
    currPg = num;
    emit currentPageChanged( currPg );
    if( sidebar )
        sidebar->setCurrentPage( currPg );
    m_canvas->setActivePage( m_pKPresenterDoc->pageList().at(currPg));
    if ( notebar ) {
        kdDebug() << "Current Page: " << currPg << endl;
        QString text = m_pKPresenterDoc->pageList().at(currPg)->getNoteText( );
        notebar->setCurrentNoteText( text );
    }
    refreshPageButton();
    //(Laurent) deselect object when we change page.
    //otherwise you can change object properties on other page
    m_canvas->deSelectAllObj();
    m_canvas->repaint( FALSE );
}


void KPresenterView::refreshPageButton()
{
    bool state = (currPg > 0);
    pgPrev->setEnabled( state );
    actionScreenFirst->setEnabled(state);
    actionScreenPrev->setEnabled(state);
    state=(currPg < (int)m_pKPresenterDoc->getPageNums() - 1);
    pgNext->setEnabled( state );
    actionScreenLast->setEnabled(state);
    actionScreenNext->setEnabled(state);
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
    if ( presStructView ) {
        delete presStructView;
        presStructView = 0;
    }
    m_canvas->deSelectAllObj();

    presStructView = new KPPresStructView( this, "", kPresenterDoc(), this );
    presStructView->setCaption( i18n( "KPresenter - Presentation Structure Viewer" ) );
    QObject::connect( presStructView, SIGNAL( presStructViewClosed() ), this, SLOT( psvClosed() ) );
    presStructView->exec();

    delete presStructView;
    presStructView = 0;
}

/*==============================================================*/
void KPresenterView::setTool( ToolEditMode toolEditMode )
{
    switch ( toolEditMode ) {
    case TEM_MOUSE:
	actionToolsMouse->setChecked( true );
	break;
    case INS_LINE:
	actionToolsLine->setChecked( true );
	break;
    case INS_RECT:
	actionToolsRectangle->setChecked( true );
	break;
    case INS_ELLIPSE:
	actionToolsCircleOrEllipse->setChecked( true );
	break;
    case INS_PIE:
	actionToolsPie->setChecked( true );
	break;
    case INS_DIAGRAMM:
	actionToolsDiagramm->setChecked( true );
	break;
    case INS_TABLE:
	actionToolsTable->setChecked( true );
	break;
    case INS_FORMULA:
	actionToolsFormula->setChecked( true );
	break;
    case INS_TEXT:
	actionToolsText->setChecked( true );
	break;
    case INS_AUTOFORM:
	actionToolsAutoform->setChecked( true );
	break;
    }
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
    return true;
}

/*================================================================*/
QWidget* KPresenterView::canvas()
{
    return m_canvas;
}

/*================================================================*/
int KPresenterView::canvasXOffset() const
{
    return m_canvas->diffx();
}

/*================================================================*/
int KPresenterView::canvasYOffset() const
{
    return m_canvas->diffy();
}

/*================================================================*/
int KPresenterView::getCurrentPresPage()
{
    if ( !presStarted )
	return -1;

    return m_canvas->presPage();
}

/*================================================================*/
int KPresenterView::getCurrentPresStep()
{
    if ( !presStarted )
	return -1;

    return m_canvas->presStep();
}

/*================================================================*/
int KPresenterView::getPresStepsOfPage()
{
    if ( !presStarted )
	return -1;

    return m_canvas->numPresSteps();
}

/*================================================================*/
int KPresenterView::getNumPresPages()
{
    if ( !presStarted )
	return -1;

    return m_canvas->numPresPages();
}

/*================================================================*/
float KPresenterView::getCurrentFaktor()
{
    if ( !presStarted )
	return 1.0;

    return m_canvas->presFakt();
}

/*================================================================*/
bool KPresenterView::gotoPresPage( int pg )
{
    if ( !presStarted )
	return false;

    m_canvas->gotoPage( pg );
    return true;
}

void KPresenterView::nextPage()
{
    if ( currPg >= (int)m_pKPresenterDoc->getPageNums() - 1 )
 	return;

    kdDebug()<<"currPg :"<<currPg<<"m_pKPresenterDoc->getPageNums() :"<<m_pKPresenterDoc->getPageNums()<<endl;
    skipToPage( currPg+1 );
}

void KPresenterView::prevPage()
{
    if ( currPg == 0 )
 	return;
    skipToPage( currPg-1 );
}

void KPresenterView::updateSideBar()
{
    if ( sidebar )
    {
        sidebar->blockSignals( TRUE );
	sidebar->thumbBar()->uptodate = false;
        sidebar->outline()->rebuildItems();
        sidebar->blockSignals( FALSE );
    }
}

void KPresenterView::updateSideBarItem( int pagenr )
{
    //kdDebug(33001) << "KPresenterView::updateSideBarItem " << pagenr << endl;
    if (sidebar)
        sidebar->updateItem( pagenr );
}

void KPresenterView::updatePageInfo()
{
  if (m_sbPageLabel)
    {
      //m_currentPage = QMIN( m_currentPage, m_doc->getPages()-1 );
      m_sbPageLabel->setText( QString(" ")+i18n("Slide %1/%2").arg(getCurrPgNum()).arg(m_pKPresenterDoc->getPageNums())+' ' );

    }
  //slotUpdateRuler();
}

void KPresenterView::updateObjectStatusBarItem()
{
  KStatusBar * sb = statusBar();
  int nbObjects = m_canvas->activePage()->objNums();

  if ( sb && nbObjects > 0 ) {
    if ( !m_sbObjectLabel ) {
      m_sbObjectLabel = sb ? new KStatusBarLabel( QString::null, 0, sb ) : 0;
      addStatusBarItem( m_sbObjectLabel );
    }

    int nbSelected = m_canvas->activePage()->numSelected();

    if (nbSelected == 1) {
      KPObject * obj = m_canvas->activePage()->getSelectedObj();
      m_sbObjectLabel->setText( i18n( "Statusbar info", "Object: %1 -  (width: %2, height: %3)" )
                                .arg(obj->getTypeString())
                                .arg(obj->getSize().width())
                                .arg(obj->getSize().height())
                                );
    }
    else
      m_sbObjectLabel->setText( i18n( "%1 objects selected" ).arg( nbSelected ) );
  }
  else if ( sb && m_sbObjectLabel ) {
    removeStatusBarItem( m_sbObjectLabel );
    delete m_sbObjectLabel;
    m_sbObjectLabel = 0L;
  }
}

void KPresenterView::pageNumChanged()
{
  updatePageInfo();
}

void KPresenterView::viewShowSideBar()
{
    if ( !sidebar )
        return;
    if ( sidebar->isVisible() )
        sidebar->hide();
    else
        sidebar->show();
}

void KPresenterView::viewShowNoteBar()
{
    if ( !notebar )
        return;
    if ( notebar->isVisible() )
        notebar->hide();
    else
        notebar->show();
}

void KPresenterView::extraChangeClip()
{
    m_canvas->chClip();
}

void KPresenterView::openPopupMenuMenuPage( const QPoint & _point )
{
    if(!koDocument()->isReadWrite() )
        return;
     static_cast<QPopupMenu*>(factory()->container("menupage_popup",this))->popup(_point);
}

void KPresenterView::openPopupMenuTextObject( const QPoint & _point )
{
    if(!koDocument()->isReadWrite() )
        return;
     static_cast<QPopupMenu*>(factory()->container("textobject_popup",this))->popup(_point);
}

void KPresenterView::openPopupMenuPartObject( const QPoint & _point )
{
    if(!koDocument()->isReadWrite() )
        return;
     static_cast<QPopupMenu*>(factory()->container("partobject_popup",this))->popup(_point);
}

void KPresenterView::openPopupMenuRectangleObject( const QPoint & _point )
{
    if(!koDocument()->isReadWrite() )
        return;
     static_cast<QPopupMenu*>(factory()->container("rectangleobject_popup",this))->popup(_point);
}

void KPresenterView::openPopupMenuGraphMenu(const QPoint & _point )
{
    if(!koDocument()->isReadWrite() )
        return;
     static_cast<QPopupMenu*>(factory()->container("graphmenu_popup",this))->popup(_point);
}


void KPresenterView::openPopupMenuPieObject( const QPoint & _point )
{
    if(!koDocument()->isReadWrite() )
        return;
     static_cast<QPopupMenu*>(factory()->container("piemenu_popup",this))->popup(_point);
}

void KPresenterView::openPopupMenuClipObject(const QPoint & _point)
{
    if(!koDocument()->isReadWrite() )
        return;
     static_cast<QPopupMenu*>(factory()->container("clipmenu_popup",this))->popup(_point);
}


void KPresenterView::openPopupMenuSideBar(const QPoint & _point)
{
    if(!koDocument()->isReadWrite() )
        return;
    static_cast<QPopupMenu*>(factory()->container("sidebarmenu_popup",this))->popup(_point);

}

void KPresenterView::openPopupMenuPicObject(const QPoint & _point)
{
    if(!koDocument()->isReadWrite() )
        return;
    static_cast<QPopupMenu*>(factory()->container("picmenu_popup",this))->popup(_point);

}

void KPresenterView::openPopupMenuPolygonObject( const QPoint &_point )
{
    if ( !koDocument()->isReadWrite() )
        return;
    static_cast<QPopupMenu*>( factory()->container( "polygonobject_popup", this ) )->popup( _point );
}

void KPresenterView::renamePageTitle()
{
    if(sidebar)
        sidebar->renamePageTitle();
}


void KPresenterView::picViewOrig640x480()
{
    m_canvas->picViewOrig640x480();
}

void KPresenterView::picViewOrig800x600()
{
    m_canvas->picViewOrig800x600();
}

void KPresenterView::picViewOrig1024x768()
{
    m_canvas->picViewOrig1024x768();
}

void KPresenterView::picViewOrig1280x1024()
{
    m_canvas->picViewOrig1280x1024();
}

void KPresenterView::picViewOrig1600x1200()
{
    m_canvas->picViewOrig1600x1200();
}

void KPresenterView::chPic()
{
    m_canvas->chPic();
}

void KPresenterView::penColorChanged( const QPen & _pen )
{
    actionPenColor->setEnabled( true );
    actionPenColor->setCurrentColor( _pen.color() );
}

void KPresenterView::brushColorChanged( const QBrush & _brush )
{
    actionBrushColor->setEnabled( true );
    actionBrushColor->setCurrentColor(_brush.style ()==Qt::NoBrush ? Qt::white : _brush.color() );
}

void KPresenterView::autoScreenPresReStartTimer()
{
    automaticScreenPresTime.start();
    automaticScreenPresWaitTime = 0;
    automaticScreenPresTimer.changeInterval( currentTimer );
}

void KPresenterView::autoScreenPresIntervalTimer()
{
    automaticScreenPresTime.restart();
    automaticScreenPresTimer.changeInterval( currentTimer - automaticScreenPresWaitTime );
}

void KPresenterView::autoScreenPresStopTimer()
{
    automaticScreenPresTimer.stop();
    automaticScreenPresWaitTime += automaticScreenPresTime.elapsed();
}

void KPresenterView::setCurrentTimer( int _currentTimer )
{
    currentTimer = _currentTimer * 1000;
    autoScreenPresReStartTimer();
}

void KPresenterView::insertSpecialChar()
{
    KPTextView *edit=m_canvas->currentTextObjectView();
    if ( !edit )
        return;
    QString f = edit->textFontFamily();
    QChar c=' ';
    if (m_specialCharDlg==0)
    {
        m_specialCharDlg = new KoCharSelectDia( this, "insert special char", f, c, false );
        connect( m_specialCharDlg, SIGNAL(insertChar(QChar,const QString &)),
                 this, SLOT(slotSpecialChar(QChar,const QString &)));
        connect( m_specialCharDlg, SIGNAL( finished() ),
                 this, SLOT( slotSpecialCharDlgClosed() ) );
    }
    m_specialCharDlg->show();
}

void KPresenterView::slotSpecialCharDlgClosed()
{
    m_specialCharDlg = 0L;
}

void KPresenterView::slotSpecialChar(QChar c, const QString &_font)
{
    KPTextView *edit=m_canvas->currentTextObjectView();
    if ( !edit )
        return;
    edit->setFamily( _font );
    edit->insertSpecialChar(c);

}

void KPresenterView::insertLink()
{
    KPTextView *edit=m_canvas->currentTextObjectView();
    if ( !edit )
        return;
    QString link;
    QString ref;
    if(KoInsertLinkDia::createLinkDia(link, ref))
    {
        if(!link.isEmpty() && !ref.isEmpty())
            edit->insertLink(link, ref);
    }
}

void KPresenterView::changeLink()
{
    KPTextView * edit = m_canvas->currentTextObjectView();
    if ( edit )
    {
        KoLinkVariable * var=edit->linkVariable();
        if(var)
        {
            QString oldhref= var->url();
            QString oldLinkName=var->value();
            QString link=oldLinkName;
            QString ref=oldhref;
            if(KoInsertLinkDia::createLinkDia(link, ref))
            {
                if(!link.isEmpty() && !ref.isEmpty())
                {
                    KPrChangeLinkVariable*cmd=new KPrChangeLinkVariable( i18n("Change link"), m_pKPresenterDoc,oldhref, ref, oldLinkName,link, var);
                    cmd->execute();
                    m_pKPresenterDoc->addCommand(cmd);
                }
            }
        }
    }
}


void KPresenterView::showFormat( const KoTextFormat &currentFormat )
{
    actionTextFontFamily->setFont( currentFormat.font().family() );
    actionTextFontSize->setFontSize( currentFormat.font().pointSize() );
    actionTextBold->setChecked( currentFormat.font().bold());
    actionTextItalic->setChecked( currentFormat.font().italic() );
    actionTextUnderline->setChecked( currentFormat.font().underline());
    actionFormatStrikeOut->setChecked( currentFormat.font().strikeOut());

    QColor col=currentFormat.textBackgroundColor();
    actionBrushColor->setEnabled(true);
    actionBrushColor->setCurrentColor( col.isValid() ? col : QApplication::palette().color( QPalette::Active, QColorGroup::Base ));
    //actionBrushColor->setText(i18n("Text Background Color"));
    actionTextColor->setCurrentColor( currentFormat.color() );

    switch(currentFormat.vAlign())
      {
      case QTextFormat::AlignSuperScript:
	{
	  actionFormatSub->setChecked( false );
	  actionFormatSuper->setChecked( true );
	  break;
	}
      case QTextFormat::AlignSubScript:
	{
	  actionFormatSub->setChecked( true );
	  actionFormatSuper->setChecked( false );
	  break;
	}
      case QTextFormat::AlignNormal:
      default:
	{
	  actionFormatSub->setChecked( false );
	  actionFormatSuper->setChecked( false );
	  break;
	}
      }
}

void KPresenterView::slotSoftHyphen()
{
    KPTextView *edit=m_canvas->currentTextObjectView();
    if ( edit )
        edit->insertSoftHyphen();
}

void KPresenterView::extraAutoFormat()
{
    m_pKPresenterDoc->getAutoFormat()->readConfig();
    KoAutoFormatDia dia( this, 0, m_pKPresenterDoc->getAutoFormat() );
    dia.exec();
}

void KPresenterView::extraSpelling()
{
    if (m_spell.kspell) return; // Already in progress
    m_spell.macroCmdSpellCheck=0L;
    m_spell.firstSpellPage=m_pKPresenterDoc->pageList().findRef(m_canvas->activePage());
    m_spell.currentSpellPage=m_spell.firstSpellPage;

    spellAddTextObject();
    startKSpell();
}

void KPresenterView::spellAddTextObject()
{
    m_spell.spellCurrTextObjNum = -1;
    m_spell.textObject.clear();
    QPtrListIterator<KPObject> it( m_canvas->getObjectList() );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_TEXT)
            m_spell.textObject.append(dynamic_cast<KPTextObject*>( it.current() ));
    }
}

void KPresenterView::startKSpell()
{
    // m_spellCurrFrameSetNum is supposed to be set by the caller of this method
    if(m_pKPresenterDoc->getKSpellConfig() && !m_ignoreWord.isEmpty())
        m_pKPresenterDoc->getKSpellConfig()->setIgnoreList(m_ignoreWord);
    m_spell.kspell = new KSpell( this, i18n( "Spell Checking" ), this, SLOT( spellCheckerReady() ), m_pKPresenterDoc->getKSpellConfig() );


     m_spell.kspell->setIgnoreUpperWords(m_pKPresenterDoc->dontCheckUpperWord());
     m_spell.kspell->setIgnoreTitleCase(m_pKPresenterDoc->dontCheckTitleCase());

    QObject::connect( m_spell.kspell, SIGNAL( death() ),
                      this, SLOT( spellCheckerFinished() ) );
    QObject::connect( m_spell.kspell, SIGNAL( misspelling( const QString &, const QStringList &, unsigned int) ),
                      this, SLOT( spellCheckerMisspelling( const QString &, const QStringList &, unsigned int) ) );
    QObject::connect( m_spell.kspell, SIGNAL( corrected( const QString &, const QString &, unsigned int) ),
                      this, SLOT( spellCheckerCorrected( const QString &, const QString &, unsigned int ) ) );
    QObject::connect( m_spell.kspell, SIGNAL( done( const QString & ) ),
                      this, SLOT( spellCheckerDone( const QString & ) ) );
}

void KPresenterView::spellCheckerReady()
{

    for ( unsigned int i = m_spell.spellCurrTextObjNum + 1; i < m_spell.textObject.count(); i++ ) {
        KPTextObject *textobj = m_spell.textObject.at( i );
        m_spell.spellCurrTextObjNum = i; // store as number, not as pointer, to implement "go to next frameset" when done
        //kdDebug() << "KPresenterView::spellCheckerReady spell-checking frameset " << spellCurrTextObjNum << endl;

        Qt3::QTextParag * p = textobj->textDocument()->firstParag();
        QString text;
        bool textIsEmpty=true;
        while ( p ) {
            QString str = p->string()->toString();
            str.truncate( str.length() - 1 ); // damn trailing space
            if(textIsEmpty)
                textIsEmpty=str.isEmpty();
            text += str + '\n';
            p = p->next();
        }
        if(textIsEmpty)
            continue;
        text += '\n';
        m_spell.kspell->check( text );
        return;
    }
    //kdDebug() << "KPresenterView::spellCheckerReady done" << endl;
    if(!spellSwitchToNewPage())
    {
        // Done
        m_spell.kspell->cleanUp();
        delete m_spell.kspell;
        m_spell.kspell = 0;
        m_spell.firstSpellPage=-1;
        m_spell.currentSpellPage=-1;
        m_spell.textObject.clear();
        m_spell.ignoreWord.clear();
        m_spell.firstSpellPage=-1;
        if(m_spell.macroCmdSpellCheck)
            m_pKPresenterDoc->addCommand(m_spell.macroCmdSpellCheck);
    }
    else
    {
        spellAddTextObject();
        spellCheckerReady();
    }
}

bool KPresenterView::spellSwitchToNewPage()
{
    //there is not other page
    if(m_pKPresenterDoc->pageList().count()==1)
        return false;
    m_spell.currentSpellPage++;
    if( m_spell.currentSpellPage>=m_pKPresenterDoc->pageList().count())
        m_spell.currentSpellPage=0;
    if( m_spell.currentSpellPage==m_spell.firstSpellPage)
        return false;
    if ( KMessageBox::questionYesNo( this,
                                     i18n( "Do you want to spellcheck new page?") )
         != KMessageBox::Yes )
        return false;
    skipToPage(m_spell.currentSpellPage);
    return true;
}


void KPresenterView::spellCheckerMisspelling( const QString &old, const QStringList &, unsigned int pos )
{
    //kdDebug() << "KPresenterView::spellCheckerMisspelling old=" << old << " pos=" << pos << endl;
    KPTextObject * textobj = m_spell.textObject.at( m_spell.spellCurrTextObjNum ) ;
    Q_ASSERT( textobj );
    if ( !textobj ) return;
    Qt3::QTextParag * p = textobj->textDocument()->firstParag();
    while ( p && (int)pos >= p->length() )
    {
        pos -= p->length();
        p = p->next();
    }
    Q_ASSERT( p );
    if ( !p ) return;
    //kdDebug() << "KPresenterView::spellCheckerMisspelling p=" << p->paragId() << " pos=" << pos << " length=" << old.length() << endl;
    textobj->highlightPortion( p, pos, old.length(), m_canvas );
}

void KPresenterView::spellCheckerCorrected( const QString &old, const QString &corr, unsigned int pos )
{
    //kdDebug() << "KWView::spellCheckerCorrected old=" << old << " corr=" << corr << " pos=" << pos << endl;

    KPTextObject * textobj = m_spell.textObject.at( m_spell.spellCurrTextObjNum ) ;
    Q_ASSERT( textobj );
    if ( !textobj ) return;
    Qt3::QTextParag * p = textobj->textDocument()->firstParag();
    while ( p && (int)pos >= p->length() )
    {
        pos -= p->length();
        p = p->next();
    }
    Q_ASSERT( p );
    if ( !p ) return;
    textobj->highlightPortion( p, pos, old.length(), m_canvas );
    QTextCursor cursor( textobj->textDocument() );
    cursor.setParag( p );
    cursor.setIndex( pos );
    if(!m_spell.macroCmdSpellCheck)
        m_spell.macroCmdSpellCheck=new KMacroCommand(i18n("Correct misspelled word"));
    m_spell.macroCmdSpellCheck->addCommand(textobj->textObject()->replaceSelectionCommand(
        &cursor, corr, KoTextObject::HighlightSelection, QString::null ));
}

void KPresenterView::spellCheckerDone( const QString & )
{
    KPTextObject * textobj = 0L;
    if(m_spell.spellCurrTextObjNum!=-1 )
    {
        textobj = m_spell.textObject.at( m_spell.spellCurrTextObjNum ) ;
        Q_ASSERT( textobj );
        if ( textobj )
            textobj->removeHighlight();
    }

    int result = m_spell.kspell->dlgResult();

    //store ignore word
    m_spell.ignoreWord=m_spell.kspell->ksConfig().ignoreList ();

    m_spell.kspell->cleanUp();
    delete m_spell.kspell;
    m_spell.kspell = 0;

    if ( result != KS_CANCEL && result != KS_STOP )
    {
        // Try to check another frameset
        startKSpell();
    }
    else
    {
        m_spell.textObject.clear();
        m_ignoreWord.clear();
        if(m_spell.macroCmdSpellCheck)
            m_pKPresenterDoc->addCommand(m_spell.macroCmdSpellCheck);
    }
}

void KPresenterView::spellCheckerFinished()
{
    KSpell::spellStatus status = m_spell.kspell->status();
    delete m_spell.kspell;
    m_spell.kspell = 0;
    bool kspellNoConfigured=false;
    if (status == KSpell::Error)
    {
        KMessageBox::sorry(this, i18n("ISpell could not be started.\n"
                                      "Please make sure you have ISpell properly configured and in your PATH."));
        kspellNoConfigured=true;
    }
    else if (status == KSpell::Crashed)
    {
        KMessageBox::sorry(this, i18n("ISpell seems to have crashed."));
    }
    KPTextObject * textobj = 0L;
    if( m_spell.spellCurrTextObjNum!=-1 )
    {
        textobj = m_spell.textObject.at( m_spell.spellCurrTextObjNum ) ;
        Q_ASSERT( textobj );
        if ( textobj )
            textobj->removeHighlight();
    }
    m_spell.textObject.clear();
    m_ignoreWord.clear();
    if(m_spell.macroCmdSpellCheck)
        m_pKPresenterDoc->addCommand(m_spell.macroCmdSpellCheck);

    KPTextView *edit=m_canvas->currentTextObjectView();
    if (edit)
        edit->drawCursor( TRUE );
    if(kspellNoConfigured)
    {
            KPConfig configDia( this );
            configDia.openPage( KPConfig::KP_KSPELL);
            configDia.exec();
    }
}

void KPresenterView::showCounter( KoParagCounter &c )
{
    actionTextTypeUnsortList->setChecked( c.numbering() == KoParagCounter::NUM_LIST && c.style()==KoParagCounter::STYLE_DISCBULLET  );
    actionTextTypeEnumList->setChecked( c.numbering() == KoParagCounter::NUM_LIST &&c.style()== KoParagCounter::STYLE_NUM );
}

void KPresenterView::formatParagraph()
{
    showParagraphDialog();
}

void KPresenterView::showParagraphDialog(int initialPage, double initialTabPos)
{
    KPTextView *edit=m_canvas->currentTextObjectView();
    if (edit)
    {
        KoParagDia *paragDia = new KoParagDia( this, "",
                                               KoParagDia::PD_SPACING | KoParagDia::PD_ALIGN |
                                               KoParagDia::PD_BORDERS |
                                               KoParagDia::PD_NUMBERING | KoParagDia::PD_TABS |KoParagDia::PD_SHADOW , m_pKPresenterDoc->getUnit(),edit->kpTextObject()->getSize().width(),false );
        paragDia->setCaption( i18n( "Paragraph settings" ) );

        // Initialize the dialog from the current paragraph's settings
        KoParagLayout lay = static_cast<KoTextParag *>(edit->cursor()->parag())->paragLayout();
        paragDia->setParagLayout( lay );
        // Set initial page and initial tabpos if necessary
        if ( initialPage != -1 )
        {
            paragDia->setCurrentPage( initialPage );
            if ( initialPage == KoParagDia::PD_TABS )
                paragDia->tabulatorsWidget()->setCurrentTab( initialTabPos );
        }

        if(!paragDia->exec())
            return;
        KMacroCommand * macroCommand = new KMacroCommand( i18n( "Paragraph settings" ) );
        KCommand *cmd=0L;
        bool changed=false;
        if(paragDia->isLeftMarginChanged())
        {
            cmd=edit->setMarginCommand( QStyleSheetItem::MarginLeft, paragDia->leftIndent() );
            if(cmd)
            {
                macroCommand->addCommand(cmd);
                changed=true;
            }

            h_ruler->setLeftIndent( KoUnit::ptToUnit( paragDia->leftIndent(), m_pKPresenterDoc->getUnit() ) );
        }

        if(paragDia->isRightMarginChanged())
        {
            cmd=edit->setMarginCommand( QStyleSheetItem::MarginRight, paragDia->rightIndent() );
            if(cmd)
            {
                macroCommand->addCommand(cmd);
                changed=true;
            }
            h_ruler->setRightIndent( KoUnit::ptToUnit( paragDia->rightIndent(), m_pKPresenterDoc->getUnit() ) );
        }
        if(paragDia->isSpaceBeforeChanged())
        {
            cmd=edit->setMarginCommand( QStyleSheetItem::MarginTop, paragDia->spaceBeforeParag() );
            if(cmd)
            {
                macroCommand->addCommand(cmd);
                changed=true;
            }
        }
        if(paragDia->isSpaceAfterChanged())
        {
            cmd=edit->setMarginCommand( QStyleSheetItem::MarginBottom, paragDia->spaceAfterParag() );
            if(cmd)
            {
                macroCommand->addCommand(cmd);
                changed=true;
            }
        }
        if(paragDia->isFirstLineChanged())
        {
            cmd=edit->setMarginCommand( QStyleSheetItem::MarginFirstLine, paragDia->firstLineIndent());
            if(cmd)
            {
                macroCommand->addCommand(cmd);
                changed=true;
            }
            h_ruler->setFirstIndent(KoUnit::ptToUnit( paragDia->leftIndent() + paragDia->firstLineIndent(), m_pKPresenterDoc->getUnit() ) );
        }

        if(paragDia->isAlignChanged())
        {
            cmd=edit->setAlignCommand( paragDia->align() );
            if(cmd)
            {
                macroCommand->addCommand(cmd);
                changed=true;
            }
        }
        if(paragDia->isCounterChanged())
        {
            cmd=edit->setCounterCommand( paragDia->counter() );
            if(cmd)
            {
                macroCommand->addCommand(cmd);
                changed=true;
            }
        }
        if(paragDia->listTabulatorChanged())
        {
            cmd=edit->setTabListCommand( paragDia->tabListTabulator() );
            if(cmd)
            {
                macroCommand->addCommand(cmd);
                changed=true;
            }
        }

        if(paragDia->isLineSpacingChanged())
        {
            cmd=edit->setLineSpacingCommand( paragDia->lineSpacing() );
            if(cmd)
            {
                macroCommand->addCommand(cmd);
                changed=true;
            }
        }
        if(paragDia->isBorderChanged())
        {
            cmd=edit->setBordersCommand( paragDia->leftBorder(), paragDia->rightBorder(),
                              paragDia->topBorder(), paragDia->bottomBorder() );
            if(cmd)
            {
                macroCommand->addCommand(cmd);
                changed=true;
            }
        }

        if( paragDia->isShadowChanged())
        {
            cmd=edit->setShadowCommand( paragDia->shadowDistance(),paragDia->shadowDirection(), paragDia->shadowColor() );
            if(cmd)
            {
                macroCommand->addCommand(cmd);
                changed=true;
            }
        }

        if(changed)
            m_pKPresenterDoc->addCommand(macroCommand);
        else
            delete macroCommand;
        delete paragDia;
    }

}

void KPresenterView::textDefaultFormat()
{
    m_canvas->setTextDefaultFormat( );
}

void KPresenterView::changeNbOfRecentFiles(int _nb)
{
    if ( shell() ) // 0 when embedded into konq !
        shell()->setMaxRecentItems( _nb );
}

QPopupMenu * KPresenterView::popupMenu( const QString& name )
{
    Q_ASSERT(factory());
    if ( factory() )
        return ((QPopupMenu*)factory()->container( name, this ));
    return 0L;
}

void KPresenterView::addVariableActions( int type, const QStringList & texts,
                                 KActionMenu * parentMenu, const QString & menuText )
{
    // Single items go directly into parentMenu.
    // For multiple items we create a submenu.
    if ( texts.count() > 1 && !menuText.isEmpty() )
    {
        KActionMenu * subMenu = new KActionMenu( menuText, actionCollection() );
        parentMenu->insert( subMenu );
        parentMenu = subMenu;
    }
    QStringList::ConstIterator it = texts.begin();
    for ( int i = 0; it != texts.end() ; ++it, ++i )
    {
        if ( !(*it).isEmpty() ) // in case of removed subtypes or placeholders
        {
            VariableDef v;
            v.type = type;
            v.subtype = i;
            KAction * act = new KAction( (*it), 0, this, SLOT( insertVariable() ),
                                         actionCollection(), "var-action" );
            m_variableDefMap.insert( act, v );
            parentMenu->insert( act );
        }
    }
}

void KPresenterView::refreshCustomMenu()
{
    actionInsertCustom->popupMenu()->clear();
    QPtrListIterator<KoVariable> it( m_pKPresenterDoc->getVariableCollection()->getVariables() );
    KAction * act=0;
    QStringList lst;
    QString varName;
    for ( ; it.current() ; ++it )
    {
        KoVariable *var = it.current();
        if ( var->type() == VT_CUSTOM )
        {
            varName=( (KoCustomVariable*) var )->name();
            if ( !lst.contains( varName) )
            {
                 lst.append( varName );
                 act = new KAction( varName, 0, this, SLOT( insertCustomVariable() ),
                                    actionCollection(), "custom-action" );
                 actionInsertCustom->insert( act );
            }
        }
    }
    bool state=!lst.isEmpty();
    if(state)
        actionInsertCustom->popupMenu()->insertSeparator();

    act = new KAction( i18n("New..."), 0, this, SLOT( insertNewCustomVariable() ), actionCollection(), "custom-action" );
    actionInsertCustom->insert( act );

    actionInsertCustom->popupMenu()->insertSeparator();

    actionEditCustomVars = new KAction( i18n( "&Custom Variables..." ), 0,
                                        this, SLOT( editCustomVars() ),
                                        actionCollection(), "edit_customvars" );
    actionEditCustomVars->setEnabled(state);
    actionInsertCustom->insert( actionEditCustomVars );
}


void KPresenterView::insertCustomVariable()
{
    KPTextView *edit=m_canvas->currentTextObjectView();
    if ( edit )
    {
        KAction * act = (KAction *)(sender());
        edit->insertCustomVariable(act->text());
    }
}

void KPresenterView::insertNewCustomVariable()
{
    KPTextView *edit=m_canvas->currentTextObjectView();
    if ( edit )
        edit->insertVariable( VT_CUSTOM, 0 );
}

void KPresenterView::editCustomVars()
{
    KoCustomVariablesDia dia( this, m_pKPresenterDoc->getVariableCollection()->getVariables() );
    QStringList listOldCustomValue;
    QPtrListIterator<KoVariable> oldIt( m_pKPresenterDoc->getVariableCollection()->getVariables() );
    for ( ; oldIt.current() ; ++oldIt )
    {
        if(oldIt.current()->type()==VT_CUSTOM)
            listOldCustomValue.append(((KoCustomVariable*)oldIt.current())->value());
    }
    if(dia.exec())
    {
        m_pKPresenterDoc->recalcVariables( VT_CUSTOM );
        //temporaly hack, for the moment we can't undo/redo change custom variables
        QPtrListIterator<KoVariable> it( m_pKPresenterDoc->getVariableCollection()->getVariables() );
        KMacroCommand * macroCommand = 0L;
        int i=0;
        for ( ; it.current() ; ++it )
        {
            if(it.current()->type() == VT_CUSTOM )
            {
                if(((KoCustomVariable*)it.current())->value()!=*(listOldCustomValue.at(i)))
                {
                    if(!macroCommand)
                        macroCommand = new KMacroCommand( i18n( "Change custom variable" ) );
                    KPrChangeCustomVariableValue *cmd=new KPrChangeCustomVariableValue(i18n( "Change custom variable" ),m_pKPresenterDoc,*(listOldCustomValue.at(i)), ((KoCustomVariable*)it.current())->value() ,((KoCustomVariable*)it.current()));
                    macroCommand->addCommand(cmd);
                }
                i++;
            }
        }
        if(macroCommand)
            m_pKPresenterDoc->addCommand(macroCommand);
    }

}

void KPresenterView::insertVariable()
{
    KPTextView *edit=m_canvas->currentTextObjectView();
    if ( edit )
    {
        KAction * act = (KAction *)(sender());
        VariableDefMap::ConstIterator it = m_variableDefMap.find( act );
        if ( it == m_variableDefMap.end() )
            kdWarning() << "Action not found in m_variableDefMap." << endl;
        else
        {
            edit->insertVariable( (*it).type, (*it).subtype );
        }
    }
}

void KPresenterView::openLink()
{
    KPTextView *edit=m_canvas->currentTextObjectView();
    if ( edit )
        edit->openLink();
}

void KPresenterView::showRulerIndent( double _leftMargin, double _firstLine, double _rightMargin )
{
  KoRuler * hRuler = getHRuler();
  if ( hRuler )
  {
      hRuler->setFirstIndent( KoUnit::ptToUnit( _firstLine + _leftMargin, m_pKPresenterDoc->getUnit() ) );
      hRuler->setLeftIndent( KoUnit::ptToUnit( _leftMargin, m_pKPresenterDoc->getUnit() ) );
      hRuler->setRightIndent( KoUnit::ptToUnit( _rightMargin, m_pKPresenterDoc->getUnit() ) );
      actionTextDepthMinus->setEnabled( _leftMargin>0);
  }
}

void KPresenterView::tabListChanged( const KoTabulatorList & tabList )
{
    if(!m_pKPresenterDoc->isReadWrite())
        return;

    m_canvas->setTabList( tabList );
}

void KPresenterView::newFirstIndent( double _firstIndent )
{
    m_canvas->setNewFirstIndent(_firstIndent);
}

void KPresenterView::newLeftIndent( double _leftIndent)
{
    m_canvas->setNewLeftIndent(_leftIndent);
}

void KPresenterView::newRightIndent( double _rightIndent)
{
    m_canvas->setNewRightIndent(_rightIndent);
}

void KPresenterView::slotUpdateRuler()
{
    // Set the "frame start" in the ruler (tabs are relative to that position)
    KPTextView *edit=m_canvas->currentTextObjectView();
    bool isText=!m_canvas->applicableTextObjects().isEmpty();
    if ( isText )
    {
        KPTextObject *txtobj= m_canvas->applicableTextObjects().first();
        if ( txtobj )
        {
            QRect r= zoomHandler()->zoomRect(txtobj->getBoundingRect(zoomHandler() ));
            getHRuler()->setFrameStartEnd( r.left() /*+ m_canvas->diffx()*//*- pc.x()*/, r.right()/*+m_canvas->diffx()*/ /*- pc.x()*/ );
            getVRuler()->setFrameStartEnd( r.top()/*+ m_canvas->diffy()*//*- pc.y()*/, r.bottom()/*+m_canvas->diffy()*//*- pc.y()*/ );
            if( getHRuler())
            {
                getHRuler()->changeFlags(KoRuler::F_INDENTS | KoRuler::F_TABS);
                getHRuler()->repaint();
            }
        }
    }
    else
    {
        if( getHRuler())
        {
            getHRuler()->changeFlags(0);
            getHRuler()->repaint();
        }
        updateRuler();
    }
}


void KPresenterView::slotHRulerDoubleClicked( double ptpos )
{
    showParagraphDialog( KoParagDia::PD_TABS, ptpos );
}

void KPresenterView::slotHRulerDoubleClicked()
{
    if ( getHRuler()->flags() & KoRuler::F_TABS )
        formatParagraph();
    else
        openPageLayoutDia();
}

void KPresenterView::changeCaseOfText()
{
    KPTextView *edit=m_canvas->currentTextObjectView();
    if(!edit)
        return;
    KoChangeCaseDia *caseDia=new KoChangeCaseDia( this,"change case" );
    if(caseDia->exec())
    {
        edit->changeCaseOfText(caseDia->getTypeOfCase());
    }
    delete caseDia;
}


void KPresenterView::editFind()
{
    // Already a find or replace running ?
    if ( m_findReplace )
    {
        m_findReplace->setActiveWindow();
        return;
    }

    if (!m_searchEntry)
        m_searchEntry = new KoSearchContext();
    KPTextView * edit = m_canvas->currentTextObjectView();
    bool hasSelection=edit && (edit->kpTextObject())->textObject()->hasSelection();
    KoSearchDia dialog( m_canvas, "find", m_searchEntry,hasSelection );
    m_searchPage=m_pKPresenterDoc->pageList().findRef(m_canvas->activePage());
    if ( dialog.exec() == QDialog::Accepted )
    {
        m_findReplace = new KPrFindReplace( m_canvas, &dialog,edit ,m_canvas->objectText());
        doFindReplace();
    }
}

void KPresenterView::editReplace()
{
    // Already a find or replace running ?
    if ( m_findReplace )
    {
        m_findReplace->setActiveWindow();
        return;
    }

    if (!m_searchEntry)
        m_searchEntry = new KoSearchContext();
    if (!m_replaceEntry)
        m_replaceEntry = new KoSearchContext();

    KPTextView * edit = m_canvas->currentTextObjectView();
    bool hasSelection=edit && (edit->kpTextObject())->textObject()->hasSelection();
    KoReplaceDia dialog( m_canvas, "replace", m_searchEntry, m_replaceEntry,hasSelection );
    QPtrList<KoTextObject> list=m_canvas->objectText();
    if( list.count()==0)
    {
        KMessageBox::sorry( this, i18n( "Sorry, there is not text object!" ) );
        return;
    }
    m_searchPage=m_pKPresenterDoc->pageList().findRef(m_canvas->activePage());
    if ( dialog.exec() == QDialog::Accepted )
    {
        kdDebug() << "KPresenterView::editReplace" << endl;
        m_findReplace = new KPrFindReplace( m_canvas, &dialog,edit ,m_canvas->objectText());
        doFindReplace();
    }
}

bool KPresenterView::searchInOtherPage()
{
    //there is not other page
    if(m_pKPresenterDoc->pageList().count()==1)
        return false;
    m_searchPage++;
    if( m_searchPage>=m_pKPresenterDoc->pageList().count())
        m_searchPage=0;
    if( m_searchPage==m_initSearchPage)
        return false;
    if ( KMessageBox::questionYesNo( this,
                                     i18n( "Do you want to search in new page?") )
         != KMessageBox::Yes )
        return false;
    skipToPage(m_searchPage);
    return true;
}

void KPresenterView::doFindReplace()
{
    KPrFindReplace* findReplace = m_findReplace; // keep a copy. "this" might be deleted before we exit this method
    m_searchPage=m_pKPresenterDoc->pageList().findRef(m_canvas->activePage());
    m_initSearchPage=m_searchPage;
    findReplace->proceed();

    bool aborted = findReplace->aborted();
    while(!aborted && searchInOtherPage() )
    {
        m_findReplace->changeListObject(m_canvas->objectText());
        findReplace->proceed();
        aborted = findReplace->aborted();
    }
    m_searchPage=-1;
    m_initSearchPage=-1;
    delete findReplace;
    if ( !aborted ) // Only if we still exist....
        m_findReplace = 0L;
}

void KPresenterView::refreshAllVariable()
{
    m_pKPresenterDoc->recalcVariables( VT_ALL );
}

void KPresenterView::changeZoomMenu( int zoom )
{
    QStringList lst;
    if(zoom>0)
    {
	if( lst.contains( i18n( "Zoom to width" ) ) == 0 )
	    lst << i18n( "Zoom to width" );
        if( lst.contains( i18n( "Zoom to Whole Page" ) )==0)
            lst << i18n( "Zoom to Whole Page" );
        QValueList<int> list;
        QString z;
        int val;
        bool ok;
        QStringList itemsList = actionViewZoom->items();
        for (QStringList::Iterator it = itemsList.begin() ; it != itemsList.end() ; ++it)
        {
            z = (*it).replace( QRegExp( "%" ), "" );
            z = z.simplifyWhiteSpace();
            val=z.toInt(&ok);
            //zoom : limit inferior=10
            if(ok && val>9  &&list.contains(val)==0)
                list.append( val );
        }
        //necessary at the beginning when we read config
        //this value is not in combo list
        if(list.contains(zoom)==0)
            list.append( zoom );

        qHeapSort( list );

        for (QValueList<int>::Iterator it = list.begin() ; it != list.end() ; ++it)
            lst.append( (QString::number(*it)+'%') );
    }
    else
    {
          lst << i18n( "Zoom to width" );
          lst << i18n( "Zoom to Whole Page" );
          lst << "33%";
          lst << "50%";
          lst << "75%";
          lst << "100%";
          lst << "125%";
          lst << "150%";
          lst << "200%";
          lst << "250%";
          lst << "350%";
          lst << "400%";
          lst << "450%";
          lst << "500%";
    }
    actionViewZoom->setItems( lst );
}

void KPresenterView::showZoom( int zoom )
{
    QStringList list = actionViewZoom->items();
    QString zoomStr = QString::number( zoom ) + '%';
    actionViewZoom->setCurrentItem( list.findIndex(zoomStr)  );
}

void KPresenterView::viewZoom( const QString &s )
{
    QString z( s );
    bool ok=false;
    int zoom = 0;
    if ( z == i18n("Zoom to width") )
    {
        zoom = qRound( static_cast<double>(m_canvas->visibleRect().width() * 100 ) / (zoomHandler()->resolutionX() * m_pKPresenterDoc->pageLayout().ptWidth ) );
        ok = true;
    }
    else if ( z == i18n("Zoom to Whole Page") )
    {
        double height = zoomHandler()->resolutionY() * m_pKPresenterDoc->pageLayout().ptHeight;
        double width = zoomHandler()->resolutionX() * m_pKPresenterDoc->pageLayout().ptWidth;
        zoom = QMIN( qRound( static_cast<double>(m_canvas->visibleRect().height() * 100 ) / height ),
                     qRound( static_cast<double>(m_canvas->visibleRect().width() * 100 ) / width ) );
        ok = true;
    }
    else
    {
    	z = z.replace( QRegExp( "%" ), "" );
    	z = z.simplifyWhiteSpace();
    	zoom = z.toInt(&ok);
    }
    if( !ok || zoom<10 ) //zoom should be valid and >10
        zoom = zoomHandler()->zoom();
    //refresh menu
    changeZoomMenu( zoom );
    //refresh menu item
    showZoom(zoom);
    //apply zoom if zoom!=m_doc->zoom()
    if( zoom != zoomHandler()->zoom() )
    {
        setZoom( zoom, true );
#if 0
        KWTextFrameSetEdit * edit = currentTextEdit();
        if ( edit )
            edit->ensureCursorVisible();
#endif
    }

    m_canvas->setFocus();
    m_canvas->repaint();
}


void KPresenterView::setZoom( int zoom, bool updateViews )
{
    m_pKPresenterDoc->zoomHandler()->setZoomAndResolution( zoom, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY());
    m_pKPresenterDoc->newZoomAndResolution(updateViews,false);
    m_pKPresenterDoc->updateZoomRuler();

    // Also set the zoom in KoView (for embedded views)
    //kdDebug() << "KWView::showZoom setting koview zoom to " << m_doc->zoomedResolutionY() << endl;
    KoView::setZoom( zoomHandler()->zoomedResolutionY() /* KoView only supports one zoom */ );
    setRanges();
}

void KPresenterView::slotUpdateScrollBarRanges()
{
    setRanges();
}

KoZoomHandler *KPresenterView::zoomHandler()
{
    return m_pKPresenterDoc->zoomHandler();
}

#include <kpresenter_view.moc>
