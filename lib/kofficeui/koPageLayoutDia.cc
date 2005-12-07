/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

// Description: Page Layout Dialog (header)

/******************************************************************/

#include <koPageLayoutDia.h>
#include <koUnit.h>
#include <koUnitWidgets.h>

#include <klocale.h>
#include <knuminput.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kmessagebox.h>

#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <knumvalidator.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qhbox.h>
#include <qvgroupbox.h>
#include <qhbuttongroup.h>

/******************************************************************/
/* class KoPagePreview                                            */
/******************************************************************/

/*===================== constrcutor ==============================*/
KoPagePreview::KoPagePreview( QWidget* parent, const char *name, const KoPageLayout& _layout )
    : QGroupBox( i18n( "Page Preview" ), parent, name )
{
    setPageLayout( _layout );
    columns = 1;
    setMinimumSize( 150, 150 );
}

/*====================== destructor ==============================*/
KoPagePreview::~KoPagePreview()
{
}

/*=================== set layout =================================*/
void KoPagePreview::setPageLayout( const KoPageLayout &layout )
{
    // resolution[XY] is in pixel per pt
    double resolutionX = POINT_TO_INCH( static_cast<double>(KoGlobal::dpiX()) );
    double resolutionY = POINT_TO_INCH( static_cast<double>(KoGlobal::dpiY()) );

    m_pageWidth = layout.ptWidth * resolutionX;
    m_pageHeight = layout.ptHeight * resolutionY;

    double zh = 110.0 / m_pageHeight;
    double zw = 110.0 / m_pageWidth;
    double z = QMIN( zw, zh );

    m_pageWidth *= z;
    m_pageHeight *= z;

    m_textFrameX = layout.ptLeft * resolutionX * z;
    m_textFrameY = layout.ptTop * resolutionY * z;
    m_textFrameWidth = m_pageWidth - ( layout.ptLeft + layout.ptRight ) * resolutionX * z;
    m_textFrameHeight = m_pageHeight - ( layout.ptTop + layout.ptBottom ) * resolutionY * z;

    repaint( true );
}

/*=================== set layout =================================*/
void KoPagePreview::setPageColumns( const KoColumns &_columns )
{
    columns = _columns.columns;
    repaint( true );
}

/*======================== draw contents =========================*/
void KoPagePreview::drawContents( QPainter *painter )
{
    double cw = m_textFrameWidth;
    if(columns!=1)
        cw/=static_cast<double>(columns);

    painter->setBrush( white );
    painter->setPen( QPen( black ) );

    int x=static_cast<int>( ( width() - m_pageWidth ) * 0.5 );
    int y=static_cast<int>( ( height() - m_pageHeight ) * 0.5 );
    int w=static_cast<int>(m_pageWidth);
    int h=static_cast<int>(m_pageHeight);
    //painter->drawRect( x + 1, y + 1, w, h);
    painter->drawRect( x, y, w, h );

    painter->setBrush( QBrush( black, HorPattern ) );
    if ( m_textFrameWidth == m_pageWidth || m_textFrameHeight == m_pageHeight )
        painter->setPen( NoPen );
    else
        painter->setPen( lightGray );

    for ( int i = 0; i < columns; ++i )
        painter->drawRect( x + static_cast<int>(m_textFrameX) + static_cast<int>(i * cw),
                           y + static_cast<int>(m_textFrameY), static_cast<int>(cw),
                           static_cast<int>(m_textFrameHeight) );
}

/******************************************************************/
/* class KoPageLayoutDia                                          */
/******************************************************************/

/*==================== constructor ===============================*/
KoPageLayoutDia::KoPageLayoutDia( QWidget* parent, const char* name,
                                  const KoPageLayout& _layout,
                                  const KoHeadFoot& _hf, int tabs,
                                  KoUnit::Unit unit, bool modal )
    : KDialogBase( KDialogBase::Tabbed, i18n("Page Layout"), KDialogBase::Ok | KDialogBase::Cancel,
                   KDialogBase::Ok, parent, name, modal)
{

    flags = tabs;
    pgPreview2 = 0;

    m_layout = _layout;
    m_unit = unit;

    m_cl.columns = 1;

    enableBorders = true;

    if ( tabs & FORMAT_AND_BORDERS ) setupTab1();
    if ( tabs & HEADER_AND_FOOTER ) setupTab2( _hf );

    setFocusPolicy( QWidget::StrongFocus );
    setFocus();
}

/*==================== constructor ===============================*/
KoPageLayoutDia::KoPageLayoutDia( QWidget* parent, const char* name,
				  const KoPageLayout& _layout,
				  const KoHeadFoot& _hf,
				  const KoColumns& _cl,
				  const KoKWHeaderFooter& _kwhf,
				  int tabs, KoUnit::Unit unit )
    : KDialogBase( KDialogBase::Tabbed, i18n("Page Layout"), KDialogBase::Ok | KDialogBase::Cancel,
                   KDialogBase::Ok, parent, name, true)
{
    flags = tabs;
    pgPreview2 = 0;

    m_layout = _layout;
    m_cl = _cl;
    kwhf = _kwhf;
    m_unit = unit;

    enableBorders = true;

    if ( tabs & DISABLE_BORDERS ) enableBorders = false;
    if ( tabs & FORMAT_AND_BORDERS ) setupTab1();
    if ( tabs & HEADER_AND_FOOTER ) setupTab2( _hf );
    if ( tabs & COLUMNS ) setupTab3();
    if ( tabs & KW_HEADER_AND_FOOTER ) setupTab4();

    setFocusPolicy( QWidget::StrongFocus );
    setFocus();
}

/*===================== destructor ===============================*/
KoPageLayoutDia::~KoPageLayoutDia()
{
}

/*======================= show dialog ============================*/
bool KoPageLayoutDia::pageLayout( KoPageLayout& _layout, KoHeadFoot& _hf, int _tabs, KoUnit::Unit& unit, QWidget* parent )
{
    bool res = false;
    KoPageLayoutDia *dlg = new KoPageLayoutDia( parent, "PageLayout", _layout, _hf, _tabs, unit );

    if ( dlg->exec() == QDialog::Accepted ) {
        res = true;
        if ( _tabs & FORMAT_AND_BORDERS ) _layout = dlg->layout();
        if ( _tabs & HEADER_AND_FOOTER ) _hf = dlg->headFoot();
        unit = dlg->unit();
    }

    delete dlg;

    return res;
}

/*======================= show dialog ============================*/
bool KoPageLayoutDia::pageLayout( KoPageLayout& _layout, KoHeadFoot& _hf, KoColumns& _cl,
                                  KoKWHeaderFooter &_kwhf, int _tabs, KoUnit::Unit& unit, QWidget* parent )
{
    bool res = false;
    KoPageLayoutDia *dlg = new KoPageLayoutDia( parent, "PageLayout", _layout, _hf, _cl, _kwhf, _tabs, unit );

    if ( dlg->exec() == QDialog::Accepted ) {
        res = true;
        if ( _tabs & FORMAT_AND_BORDERS ) _layout = dlg->layout();
        if ( _tabs & HEADER_AND_FOOTER ) _hf = dlg->headFoot();
        if ( _tabs & COLUMNS ) _cl = dlg->columns();
        if ( _tabs & KW_HEADER_AND_FOOTER ) _kwhf = dlg->getKWHeaderFooter();
        unit = dlg->unit();
    }

    delete dlg;

    return res;
}

/*===================== get a standard page layout ===============*/
KoPageLayout KoPageLayoutDia::standardLayout()
{
    return KoPageLayout::standardLayout();
}

/*====================== get header - footer =====================*/
KoHeadFoot KoPageLayoutDia::headFoot() const
{
    KoHeadFoot hf;
    hf.headLeft = eHeadLeft->text();
    hf.headMid = eHeadMid->text();
    hf.headRight = eHeadRight->text();
    hf.footLeft = eFootLeft->text();
    hf.footMid = eFootMid->text();
    hf.footRight = eFootRight->text();
    return hf;
}

/*================================================================*/
const KoColumns& KoPageLayoutDia::columns()
{
    m_cl.columns = nColumns->value();
    m_cl.ptColumnSpacing = nCSpacing->value();
    return m_cl;
}

/*================================================================*/
const KoKWHeaderFooter& KoPageLayoutDia::getKWHeaderFooter()
{
    if ( rhFirst->isChecked() && rhEvenOdd->isChecked() )
        kwhf.header = HF_FIRST_EO_DIFF;
    else if ( rhFirst->isChecked() )
        kwhf.header = HF_FIRST_DIFF;
    else if ( rhEvenOdd->isChecked() )
        kwhf.header = HF_EO_DIFF;
    else
        kwhf.header = HF_SAME;

    kwhf.ptHeaderBodySpacing = nHSpacing->value();
    kwhf.ptFooterBodySpacing = nFSpacing->value();
    kwhf.ptFootNoteBodySpacing = nFNSpacing->value();
    if ( rfFirst->isChecked() && rfEvenOdd->isChecked() )
        kwhf.footer = HF_FIRST_EO_DIFF;
    else if ( rfFirst->isChecked() )
        kwhf.footer = HF_FIRST_DIFF;
    else if ( rfEvenOdd->isChecked() )
        kwhf.footer = HF_EO_DIFF;
    else
        kwhf.footer = HF_SAME;

    return kwhf;
}

/*================ setup page size & margins tab ==================*/
void KoPageLayoutDia::setupTab1()
{
    QWidget *tab1 = addPage(i18n( "Page Size && &Margins" ));
    QHBoxLayout *lay = new QHBoxLayout(tab1);
    m_sizeWidget = new KoPageLayoutSize(tab1, m_layout, m_unit, m_cl, !(flags & DISABLE_UNIT), enableBorders );
    lay->addWidget(m_sizeWidget);
    m_sizeWidget->show();
    connect (m_sizeWidget, SIGNAL( propertyChange(KoPageLayout&)),
            this, SLOT (sizeUpdated( KoPageLayout&)));
}

void KoPageLayoutDia::sizeUpdated(KoPageLayout &layout) {
    m_layout.ptWidth = layout.ptWidth;
    m_layout.ptHeight = layout.ptHeight;
    m_layout.ptLeft = layout.ptLeft;
    m_layout.ptRight = layout.ptRight;
    m_layout.ptTop = layout.ptTop;
    m_layout.ptBottom = layout.ptBottom;
    m_layout.format = layout.format;
    m_layout.orientation = layout.orientation;
}

/*================ setup header and footer tab ===================*/
void KoPageLayoutDia::setupTab2( const KoHeadFoot& hf )
{
    QWidget *tab2 = addPage(i18n( "H&eader && Footer" ));
    QGridLayout *grid2 = new QGridLayout( tab2, 7, 2, 0, KDialog::spacingHint() );

    // ------------- header ---------------
    QGroupBox *gHead = new QGroupBox( 0, Qt::Vertical, i18n( "Head Line" ), tab2 );
    gHead->layout()->setSpacing(KDialog::spacingHint());
    gHead->layout()->setMargin(KDialog::marginHint());
    QGridLayout *headGrid = new QGridLayout( gHead->layout(), 2, 3 );

    QLabel *lHeadLeft = new QLabel( i18n( "Left:" ), gHead );
    headGrid->addWidget( lHeadLeft, 0, 0 );

    eHeadLeft = new QLineEdit( gHead );
    headGrid->addWidget( eHeadLeft, 1, 0 );
    eHeadLeft->setText( hf.headLeft );

    QLabel *lHeadMid = new QLabel( i18n( "Mid:" ), gHead );
    headGrid->addWidget( lHeadMid, 0, 1 );

    eHeadMid = new QLineEdit( gHead );
    headGrid->addWidget( eHeadMid, 1, 1 );
    eHeadMid->setText( hf.headMid );

    QLabel *lHeadRight = new QLabel( i18n( "Right:" ), gHead );
    headGrid->addWidget( lHeadRight, 0, 2 );

    eHeadRight = new QLineEdit( gHead );
    headGrid->addWidget( eHeadRight, 1, 2 );
    eHeadRight->setText( hf.headRight );

    grid2->addMultiCellWidget( gHead, 0, 1, 0, 1 );

    // ------------- footer ---------------
    QGroupBox *gFoot = new QGroupBox( 0, Qt::Vertical, i18n( "Foot Line" ), tab2 );
    gFoot->layout()->setSpacing(KDialog::spacingHint());
    gFoot->layout()->setMargin(KDialog::marginHint());
    QGridLayout *footGrid = new QGridLayout( gFoot->layout(), 2, 3 );

    QLabel *lFootLeft = new QLabel( i18n( "Left:" ), gFoot );
    footGrid->addWidget( lFootLeft, 0, 0 );

    eFootLeft = new QLineEdit( gFoot );
    footGrid->addWidget( eFootLeft, 1, 0 );
    eFootLeft->setText( hf.footLeft );

    QLabel *lFootMid = new QLabel( i18n( "Mid:" ), gFoot );
    footGrid->addWidget( lFootMid, 0, 1 );

    eFootMid = new QLineEdit( gFoot );
    footGrid->addWidget( eFootMid, 1, 1 );
    eFootMid->setText( hf.footMid );

    QLabel *lFootRight = new QLabel( i18n( "Right:" ), gFoot );
    footGrid->addWidget( lFootRight, 0, 2 );

    eFootRight = new QLineEdit( gFoot );
    footGrid->addWidget( eFootRight, 1, 2 );
    eFootRight->setText( hf.footRight );

    grid2->addMultiCellWidget( gFoot, 2, 3, 0, 1 );

    QLabel *lMacros2 = new QLabel( i18n( "You can insert several tags in the text:" ), tab2 );
    grid2->addMultiCellWidget( lMacros2, 4, 4, 0, 1 );

    QLabel *lMacros3 = new QLabel( i18n("<qt><ul><li>&lt;sheet&gt; The sheet name</li>"
                           "<li>&lt;page&gt; The current page</li>"
                           "<li>&lt;pages&gt; The total number of pages</li>"
                           "<li>&lt;name&gt; The filename or URL</li>"
                           "<li>&lt;file&gt; The filename with complete path or the URL</li></ul></qt>"), tab2 );
    grid2->addMultiCellWidget( lMacros3, 5, 6, 0, 0, Qt::AlignTop );

    QLabel *lMacros4 = new QLabel( i18n("<qt><ul><li>&lt;time&gt; The current time</li>"
                           "<li>&lt;date&gt; The current date</li>"
                           "<li>&lt;author&gt; Your full name</li>"
                           "<li>&lt;org&gt; Your organization</li>"
                           "<li>&lt;email&gt; Your email address</li></ul></qt>"), tab2 );
    grid2->addMultiCellWidget( lMacros4, 5, 6, 1, 1, Qt::AlignTop );
}

/*================================================================*/
void KoPageLayoutDia::setupTab3()
{
    QWidget *tab3 = addPage(i18n( "Col&umns" ));

    QGridLayout *grid3 = new QGridLayout( tab3, 5, 2, 0, KDialog::spacingHint() );

    QLabel *lColumns = new QLabel( i18n( "Co&lumns:" ), tab3 );
    grid3->addWidget( lColumns, 0, 0 );

    nColumns = new QSpinBox( 1, 16, 1, tab3 );
    lColumns->setBuddy( nColumns );
    grid3->addWidget( nColumns, 1, 0 );
    nColumns->setValue( m_cl.columns );
    connect( nColumns, SIGNAL( valueChanged( int ) ), this, SLOT( nColChanged( int ) ) );

    QString str = KoUnit::unitName( m_unit );


    QLabel *lCSpacing = new QLabel( i18n("Column &spacing:"), tab3 );
    grid3->addWidget( lCSpacing, 2, 0 );

    nCSpacing = new KoUnitDoubleSpinBox( tab3 );
    nCSpacing->setValue(  m_cl.ptColumnSpacing );
    nCSpacing->setUnit( m_unit );
    lCSpacing->setBuddy( nCSpacing );
    grid3->addWidget( nCSpacing, 3, 0 );

    connect( nCSpacing, SIGNAL( valueChanged(double) ),
             this, SLOT( nSpaceChanged( double ) ) );

    // ------------- preview -----------
    pgPreview2 = new KoPagePreview( tab3, "Preview", m_layout );
    grid3->addMultiCellWidget( pgPreview2, 0, 4, 1, 1 );

    // --------------- main grid ------------------
    grid3->addColSpacing( 0, lColumns->width() );
    grid3->addColSpacing( 0, nColumns->width() );
    grid3->addColSpacing( 0, lCSpacing->width() );
    grid3->addColSpacing( 0, nCSpacing->width() );
    grid3->addColSpacing( 1, pgPreview2->width() );
    grid3->setColStretch( 1, 1 );

    grid3->addRowSpacing( 0, lColumns->height() );
    grid3->addRowSpacing( 1, nColumns->height() );
    grid3->addRowSpacing( 2, lCSpacing->height() );
    grid3->addRowSpacing( 3, nCSpacing->height() );
    grid3->setRowStretch( 4, 1 );

    pgPreview2->setPageColumns( m_cl );
}

/*================================================================*/
void KoPageLayoutDia::setupTab4()
{
    QWidget *tab4 = addPage(i18n( "H&eader && Footer" ));
    QGridLayout *grid4 = new QGridLayout( tab4, 4, 1, 0, KDialog::spacingHint() );

    QButtonGroup *gHeader = new QButtonGroup( 0, Qt::Vertical, i18n( "&Header" ), tab4 );
    gHeader->layout()->setSpacing(KDialog::spacingHint());
    gHeader->layout()->setMargin(KDialog::marginHint());
    QGridLayout *headerGrid = new QGridLayout( gHeader->layout(), 4, 2 );

    rhFirst = new QCheckBox( i18n( "Different header for the first page" ), gHeader );
    gHeader->insert( rhFirst );
    headerGrid->addMultiCellWidget( rhFirst, 1, 1, 0, 1 );
    if ( kwhf.header == HF_FIRST_DIFF || kwhf.header == HF_FIRST_EO_DIFF )
        rhFirst->setChecked( true );

    rhEvenOdd = new QCheckBox( i18n( "Different header for even and odd pages" ), gHeader );
    gHeader->insert( rhEvenOdd );
    headerGrid->addMultiCellWidget( rhEvenOdd, 2, 2, 0, 1 );
    if ( kwhf.header == HF_EO_DIFF || kwhf.header == HF_FIRST_EO_DIFF )
        rhEvenOdd->setChecked( true );

    QLabel *lHSpacing = new QLabel( i18n("Spacing between header and body:"), gHeader );
    lHSpacing->setAlignment( AlignRight | AlignVCenter );
    headerGrid->addWidget( lHSpacing, 4, 0 );

    nHSpacing = new KoUnitDoubleSpinBox( gHeader, "" );
    headerGrid->addWidget( nHSpacing, 4, 1 );

    nHSpacing->setValue( kwhf.ptHeaderBodySpacing );
    nHSpacing->setUnit( m_unit );


    headerGrid->addRowSpacing( 0, KDialog::spacingHint() );

    grid4->addWidget( gHeader, 0, 0 );

    QButtonGroup *gFooter = new QButtonGroup( 0, Qt::Vertical, i18n( "&Footer" ), tab4 );
    gFooter->layout()->setSpacing(KDialog::spacingHint());
    gFooter->layout()->setMargin(KDialog::marginHint());
    QGridLayout *footerGrid = new QGridLayout( gFooter->layout(), 4, 2 );

    rfFirst = new QCheckBox( i18n( "Different footer for the first page" ), gFooter );
    gFooter->insert( rfFirst );
    footerGrid->addMultiCellWidget( rfFirst, 1, 1, 0, 1 );
    if ( kwhf.footer == HF_FIRST_DIFF || kwhf.footer == HF_FIRST_EO_DIFF )
        rfFirst->setChecked( true );

    rfEvenOdd = new QCheckBox( i18n( "Different footer for even and odd pages" ), gFooter );
    gFooter->insert( rfEvenOdd );
    footerGrid->addMultiCellWidget( rfEvenOdd, 2, 2, 0, 1 );
    if ( kwhf.footer == HF_EO_DIFF || kwhf.footer == HF_FIRST_EO_DIFF )
        rfEvenOdd->setChecked( true );

    QLabel *lFSpacing = new QLabel( i18n("Spacing between footer and body:"), gFooter );
    lFSpacing->setAlignment( AlignRight | AlignVCenter );
    footerGrid->addWidget( lFSpacing, 4, 0 );

    nFSpacing = new KoUnitDoubleSpinBox( gFooter, "" );
    footerGrid->addWidget( nFSpacing, 4, 1 );

    nFSpacing->setValue( kwhf.ptFooterBodySpacing );
    nFSpacing->setUnit( m_unit );

    footerGrid->addRowSpacing( 0, KDialog::spacingHint() );

    grid4->addWidget( gFooter, 2, 0 );

    QButtonGroup *gFootNote = new QButtonGroup( 0, Qt::Vertical, i18n( "Foot&note/Endnote" ), tab4 ); // why doesn't the accel work??? - Clarence
    gFootNote->layout()->setSpacing(KDialog::spacingHint());
    gFootNote->layout()->setMargin(KDialog::marginHint());
    QGridLayout *footNoteGrid = new QGridLayout( gFootNote->layout(), 2, 2 );

    QLabel *lFNSpacing = new QLabel( i18n("Spacing between footnote and body:"), gFootNote );
    lFNSpacing->setAlignment( AlignRight | AlignVCenter );
    footNoteGrid->addWidget( lFNSpacing, 1, 0 );

    nFNSpacing = new KoUnitDoubleSpinBox( gFootNote, "" );
    footNoteGrid->addWidget( nFNSpacing, 1, 1 );

    nFNSpacing->setValue( kwhf.ptFootNoteBodySpacing );
    nFNSpacing->setUnit( m_unit );

    footNoteGrid->addRowSpacing( 0, KDialog::spacingHint() );

    grid4->addWidget( gFootNote, 3, 0 );

    grid4->setRowStretch( 1, 1 ); // between the groupboxes
    grid4->setRowStretch( 2, 1 ); // between the groupboxes
    grid4->setRowStretch( 4, 10 ); // bottom
}


/*==================================================================*/
void KoPageLayoutDia::nColChanged( int _val )
{
    m_cl.columns = _val;
    updatePreview( );
}

/*==================================================================*/
void KoPageLayoutDia::nSpaceChanged( double _val )
{
    m_cl.ptColumnSpacing = KoUnit::fromUserValue( _val, m_unit );
    updatePreview( );
}

/* Validation when closing. Error messages are never liked, but
  better let the users enter all values in any order, and have one
  final validation, than preventing them from entering values. */
void KoPageLayoutDia::slotOk()
{
    if( m_sizeWidget )
        m_sizeWidget->queryClose();
    KDialogBase::slotOk(); // accept
}

void KoPageLayoutDia::updatePreview() {
    pgPreview2->setPageLayout( m_layout );
    pgPreview2->setPageColumns( m_cl );
}


KoPageLayoutSize::KoPageLayoutSize(QWidget *parent, const KoPageLayout& layout, KoUnit::Unit unit,const KoColumns& columns,  bool unitChooser, bool enableBorders)
    : QWidget(parent) {
    m_columns = columns;
    m_layout = layout;
    m_unit = unit;

    QGridLayout *grid1 = new QGridLayout( this, 5, 2, 0, KDialog::spacingHint() );
    if ( unitChooser ) {
        // ------------- unit _______________
        QWidget* unitFrame = new QWidget( this );
        grid1->addWidget( unitFrame, 0, 0, Qt::AlignLeft );
        QBoxLayout* unitLayout = new QHBoxLayout( unitFrame, KDialog::marginHint(), KDialog::spacingHint() );

        // label unit
        QLabel *lpgUnit = new QLabel( i18n( "Unit:" ), unitFrame );
        unitLayout->addWidget( lpgUnit, 0, Qt::AlignRight | Qt::AlignVCenter );

        // combo unit
        cpgUnit = new QComboBox( false, unitFrame, "cpgUnit" );
        cpgUnit->insertStringList( KoUnit::listOfUnitName() );
        cpgUnit->setCurrentItem( unit );
        unitLayout->addWidget( cpgUnit, 0, Qt::AlignLeft | Qt::AlignVCenter );
        connect( cpgUnit, SIGNAL( activated( int ) ), this, SLOT( setUnit( int ) ) );
    }
    else {
        QString str=KoUnit::unitDescription(unit);

        QLabel *lpgUnit = new QLabel( i18n("All values are given in %1.").arg(str), this );
        grid1->addWidget( lpgUnit, 0, 0, Qt::AlignLeft );
    }

    // -------------- page size -----------------
    QVGroupBox *formatFrame = new QVGroupBox( i18n( "Page Size" ), this );
    grid1->addWidget( formatFrame, 1, 0 );

    QHBox *formatPageSize = new QHBox( formatFrame );
    formatPageSize->setSpacing( KDialog::spacingHint() );

    // label page size
    QLabel *lpgFormat = new QLabel( i18n( "&Size:" ), formatPageSize );

    // combo size
    cpgFormat = new QComboBox( false, formatPageSize, "cpgFormat" );
    cpgFormat->insertStringList( KoPageFormat::allFormats() );
    lpgFormat->setBuddy( cpgFormat );
    connect( cpgFormat, SIGNAL( activated( int ) ), this, SLOT( formatChanged( int ) ) );

    // spacer
    formatPageSize->setStretchFactor( new QWidget( formatPageSize ), 10 );

    QHBox *formatCustomSize = new QHBox( formatFrame );
    formatCustomSize->setSpacing( KDialog::spacingHint() );

    // label width
    QLabel *lpgWidth = new QLabel( i18n( "&Width:" ), formatCustomSize );

    // linedit width
    epgWidth = new KoUnitDoubleSpinBox( formatCustomSize, "Width" );
    lpgWidth->setBuddy( epgWidth );
    if ( m_layout.format != PG_CUSTOM )
        epgWidth->setEnabled( false );
    connect( epgWidth, SIGNAL( valueChangedPt(double) ), this, SLOT( widthChanged(double) ) );

    // label height
    QLabel *lpgHeight = new QLabel( i18n( "&Height:" ), formatCustomSize );

    // linedit height
    epgHeight = new KoUnitDoubleSpinBox( formatCustomSize, "Height" );
    lpgHeight->setBuddy( epgHeight );
    if ( m_layout.format != PG_CUSTOM )
        epgHeight->setEnabled( false );
    connect( epgHeight, SIGNAL( valueChangedPt(double ) ), this, SLOT( heightChanged(double) ) );

    // --------------- orientation ---------------
    m_orientGroup = new QHButtonGroup( i18n( "Orientation" ), this );
    m_orientGroup->setInsideSpacing( KDialog::spacingHint() );
    grid1->addWidget( m_orientGroup, 2, 0 );

    QLabel* lbPortrait = new QLabel( m_orientGroup );
    lbPortrait->setPixmap( QPixmap( UserIcon( "koPortrait" ) ) );
    lbPortrait->setMaximumWidth( lbPortrait->pixmap()->width() );
    new QRadioButton( i18n("&Portrait"), m_orientGroup );

    QLabel* lbLandscape = new QLabel( m_orientGroup );
    lbLandscape->setPixmap( QPixmap( UserIcon( "koLandscape" ) ) );
    lbLandscape->setMaximumWidth( lbLandscape->pixmap()->width() );
    new QRadioButton( i18n("La&ndscape"), m_orientGroup );

    connect( m_orientGroup, SIGNAL (clicked (int)), this, SLOT( orientationChanged(int) ));

    // --------------- page margins ---------------
    QVGroupBox *marginsFrame = new QVGroupBox( i18n( "Margins" ), this );
    marginsFrame->setColumnLayout( 0, Qt::Vertical );
    marginsFrame->setMargin( KDialog::marginHint() );
    grid1->addWidget( marginsFrame, 3, 0 );

    QGridLayout *marginsLayout = new QGridLayout( marginsFrame->layout(), 3, 3,
       KDialog::spacingHint() );

    // left margin
    ebrLeft = new KoUnitDoubleSpinBox( marginsFrame, "Left" );
    marginsLayout->addWidget( ebrLeft, 1, 0 );
    connect( ebrLeft, SIGNAL( valueChangedPt( double ) ), this, SLOT( leftChanged( double ) ) );
    if ( !enableBorders ) ebrLeft->setEnabled( false );

    // right margin
    ebrRight = new KoUnitDoubleSpinBox( marginsFrame, "Right" );
    marginsLayout->addWidget( ebrRight, 1, 2 );
    connect( ebrRight, SIGNAL( valueChangedPt( double ) ), this, SLOT( rightChanged( double ) ) );
    if ( !enableBorders ) ebrRight->setEnabled( false );

    // top margin
    ebrTop = new KoUnitDoubleSpinBox( marginsFrame, "Top" );
    marginsLayout->addWidget( ebrTop, 0, 1 , Qt::AlignCenter );
    connect( ebrTop, SIGNAL( valueChangedPt( double ) ), this, SLOT( topChanged( double ) ) );
    if ( !enableBorders ) ebrTop->setEnabled( false );

    // bottom margin
    ebrBottom = new KoUnitDoubleSpinBox( marginsFrame, "Bottom" );
    marginsLayout->addWidget( ebrBottom, 2, 1, Qt::AlignCenter );
    connect( ebrBottom, SIGNAL( valueChangedPt( double ) ), this, SLOT( bottomChanged( double ) ) );
    if ( !enableBorders ) ebrBottom->setEnabled( false );

    // ------------- preview -----------
    pgPreview = new KoPagePreview( this, "Preview", m_layout );
    grid1->addMultiCellWidget( pgPreview, 1, 3, 1, 1 );

    // ------------- spacers -----------
    QWidget* spacer1 = new QWidget( this );
    QWidget* spacer2 = new QWidget( this );
    spacer1->setSizePolicy( QSizePolicy( QSizePolicy::Expanding,
       QSizePolicy::Expanding ) );
    spacer2->setSizePolicy( QSizePolicy( QSizePolicy::Expanding,
       QSizePolicy::Expanding ) );
    grid1->addWidget( spacer1, 4, 0 );
    grid1->addWidget( spacer2, 4, 1 );

    setValues();
    updatePreview();
}

void KoPageLayoutSize::updatePreview() {
    pgPreview->setPageLayout( m_layout );
    pgPreview->setPageColumns( m_columns );
}

void KoPageLayoutSize::setValues() {
    // page format
    cpgFormat->setCurrentItem( m_layout.format );
    // orientation
    m_orientGroup->setButton( m_layout.orientation == PG_PORTRAIT ? 0: 1 );

    setUnit( m_unit );
    pgPreview->setPageLayout( m_layout );
}

void KoPageLayoutSize::setUnit( KoUnit::Unit unit ) {
    m_unit = unit;

    //setUnit always befor changeValue
    epgWidth->setUnit( m_unit );
    epgWidth->setMinMaxStep( 0, KoUnit::fromUserValue( 9999, m_unit ), KoUnit::fromUserValue( 0.01, m_unit ) );
    epgWidth->changeValue( m_layout.ptWidth );

    epgHeight->setUnit( m_unit );
    epgHeight->setMinMaxStep( 0, KoUnit::fromUserValue( 9999, m_unit ), KoUnit::fromUserValue( 0.01, m_unit ) );
    epgHeight->changeValue( m_layout.ptHeight );

    double dStep = KoUnit::fromUserValue( 0.2, m_unit );

    ebrLeft->setUnit( m_unit );
    ebrLeft->changeValue( m_layout.ptLeft );
    ebrLeft->setMinMaxStep( 0, m_layout.ptWidth, dStep );

    ebrRight->setUnit( m_unit );
    ebrRight->changeValue( m_layout.ptRight );
    ebrRight->setMinMaxStep( 0, m_layout.ptWidth, dStep );

    ebrTop->setUnit( m_unit );
    ebrTop->changeValue( m_layout.ptTop );
    ebrTop->setMinMaxStep( 0, m_layout.ptHeight, dStep );

    ebrBottom->setUnit( m_unit );
    ebrBottom->changeValue( m_layout.ptBottom );
    ebrBottom->setMinMaxStep( 0, m_layout.ptHeight, dStep );
    emit propertyChange(m_layout);
}

void KoPageLayoutSize::formatChanged( int format ) {
    if ( ( KoFormat )format == m_layout.format )
        return;
    m_layout.format = ( KoFormat )format;
    bool enable =  (KoFormat) format != PG_CUSTOM;
    epgWidth->setEnabled( enable );
    epgHeight->setEnabled( enable );

    if ( m_layout.format != PG_CUSTOM ) {
        m_layout.ptWidth = MM_TO_POINT( KoPageFormat::width(
                    m_layout.format, m_layout.orientation ) );
        m_layout.ptHeight = MM_TO_POINT( KoPageFormat::height(
                    m_layout.format, m_layout.orientation ) );
kdDebug() << "updated to " << MM_TO_POINT( KoPageFormat::width( m_layout.format, m_layout.orientation )) << endl;
kdDebug() << "         2 " << m_layout.ptWidth << endl;
    }

    epgWidth->changeValue( m_layout.ptWidth );
    epgHeight->changeValue( m_layout.ptHeight );

    updatePreview( );
    emit propertyChange(m_layout);
}

void KoPageLayoutSize::orientationChanged(int which) {
    KoOrientation oldOrientation = m_layout.orientation;
    m_layout.orientation = which == 0 ? PG_PORTRAIT : PG_LANDSCAPE;

    // without this check, width & height would be swapped around (below)
    // even though the orientation has not changed
    if (m_layout.orientation == oldOrientation) return;

    m_layout.ptWidth = epgWidth->value();
kdDebug() << "         3 " << m_layout.ptWidth << endl;
    m_layout.ptHeight = epgHeight->value();
    m_layout.ptLeft = ebrLeft->value();
    m_layout.ptRight = ebrRight->value();
    m_layout.ptTop = ebrTop->value();
    m_layout.ptBottom = ebrBottom->value();

    // swap dimension and adjust margins
    qSwap( m_layout.ptWidth, m_layout.ptHeight );
    double tmp = m_layout.ptTop;
    m_layout.ptTop = m_layout.ptRight;
    m_layout.ptRight = m_layout.ptBottom;
    m_layout.ptBottom = m_layout.ptLeft;
    m_layout.ptLeft = tmp;

    setValues();
    updatePreview( );
    emit propertyChange(m_layout);
}

void KoPageLayoutSize::widthChanged(double width) {
    m_layout.ptWidth = width;
kdDebug() << "         4 " << m_layout.ptWidth << endl;
    updatePreview();
    emit propertyChange(m_layout);
}
void KoPageLayoutSize::heightChanged(double height) {
    m_layout.ptHeight = height;
    updatePreview( );
    emit propertyChange(m_layout);
}
void KoPageLayoutSize::leftChanged( double left ) {
    m_layout.ptLeft = left;
    updatePreview();
    emit propertyChange(m_layout);
}
void KoPageLayoutSize::rightChanged(double right) {
    m_layout.ptRight = right;
    updatePreview();
    emit propertyChange(m_layout);
}
void KoPageLayoutSize::topChanged(double top) {
    m_layout.ptTop = top;
    updatePreview();
    emit propertyChange(m_layout);
}
void KoPageLayoutSize::bottomChanged(double bottom) {
    m_layout.ptBottom = bottom;
    updatePreview();
    emit propertyChange(m_layout);
}

bool KoPageLayoutSize::queryClose() {
    if ( m_layout.ptLeft + m_layout.ptRight > m_layout.ptWidth ) {
        KMessageBox::error( this,
            i18n("The page width is smaller than the left and right margins."),
                            i18n("Page Layout Problem") );
        return false;
    }
    if ( m_layout.ptTop + m_layout.ptBottom > m_layout.ptHeight ) {
        KMessageBox::error( this,
            i18n("The page height is smaller than the top and bottom margins."),
                            i18n("Page Layout Problem") );
        return false;
    }
    return true;
}

#include <koPageLayoutDia.moc>
