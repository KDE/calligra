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

// Description: Page Layout Dialog (header)

/******************************************************************/

#include <koPageLayoutDia.h>

#include <qcombobox.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpen.h>
#include <qbrush.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qvalidator.h>
#include <qspinbox.h>

#include <klocale.h>


/******************************************************************/
/* class KoPagePreview						  */
/******************************************************************/

/*===================== constrcutor ==============================*/
KoPagePreview::KoPagePreview( QWidget* parent, const char *name, KoPageLayout _layout )
    : QGroupBox( i18n( "Page Preview" ), parent, name )
{
    setPageLayout( _layout );
    columns = 1;
}

/*====================== destructor ==============================*/
KoPagePreview::~KoPagePreview()
{
}

/*=================== set layout =================================*/
void KoPagePreview::setPageLayout( const KoPageLayout &_layout )
{
    pgWidth = _layout.mmWidth * 0.5;
    pgHeight = _layout.mmHeight * 0.5;

    pgX =  _layout.mmLeft * 0.5;
    pgY = _layout.mmTop * 0.5;
    pgW = pgWidth - ( _layout.mmLeft + _layout.mmRight ) * 0.5;
    pgH = pgHeight - ( _layout.mmTop + _layout.mmBottom ) * 0.5;

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
    double cw = pgW;
    if(columns!=1)
	cw/=static_cast<double>(columns);

    painter->setBrush( white );
    painter->setPen( QPen( black ) );

    int x=static_cast<int>( ( width() - pgWidth ) * 0.5 );
    int y=static_cast<int>( ( height() - pgHeight ) * 0.5 );
    int w=static_cast<int>(pgWidth);
    int h=static_cast<int>(pgHeight);
    painter->drawRect( x + 1, y + 1, w, h);
    painter->drawRect( x, y, w, h );

    painter->setBrush( QBrush( black, HorPattern ) );
    if ( pgW == pgWidth || pgH == pgHeight )
	painter->setPen( NoPen );
    else
	painter->setPen( lightGray );

    for ( int i = 0; i < columns; ++i )
	painter->drawRect( x + static_cast<int>(pgX) + static_cast<int>(i * cw),
			   y + static_cast<int>(pgY), static_cast<int>(cw),
			   static_cast<int>(pgH) );
}

/******************************************************************/
/* class KoPageLayoutDia					  */
/******************************************************************/

/*==================== constructor ===============================*/
KoPageLayoutDia::KoPageLayoutDia( QWidget* parent, const char* name, KoPageLayout _layout,
				  KoHeadFoot _hf, int tabs )
    : QTabDialog( parent, name, true )
{
    flags = tabs;
    pgPreview = 0;
    pgPreview2 = 0;

    layout = _layout;
    hf = _hf;

    cl.columns = 1;

    enableBorders = true;

    if ( tabs & FORMAT_AND_BORDERS ) setupTab1();
    if ( tabs & HEADER_AND_FOOTER ) setupTab2();

    setCancelButton( i18n( "Cancel" ) );
    setOkButton( i18n( "OK" ) );

    retPressed = false;

    setCaption( i18n( "Page Layout" ) );
    setFocusPolicy( QWidget::StrongFocus );
    setFocus();

    // FIXME (Werner)
    resize( 600, 500 );
}

/*==================== constructor ===============================*/
KoPageLayoutDia::KoPageLayoutDia( QWidget* parent, const char* name, KoPageLayout _layout, KoHeadFoot _hf,
				 KoColumns _cl, KoKWHeaderFooter _kwhf, int tabs )
    : QTabDialog( parent, name, true )
{
    flags = tabs;
    pgPreview = 0;
    pgPreview2 = 0;

    layout = _layout;
    hf = _hf;
    cl = _cl;
    kwhf = _kwhf;

    enableBorders = true;

    if ( tabs & DISABLE_BORDERS ) enableBorders = false;
    if ( tabs & FORMAT_AND_BORDERS ) setupTab1();
    if ( tabs & HEADER_AND_FOOTER ) setupTab2();
    if ( tabs & COLUMNS ) setupTab3();
    if ( tabs & KW_HEADER_AND_FOOTER ) setupTab4();

    setCancelButton( i18n( "Cancel" ) );
    setOkButton( i18n( "OK" ) );

    retPressed = false;

    setCaption( i18n( "Page Layout" ) );
    setFocusPolicy( QWidget::StrongFocus );
    setFocus();

    // FIXME (Werner)
    resize( 600, 500 );
}

/*===================== destructor ===============================*/
KoPageLayoutDia::~KoPageLayoutDia()
{
}

/*======================= show dialog ============================*/
bool KoPageLayoutDia::pageLayout( KoPageLayout& _layout, KoHeadFoot& _hf, int _tabs )
{
    bool res = false;
    KoPageLayoutDia *dlg = new KoPageLayoutDia( 0, "PageLayout", _layout, _hf, _tabs );

    if ( dlg->exec() == QDialog::Accepted ) {
	res = true;
	if ( _tabs & FORMAT_AND_BORDERS ) _layout = dlg->getLayout();
	if ( _tabs & HEADER_AND_FOOTER ) _hf = dlg->getHeadFoot();
    }

    delete dlg;

    return res;
}

/*======================= show dialog ============================*/
bool KoPageLayoutDia::pageLayout( KoPageLayout& _layout, KoHeadFoot& _hf, KoColumns& _cl,
				  KoKWHeaderFooter &_kwhf, int _tabs )
{
    bool res = false;
    KoPageLayoutDia *dlg = new KoPageLayoutDia( 0, "PageLayout", _layout, _hf, _cl, _kwhf, _tabs );

    if ( dlg->exec() == QDialog::Accepted ) {
	res = true;
	if ( _tabs & FORMAT_AND_BORDERS ) _layout = dlg->getLayout();
	if ( _tabs & HEADER_AND_FOOTER ) _hf = dlg->getHeadFoot();
	if ( _tabs & COLUMNS ) _cl = dlg->getColumns();
	if ( _tabs & KW_HEADER_AND_FOOTER ) _kwhf = dlg->getKWHeaderFooter();
    }

    delete dlg;

    return res;
}

/*===================== get a standard page layout ===============*/
KoPageLayout KoPageLayoutDia::standardLayout()
{
    KoPageLayout	_layout;

    _layout.format = PG_DIN_A4;
    _layout.orientation = PG_PORTRAIT;
    _layout.mmWidth = PG_A4_WIDTH;
    _layout.mmHeight = PG_A4_HEIGHT;
    _layout.mmLeft = 20.0;
    _layout.mmRight = 20.0;
    _layout.mmTop = 20.0;
    _layout.mmBottom = 20.0;
    _layout.unit = PG_MM;
    _layout.ptWidth = MM_TO_POINT( PG_A4_WIDTH );
    _layout.ptHeight = MM_TO_POINT( PG_A4_HEIGHT );
    _layout.ptLeft = MM_TO_POINT( 20.0 );
    _layout.ptRight = MM_TO_POINT( 20.0 );
    _layout.ptTop = MM_TO_POINT( 20.0 );
    _layout.ptBottom = MM_TO_POINT( 20.0 );
    _layout.inchWidth = PG_A4_WIDTH_I;
    _layout.inchHeight = PG_A4_HEIGHT_I;
    _layout.inchLeft = MM_TO_INCH( 20.0 );
    _layout.inchRight = MM_TO_INCH( 20.0 );
    _layout.inchTop = MM_TO_INCH( 20.0 );
    _layout.inchBottom = MM_TO_INCH( 20.0 );

    return  _layout;
}

/*====================== get header - footer =====================*/
KoHeadFoot KoPageLayoutDia::getHeadFoot()
{
    hf.headLeft = eHeadLeft->text();
    hf.headMid = eHeadMid->text();
    hf.headRight = eHeadRight->text();
    hf.footLeft = eFootLeft->text();
    hf.footMid = eFootMid->text();
    hf.footRight = eFootRight->text();

    return hf;
}

/*================================================================*/
KoColumns KoPageLayoutDia::getColumns()
{
    cl.columns = nColumns->value();

    switch ( layout.unit ) {
    case PG_MM: {
	cl.mmColumnSpacing = nCSpacing->text().toDouble();
	cl.ptColumnSpacing = MM_TO_POINT( cl.mmColumnSpacing );
	cl.inchColumnSpacing = MM_TO_INCH( cl.mmColumnSpacing );
    } break;
    case PG_PT: {
	cl.ptColumnSpacing = nCSpacing->text().toDouble();
	cl.mmColumnSpacing = POINT_TO_MM( cl.ptColumnSpacing );
	cl.inchColumnSpacing = POINT_TO_INCH( cl.ptColumnSpacing );
    } break;
    case PG_INCH: {
	cl.inchColumnSpacing = nCSpacing->text().toDouble();
	cl.ptColumnSpacing = INCH_TO_POINT( cl.inchColumnSpacing );
	cl.mmColumnSpacing = INCH_TO_MM( cl.inchColumnSpacing );
    } break;
    }

    return cl;
}

/*================================================================*/
KoKWHeaderFooter KoPageLayoutDia::getKWHeaderFooter()
{
    if ( rhSame->isChecked() )
	kwhf.header = HF_SAME;
    else if ( rhFirst->isChecked() )
	kwhf.header = HF_FIRST_DIFF;
    else if ( rhEvenOdd->isChecked() )
	kwhf.header = HF_EO_DIFF;

    switch ( layout.unit ) {
	case PG_MM: {
	    kwhf.mmHeaderBodySpacing = nHSpacing->text().toDouble();
	    kwhf.ptHeaderBodySpacing = MM_TO_POINT( kwhf.mmHeaderBodySpacing );
	    kwhf.inchHeaderBodySpacing = MM_TO_INCH( kwhf.mmHeaderBodySpacing );
	    kwhf.mmFooterBodySpacing = nFSpacing->text().toDouble();
	    kwhf.ptFooterBodySpacing = MM_TO_POINT( kwhf.mmFooterBodySpacing );
	    kwhf.inchFooterBodySpacing = MM_TO_INCH( kwhf.mmFooterBodySpacing );
	} break;
	case PG_PT: {
	    kwhf.ptHeaderBodySpacing = nHSpacing->text().toDouble();
	    kwhf.mmHeaderBodySpacing = POINT_TO_MM( kwhf.ptHeaderBodySpacing );
	    kwhf.inchHeaderBodySpacing = POINT_TO_INCH( kwhf.ptHeaderBodySpacing );
	    kwhf.ptFooterBodySpacing = nFSpacing->text().toDouble();
	    kwhf.mmFooterBodySpacing = POINT_TO_MM( kwhf.ptFooterBodySpacing );
	    kwhf.inchFooterBodySpacing = POINT_TO_INCH( kwhf.ptFooterBodySpacing );
	} break;
	case PG_INCH: {
	    kwhf.inchHeaderBodySpacing = nHSpacing->text().toDouble();
	    kwhf.ptHeaderBodySpacing = INCH_TO_POINT( kwhf.inchHeaderBodySpacing );
	    kwhf.mmHeaderBodySpacing = INCH_TO_MM( kwhf.inchHeaderBodySpacing );
	    kwhf.inchFooterBodySpacing = nHSpacing->text().toDouble();
	    kwhf.ptFooterBodySpacing = INCH_TO_POINT( kwhf.inchFooterBodySpacing );
	    kwhf.mmFooterBodySpacing = INCH_TO_MM( kwhf.inchFooterBodySpacing );
	} break;
    }

    if ( rfSame->isChecked() )
	kwhf.footer = HF_SAME;
    else if ( rfFirst->isChecked() )
	kwhf.footer = HF_FIRST_DIFF;
    else if ( rfEvenOdd->isChecked() )
	kwhf.footer = HF_EO_DIFF;

    return kwhf;
}

/*================ setup format and borders tab ==================*/
void KoPageLayoutDia::setupTab1()
{
    tab1 = new QWidget( this );

    grid1 = new QGridLayout( tab1, 5, 2, 15, 7 );

    if ( !( flags & DISABLE_UNIT ) ) {
	// ------------- unit _______________
	// label unit
	lpgUnit = new QLabel( i18n( "Unit:" ), tab1 );
	lpgUnit->resize( lpgUnit->sizeHint() );
	grid1->addWidget( lpgUnit, 0, 0 );

	// combo unit
	cpgUnit = new QComboBox( false, tab1, "cpgUnit" );
	cpgUnit->setAutoResize( false );
	cpgUnit->insertItem( i18n( "Millimeters ( mm )" ) );
	cpgUnit->insertItem( i18n( "Points ( pt )" ) );
	cpgUnit->insertItem( i18n( "Inches ( in )" ) );
	cpgUnit->resize( cpgUnit->sizeHint() );
	grid1->addWidget( cpgUnit, 1, 0 );
	connect( cpgUnit, SIGNAL( activated( int ) ), this, SLOT( unitChanged( int ) ) );
	//if ( !( flags & USE_NEW_STUFF ) ) cpgUnit->setEnabled( false );
    } else {
	QString str;
	switch ( layout.unit ) {
	case PG_MM: str = "mm"; break;
	case PG_PT: str = "pt"; break;
	case PG_INCH: str = "inch"; break;
	}

	lpgUnit = new QLabel( i18n( QString( "All values are given in " + str ) ), tab1 );
	lpgUnit->resize( lpgUnit->sizeHint() );
	grid1->addWidget( lpgUnit, 0, 0 );
    }

    // -------------- page format -----------------
    formatFrame = new QGroupBox( i18n( "Page Format" ), tab1 );
    formatGrid = new QGridLayout( formatFrame, 4, 2, 15, 7 );

    // label format
    lpgFormat = new QLabel( i18n( "\nFormat:" ), formatFrame );
    lpgFormat->resize( lpgFormat->sizeHint() );
    formatGrid->addWidget( lpgFormat, 0, 0 );

    // label orientation
    lpgOrientation = new QLabel( i18n( "\nOrientation:" ), formatFrame );
    lpgOrientation->resize( lpgOrientation->sizeHint() );
    formatGrid->addWidget( lpgOrientation, 0, 1 );

    // combo format
    cpgFormat = new QComboBox( false, formatFrame, "cpgFormat" );
    cpgFormat->setAutoResize( false );
    cpgFormat->insertItem( i18n( "DIN A3" ) );
    cpgFormat->insertItem( i18n( "DIN A4" ) );
    cpgFormat->insertItem( i18n( "DIN A5" ) );
    cpgFormat->insertItem( i18n( "US Letter" ) );
    cpgFormat->insertItem( i18n( "US Legal" ) );
    cpgFormat->insertItem( i18n( "Screen" ) );
    cpgFormat->insertItem( i18n( "Custom" ) );
    cpgFormat->insertItem( i18n( "DIN B5" ) );
    cpgFormat->insertItem( i18n( "US Executive" ) );
    cpgFormat->resize( cpgFormat->sizeHint() );
    formatGrid->addWidget( cpgFormat, 1, 0 );
    connect( cpgFormat, SIGNAL( activated( int ) ), this, SLOT( formatChanged( int ) ) );

    // combo orientation
    cpgOrientation = new QComboBox( false, formatFrame, "cpgOrientation" );
    cpgOrientation->setAutoResize( false );
    cpgOrientation->insertItem( i18n( "Portrait" ) );
    cpgOrientation->insertItem( i18n( "Landscape" ) );
    cpgOrientation->resize( cpgOrientation->sizeHint() );
    formatGrid->addWidget( cpgOrientation, 1, 1 );
    connect( cpgOrientation, SIGNAL( activated( int ) ), this, SLOT( orientationChanged( int ) ) );

    // label width
    lpgWidth = new QLabel( i18n( "Width:" ), formatFrame );
    lpgWidth->resize( lpgWidth->sizeHint() );
    formatGrid->addWidget( lpgWidth, 2, 0 );

    // linedit width
    epgWidth = new QLineEdit( formatFrame, "Width" );
    epgWidth->setValidator( new QDoubleValidator( epgWidth ) );
    epgWidth->setText( "000.00" );
    epgWidth->setMaxLength( 6 );
    epgWidth->setEchoMode( QLineEdit::Normal );
    epgWidth->setFrame( true );
    epgWidth->resize( epgWidth->sizeHint().width()/2, epgWidth->sizeHint().height() );
    formatGrid->addWidget( epgWidth, 3, 0 );
    if ( layout.format != PG_CUSTOM )
	epgWidth->setEnabled( false );
    connect( epgWidth, SIGNAL( returnPressed() ), this, SLOT( rPressed() ) );
    connect( epgWidth, SIGNAL( returnPressed() ), this, SLOT( widthChanged() ) );
    connect( epgWidth, SIGNAL( textChanged( const QString& ) ), this, SLOT( widthChanged() ) );

    // label height
    lpgHeight = new QLabel( i18n( "Height:" ), formatFrame );
    lpgHeight->resize( lpgHeight->sizeHint() );
    formatGrid->addWidget( lpgHeight, 2, 1 );

    // linedit height
    epgHeight = new QLineEdit( formatFrame, "Height" );
    epgHeight->setValidator( new QDoubleValidator( epgHeight ) );
    epgHeight->setText( "000.00" );
    epgHeight->setMaxLength( 6 );
    epgHeight->setEchoMode( QLineEdit::Normal );
    epgHeight->setFrame( true );
    epgHeight->resize( epgHeight->sizeHint().width()/2, epgHeight->sizeHint().height() );
    formatGrid->addWidget( epgHeight, 3, 1 );
    if ( layout.format != PG_CUSTOM )
	epgHeight->setEnabled( false );
    connect( epgHeight, SIGNAL( returnPressed() ), this, SLOT( rPressed() ) );
    connect( epgHeight, SIGNAL( returnPressed() ), this, SLOT( heightChanged() ) );
    connect( epgHeight, SIGNAL( textChanged( const QString & ) ), this, SLOT( heightChanged() ) );

    // grid col spacing
    formatGrid->addColSpacing( 0, lpgFormat->width() );
    formatGrid->addColSpacing( 0, cpgFormat->width() );
    formatGrid->addColSpacing( 0, lpgWidth->width() );
    formatGrid->addColSpacing( 0, epgWidth->width() );
    formatGrid->addColSpacing( 1, lpgOrientation->width() );
    formatGrid->addColSpacing( 1, cpgOrientation->width() );
    formatGrid->addColSpacing( 1, lpgHeight->width() );
    formatGrid->addColSpacing( 1, epgHeight->width() );

    // grid row spacing
    formatGrid->addRowSpacing( 0, lpgFormat->height() );
    formatGrid->addRowSpacing( 0, lpgOrientation->height() );
    formatGrid->addRowSpacing( 1, cpgFormat->height() );
    formatGrid->addRowSpacing( 1, cpgOrientation->height() );
    formatGrid->addRowSpacing( 2, lpgWidth->height() );
    formatGrid->addRowSpacing( 2, lpgHeight->height() );
    formatGrid->addRowSpacing( 3, epgWidth->height() );
    formatGrid->addRowSpacing( 3, epgHeight->height() );

    // activate grid
    //formatGrid->activate();
    //formatFrame->resize( 0, 0 );
    grid1->addWidget( formatFrame, 2, 0 );

    // --------------- page borders ---------------
    borderFrame = new QGroupBox( i18n( "Page Borders" ), tab1 );
    borderGrid = new QGridLayout( borderFrame, 4, 2, 15, 7 );

    // label left
    lbrLeft = new QLabel( i18n( "\nLeft:" ), borderFrame );
    lbrLeft->resize( lbrLeft->sizeHint() );
    borderGrid->addWidget( lbrLeft, 0, 0 );

    // linedit left
    ebrLeft = new QLineEdit( borderFrame, "Left" );
    ebrLeft->setValidator( new QDoubleValidator( ebrLeft ) );
    ebrLeft->setText( "000.00" );
    ebrLeft->setMaxLength( 6 );
    ebrLeft->setEchoMode( QLineEdit::Normal );
    ebrLeft->setFrame( true );
    ebrLeft->resize( ebrLeft->sizeHint().width()/2, ebrLeft->sizeHint().height() );
    borderGrid->addWidget( ebrLeft, 1, 0 );
    connect( ebrLeft, SIGNAL( returnPressed() ), this, SLOT( rPressed() ) );
    connect( ebrLeft, SIGNAL( returnPressed() ), this, SLOT( leftChanged() ) );
    connect( ebrLeft, SIGNAL( textChanged( const QString & ) ), this, SLOT( leftChanged() ) );
    if ( !enableBorders ) ebrLeft->setEnabled( false );

    // label right
    lbrRight = new QLabel( i18n( "\nRight:" ), borderFrame );
    lbrRight->resize( lbrRight->sizeHint() );
    borderGrid->addWidget( lbrRight, 0, 1 );

    // linedit right
    ebrRight = new QLineEdit( borderFrame, "Right" );
    ebrRight->setValidator( new QDoubleValidator( ebrRight ) );
    ebrRight->setText( "000.00" );
    ebrRight->setMaxLength( 6 );
    ebrRight->setEchoMode( QLineEdit::Normal );
    ebrRight->setFrame( true );
    ebrRight->resize( ebrRight->sizeHint().width()/2, ebrRight->sizeHint().height() );
    borderGrid->addWidget( ebrRight, 1, 1 );
    connect( ebrRight, SIGNAL( returnPressed() ), this, SLOT( rPressed() ) );
    connect( ebrRight, SIGNAL( returnPressed() ), this, SLOT( rightChanged() ) );
    connect( ebrRight, SIGNAL( textChanged( const QString & ) ), this, SLOT( rightChanged() ) );
    if ( !enableBorders ) ebrRight->setEnabled( false );

    // label top
    lbrTop = new QLabel( i18n( "Top:" ), borderFrame );
    lbrTop->resize( lbrTop->sizeHint() );
    borderGrid->addWidget( lbrTop, 2, 0 );

    // linedit top
    ebrTop = new QLineEdit( borderFrame, "Top" );
    ebrTop->setValidator( new QDoubleValidator( ebrTop ) );
    ebrTop->setText( "000.00" );
    ebrTop->setMaxLength( 6 );
    ebrTop->setEchoMode( QLineEdit::Normal );
    ebrTop->setFrame( true );
    ebrTop->resize( ebrTop->sizeHint().width()/2, ebrTop->sizeHint().height() );
    borderGrid->addWidget( ebrTop, 3, 0 );
    connect( ebrTop, SIGNAL( returnPressed() ), this, SLOT( rPressed() ) );
    connect( ebrTop, SIGNAL( returnPressed() ), this, SLOT( topChanged() ) );
    connect( ebrTop, SIGNAL( textChanged( const QString & ) ), this, SLOT( topChanged() ) );
    if ( !enableBorders ) ebrTop->setEnabled( false );

    // label bottom
    lbrBottom = new QLabel( i18n( "Bottom:" ), borderFrame );
    lbrBottom->resize( lbrBottom->sizeHint() );
    borderGrid->addWidget( lbrBottom, 2, 1 );

    // linedit bottom
    ebrBottom = new QLineEdit( borderFrame, "Bottom" );
    ebrBottom->setValidator( new QDoubleValidator( ebrBottom ) );
    ebrBottom->setText( "000.00" );
    ebrBottom->setMaxLength( 6 );
    ebrBottom->setEchoMode( QLineEdit::Normal );
    ebrBottom->setFrame( true );
    ebrBottom->resize( ebrBottom->sizeHint().width()/2, ebrBottom->sizeHint().height() );
    borderGrid->addWidget( ebrBottom, 3, 1 );
    connect( ebrBottom, SIGNAL( returnPressed() ), this, SLOT( rPressed() ) );
    connect( ebrBottom, SIGNAL( returnPressed() ), this, SLOT( bottomChanged() ) );
    connect( ebrBottom, SIGNAL( textChanged( const QString & ) ), this, SLOT( bottomChanged() ) );
    if ( !enableBorders ) ebrBottom->setEnabled( false );

    // grid col spacing
    borderGrid->addColSpacing( 0, lbrLeft->width() );
    borderGrid->addColSpacing( 0, ebrLeft->width() );
    borderGrid->addColSpacing( 0, lbrTop->width() );
    borderGrid->addColSpacing( 0, ebrTop->width() );
    borderGrid->addColSpacing( 1, lbrRight->width() );
    borderGrid->addColSpacing( 1, ebrRight->width() );
    borderGrid->addColSpacing( 1, lbrBottom->width() );
    borderGrid->addColSpacing( 1, ebrBottom->width() );

    // grid row spacing
    borderGrid->addRowSpacing( 0, lbrLeft->height() );
    borderGrid->addRowSpacing( 0, lbrRight->height() );
    borderGrid->addRowSpacing( 1, ebrLeft->height() );
    borderGrid->addRowSpacing( 1, ebrRight->height() );
    borderGrid->addRowSpacing( 2, lbrTop->height() );
    borderGrid->addRowSpacing( 2, lbrBottom->height() );
    borderGrid->addRowSpacing( 3, ebrTop->height() );
    borderGrid->addRowSpacing( 3, ebrBottom->height() );

    // activate grid
    //borderGrid->activate();
    //borderFrame->resize( 0, 0 );
    grid1->addWidget( borderFrame, 3, 0 );

    // ------------- preview -----------
    pgPreview = new KoPagePreview( tab1, "Preview", layout );
    grid1->addMultiCellWidget( pgPreview, 2, 4, 1, 1 );

    // --------------- main grid ------------------
    grid1->addColSpacing( 0, lpgUnit->width() );
    grid1->addColSpacing( 0, ( flags & DISABLE_UNIT ) ? 0 : cpgUnit->width() );
    grid1->addColSpacing( 0, formatFrame->width() );
    grid1->addColSpacing( 0, borderFrame->width() );
    grid1->addColSpacing( 1, 280 );
    grid1->setColStretch( 1, 1 );

    grid1->addRowSpacing( 0, lpgUnit->height() );
    grid1->addRowSpacing( 1, ( flags & DISABLE_UNIT ) ? 0 : cpgUnit->height() );
    grid1->addRowSpacing( 2, formatFrame->height() );
    grid1->addRowSpacing( 2, 120 );
    grid1->addRowSpacing( 3, borderFrame->height() );
    grid1->addRowSpacing( 3, 120 );
    grid1->setRowStretch( 4, 1 );

    //grid1->activate();

    addTab( tab1, i18n( "Format and Borders" ) );

    setValuesTab1();
    updatePreview( layout );
}

/*================= setup values for tab one =====================*/
void KoPageLayoutDia::setValuesTab1()
{
    // unit
    if ( !( flags & DISABLE_UNIT ) )
	cpgUnit->setCurrentItem( layout.unit );

    // page format
    cpgFormat->setCurrentItem( layout.format );

    // orientation
    cpgOrientation->setCurrentItem( layout.orientation );

    QString tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;

    switch ( layout.unit ) {
	case PG_MM: {
	    tmp1=QString::number(layout.mmWidth, 'f', 2);
	    tmp2=QString::number(layout.mmHeight, 'f', 2);
	    tmp3=QString::number(layout.mmLeft, 'f', 2);
	    tmp4=QString::number(layout.mmRight, 'f', 2);
	    tmp5=QString::number(layout.mmTop, 'f', 2);
	    tmp6=QString::number(layout.mmBottom, 'f', 2);
	} break;
	case PG_PT: {
	    tmp1=QString::number(layout.ptWidth, 'f', 2);
	    tmp2=QString::number(layout.ptHeight, 'f', 2);
	    tmp3=QString::number(layout.ptLeft, 'f', 2);
	    tmp4=QString::number(layout.ptRight, 'f', 2);
	    tmp5=QString::number(layout.ptTop, 'f', 2);
	    tmp6=QString::number(layout.ptBottom, 'f', 2);
	} break;
	case PG_INCH: {
	    tmp1=QString::number(layout.inchWidth, 'f', 2);
	    tmp2=QString::number(layout.inchHeight, 'f', 2);
	    tmp3=QString::number(layout.inchLeft, 'f', 2);
	    tmp4=QString::number(layout.inchRight, 'f', 2);
	    tmp5=QString::number(layout.inchTop, 'f', 2);
	    tmp6=QString::number(layout.inchBottom, 'f', 2);
	} break;
    }

    epgWidth->setText( tmp1 );
    epgHeight->setText( tmp2 );
    ebrLeft->setText( tmp3 );
    ebrRight->setText( tmp4 );
    ebrTop->setText( tmp5 );
    ebrBottom->setText( tmp6 );

    pgPreview->setPageLayout( layout );
}

/*================ setup header and footer tab ===================*/
void KoPageLayoutDia::setupTab2()
{
    tab2 = new QWidget( this );

    grid2 = new QGridLayout( tab2, 7, 3, 6, 6 );

    // ------------- header ---------------
    lHead = new QLabel( i18n( "Head line:" ), tab2 );
    grid2->addWidget( lHead, 0, 0 );

    lHeadLeft = new QLabel( i18n( "Left:" ), tab2 );
    grid2->addWidget( lHeadLeft, 1, 0 );

    eHeadLeft = new QLineEdit( tab2 );
    grid2->addWidget( eHeadLeft, 2, 0 );
    eHeadLeft->setText( hf.headLeft );

    lHeadMid = new QLabel( i18n( "Mid:" ), tab2 );
    grid2->addWidget( lHeadMid, 1, 1 );

    eHeadMid = new QLineEdit( tab2 );
    grid2->addWidget( eHeadMid, 2, 1 );
    eHeadMid->setText( hf.headMid );

    lHeadRight = new QLabel( i18n( "Right:" ), tab2 );
    grid2->addWidget( lHeadRight, 1, 2 );

    eHeadRight = new QLineEdit( tab2 );
    grid2->addWidget( eHeadRight, 2, 2 );
    eHeadRight->setText( hf.headRight );

    // ------------- footer ---------------
    lFoot = new QLabel( i18n( "\nFoot line:" ), tab2 );
    grid2->addWidget( lFoot, 3, 0 );

    lFootLeft = new QLabel( i18n( "Left:" ), tab2 );
    grid2->addWidget( lFootLeft, 4, 0 );

    eFootLeft = new QLineEdit( tab2 );
    grid2->addWidget( eFootLeft, 5, 0 );
    eFootLeft->setText( hf.footLeft );

    lFootMid = new QLabel( i18n( "Mid:" ), tab2 );
    grid2->addWidget( lFootMid, 4, 1 );

    eFootMid = new QLineEdit( tab2 );
    grid2->addWidget( eFootMid, 5, 1 );
    eFootMid->setText( hf.footMid );

    lFootRight = new QLabel( i18n( "Right:" ), tab2 );
    grid2->addWidget( lFootRight, 4, 2 );

    eFootRight = new QLineEdit( tab2 );
    grid2->addWidget( eFootRight, 5, 2 );
    eFootRight->setText( hf.footRight );

    lMacros2 = new QLabel( "<qt>You can insert several tags in the text:"
			   "<ul><li>&lt;page&gt;: The current page</li>"
			   "<li>&lt;name&gt;: The filename or URL</li>"
			   "<li>&lt;file&gt;: The filename with complete path or the URL</li>"
			   "<li>&lt;time&gt;: The current time</li>"
			   "<li>&lt;date&gt;: The current date</li>"
			   "<li>&lt;author&gt;: Your full name</li>"
			   "<li>&lt;org&gt;: Your organisation</li>"
			   "<li>&lt;email&gt;: Your email address</li></ul></qt>", tab2 );
    lMacros2->resize( lMacros2->sizeHint() );
    grid2->addMultiCellWidget( lMacros2, 6, 6, 0, 2 );

    addTab( tab2, i18n( "Header and Footer" ) );
}

/*================================================================*/
void KoPageLayoutDia::setupTab3()
{
    tab3 = new QWidget( this );

    grid3 = new QGridLayout( tab3, 5, 2, 15, 7 );

    lColumns = new QLabel( i18n( "Columns:" ), tab3 );
    lColumns->resize( lColumns->sizeHint() );
    grid3->addWidget( lColumns, 0, 0 );

    nColumns = new QSpinBox( 1, 16, 1, tab3 );
    nColumns->resize( nColumns->sizeHint() );
    grid3->addWidget( nColumns, 1, 0 );
    nColumns->setValue( cl.columns );
    connect( nColumns, SIGNAL( valueChanged( int ) ), this, SLOT( nColChanged( int ) ) );

    QString str;
    switch ( layout.unit ) {
    case PG_MM: str = "mm"; break;
    case PG_PT: str = "pt"; break;
    case PG_INCH: str = "inch"; break;
    }

    lCSpacing = new QLabel( i18n( QString( "Columns Spacing ( " + str + " ):" ) ), tab3 );
    lCSpacing->resize( lCSpacing->sizeHint() );
    grid3->addWidget( lCSpacing, 2, 0 );

    nCSpacing = new QLineEdit( tab3, "" );
    nCSpacing->setValidator( new QDoubleValidator( nCSpacing ) );
    nCSpacing->setText( "0.00" );
    nCSpacing->setMaxLength( 5 );
    nCSpacing->setEchoMode( QLineEdit::Normal );
    nCSpacing->setFrame( true );
    nCSpacing->resize( nCSpacing->sizeHint() );
    grid3->addWidget( nCSpacing, 3, 0 );

    switch ( layout.unit ) {
    case PG_MM: nCSpacing->setText( QString::number( cl.mmColumnSpacing ) );
	break;
    case PG_PT: nCSpacing->setText( QString::number( cl.ptColumnSpacing ) );
	break;
    case PG_INCH: nCSpacing->setText( QString::number( cl.inchColumnSpacing ) );
	break;
    }
    connect( nCSpacing, SIGNAL( textChanged( const QString & ) ), this, SLOT( nSpaceChanged( const QString & ) ) );

    // ------------- preview -----------
    pgPreview2 = new KoPagePreview( tab3, "Preview", layout );
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

    //grid3->activate();

    addTab( tab3, i18n( "Columns" ) );
    if ( pgPreview ) pgPreview->setPageColumns( cl );
    pgPreview2->setPageColumns( cl );
}

/*================================================================*/
void KoPageLayoutDia::setupTab4()
{
    QString str;
    switch ( layout.unit ) {
    case PG_MM: str = "mm"; break;
    case PG_PT: str = "pt"; break;
    case PG_INCH: str = "inch"; break;
    }

    tab4 = new QWidget( this );
    grid4 = new QGridLayout( tab4, 3, 1, 15, 7 );

    gHeader = new QButtonGroup( i18n( "Header" ), tab4 );
    gHeader->setExclusive( true );
    headerGrid = new QGridLayout( gHeader, 5, 2, 15, 7 );

    rhSame = new QRadioButton( i18n( "Same header for all pages" ), gHeader );
    rhSame->resize( rhSame->sizeHint() );
    gHeader->insert( rhSame );
    headerGrid->addMultiCellWidget( rhSame, 1, 1, 0, 1 );
    if ( kwhf.header == HF_SAME ) rhSame->setChecked( true );

    rhFirst = new QRadioButton( i18n( "Different header for the first page" ), gHeader );
    rhFirst->resize( rhFirst->sizeHint() );
    gHeader->insert( rhFirst );
    headerGrid->addMultiCellWidget( rhFirst, 2, 2, 0, 1 );
    if ( kwhf.header == HF_FIRST_DIFF ) rhFirst->setChecked( true );

    rhEvenOdd = new QRadioButton( i18n( "Different header for even and odd pages" ), gHeader );
    rhEvenOdd->resize( rhEvenOdd->sizeHint() );
    gHeader->insert( rhEvenOdd );
    headerGrid->addMultiCellWidget( rhEvenOdd, 3, 3, 0, 1 );
    if ( kwhf.header == HF_EO_DIFF ) rhEvenOdd->setChecked( true );

    lHSpacing = new QLabel( i18n( QString( "Spacing between header and body ( " + str + " ):" ) ), gHeader );
    lHSpacing->resize( lHSpacing->sizeHint() );
    lHSpacing->setAlignment( AlignRight | AlignVCenter );
    headerGrid->addWidget( lHSpacing, 4, 0 );

    nHSpacing = new QLineEdit( gHeader, "" );
    nHSpacing->setValidator( new QDoubleValidator( nHSpacing ) );
    nHSpacing->setText( "0.00" );
    nHSpacing->setMaxLength( 5 );
    nHSpacing->setEchoMode( QLineEdit::Normal );
    nHSpacing->setFrame( true );
    nHSpacing->resize( nHSpacing->sizeHint() );
    headerGrid->addWidget( nHSpacing, 4, 1 );

    switch ( layout.unit ) {
    case PG_MM: nHSpacing->setText( QString::number( kwhf.mmHeaderBodySpacing ) );
	break;
    case PG_PT: nHSpacing->setText( QString::number( kwhf.ptHeaderBodySpacing ) );
	break;
    case PG_INCH: nHSpacing->setText( QString::number( kwhf.inchHeaderBodySpacing ) );
	break;
    }

    headerGrid->addColSpacing( 0, rhSame->width() / 2 );
    headerGrid->addColSpacing( 1, rhSame->width() / 2 );
    headerGrid->addColSpacing( 0, rhFirst->width() / 2 );
    headerGrid->addColSpacing( 1, rhFirst->width() / 2 );
    headerGrid->addColSpacing( 0, rhEvenOdd->width() / 2 );
    headerGrid->addColSpacing( 1, rhEvenOdd->width() / 2 );
    headerGrid->addColSpacing( 0, lHSpacing->width() );
    headerGrid->addColSpacing( 1, nHSpacing->width() );
    headerGrid->setColStretch( 1, 1 );

    headerGrid->addRowSpacing( 0, 7 );
    headerGrid->addRowSpacing( 1, rhSame->height() );
    headerGrid->addRowSpacing( 2, rhFirst->height() );
    headerGrid->addRowSpacing( 3, rhEvenOdd->height() );
    headerGrid->addRowSpacing( 4, lHSpacing->height() );
    headerGrid->addRowSpacing( 4, nHSpacing->height() );
    headerGrid->setRowStretch( 0, 0 );
    headerGrid->setRowStretch( 1, 0 );
    headerGrid->setRowStretch( 2, 0 );
    headerGrid->setRowStretch( 3, 0 );
    headerGrid->setRowStretch( 4, 0 );

    //headerGrid->activate();
    grid4->addWidget( gHeader, 0, 0 );

    gFooter = new QButtonGroup( i18n( "Footer" ), tab4 );
    gFooter->setExclusive( true );
    footerGrid = new QGridLayout( gFooter, 5, 2, 15, 7 );

    rfSame = new QRadioButton( i18n( "Same footer for all pages" ), gFooter );
    rfSame->resize( rfSame->sizeHint() );
    gFooter->insert( rfSame );
    footerGrid->addMultiCellWidget( rfSame, 1, 1, 0, 1 );
    if ( kwhf.footer == HF_SAME ) rfSame->setChecked( true );

    rfFirst = new QRadioButton( i18n( "Different footer for the first page" ), gFooter );
    rfFirst->resize( rfFirst->sizeHint() );
    gFooter->insert( rfFirst );
    footerGrid->addMultiCellWidget( rfFirst, 2, 2, 0, 1 );
    if ( kwhf.footer == HF_FIRST_DIFF ) rfFirst->setChecked( true );

    rfEvenOdd = new QRadioButton( i18n( "Different footer for even and odd pages" ), gFooter );
    rfEvenOdd->resize( rfEvenOdd->sizeHint() );
    gFooter->insert( rfEvenOdd );
    footerGrid->addMultiCellWidget( rfEvenOdd, 3, 3, 0, 1 );
    if ( kwhf.footer == HF_EO_DIFF ) rfEvenOdd->setChecked( true );

    lFSpacing = new QLabel( i18n( QString( "Spacing between footer and body ( " + str + " ):" ) ), gFooter );
    lFSpacing->resize( lFSpacing->sizeHint() );
    lFSpacing->setAlignment( AlignRight | AlignVCenter );
    footerGrid->addWidget( lFSpacing, 4, 0 );

    nFSpacing = new QLineEdit( gFooter, "" );
    nFSpacing->setValidator( new QDoubleValidator( nFSpacing ) );
    nFSpacing->setText( "0.00" );
    nFSpacing->setMaxLength( 5 );
    nFSpacing->setEchoMode( QLineEdit::Normal );
    nFSpacing->setFrame( true );
    nFSpacing->resize( nFSpacing->sizeHint() );
    footerGrid->addWidget( nFSpacing, 4, 1 );

    switch ( layout.unit ) {
    case PG_MM: nFSpacing->setText( QString::number( kwhf.mmFooterBodySpacing ) );
	break;
    case PG_PT: nFSpacing->setText( QString::number( kwhf.ptFooterBodySpacing ) );
	break;
    case PG_INCH: nFSpacing->setText( QString::number( kwhf.inchFooterBodySpacing ) );
	break;
    }

    footerGrid->addColSpacing( 0, rfSame->width() / 2 );
    footerGrid->addColSpacing( 1, rfSame->width() / 2 );
    footerGrid->addColSpacing( 0, rfFirst->width() / 2 );
    footerGrid->addColSpacing( 1, rfFirst->width() / 2 );
    footerGrid->addColSpacing( 0, rfEvenOdd->width() / 2 );
    footerGrid->addColSpacing( 1, rfEvenOdd->width() / 2 );
    footerGrid->addColSpacing( 0, lFSpacing->width() );
    footerGrid->addColSpacing( 1, nFSpacing->width() );
    footerGrid->setColStretch( 1, 1 );

    footerGrid->addRowSpacing( 0, 7 );
    footerGrid->addRowSpacing( 1, rfSame->height() );
    footerGrid->addRowSpacing( 2, rfFirst->height() );
    footerGrid->addRowSpacing( 3, rfEvenOdd->height() );
    footerGrid->addRowSpacing( 4, lFSpacing->height() );
    footerGrid->addRowSpacing( 4, nFSpacing->height() );
    footerGrid->setRowStretch( 0, 0 );
    footerGrid->setRowStretch( 1, 0 );
    footerGrid->setRowStretch( 2, 0 );
    footerGrid->setRowStretch( 3, 0 );
    footerGrid->setRowStretch( 4, 0 );

    //footerGrid->activate();
    grid4->addWidget( gFooter, 1, 0 );

    grid4->addColSpacing( 0, gHeader->width() );
    grid4->addColSpacing( 0, gFooter->width() );
    grid4->setColStretch( 0, 1 );

    grid4->addRowSpacing( 0, gHeader->height() );
    grid4->addRowSpacing( 1, gFooter->height() );
    grid4->setRowStretch( 2, 0 );
    grid4->setRowStretch( 2, 1 );

    //grid4->activate();

    addTab( tab4, i18n( "Header and Footer" ) );
}

/*====================== update the preview ======================*/
void KoPageLayoutDia::updatePreview( KoPageLayout )
{
    if ( pgPreview ) pgPreview->setPageLayout( layout );
    if ( pgPreview ) pgPreview->setPageColumns( cl );
    if ( pgPreview2 ) pgPreview2->setPageLayout( layout );
    if ( pgPreview2 ) pgPreview2->setPageColumns( cl );
}

/*===================== unit changed =============================*/
void KoPageLayoutDia::unitChanged( int _unit )
{
//   if ( ( KoUnit )_unit != layout.unit )
//     {
//	 double fact = 1;
//	 if ( layout.unit == PG_CM ) fact = 10;
//	 if ( layout.unit == PG_INCH ) fact = 25.4;

//	 layout.width *= fact;
//	 layout.height *= fact;
//	 layout.left *= fact;
//	 layout.right *= fact;
//	 layout.top *= fact;
//	 layout.bottom *= fact;

//	 layout.ptWidth = MM_TO_POINT( layout.width );
//	 layout.ptHeight = MM_TO_POINT( layout.height );
//	 layout.ptLeft = MM_TO_POINT( layout.left );
//	 layout.ptRight = MM_TO_POINT( layout.right );
//	 layout.ptTop = MM_TO_POINT( layout.top );
//	 layout.ptBottom = MM_TO_POINT( layout.bottom );

//	 fact = 1;
//	 if ( _unit == PG_CM ) fact = 0.1;
//	 if ( _unit == PG_INCH ) fact = 1/25.4;

//	 layout.width *= fact;
//	 layout.height *= fact;
//	 layout.left *= fact;
//	 layout.right *= fact;
//	 layout.top *= fact;
//	 layout.bottom *= fact;

//	 layout.ptWidth = MM_TO_POINT( layout.width );
//	 layout.ptHeight = MM_TO_POINT( layout.height );
//	 layout.ptLeft = MM_TO_POINT( layout.left );
//	 layout.ptRight = MM_TO_POINT( layout.right );
//	 layout.ptTop = MM_TO_POINT( layout.top );
//	 layout.ptBottom = MM_TO_POINT( layout.bottom );

//	 layout.unit = ( KoUnit )_unit;
//	 setValuesTab1();

//	 layout.width = atof( epgWidth->text() );
//	 layout.height = atof( epgHeight->text() );
//	 layout.left = atof( ebrLeft->text() );
//	 layout.right = atof( ebrRight->text() );
//	 layout.top = atof( ebrTop->text() );
//	 layout.bottom = atof( ebrBottom->text() );

//	 layout.ptWidth = MM_TO_POINT( layout.width );
//	 layout.ptHeight = MM_TO_POINT( layout.height );
//	 layout.ptLeft = MM_TO_POINT( layout.left );
//	 layout.ptRight = MM_TO_POINT( layout.right );
//	 layout.ptTop = MM_TO_POINT( layout.top );
//	 layout.ptBottom = MM_TO_POINT( layout.bottom );

//	 updatePreview( layout );
//     }

    layout.unit = static_cast<KoUnit>( _unit );

    // FIXME: put that into a separate private method! (dup.code)
    QString tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;

    switch ( layout.unit ) {
	case PG_MM: {
	    tmp1=QString::number(layout.mmWidth, 'f', 2);
	    tmp2=QString::number(layout.mmHeight, 'f', 2);
	    tmp3=QString::number(layout.mmLeft, 'f', 2);
	    tmp4=QString::number(layout.mmRight, 'f', 2);
	    tmp5=QString::number(layout.mmTop, 'f', 2);
	    tmp6=QString::number(layout.mmBottom, 'f', 2);
	} break;
	case PG_PT: {
	    tmp1=QString::number(layout.ptWidth, 'f', 2);
	    tmp2=QString::number(layout.ptHeight, 'f', 2);
	    tmp3=QString::number(layout.ptLeft, 'f', 2);
	    tmp4=QString::number(layout.ptRight, 'f', 2);
	    tmp5=QString::number(layout.ptTop, 'f', 2);
	    tmp6=QString::number(layout.ptBottom, 'f', 2);
	} break;
	case PG_INCH: {
	    tmp1=QString::number(layout.inchWidth, 'f', 2);
	    tmp2=QString::number(layout.inchHeight, 'f', 2);
	    tmp3=QString::number(layout.inchLeft, 'f', 2);
	    tmp4=QString::number(layout.inchRight, 'f', 2);
	    tmp5=QString::number(layout.inchTop, 'f', 2);
	    tmp6=QString::number(layout.inchBottom, 'f', 2);
	} break;
    }

    epgWidth->setText( tmp1 );
    epgHeight->setText( tmp2 );
    ebrLeft->setText( tmp3 );
    ebrRight->setText( tmp4 );
    ebrTop->setText( tmp5 );
    ebrBottom->setText( tmp6 );

    epgWidth->setText( tmp1 );
    epgHeight->setText( tmp2 );
    ebrLeft->setText( tmp3 );
    ebrRight->setText( tmp4 );
    ebrTop->setText( tmp5 );
    ebrBottom->setText( tmp6 );

    updatePreview( layout );
}

/*===================== format changed =============================*/
void KoPageLayoutDia::formatChanged( int _format )
{
    if ( ( KoFormat )_format != layout.format ) {
	bool enable = true;
	double w = 0, h = 0, dtmp = 0;
	double wi = 0, hi = 0, dtmpi = 0;

	layout.format = ( KoFormat )_format;
	if ( ( KoFormat )_format != PG_CUSTOM ) enable = false;
	epgWidth->setEnabled( enable );
	epgHeight->setEnabled( enable );

	switch ( layout.format ) {
	case PG_DIN_A4: case PG_CUSTOM: {
	    w = PG_A4_WIDTH;
	    h = PG_A4_HEIGHT;
	    wi = PG_A4_WIDTH_I;
	    hi = PG_A4_HEIGHT_I;
	} break;
	case PG_DIN_A3: {
	    w = PG_A3_WIDTH;
	    h = PG_A3_HEIGHT;
	    wi = PG_A3_WIDTH_I;
	    hi = PG_A3_HEIGHT_I;
	} break;
	case PG_DIN_A5: {
	    w = PG_A5_WIDTH;
	    h = PG_A5_HEIGHT;
	    wi = PG_A5_WIDTH_I;
	    hi = PG_A5_HEIGHT_I;
	} break;
	case PG_US_LETTER: {
	    w = PG_US_LETTER_WIDTH;
	    h = PG_US_LETTER_HEIGHT;
	    wi = PG_US_LETTER_WIDTH_I;
	    hi = PG_US_LETTER_HEIGHT_I;
	} break;
	case PG_US_LEGAL: {
	    w = PG_US_LEGAL_WIDTH;
	    h = PG_US_LEGAL_HEIGHT;
	    wi = PG_US_LEGAL_WIDTH_I;
	    hi = PG_US_LEGAL_HEIGHT_I;
	} break;
	case PG_SCREEN: {
	    w = PG_SCREEN_WIDTH;
	    h = PG_SCREEN_HEIGHT;
	    wi = PG_SCREEN_WIDTH_I;
	    hi = PG_SCREEN_HEIGHT_I;
	} break;
	case PG_DIN_B5: {
	    w = PG_B5_WIDTH;
	    h = PG_B5_HEIGHT;
	    wi = PG_B5_WIDTH_I;
	    hi = PG_B5_HEIGHT_I;
	} break;
	case PG_US_EXECUTIVE: {
	    w = PG_US_EXECUTIVE_WIDTH;
	    h = PG_US_EXECUTIVE_HEIGHT;
	    wi = PG_US_EXECUTIVE_WIDTH_I;
	    hi = PG_US_EXECUTIVE_HEIGHT_I;
	} break;
	}
	if ( layout.orientation == PG_LANDSCAPE ) {
	    dtmp = w;
	    w = h;
	    h = dtmp;
	    dtmpi = wi;
	    wi = hi;
	    hi = dtmpi;
	}
	
	layout.mmWidth = w;
	layout.mmHeight = h;
	layout.ptWidth = MM_TO_POINT( w );
	layout.ptHeight = MM_TO_POINT( h );
	layout.inchWidth = wi;
	layout.inchHeight = hi;

	QString tmp1, tmp2;
	switch ( layout.unit ) {
	    case PG_MM: {
		tmp1=QString::number(layout.mmWidth, 'f', 2);
		tmp2=QString::number(layout.mmHeight, 'f', 2);
	    } break;
	    case PG_PT: {
		tmp1=QString::number(layout.ptWidth, 'f', 2);
		tmp2=QString::number(layout.ptHeight, 'f', 2);
	    } break;
	    case PG_INCH: {
		tmp1=QString::number(layout.inchWidth, 'f', 2);
		tmp2=QString::number(layout.inchHeight, 'f', 2);
	    } break;
	}

	epgWidth->setText( tmp1 );
	epgHeight->setText( tmp2 );

	updatePreview( layout );
    }
}

/*===================== format changed =============================*/
void KoPageLayoutDia::orientationChanged( int _orientation )
{
    if ( ( KoOrientation )_orientation != layout.orientation ) {
	double tmp;

	layout.mmWidth = epgWidth->text().toDouble();
	layout.mmHeight = epgHeight->text().toDouble();
	layout.mmLeft = ebrLeft->text().toDouble();
	layout.mmRight = ebrRight->text().toDouble();
	layout.mmTop =  ebrTop->text().toDouble();
	layout.mmBottom = ebrBottom->text().toDouble();

	tmp = layout.mmWidth;
	layout.mmWidth = layout.mmHeight;
	layout.mmHeight = tmp;

	if ( ( KoOrientation )_orientation == PG_LANDSCAPE ) {
	    tmp = layout.mmLeft;
	    layout.mmLeft = layout.mmBottom;
	    layout.mmBottom = layout.mmRight;
	    layout.mmRight = layout.mmTop;
	    layout.mmTop = tmp;
	} else {
	    tmp = layout.mmTop;
	    layout.mmTop = layout.mmRight;
	    layout.mmRight = layout.mmBottom;
	    layout.mmBottom = layout.mmLeft;
	    layout.mmLeft = tmp;
	}

	layout.ptWidth = MM_TO_POINT( layout.mmWidth );
	layout.ptHeight = MM_TO_POINT( layout.mmHeight );
	layout.ptLeft = MM_TO_POINT( layout.mmLeft );
	layout.ptRight = MM_TO_POINT( layout.mmRight );
	layout.ptTop = MM_TO_POINT( layout.mmTop );
	layout.ptBottom = MM_TO_POINT( layout.mmBottom );

	layout.orientation = ( KoOrientation )_orientation;
	setValuesTab1();
	updatePreview( layout );
    }
}

/*===================== width changed =============================*/
void KoPageLayoutDia::widthChanged()
{
    if ( epgWidth->text().length() == 0 && retPressed )
	epgWidth->setText( "0.00" );

    switch ( layout.unit ) {
	case PG_MM: {
	    layout.mmWidth = epgWidth->text().toDouble();
	    layout.ptWidth = MM_TO_POINT( layout.mmWidth );
	    layout.inchWidth = MM_TO_INCH( layout.mmWidth );
	} break;
	case PG_PT: {
	    layout.ptWidth = epgWidth->text().toDouble();
	    layout.mmWidth = POINT_TO_MM( layout.ptWidth );
	    layout.inchWidth = POINT_TO_INCH( layout.ptWidth );
	} break;
	case PG_INCH: {
	    layout.inchWidth = epgWidth->text().toDouble();
	    layout.mmWidth = INCH_TO_MM( layout.inchWidth );
	    layout.ptWidth = INCH_TO_POINT( layout.inchWidth );	
	} break;
    }

    updatePreview( layout );
    retPressed = false;
}

/*===================== height changed ============================*/
void KoPageLayoutDia::heightChanged()
{
    if ( epgHeight->text().length() == 0 && retPressed )
	epgHeight->setText( "0.00" );

    switch ( layout.unit ) {
	case PG_MM: {
	    layout.mmHeight = epgHeight->text().toDouble();
	    layout.ptHeight = MM_TO_POINT( layout.mmHeight );
	    layout.inchHeight = MM_TO_INCH( layout.mmHeight );
	} break;
	case PG_PT: {
	    layout.ptHeight = epgHeight->text().toDouble();
	    layout.mmHeight = POINT_TO_MM( layout.ptHeight );
	    layout.inchHeight = POINT_TO_INCH( layout.ptHeight );
	} break;
	case PG_INCH: {
	    layout.inchHeight = epgHeight->text().toDouble();
	    layout.mmHeight = INCH_TO_MM( layout.inchHeight );
	    layout.ptHeight = INCH_TO_POINT( layout.inchHeight );
	} break;
    }

    updatePreview( layout );
    retPressed = false;
}

/*===================== left border changed =======================*/
void KoPageLayoutDia::leftChanged()
{
    if ( ebrLeft->text().length() == 0 && retPressed )
	ebrLeft->setText( "0.00" );

    switch ( layout.unit ) {
	case PG_MM: {
	    layout.mmLeft = ebrLeft->text().toDouble();
	    layout.ptLeft = MM_TO_POINT( layout.mmLeft );
	    layout.inchLeft = MM_TO_INCH( layout.mmLeft );
	} break;
	case PG_PT: {
	    layout.ptLeft = ebrLeft->text().toDouble();
	    layout.mmLeft = POINT_TO_MM( layout.ptLeft );
	    layout.inchLeft = POINT_TO_INCH( layout.ptLeft );
	} break;
	case PG_INCH: {
	    layout.inchLeft = ebrLeft->text().toDouble();
	    layout.mmLeft = INCH_TO_MM( layout.inchLeft );
	    layout.ptLeft = INCH_TO_POINT( layout.inchLeft );
	} break;
    }

    updatePreview( layout );
    retPressed = false;
}

/*===================== right border changed =======================*/
void KoPageLayoutDia::rightChanged()
{
    if ( ebrRight->text().length() == 0 && retPressed )
	ebrRight->setText( "0.00" );

    switch ( layout.unit ) {
	case PG_MM: {
	    layout.mmRight = ebrRight->text().toDouble();
	    layout.ptRight = MM_TO_POINT( layout.mmRight );
	    layout.inchRight = MM_TO_INCH( layout.mmRight );
	} break;
	case PG_PT: {
	    layout.ptRight = ebrRight->text().toDouble();
	    layout.mmRight = POINT_TO_MM( layout.ptRight );
	    layout.inchRight = POINT_TO_INCH( layout.ptRight );
	} break;
	case PG_INCH: {
	    layout.inchRight = ebrRight->text().toDouble();
	    layout.mmRight = INCH_TO_MM( layout.inchRight );
	    layout.ptRight = INCH_TO_POINT( layout.inchRight );
	} break;
    }

    updatePreview( layout );
    retPressed = false;
}

/*===================== top border changed =========================*/
void KoPageLayoutDia::topChanged()
{
    if ( ebrTop->text().length() == 0 && retPressed )
	ebrTop->setText( "0.00" );

    switch ( layout.unit ) {
	case PG_MM: {
	    layout.mmTop = ebrTop->text().toDouble();
	    layout.ptTop = MM_TO_POINT( layout.mmTop );
	    layout.inchTop = MM_TO_INCH( layout.mmTop );
	} break;
	case PG_PT: {
	    layout.ptTop = ebrTop->text().toDouble();
	    layout.mmTop = POINT_TO_MM( layout.ptTop );
	    layout.inchTop = POINT_TO_INCH( layout.ptTop );
	} break;
	case PG_INCH: {
	    layout.inchTop = ebrTop->text().toDouble();
	    layout.mmTop = INCH_TO_MM( layout.inchTop );
	    layout.ptTop = INCH_TO_POINT( layout.inchTop );
	} break;
    }

    updatePreview( layout );
    retPressed = false;
}

/*===================== bottom border changed ======================*/
void KoPageLayoutDia::bottomChanged()
{
    if ( ebrBottom->text().length() == 0 && retPressed )
	ebrBottom->setText( "0.00" );

    switch ( layout.unit ) {
	case PG_MM: {
	    layout.mmBottom = ebrBottom->text().toDouble();
	    layout.ptBottom = MM_TO_POINT( layout.mmBottom );
	    layout.inchBottom = MM_TO_INCH( layout.mmBottom );
	} break;
	case PG_PT: {
	    layout.ptBottom = ebrBottom->text().toDouble();
	    layout.mmBottom = POINT_TO_MM( layout.ptBottom );
	    layout.inchBottom = POINT_TO_INCH( layout.ptBottom );
	} break;
	case PG_INCH: {
	    layout.inchBottom = ebrBottom->text().toDouble();
	    layout.mmBottom = INCH_TO_MM( layout.inchBottom );
	    layout.ptBottom = INCH_TO_POINT( layout.inchBottom );
	} break;
    }

    updatePreview( layout );
    retPressed = false;
}

/*==================================================================*/
void KoPageLayoutDia::nColChanged( int _val )
{
    cl.columns = _val;
    updatePreview( layout );
}

/*==================================================================*/
void KoPageLayoutDia::nSpaceChanged( const QString &_val )
{
    switch ( layout.unit ) {
	case PG_MM: {
	    cl.mmColumnSpacing = _val.toDouble();
	    cl.ptColumnSpacing = MM_TO_POINT( cl.mmColumnSpacing );
	    cl.inchColumnSpacing = MM_TO_INCH( cl.mmColumnSpacing );
	} break;
	case PG_PT: {
	    cl.ptColumnSpacing = _val.toDouble();
	    cl.mmColumnSpacing = POINT_TO_MM( cl.ptColumnSpacing );
	    cl.inchColumnSpacing = POINT_TO_INCH( cl.ptColumnSpacing );
	} break;
	case PG_INCH: {
	    cl.inchColumnSpacing = _val.toDouble();
	    cl.ptColumnSpacing = INCH_TO_POINT( cl.inchColumnSpacing );
	    cl.mmColumnSpacing = INCH_TO_MM( cl.inchColumnSpacing );
	} break;
    }

    updatePreview( layout );
}

#include <koPageLayoutDia.moc>
