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
#include <kppartobject.h>
#include <textdialog.h>
#include <sidebar.h>
#include <notebar.h>
#include <insertpagedia.h>
#include <koPictureFilePreview.h>

#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kstdaction.h>
#include <kapplication.h>
#include <kio/netaccess.h>

#include <kpresenter_view.h>
#include <page.h>
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
    xOffset = 0;
    yOffset = 0;
    v_ruler = 0;
    h_ruler = 0;
    searchDialog=0L;
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
    page = 0L;
    m_spell.kspell = 0;
    automaticScreenPresFirstTimer = true;

    m_pKPresenterDoc = _doc;
    // Currently unused (formatting needs a zoom handler, so we use the one in KPresenterDocument)
    m_zoomHandler = new KoZoomHandler();

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
        connect( page, SIGNAL(selectionChanged(bool)),
                 actionEditCut, SLOT(setEnabled(bool)) );
    }
    else
    {
        actionEditCut->setEnabled( false );
    }
    connect( page, SIGNAL(selectionChanged(bool)),
             actionEditCopy, SLOT(setEnabled(bool)) );

    connect (page, SIGNAL(selectionChanged(bool)),
             actionChangeCase, SLOT(setEnabled(bool)));

    connect( h_ruler, SIGNAL( tabListChanged( const KoTabulatorList & ) ), this,
             SLOT( tabListChanged( const KoTabulatorList & ) ) );

    //statusbar stuff
    connect( m_pKPresenterDoc, SIGNAL( pageNumChanged() ), this, SLOT( pageNumChanged()) );
    connect( this, SIGNAL( currentPageChanged(int) ), this, SLOT( pageNumChanged()) );
    connect( page, SIGNAL( objectSelectedChanged() ), this, SLOT( updateObjectStatusBarItem() ));

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
        KPTextObject * objtxt = m_spell.textObject.at( m_spell.spellCurrTextObjNum ) ;
        Q_ASSERT( objtxt );
        if ( objtxt )
            objtxt->removeHighlight();
        delete m_spell.kspell;
    }

    delete searchDialog;
    delete presStructView;
    delete rb_oalign;
    delete rb_lbegin;
    delete rb_lend;
    delete dcop;
    delete page; // it's a child widget, but it emits a signal on destruction
    delete m_zoomHandler;
    delete m_specialCharDlg;
    delete m_sbPageLabel;
    delete notebar;
}

/*=========================== file print =======================*/
void KPresenterView::setupPrinter( KPrinter &prt )
{
    m_pKPresenterDoc->recalcVariables(  VT_TIME );
    m_pKPresenterDoc->recalcVariables(  VT_DATE );
    prt.setMinMax( 1, m_pKPresenterDoc->getPageNums() );
    prt.setFromTo( 1, m_pKPresenterDoc->getPageNums() );
    prt.setOption( "kde-range", m_pKPresenterDoc->selectedForPrinting() );
    KoFormat pageFormat = m_pKPresenterDoc->pageLayout().format;
    prt.setPageSize( static_cast<KPrinter::PageSize>( KoPageFormat::printerPageSize( pageFormat ) ) );

    if ( m_pKPresenterDoc->pageLayout().orientation == PG_LANDSCAPE || pageFormat == PG_SCREEN )
        prt.setOrientation( KPrinter::Landscape );
    else
        prt.setOrientation( KPrinter::Portrait );
}

void KPresenterView::print( KPrinter &prt )
{
    float left_margin = 0.0;
    float top_margin = 0.0;

    if ( m_pKPresenterDoc->pageLayout().format == PG_SCREEN )
    {
        left_margin = 28.5;
        top_margin = 15.0;
    }

    page->deSelectAllObj();
    QPainter painter;
    painter.begin( &prt );
    page->print( &painter, &prt, left_margin, top_margin );
    painter.end();
}

/*===============================================================*/
void KPresenterView::editCut()
{
    if ( !page->currentTextObjectView() ) {
	page->setToolEditMode( TEM_MOUSE );
	m_pKPresenterDoc->copyObjs( xOffset, yOffset );
	m_pKPresenterDoc->deleteObjs();
    } else {
	page->currentTextObjectView()->cut();
    }
}

/*===============================================================*/
void KPresenterView::editCopy()
{
    if ( !page->currentTextObjectView() ) {
	page->setToolEditMode( TEM_MOUSE );
	m_pKPresenterDoc->copyObjs( xOffset, yOffset );
    } else {
	page->currentTextObjectView()->copy();
    }
}

/*===============================================================*/
void KPresenterView::editPaste()
{
    if ( !page->currentTextObjectView() ) {
        page->setToolEditMode( TEM_MOUSE );
        page->deSelectAllObj();
        QMimeSource *data = QApplication::clipboard()->data();
        if ( data->provides( "text/uri-list" ) )
        {
            m_pKPresenterDoc->pastePage( data, currPg );
        }
        else if ( data->provides( "application/x-kpresenter-selection" ) )
        {
            m_pKPresenterDoc->pasteObjs( data->encodedData("application/x-kpresenter-selection"),
                                         xOffset, yOffset, currPg );

            page->setMouseSelectedObject(true);
            emit objectSelectedChanged();
        }
        else if (QImageDrag::canDecode (data)) {
            page->setToolEditMode( TEM_MOUSE );
            page->deSelectAllObj();

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
            m_pKPresenterDoc->insertPicture( tmpFile.name(), xOffset, yOffset );
            setCursor( c );
        }
    } else {
        page->currentTextObjectView()->paste();
    }
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
    KPTextView *edit=page->currentTextObjectView();
    if ( !edit ) {
	page->setToolEditMode( TEM_MOUSE );
	page->selectAllObj();
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
    actionEditDelPage->setEnabled( m_pKPresenterDoc->getPageNums() > 1 );
}

/*===============================================================*/
void KPresenterView::editDelPage()
{
    if ( KMessageBox::questionYesNo( this,
                                     i18n( "Do you want to remove the current page?\n"
                                           "This operation cannot be undone.") )
         != KMessageBox::Yes )
        return;
    page->exitEditMode();
    m_pKPresenterDoc->deletePage( currPg );
    setRanges();
    currPg = QMIN( currPg, (int)m_pKPresenterDoc->getPageNums() - 1 );
    skipToPage( currPg );
    actionEditDelPage->setEnabled(  m_pKPresenterDoc->getPageNums() > 1 );
}

/*===============================================================*/
void KPresenterView::editFind()
{
    if ( !searchDialog ) {
	searchDialog = new SearchDialog( this, 0, FALSE );
	connect( searchDialog->buttonFind, SIGNAL( clicked() ),
		 this, SLOT( search() ) );
    }
    searchDialog->lineEdit->setFocus();
    searchDialog->show();
    searchDialog->raise();
}

/*===============================================================*/
void KPresenterView::editHeaderFooter()
{
    page->setToolEditMode( TEM_MOUSE );
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
    int pg = m_pKPresenterDoc->insertPage( currPg, pos, dia.radioDifferent->isChecked(), QString::null );
    setRanges();
    if ( pg != -1 )
	skipToPage( pg );
    actionEditDelPage->setEnabled( m_pKPresenterDoc->getPageNums() > 1 );
}

/*====================== insert a picture =======================*/
void KPresenterView::insertPicture()
{
    page->setToolEditMode( TEM_MOUSE );
    page->deSelectAllObj();

    //url = KFileDialog::getImageOpenURL(); lukas: put this back in KDE 3.0

    KFileDialog fd( QString::null, KImageIO::pattern(KImageIO::Reading), 0, 0, true );
    fd.setCaption(i18n("Insert Picture"));
    fd.setPreviewWidget( new KoPictureFilePreview( &fd ) );

    KURL url;
    if ( fd.exec() == QDialog::Accepted )
        url = fd.selectedURL();

    if( url.isEmpty() || !url.isValid())
        return;

    QString file;
    if (!KIO::NetAccess::download( url, file ))
        return;

    QCursor c = page->cursor();
    page->setCursor( waitCursor );
    if ( !file.isEmpty() )
        m_pKPresenterDoc->insertPicture( file, xOffset, yOffset );
    page->setCursor( c );
}

/*====================== insert a picture (w/o filedialog) =======================*/
void KPresenterView::insertPicture(const QString &file)
{
    page->setToolEditMode( TEM_MOUSE );
    page->deSelectAllObj();

    QCursor c = page->cursor();
    page->setCursor( waitCursor );
    if ( !file.isEmpty() ) m_pKPresenterDoc->insertPicture( file, xOffset, yOffset );
    page->setCursor( c );
}

/*====================== insert a clipart =======================*/
void KPresenterView::insertClipart()
{
    page->setToolEditMode( TEM_MOUSE );
    page->deSelectAllObj();

    KFileDialog fd( QString::null, KoPictureFilePreview::clipartPattern(), 0, 0, true );
    fd.setCaption(i18n("Insert Clipart"));
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
        m_pKPresenterDoc->insertClipart( file, xOffset, yOffset );
}

/*==============================================================*/
void KPresenterView::toolsMouse()
{
    if ( actionToolsMouse ->isChecked() )
        page->setToolEditMode( TEM_MOUSE, false );
    else
        actionToolsMouse->setChecked(true);
    //page->deSelectAllObj();
}

/*=========================== insert line =======================*/
void KPresenterView::toolsLine()
{
    if ( actionToolsLine->isChecked() )
    {
        page->setToolEditMode( INS_LINE, false );
        page->deSelectAllObj();
    }
    else
        actionToolsLine->setChecked(true);
}

/*===================== insert rectangle ========================*/
void KPresenterView::toolsRectangle()
{
    if ( actionToolsRectangle->isChecked() )
    {
        page->deSelectAllObj();
        page->setToolEditMode( INS_RECT, false );
    }
    else
        actionToolsRectangle->setChecked(true);
}

/*===================== insert circle or ellipse ================*/
void KPresenterView::toolsCircleOrEllipse()
{
    if ( actionToolsCircleOrEllipse->isChecked() )
    {
        page->deSelectAllObj();
        page->setToolEditMode( INS_ELLIPSE, false );
    }
    else
        actionToolsCircleOrEllipse->setChecked(true);
}

/*==============================================================*/
void KPresenterView::toolsPie()
{
    if ( actionToolsPie->isChecked() )
    {
        page->deSelectAllObj();
        page->setToolEditMode( INS_PIE, false );
    }
    else
        actionToolsPie->setChecked(true);
}

/*==============================================================*/
void KPresenterView::toolsDiagramm()
{
    if ( actionToolsDiagramm->isChecked() )
    {
        page->deSelectAllObj();
        page->setToolEditMode( INS_DIAGRAMM, false );

        KoDocumentEntry entry = KoDocumentEntry::queryByMimeType( "application/x-kchart" );
        if (entry.isEmpty())
        {
            KMessageBox::sorry( this, i18n( "Sorry, no chart component registered" ) );
            page->setToolEditMode( TEM_MOUSE );
        }
        else
            page->setPartEntry( entry );
    }
    else
        actionToolsDiagramm->setChecked(true);
}

/*==============================================================*/
void KPresenterView::toolsTable()
{
    if ( actionToolsTable->isChecked() )
    {
        page->deSelectAllObj();
        page->setToolEditMode( INS_TABLE, false );

        KoDocumentEntry entry = KoDocumentEntry::queryByMimeType( "application/x-kspread" );
        if (entry.isEmpty())
        {
            KMessageBox::sorry( this, i18n( "Sorry, no table component registered" ) );
            page->setToolEditMode( TEM_MOUSE );
        }
        else
            page->setPartEntry( entry );
    }
    else
        actionToolsTable->setChecked(true);
}

/*==============================================================*/
void KPresenterView::toolsFormula()
{
    if ( actionToolsFormula->isChecked() )
    {
        page->deSelectAllObj();
        page->setToolEditMode( INS_FORMULA, false );

        KoDocumentEntry entry = KoDocumentEntry::queryByMimeType( "application/x-kformula" );
        if (entry.isEmpty())
        {
            KMessageBox::sorry( this, i18n( "Sorry, no formula component registered" ) );
            page->setToolEditMode( TEM_MOUSE );
        }
        else
            page->setPartEntry( entry );
    }
    else
        actionToolsFormula->setChecked(true);
}

/*===================== insert a textobject =====================*/
void KPresenterView::toolsText()
{
    if ( actionToolsText->isChecked() )
    {
        page->deSelectAllObj();
        page->setToolEditMode( INS_TEXT, false );
    }
    else
        actionToolsText->setChecked(true);
}

/*===============================================================*/
void KPresenterView::toolsAutoform()
{
    if ( actionToolsAutoform->isChecked() )
    {
        page->deSelectAllObj();
        page->setToolEditMode( TEM_MOUSE, false );
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
        QObject::connect( afChoose, SIGNAL( afchooseCanceled()),
                          this,SLOT(slotAfchooseCanceled()));
        afChoose->show();
    }
    else
        actionToolsAutoform->setChecked(true);
}

/*===============================================================*/
void KPresenterView::toolsObject()
{
    KoDocumentEntry pe = actionToolsObject->documentEntry();
    if ( pe.isEmpty() ) {
        page->setToolEditMode( TEM_MOUSE );
        return;
    }

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
    styleDia->setSticky( m_pKPresenterDoc->getSticky( sticky ) );
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
    backDia = new BackDia( this, "InfoDia", m_pKPresenterDoc->getBackType( currPg ),
			   m_pKPresenterDoc->getBackColor1( currPg ),
			   m_pKPresenterDoc->getBackColor2( currPg ),
			   m_pKPresenterDoc->getBackColorType( currPg ),
			   m_pKPresenterDoc->getBackPixKey( currPg ).filename(),
                           m_pKPresenterDoc->getBackPixKey( currPg ).lastModified(),
			   m_pKPresenterDoc->getBackClipKey( currPg ).filename(),
			   m_pKPresenterDoc->getBackClipKey( currPg ).lastModified(),
			   m_pKPresenterDoc->getBackView( currPg ),
			   m_pKPresenterDoc->getBackUnbalanced( currPg ),
			   m_pKPresenterDoc->getBackXFactor( currPg ),
			   m_pKPresenterDoc->getBackYFactor( currPg ),
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

    if ( KoPageLayoutDia::pageLayout( pgLayout, hf, FORMAT_AND_BORDERS ) ) {
	PgLayoutCmd *pgLayoutCmd = new PgLayoutCmd( i18n( "Set Page Layout" ),
						    pgLayout, oldLayout, this );
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
    QPixmap pix( QSize( m_pKPresenterDoc->getPageRect( 0, 0, 0 ).width(),
			m_pKPresenterDoc->getPageRect( 0, 0, 0 ).height() ) );
    pix.fill( Qt::white );
    int i = getCurrPgNum() - 1;
    page->drawPageInPix2( pix, i * m_pKPresenterDoc->getPageRect( 0, 0, 0 ).height(), i );

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
	   i18n( "Do you want to load a configuration, that you have saved earlier,\n"
		 "which should be used for this HTML Presentation?" ),
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
    objectSelectedChanged();
}

/*===============================================================*/
void KPresenterView::extraUnGroup()
{
    page->setToolEditMode( TEM_MOUSE );
    m_pKPresenterDoc->ungroupObjects();
    objectSelectedChanged();
}

/*===============================================================*/
void KPresenterView::extraPenStyle()
{
    page->setToolEditMode( TEM_MOUSE );
    QPoint pnt( QCursor::pos() );
    rb_pstyle->popup( pnt );
}

/*===============================================================*/
void KPresenterView::extraPenWidth()
{
    page->setToolEditMode( TEM_MOUSE );
    QPoint pnt( QCursor::pos() );
    rb_pwidth->popup( pnt );
}


/*========================== screen config pages ================*/
void KPresenterView::screenConfigPages()
{
    pgConfDia = new PgConfDia( this, "PageConfig", kPresenterDoc()->spInfinitLoop(),
			       kPresenterDoc()->spManualSwitch(), getCurrPgNum(),
			       kPresenterDoc()->backgroundList()->at( getCurrPgNum() - 1 )->getPageEffect(),
			       kPresenterDoc()->getPresSpeed(),
			       kPresenterDoc()->backgroundList()->at( getCurrPgNum() - 1 )->getPageTimer(),
			       kPresenterDoc()->backgroundList()->at( getCurrPgNum() - 1 )->getPageSoundEffect(),
			       kPresenterDoc()->backgroundList()->at( getCurrPgNum() - 1 )->getPageSoundFileName() );
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
//     if ( !presStructView ) {
	page->deSelectAllObj();
	page->setToolEditMode( TEM_MOUSE );

	presStructView = new KPPresStructView( this, "", kPresenterDoc(), this );
	presStructView->setCaption( i18n( "KPresenter - Presentation Structure Viewer" ) );
	QObject::connect( presStructView, SIGNAL( presStructViewClosed() ), this, SLOT( psvClosed() ) );
	presStructView->show();
//     }
}

/*===============================================================*/
void KPresenterView::screenAssignEffect()
{
    page->setToolEditMode( TEM_MOUSE );

    QPtrList<KPObject> objs;
    if ( page->canAssignEffect( objs ) ) {
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
    page->setToolEditMode( TEM_MOUSE );

    if ( page && !presStarted ) {
	// disable screensaver
	QString pidFile = QDir::homeDirPath();
	pidFile += "/.kss.pid";
	FILE *fp;
	if ( ( fp = fopen( QFile::encodeName(pidFile), "r" ) ) != NULL ) {
	    fscanf( fp, "%d", &screensaver_pid );
	    fclose( fp );
	    kill( screensaver_pid, SIGSTOP );
	}

	page->deSelectAllObj();
	presStarted = true;
	int deskw = QApplication::desktop()->width();
	int deskh = QApplication::desktop()->height();
        QRect pgRect = kPresenterDoc()->getPageRect( 0, 0, 0, 1.0, false );
	float _presFaktW = static_cast<float>( deskw ) /
			   static_cast<float>( pgRect.width() ) >
			   1.0 ?
			   static_cast<float>( deskw ) /
			   static_cast<float>( pgRect.width() )
			   : 1.0;
	float _presFaktH = static_cast<float>( deskh ) /
			   static_cast<float>( pgRect.height() ) >
			   1.0 ? static_cast<float>( deskh ) /
			   static_cast<float>( pgRect.height() )
			   : 1.0;
	float _presFakt = QMIN(_presFaktW,_presFaktH);
        kdDebug(33001) << "KPresenterView::startScreenPres page->setPresFakt " << _presFakt << endl;

	xOffsetSaved = xOffset;
	yOffsetSaved = yOffset;
	xOffset = 0;
	yOffset = 0;

        // Center the slide in the screen, if it's smaller...
        pgRect = kPresenterDoc()->getPageRect( 0, 0, 0, _presFakt, false );
        kdDebug(33001) << "                                pgRect: " << pgRect.x() << "," << pgRect.y()
                  << " " << pgRect.width() << "x" << pgRect.height() << endl;
	if ( deskw > pgRect.width() )
	    xOffset -= ( deskw - pgRect.width() ) / 2;
	if ( deskh > pgRect.height() )
	    yOffset -= ( deskh - pgRect.height() ) / 2;

	vert->setEnabled( false );
	horz->setEnabled( false );
	m_bShowGUI = false;
        page->reparent( ( QWidget* )0L, 0, QPoint( 0, 0 ), FALSE );
	page->showFullScreen();
	page->setFocusPolicy( QWidget::StrongFocus );
	//page->setFocus(); done in gotoPage
	page->startScreenPresentation( _presFakt, pgNum );

	actionScreenStart->setEnabled( false );
	actionScreenViewPage->setEnabled( false );

	if ( !kPresenterDoc()->spManualSwitch() && pgNum == -1 ) {
	    continuePres = true;
	    exitPres = false;
            page->repaint( false );

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
        page->setNextPageTimer( true );
        page->stopSound();
        page->showNormal();
        page->hide();
        page->reparent( pageBase, 0, QPoint( 0, 0 ), true );
        page->lower();
	xOffset = xOffsetSaved;
	yOffset = yOffsetSaved;
	page->stopScreenPresentation();
	presStarted = false;
	vert->setEnabled( true );
	horz->setEnabled( true );
	m_bShowGUI = true;
	page->setMouseTracking( true );
	page->setBackgroundColor( white );
	// start screensaver again
	QString pidFile = QDir::homeDirPath();
	pidFile += "/.kss.pid";
	FILE *fp;
	if ( ( fp = fopen( QFile::encodeName(pidFile), "r" ) ) != NULL ) {
	    fscanf( fp, "%d", &screensaver_pid );
	    fclose( fp );
	    kill( screensaver_pid, SIGCONT );
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
    if ( page->currentTextObjectView() )
#if 0
	page->currentTextObjectView()->home();
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
    if ( page->currentTextObjectView() )
        return;

    if ( presStarted ) {
        if ( !kPresenterDoc()->spManualSwitch() ) {
            setCurrentTimer( 1 );
            page->setNextPageTimer( true );
        }
	if ( page->pPrev( true ) ) {
            QRect pgRect = kPresenterDoc()->getPageRect( 0, 0, 0, page->presFakt(), false );
	    yOffset = ( page->presPage() - 1 ) * pgRect.height();
	    if ( page->height() > pgRect.height() )
		yOffset -= ( page->height() - pgRect.height() ) / 2;
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    page->repaint( false );
	    page->setFocus();
	} else {
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    page->setFocus();
	}
    } else {
	prevPage();
    }
}

/*========================== screen next ========================*/
void KPresenterView::screenNext()
{
    if ( page->currentTextObjectView() )
        return;
    if ( presStarted ) {
	if ( page->pNext( true ) ) {
            QRect pgRect = kPresenterDoc()->getPageRect( 0, 0, 0, page->presFakt(), false );
	    yOffset = ( page->presPage() - 1 ) * pgRect.height();
	    if ( page->height() > pgRect.height() )
		yOffset -= ( page->height() - pgRect.height() ) / 2;
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    page->setFocus();

            if ( !kPresenterDoc()->spManualSwitch() ) {
                setCurrentTimer( 1 );
                page->setNextPageTimer( true );
            }
	} else {
	    page->resize( QApplication::desktop()->width(), QApplication::desktop()->height() );
	    page->setFocus();
	}
    } else {
	nextPage();
    }
}

/*========================== screen last ========================*/
void KPresenterView::screenLast()
{
    if ( page->currentTextObjectView() )
#if 0
	page->currentTextObjectView()->end();
#else
    ;
#endif
    else {
	if ( !presStarted ) {
	    skipToPage( m_pKPresenterDoc->getPageNums() - 1 );
	} else {
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
    page->setTextPointSize( tbFont.pointSize() );
    page->setFocus();
    kdDebug(33001) << "sizeSelected() " << tbFont.pointSize() << endl;
}

/*===============================================================*/
void KPresenterView::fontSelected()
{
    tbFont.setFamily( actionTextFontFamily->currentText() );
    page->setTextFamily( tbFont.family() );
    page->setFocus();
    kdDebug(33001) << "fontSelected() " << tbFont.family() << endl;
}

void KPresenterView::textBold()
{
    tbFont.setBold( !tbFont.bold() );
    page->setTextBold( tbFont.bold() );
}


void KPresenterView::textStrikeOut()
{
    page->setTextStrikeOut(actionFormatStrikeOut->isChecked());
}

void KPresenterView::textItalic()
{
    tbFont.setItalic( !tbFont.italic() );
    page->setTextItalic( tbFont.italic() );
}

void KPresenterView::textUnderline()
{
    tbFont.setUnderline( !tbFont.underline() );
    page->setTextUnderline( tbFont.underline() );
}

void KPresenterView::textColor()
{
    tbColor = actionTextColor->color();
    page->setTextColor( tbColor );
}

void KPresenterView::textAlignLeft()
{
    if ( actionTextAlignLeft->isChecked() )
    {
        tbAlign = Qt::AlignLeft;
        page->setTextAlign( tbAlign );
    }
    else
        actionTextAlignLeft->setChecked(true);
}

void KPresenterView::textAlignCenter()
{
    if ( actionTextAlignCenter->isChecked() )
    {
        tbAlign = Qt::AlignCenter;
        page->setTextAlign( Qt::AlignCenter );
    }
    else
         actionTextAlignCenter->setChecked(true);
}

void KPresenterView::textAlignRight()
{
    if ( actionTextAlignRight->isChecked() )
    {
        tbAlign = Qt::AlignRight;
        page->setTextAlign( Qt::AlignRight );
    }
    else
        actionTextAlignRight->setChecked(true);

}

void KPresenterView::textAlignBlock()
{
    if ( actionTextAlignBlock->isChecked() )
    {
        tbAlign = Qt::AlignJustify;
        page->setTextAlign(Qt::AlignJustify);
    }
    else
        actionTextAlignBlock->setChecked(true);

}

void KPresenterView::textInsertPageNum()
{
#if 0 // note: also the action is disabled now (Werner)
    if ( page->currentTextObjectView() )
	page->currentTextObjectView()->insertPageNum();
#endif
}

void KPresenterView::mtextFont()
{
    KoTextFormatInterface* textIface = page->applicableTextInterfaces().first();

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
        page->setFont(fontDia->getNewFont(),
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
    KPTextView * edit = page->currentTextObjectView();
    Q_ASSERT(edit);
    if ( edit )
    {
        KCommand *cmd=edit->setCounterCommand( c );
        if(cmd)
            m_pKPresenterDoc->addCommand(cmd);
    }
#if 0
    KPTextObject *txtObj = page->currentTextObjectView()->textObject();
    if ( !txtObj )
	txtObj = page->selectedTextObj();
    if ( txtObj ) {
	if ( txtObj->paragType() != KTextEdit::EnumList ) {
	    txtObj->setParagType( KTextEdit::EnumList );
	    txtObj->setListDepth( 0 );
	}
	if ( !page->currentTextObjectView() )
	    page->repaint( false );
	else
	    txtObj->repaint( FALSE );
    }
#endif
}

/*===============================================================*/
void KPresenterView::textUnsortList()
{
    KoParagCounter c;
    if ( actionTextTypeUnsortList->isChecked() )
    {
        if(actionTextTypeEnumList->isChecked())
            actionTextTypeEnumList->setChecked(false);
        c.setNumbering( KoParagCounter::NUM_LIST );
        c.setStyle( KoParagCounter::STYLE_DISCBULLET );
    }
    else
    {
        c.setNumbering( KoParagCounter::NUM_NONE );
    }
    KPTextView * edit = page->currentTextObjectView();
    Q_ASSERT(edit);
    if ( edit )
    {
        KCommand *cmd=edit->setCounterCommand( c );
        if(cmd)
            m_pKPresenterDoc->addCommand(cmd);
    }
#if 0
    KPTextObject *txtObj = page->currentTextObjectView()->textObject();
    if ( !txtObj )
	txtObj = page->selectedTextObj();
    if ( txtObj ) {
	if ( txtObj->paragType() != KTextEdit::BulletList ) {
	    txtObj->setParagType( KTextEdit::BulletList );
	    txtObj->setListDepth( 0 );
	}
	if ( !page->currentTextObjectView() )
	    page->repaint( false );
	else
	    txtObj->repaint( FALSE );
    }
#endif
}

/*===============================================================*/
void KPresenterView::textDepthPlus()
{
    KPTextView *edit=page->currentTextObjectView();
    if ( edit )
    {
        double leftMargin = edit->currentParagLayout().margins[QStyleSheetItem::MarginLeft];
        double indent = m_pKPresenterDoc->getIndentValue();
        double newVal = leftMargin + indent;
        KCommand *cmd=edit->setMarginCommand( QStyleSheetItem::MarginLeft, newVal );
        if(cmd)
            m_pKPresenterDoc->addCommand(cmd);
    }

#if 0
    KPTextObject *txtObj = page->currentTextObjectView()->textObject();
    if ( !txtObj )
	txtObj = page->selectedTextObj();
    if ( txtObj ) {
	txtObj->setListDepth( 1 );
	if ( !page->currentTextObjectView() )
	    page->repaint( false );
	else
	    txtObj->repaint( FALSE );
    }
#endif
}

/*===============================================================*/
void KPresenterView::textDepthMinus()
{
    KPTextView *edit=page->currentTextObjectView();
    if ( edit )
    {
        double leftMargin = edit->currentParagLayout().margins[QStyleSheetItem::MarginLeft];
        if ( leftMargin > 0 )
        {
            double indent = m_pKPresenterDoc->getIndentValue();
            double newVal = leftMargin - indent;
            KCommand *cmd=edit->setMarginCommand( QStyleSheetItem::MarginLeft, QMAX( newVal, 0 ) );
            if(cmd)
                m_pKPresenterDoc->addCommand(cmd);
        }
    }

#if 0
    KPTextObject *txtObj = page->currentTextObjectView();
    if ( !txtObj )
	txtObj = page->selectedTextObj();
    if ( txtObj ) {
	txtObj->setListDepth( -1 );
	if ( !page->currentTextObjectView() )
	    page->repaint( false );
	else
	    txtObj->repaint( FALSE );
    }
#endif
}

/*===============================================================*/
void KPresenterView::textSettings()
{
#ifdef __GNUC__
#warning Fixme textSettings()
#endif


#if 0
    KPTextObject *txtObj = page->currentTextObjectView();
    if ( !txtObj )
	txtObj = page->selectedTextObj();
    if ( txtObj ) {
	TextDialog dlg( this, 0, TRUE );
	dlg.comboBullet1->setCurrentItem( (int)txtObj->document()->textSettings().bulletType[0] );
	dlg.comboBullet2->setCurrentItem( (int)txtObj->document()->textSettings().bulletType[1] );
	dlg.comboBullet3->setCurrentItem( (int)txtObj->document()->textSettings().bulletType[2] );
	dlg.comboBullet4->setCurrentItem( (int)txtObj->document()->textSettings().bulletType[3] );
	dlg.colorBullet1->setColor( txtObj->document()->textSettings().bulletColor[0] );
	dlg.colorBullet2->setColor( txtObj->document()->textSettings().bulletColor[1] );
	dlg.colorBullet3->setColor( txtObj->document()->textSettings().bulletColor[2] );
	dlg.colorBullet4->setColor( txtObj->document()->textSettings().bulletColor[3] );
	dlg.spinLineSpacing->setValue( txtObj->document()->textSettings().lineSpacing );
	dlg.spinParagSpacing->setValue( txtObj->document()->textSettings().paragSpacing );
	dlg.spinMargin->setValue( txtObj->document()->textSettings().margin );
	if ( dlg.exec() == QDialog::Accepted ) {
	    KTextEditDocument::TextSettings s;
	    s.bulletType[0] = (KTextEditDocument::Bullet)dlg.comboBullet1->currentItem();
	    s.bulletType[1] = (KTextEditDocument::Bullet)dlg.comboBullet2->currentItem();
	    s.bulletType[2] = (KTextEditDocument::Bullet)dlg.comboBullet3->currentItem();
	    s.bulletType[3] = (KTextEditDocument::Bullet)dlg.comboBullet4->currentItem();
	    s.bulletColor[0] = dlg.colorBullet1->color();
	    s.bulletColor[1] = dlg.colorBullet2->color();
	    s.bulletColor[2] = dlg.colorBullet3->color();
	    s.bulletColor[3] = dlg.colorBullet4->color();
	    s.lineSpacing = dlg.spinLineSpacing->value();
	    s.paragSpacing = dlg.spinParagSpacing->value();
	    s.margin = dlg.spinMargin->value();
	    txtObj->document()->setTextSettings( s );
	    if ( !page->currentTextObjectView() )
		page->repaint( false );
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
    KPTextObject *txtObj = page->currentTextObjectView();
    if ( !txtObj )
	txtObj = page->selectedTextObj();

    if ( txtObj )
	txtObj->extendContents2Height();

    if ( page->selectedTextObj() )
	m_pKPresenterDoc->repaint( false );
    else if ( txtObj )
	txtObj->repaint( FALSE );
#endif
}

/*===============================================================*/
void KPresenterView::textObjectToContents()
{
#if 0
    KPTextObject *txtObj = page->currentTextObjectView();
    if ( !txtObj )
	txtObj = page->selectedTextObj();

    if ( txtObj )
	txtObj->extendObject2Contents( this );

    if ( page->selectedKPTextObj() )
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
    if ( !page->currentTextObjectView() ) {
	bool fill = true;

	if ( !m_pKPresenterDoc->setPenColor( c, fill ) ) {
	    if ( fill )
		pen.setColor( c );
	    else
		pen = NoPen;
	}
    } else {
	tbColor = c;
	page->setTextColor( tbColor );
    }
}

/*===============================================================*/
void KPresenterView::brushChosen()
{
    QColor c = actionBrushColor->color();
    KPTextView *edit=page->currentTextObjectView();
    if ( !edit )
    {
	bool fill = true;

	if ( !m_pKPresenterDoc->setBrushColor( c, fill ) )
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
        page->setTextBackgroundColor(c);
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
void KPresenterView::extraPenStyleSolid()
{
    KPresenterDoc *doc = m_pKPresenterDoc;
    QPen e_pen = QPen( (doc->getPen( pen )).color(), (doc->getPen( pen )).width(),
                       Qt::SolidLine );
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
void KPresenterView::extraPenStyleDash()
{
    KPresenterDoc *doc = m_pKPresenterDoc;
    QPen e_pen = QPen( (doc->getPen( pen )).color(), (doc->getPen( pen )).width(),
                       Qt::DashLine );
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
void KPresenterView::extraPenStyleDot()
{
    KPresenterDoc *doc = m_pKPresenterDoc;
    QPen e_pen = QPen( (doc->getPen( pen )).color(), (doc->getPen( pen )).width(),
                       Qt::DotLine );
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
void KPresenterView::extraPenStyleDashDot()
{
    KPresenterDoc *doc = m_pKPresenterDoc;
    QPen e_pen = QPen( (doc->getPen( pen )).color(), (doc->getPen( pen )).width(),
                       Qt::DashDotLine );
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
void KPresenterView::extraPenStyleDashDotDot()
{
    KPresenterDoc *doc = m_pKPresenterDoc;
    QPen e_pen = QPen( (doc->getPen( pen )).color(), (doc->getPen( pen )).width(),
                       Qt::DashDotDotLine );
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
void KPresenterView::extraPenStyleNoPen()
{
    KPresenterDoc *doc = m_pKPresenterDoc;
    QPen e_pen = QPen( (doc->getPen( pen )).color(), (doc->getPen( pen )).width(),
                       Qt::NoPen );
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
    KPresenterDoc *doc = m_pKPresenterDoc;
    QPen e_pen = QPen( (doc->getPen( pen )).color(), 1,
                       (doc->getPen( pen )).style() );
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
void KPresenterView::extraPenWidth4()
{
    KPresenterDoc *doc = m_pKPresenterDoc;
    QPen e_pen = QPen( (doc->getPen( pen )).color(), 4,
                       (doc->getPen( pen )).style() );
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
void KPresenterView::extraPenWidth7()
{
    KPresenterDoc *doc = m_pKPresenterDoc;
    QPen e_pen = QPen( (doc->getPen( pen )).color(), 7,
                       (doc->getPen( pen )).style() );
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
void KPresenterView::extraPenWidth10()
{
    KPresenterDoc *doc = m_pKPresenterDoc;
    QPen e_pen = QPen( (doc->getPen( pen )).color(), 10,
                       (doc->getPen( pen )).style() );
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
void KPresenterView::newPageLayout( KoPageLayout _layout )
{
    KoPageLayout oldLayout = m_pKPresenterDoc->pageLayout();

    PgLayoutCmd *pgLayoutCmd = new PgLayoutCmd( i18n( "Set Page Layout" ), _layout, oldLayout, this );
    pgLayoutCmd->execute();
    kPresenterDoc()->addCommand( pgLayoutCmd );
    updateRuler();
}


void KPresenterView::updateRuler()
{
    //update koruler
    QRect r=m_pKPresenterDoc->getPageRect( currPg, page->diffx(), page->diffy() );
    getHRuler()->setFrameStartEnd( r.left() , r.right() );
    getVRuler()->setFrameStartEnd( r.top() , r.bottom() );
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

    page = new Page( pageBase, "Page", ( KPresenterView* )this );
    QObject::connect( page, SIGNAL( fontChanged( const QFont & ) ),
		      this, SLOT( fontChanged( const QFont & ) ) );
    QObject::connect( page, SIGNAL( colorChanged( const QColor & ) ),
		      this, SLOT( colorChanged( const QColor & ) ) );
    QObject::connect( page, SIGNAL( alignChanged( int ) ),
		      this, SLOT( alignChanged( int ) ) );
    QObject::connect( page, SIGNAL( updateSideBarItem( int ) ),
                      this, SLOT( updateSideBarItem( int ) ) );
    QObject::connect( page, SIGNAL( objectSelectedChanged()),
                      this, SLOT( objectSelectedChanged()));
    QObject::connect( page, SIGNAL( mouseWheelEvent( QWheelEvent* ) ),
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

    if ( m_pKPresenterDoc && page )
	QObject::connect( page, SIGNAL( stopPres() ), this, SLOT( stopPres() ) );

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
    actionEditDelPage->setEnabled( m_pKPresenterDoc->getPageNums() > 1 );
    objectSelectedChanged();
    refreshPageButton();

    KStatusBar * sb = statusBar();
    if ( sb )
        sb->show();

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

    actionEditFind = KStdAction::find( this, SLOT( editFind() ), actionCollection(), "edit_find" );
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

    actionExtraRaise = new KAction( i18n( "Ra&ise object(s)" ), "raise",
				    CTRL + Key_R, this, SLOT( extraRaise() ),
				    actionCollection(), "extra_raise" );

    actionExtraLower = new KAction( i18n( "&Lower object(s)" ), "lower", CTRL + Key_L,
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

    /*  //these are not used
    actionExtraPenStyle = new KAction( i18n("Pen Style"), "pen_style", 0,
					this, SLOT( extraPenStyle() ),
					actionCollection(), "pen_style" );

    actionExtraPenWidth = new KAction( i18n("Pen Width"), "pen_width", 0,
					this, SLOT( extraPenWidth() ),
					actionCollection(), "pen_width" );*/

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
                                                 actionCollection(), "brush_color" );
     connect( actionBrushColor, SIGNAL( activated() ), SLOT( brushChosen() ) );

     actionPenColor = new TKSelectColorAction( i18n( "Pen Color" ), TKSelectColorAction::LineColor,
                                               actionCollection(), "pen_color" );
     connect( actionPenColor, SIGNAL( activated() ), SLOT( penChosen() ) );

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

    m_variableDefMap.clear();
    actionInsertVariable = new KActionMenu( i18n( "&Variable" ),
                                            actionCollection(), "insert_variable" );
    // The last argument is only needed if a submenu is to be created
    addVariableActions( VT_FIELD, KoFieldVariable::actionTexts(), actionInsertVariable, i18n("&Property") );
    addVariableActions( VT_DATE, KoDateVariable::actionTexts(), actionInsertVariable, i18n("&Date") );
    addVariableActions( VT_TIME, KoTimeVariable::actionTexts(), actionInsertVariable, i18n("&Time") );
    addVariableActions( VT_PGNUM, KoPgNumVariable::actionTexts(), actionInsertVariable, QString::null );

    actionInsertCustom = new KActionMenu( i18n( "&Custom" ),
                                            actionCollection(), "insert_custom" );
    actionInsertVariable->insert(actionInsertCustom);
    refreshCustomMenu();

    actionIncreaseFontSize = new KAction( i18n("Increase font size"),"fontsizeup", 0,
                                  this, SLOT( increaseFontSize() ),
                                  actionCollection(), "increaseFontSize" );

    actionDecreaseFontSize = new KAction( i18n("Decrease font size"),"fontsizedown", 0,
                                  this, SLOT( decreaseFontSize() ),
                                  actionCollection(), "decreaseFontSize" );

    actionChangeCase=new KAction( i18n( "Change case..." ), 0,
                                  this, SLOT( changeCaseOfText() ),
                                  actionCollection(), "change_case" );

}

void KPresenterView::textSubScript()
{
    page->setTextSubScript(actionFormatSub->isChecked());
}

void KPresenterView::textSuperScript()
{
    page->setTextSuperScript(actionFormatSuper->isChecked());
}

void KPresenterView::decreaseFontSize()
{
    page->setDecreaseFontSize();
}

void KPresenterView::increaseFontSize()
{
    page->setIncreaseFontSize();
}

void KPresenterView::objectSelectedChanged()
{
    bool state=page->isOneObjectSelected();
    actionScreenAssignEffect->setEnabled(state);
    actionExtraRotate->setEnabled(state);
    actionExtraAlignObjs->setEnabled(state);
    actionExtraGroup->setEnabled(state && m_pKPresenterDoc->numSelected()>1);
    actionExtraUnGroup->setEnabled(state);
    actionExtraAlignObjLeft->setEnabled(state);
    actionExtraAlignObjCenterH->setEnabled(state);
    actionExtraAlignObjRight->setEnabled(state);
    actionExtraAlignObjTop->setEnabled(state);
    actionExtraAlignObjCenterV->setEnabled(state);
    actionExtraAlignObjBottom->setEnabled(state);
    actionEditDelete->setEnabled(state);
    actionExtraRaise->setEnabled(state && m_pKPresenterDoc->numSelected()==1);
    actionExtraLower->setEnabled(state && m_pKPresenterDoc->numSelected()==1);
    //actionBrushColor->setEnabled(state);
    actionPenColor->setEnabled(state);
    //actionExtraPenStyle->setEnabled(state);
    //actionExtraPenWidth->setEnabled(state);

    bool isText=!page->applicableTextInterfaces().isEmpty();
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
    if ( !page->applicableTextInterfaces().isEmpty() )
    {
        KoTextFormat format =*(page->applicableTextInterfaces().first()->currentFormat());
        //unzoom font
        format.setPointSize( KoTextZoomHandler::layoutUnitToPt( format.font().pointSize() ) );
        showFormat( format );
        const KoParagLayout * paragLayout=page->applicableTextInterfaces().first()->currentParagLayoutFormat();
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

    KPTextView *edit=page->currentTextObjectView();
    bool val=edit && isText;
    actionInsertSpecialChar->setEnabled(val);
    actionInsertLink->setEnabled(val);
    actionEditFind->setEnabled(val);
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

    actionExtraShadow->setEnabled(!page->haveASelectedPictureObj());

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
					     m_pKPresenterDoc->getBackColor1( currPg ),
					     m_pKPresenterDoc->getBackColor2( currPg ),
					     m_pKPresenterDoc->getBackColorType( currPg ),
					     m_pKPresenterDoc->getBackUnbalanced( currPg ),
					     m_pKPresenterDoc->getBackXFactor( currPg ),
					     m_pKPresenterDoc->getBackYFactor( currPg ),
					     m_pKPresenterDoc->getBackPixKey( currPg ),
					     m_pKPresenterDoc->getBackClipKey( currPg ),
					     m_pKPresenterDoc->getBackView( currPg ),
					     m_pKPresenterDoc->getBackType( currPg ),
					     takeGlobal, getCurrPgNum(), m_pKPresenterDoc );
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

    //page->deSelectAllObj();

    page->deSelectAllObj();
    page->setToolEditMode( INS_AUTOFORM );
    page->setAutoForm( fileName );
}

void KPresenterView::slotAfchooseCanceled()
{
    setTool( TEM_MOUSE );
}

/*=========== take changes for style dialog =====================*/
void KPresenterView::styleOk()
{
    if ( !m_pKPresenterDoc->setPenBrush( styleDia->getPen(), styleDia->getBrush(), styleDia->getLineBegin(),
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
					  kPresenterDoc()->backgroundList()->
					  at( getCurrPgNum() - 1 )->getPageEffect(),
					  kPresenterDoc()->getPresSpeed(),
					  kPresenterDoc()->backgroundList()->
					  at( getCurrPgNum() - 1 )->getPageTimer(),
					  kPresenterDoc()->backgroundList()->
					  at( getCurrPgNum() - 1 )->getPageSoundEffect(),
					  kPresenterDoc()->backgroundList()->
					  at( getCurrPgNum() - 1 )->getPageSoundFileName(),
					  kPresenterDoc(), getCurrPgNum() - 1 );
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
    KPObject *kpobject = 0;
    QPtrList<KPObject> _objects;
    QPtrList<RotateCmd::RotateValues> _oldRotate;
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

            if(!newAngle &&tmp->angle!= _newAngle)
                newAngle=true;

	    _oldRotate.append( tmp );
	    _objects.append( kpobject );
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
    KPObject *kpobject = 0;
    QPtrList<KPObject> _objects;
    QPtrList<ShadowCmd::ShadowValues> _oldShadow;
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

            if(!newShadow &&( tmp->shadowDirection!=_newShadow.shadowDirection
               || tmp->shadowDistance!=_newShadow.shadowDistance
               || tmp->shadowColor!=_newShadow.shadowColor))
                newShadow=true;

	    _oldShadow.append( tmp );
	    _objects.append( kpobject );
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
    if ( !m_pKPresenterDoc->setPieSettings( confPieDia->getType(),
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
    if ( !m_pKPresenterDoc->setRectSettings( confRectDia->getRndX(), confRectDia->getRndY() ) ) {
	rndX = confRectDia->getRndX();
	rndY = confRectDia->getRndY();
    }
}

/*================================================================*/
unsigned int KPresenterView::getCurrPgNum()
{
    return currPg + 1;
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
void KPresenterView::scrollV( int value )
{
    if ( !presStarted ) {
	int yo = yOffset;

	yOffset = kPresenterDoc()->getPageRect( 0, 0, 0, 1.0, false ).height() * currPg + value;
	page->scroll( 0, yo - yOffset );

	if ( v_ruler )
	    v_ruler->setOffset( 0, -kPresenterDoc()->getPageRect( getCurrPgNum() - 1, xOffset, yOffset, 1.0, false ).y() );
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
      m_pKPresenterDoc->changePicture( file );
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
        m_pKPresenterDoc->changeClipart( file );
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
            page->move( 20, 20 );
            if ( h_ruler )
            {
                h_ruler->show();
                h_ruler->setGeometry( 20, 0, page->width(), 20 );
            }
            if (v_ruler )
            {
                v_ruler->show();
                v_ruler->setGeometry( 0, 20, 20, page->height() );
            }
            if(getTabChooser())
            {
                getTabChooser()->setGeometry(0,0,20,20);
                getTabChooser()->show();
            }
        }
        else
        {
            page->move( 0, 0 );
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
	page->move( 0, 0 );
    }
}

void PageBase::resizeEvent( QResizeEvent *e )
{
    if ( !view->presStarted )
	QWidget::resizeEvent( e );

    QSize s = e ? e->size() : size();

    if ( view->m_bShowGUI ) {

        view->page->resize( s.width() - 36, s.height() - 36 );
	view->vert->setGeometry( s.width() - 16, 0, 16, s.height() - 32 );
	view->pgPrev->setGeometry( s.width() - 15, s.height() - 32, 15, 16 );
	view->pgNext->setGeometry( s.width() - 15, s.height() - 16, 15, 16 );
	view->horz->setGeometry( 0, s.height() - 16, s.width() - 16, 16 );
    }
    else
    {
	view->page->move( 0, 0 );
	view->page->resize( s.width(), s.height() );
    }
    view->reorganize();
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

/*===============================================================*/
void KPresenterView::getPageMouseWheelEvent( QWheelEvent *e )
{
  QApplication::sendEvent( vert, e );
}

/*======================= key press event =======================*/
void KPresenterView::keyPressEvent( QKeyEvent *e )
{
    if ( e->key() == Key_Delete && !page->currentTextObjectView() )
	editDelete();
    else
	QApplication::sendEvent( page, e );
}

/*====================== do automatic screenpresentation ========*/
void KPresenterView::doAutomaticScreenPres()
{
    if ( exitPres ) // A user pushed Escape key or clicked "Exit presentation" menu.
        return;
    else if ( !continuePres && kPresenterDoc()->spInfinitLoop() ) {
        continuePres = true;
        page->gotoPage( 1 ); // return to first page.
        setCurrentTimer( 1 );
        page->setNextPageTimer( true );
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
    rb_pstyle->insertItem( KPBarIcon( "pen_style_dotdot" ), this, SLOT( extraPenStyleDashDotDot() ) );
    rb_pstyle->insertSeparator( -1 );
    rb_pstyle->insertItem( KPBarIcon( "pen_style_nopen" ), this, SLOT( extraPenStyleNoPen() ) );
    rb_pstyle->setMouseTracking( true );
    rb_pstyle->setCheckable( false );

    // create right button pen width
    rb_pwidth = new QPopupMenu();
    Q_CHECK_PTR( rb_pwidth );
    rb_pwidth->insertItem( KPBarIcon( "pen_width1" ), this, SLOT( extraPenWidth1() ) );
    rb_pwidth->insertSeparator( -1 );
    rb_pwidth->insertItem( KPBarIcon( "pen_width4" ), this, SLOT( extraPenWidth4() ) );
    rb_pwidth->insertSeparator( -1 );
    rb_pwidth->insertItem( KPBarIcon( "pen_width7" ), this, SLOT( extraPenWidth7() ) );
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

    h_ruler = new KoRuler( pageBase, page, Qt::Horizontal, kPresenterDoc()->pageLayout(), KoRuler::F_INDENTS | KoRuler::F_TABS, tabChooser );
    h_ruler->changeFlags(0);
    h_ruler->setReadWrite(kPresenterDoc()->isReadWrite());
    v_ruler = new KoRuler( pageBase, page, Qt::Vertical, kPresenterDoc()->pageLayout(), 0 );
    v_ruler->setReadWrite(kPresenterDoc()->isReadWrite());
    page->resize( page->width() - 20, page->height() - 20 );
    page->move( 20, 20 );
    h_ruler->setGeometry( 20, 0, page->width(), 20 );
    v_ruler->setGeometry( 0, 20, 20, page->height() );

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

    h_ruler->setUnit( m_pKPresenterDoc->getUnitName() );
    v_ruler->setUnit( m_pKPresenterDoc->getUnitName() );
}

/*==============================================================*/
void KPresenterView::unitChanged( QString u )
{
    m_pKPresenterDoc->setUnit(KoUnit::unit( u ) );
}

/*===================== set ranges of scrollbars ===============*/
void KPresenterView::setRanges()
{
    if ( vert && horz && page && m_pKPresenterDoc ) {
	vert->setSteps( 10, page->height() );
	vert->setRange( 0, QMAX( 0, m_pKPresenterDoc->getPageRect( 0, xOffset, yOffset, 1.0, false ).height()  - page->height() ) );
	horz->setSteps( 10, m_pKPresenterDoc->getPageRect( 0, xOffset, yOffset, 1.0, false ).width() +
			16 - page->width() );
	int range = m_pKPresenterDoc->getPageRect( 0, xOffset, yOffset, 1.0, false ).width() +
		16 - page->width() < 0 ? 0 :
	    m_pKPresenterDoc->getPageRect( 0, xOffset, yOffset, 1.0, false ).width() + 16 - page->width();
	horz->setRange( 0, range );
    }
}

/*==============================================================*/
void KPresenterView::skipToPage( int num )
{
    if ( num < 0 || num > static_cast<int>( m_pKPresenterDoc->getPageNums() ) - 1 || m_pKPresenterDoc->isEmbedded() || !page )
	return;

    page->exitEditMode();
    vert->setValue( 0 );
    currPg = num;
    emit currentPageChanged( currPg );
    if( sidebar )
        sidebar->setCurrentPage( currPg );

    if ( notebar ) {
        kdDebug() << "Current Page: " << currPg << endl;
        QString text = m_pKPresenterDoc->getNoteText( currPg );
        notebar->setCurrentNoteText( text );
    }

    refreshPageButton();

    yOffset = kPresenterDoc()->getPageRect( 0, 0, 0, 1.0, false ).height() * currPg;
    //(Laurent) deselect object when we change page.
    //otherwise you can change object properties on other page
    page->deSelectAllObj();
    page->repaint( FALSE );
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
    return page;
}

/*================================================================*/
int KPresenterView::canvasXOffset() const
{
    return getDiffX();
}

/*================================================================*/
int KPresenterView::canvasYOffset() const
{
    return getDiffY();
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
	return false;

    page->gotoPage( pg );
    return true;
}

void KPresenterView::search()
{
    if ( !searchDialog )
	return;
    KPTextObject *txtObj = page->applicableTextObjects().first();
    if ( !txtObj )
	return;
    // TODO find dialog from kword
#if 0
    QString txt = searchDialog->lineEdit->text();
    if ( !txtObj->find( txt, searchDialog->cs, searchDialog->wo, !searchDialog->back ) )
	KMessageBox::information( this, i18n( "%1 not found!" ).arg( txt ), i18n( "Find" ) );
#endif
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
  int nbObjects = m_pKPresenterDoc->objNums();

  if ( sb && nbObjects > 0 ) {
    if ( !m_sbObjectLabel ) {
      m_sbObjectLabel = sb ? new KStatusBarLabel( QString::null, 0, sb ) : 0;
      addStatusBarItem( m_sbObjectLabel );
    }

    int nbSelected = m_pKPresenterDoc->numSelected();

    if (nbSelected == 1) {
      KPObject * obj = m_pKPresenterDoc->getSelectedObj();
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
    page->chClip();
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


void KPresenterView::renamePageTitle()
{
    if(sidebar)
        sidebar->renamePageTitle();
}


void KPresenterView::picViewOrig640x480()
{
    page->picViewOrig640x480();
}

void KPresenterView::picViewOrig800x600()
{
    page->picViewOrig800x600();
}

void KPresenterView::picViewOrig1024x768()
{
    page->picViewOrig1024x768();
}

void KPresenterView::picViewOrig1280x1024()
{
    page->picViewOrig1280x1024();
}

void KPresenterView::picViewOrig1600x1200()
{
    page->picViewOrig1600x1200();
}

void KPresenterView::chPic()
{
    page->chPic();
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
    KPTextView *edit=page->currentTextObjectView();
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
    KPTextView *edit=page->currentTextObjectView();
    if ( !edit )
        return;
    edit->setFamily( _font );
    edit->insertSpecialChar(c);

}

void KPresenterView::insertLink()
{
    KPTextView *edit=page->currentTextObjectView();
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
    KPTextView *edit=page->currentTextObjectView();
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
    m_spell.spellCurrTextObjNum = -1;
    m_spell.macroCmdSpellCheck=0L;
    m_spell.textObject.clear();
    QPtrList<KPObject> *listObj(page->objectList());
    for ( unsigned int i = 0; i < listObj->count(); i++ ) {
        if(listObj->at( i )->getType() == OT_TEXT)
            m_spell.textObject.append(dynamic_cast<KPTextObject*>( listObj->at( i ) ));
    }
    startKSpell();
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
    QObject::connect( m_spell.kspell, SIGNAL( misspelling( QString, QStringList*, unsigned ) ),
                      this, SLOT( spellCheckerMisspelling( QString, QStringList*, unsigned ) ) );
    QObject::connect( m_spell.kspell, SIGNAL( corrected( QString, QString, unsigned ) ),
                      this, SLOT( spellCheckerCorrected( QString, QString, unsigned ) ) );
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

    // Done
    m_spell.kspell->cleanUp();
    delete m_spell.kspell;
    m_spell.kspell = 0;
    m_spell.textObject.clear();
    m_spell.ignoreWord.clear();
    if(m_spell.macroCmdSpellCheck)
        m_pKPresenterDoc->addCommand(m_spell.macroCmdSpellCheck);
}

void KPresenterView::spellCheckerMisspelling( QString old, QStringList* , unsigned pos )
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
    textobj->highlightPortion( p, pos, old.length(), page );
}

void KPresenterView::spellCheckerCorrected( QString old, QString corr, unsigned pos )
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
    textobj->highlightPortion( p, pos, old.length(), page );
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
    KPTextObject * textobj = m_spell.textObject.at( m_spell.spellCurrTextObjNum ) ;
    Q_ASSERT( textobj );
    if ( textobj )
        textobj->removeHighlight();

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
    if (status == KSpell::Error)
    {
        KMessageBox::sorry(this, i18n("ISpell could not be started.\n"
                                      "Please make sure you have ISpell properly configured and in your PATH.\nUsed settings->configure."));
    }
    else if (status == KSpell::Crashed)
    {
        KMessageBox::sorry(this, i18n("ISpell seems to have crashed."));
    }
    KPTextObject * textobj = m_spell.textObject.at( m_spell.spellCurrTextObjNum ) ;
    Q_ASSERT( textobj );
    if ( textobj )
        textobj->removeHighlight();
    m_spell.textObject.clear();
    m_ignoreWord.clear();
    if(m_spell.macroCmdSpellCheck)
        m_pKPresenterDoc->addCommand(m_spell.macroCmdSpellCheck);

    KPTextView *edit=page->currentTextObjectView();
    if (edit)
        edit->drawCursor( TRUE );
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
    KPTextView *edit=page->currentTextObjectView();
    if (edit)
    {
        KoParagDia *paragDia = new KoParagDia( this, "",
                                               KoParagDia::PD_SPACING | KoParagDia::PD_ALIGN |
                                               KoParagDia::PD_BORDERS |
                                               KoParagDia::PD_NUMBERING | KoParagDia::PD_TABS, m_pKPresenterDoc->getUnit(),edit->kpTextObject()->getSize().width(),false );
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
#if 0
            m_gui->getHorzRuler()->setLeftIndent( KoUnit::userValue( paragDia->leftIndent(), m_doc->getUnit() ) );
#endif
        }

        if(paragDia->isRightMarginChanged())
        {
            cmd=edit->setMarginCommand( QStyleSheetItem::MarginRight, paragDia->rightIndent() );
            if(cmd)
            {
                macroCommand->addCommand(cmd);
                changed=true;
            }
            //m_gui->getHorzRuler()->setRightIndent( KoUnit::userValue( paragDia->rightIndent(), m_doc->getUnit() ) );
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
            /*m_gui->getHorzRuler()->setFirstIndent(
              KoUnit::userValue( paragDia->leftIndent() + paragDia->firstLineIndent(), m_doc->getUnit() ) );*/
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
        if ( paragDia->isPageBreakingChanged() )
        {
#if 0
            cmd=edit->setPageBreakingCommand( paragDia->pageBreaking() );
            if(cmd)
            {
                macroCommand->addCommand(cmd);
                changed=true;
            }
#endif
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
    page->setTextDefaultFormat( );
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
    KPTextView *edit=page->currentTextObjectView();
    if ( edit )
    {
        KAction * act = (KAction *)(sender());
        edit->insertCustomVariable(act->text());
    }
}

void KPresenterView::insertNewCustomVariable()
{
    KPTextView *edit=page->currentTextObjectView();
    if ( edit )
        edit->insertVariable( VT_CUSTOM, 0 );
}

void KPresenterView::editCustomVars()
{
    KoCustomVariablesDia dia( this, m_pKPresenterDoc->getVariableCollection()->getVariables() );
    if(dia.exec())
        m_pKPresenterDoc->recalcVariables( VT_CUSTOM );
}

void KPresenterView::insertVariable()
{
    KPTextView *edit=page->currentTextObjectView();
    if ( edit )
    {
        KAction * act = (KAction *)(sender());
        VariableDefMap::Iterator it = m_variableDefMap.find( act );
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
    KPTextView *edit=page->currentTextObjectView();
    if ( edit )
        edit->openLink();
}

void KPresenterView::showRulerIndent( double _leftMargin, double _firstLine, double _rightMargin )
{
  KoRuler * hRuler = getHRuler();
  if ( hRuler )
  {
      hRuler->setFirstIndent( KoUnit::userValue( _firstLine + _leftMargin, m_pKPresenterDoc->getUnit() ) );
      hRuler->setLeftIndent( KoUnit::userValue( _leftMargin, m_pKPresenterDoc->getUnit() ) );
      hRuler->setRightIndent( KoUnit::userValue( _rightMargin, m_pKPresenterDoc->getUnit() ) );
      actionTextDepthMinus->setEnabled( _leftMargin>0);
  }
}

void KPresenterView::tabListChanged( const KoTabulatorList & tabList )
{
    if(!m_pKPresenterDoc->isReadWrite())
        return;

    page->setTabList( tabList );

#if 0
    KPTextView *edit=page->currentTextObjectView();
    if (!edit)
        return;
    KCommand *cmd=edit->setTabListCommand( tabList );
    if(cmd)
        m_pKPresenterDoc->addCommand(cmd);
#endif
}

void KPresenterView::newFirstIndent( double _firstIndent )
{
    KPTextView *edit=page->currentTextObjectView();
    if (!edit) return;
    double val = _firstIndent - edit->currentParagLayout().margins[QStyleSheetItem::MarginLeft];
    KCommand *cmd=edit->setMarginCommand( QStyleSheetItem::MarginFirstLine, val );
    if(cmd)
        m_pKPresenterDoc->addCommand(cmd);
}

void KPresenterView::newLeftIndent( double _leftIndent)
{
    KPTextView *edit=page->currentTextObjectView();
    if (edit)
    {
        KCommand *cmd=edit->setMarginCommand( QStyleSheetItem::MarginLeft, _leftIndent );
        if(cmd)
            m_pKPresenterDoc->addCommand(cmd);
    }
}

void KPresenterView::newRightIndent( double _rightIndent)
{
    KPTextView *edit=page->currentTextObjectView();
    if (edit)
    {
        KCommand *cmd=edit->setMarginCommand( QStyleSheetItem::MarginRight, _rightIndent );
        if(cmd)
            m_pKPresenterDoc->addCommand(cmd);
    }
}

void KPresenterView::slotUpdateRuler()
{
    // Set the "frame start" in the ruler (tabs are relative to that position)
    KPTextView *edit=page->currentTextObjectView();
    bool isText=!page->applicableTextObjects().isEmpty();
    if ( isText )
    {
        KPTextObject *txtobj= page->applicableTextObjects().first();
        if ( txtobj )
        {
            QRect r= txtobj->getBoundingRect(page->diffx(),page->diffy() );
            getHRuler()->setFrameStartEnd( r.left() /*- pc.x()*/, r.right() /*- pc.x()*/ );
            getVRuler()->setFrameStartEnd( r.top() /*- pc.y()*/, r.bottom() /*- pc.y()*/ );
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
    KPTextView *edit=page->currentTextObjectView();
    if(!edit)
        return;
    KoChangeCaseDia *caseDia=new KoChangeCaseDia( this,"change case" );
    if(caseDia->exec())
    {
        edit->changeCaseOfText(caseDia->getTypeOfCase());
    }
    delete caseDia;
}

#include <kpresenter_view.moc>
