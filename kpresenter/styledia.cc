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

#include <styledia.h>
#include <generalconfig.h>

#include <klocale.h>
#include <kcolordialog.h>
#include <kcolorbutton.h>
#include <kcombobox.h>
#include <knuminput.h>

#include <qlayout.h>
#include <qvbox.h>
#include <qpainter.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qbrush.h>
#include <qslider.h>
#include <qcheckbox.h>

#include <kozoomhandler.h>
#include <kpresenter_utils.h>
#include <kpgradient.h>
#include <qgroupbox.h>
#include "kpresenter_doc.h"
#include <koUnit.h>
#include <knuminput.h>

/******************************************************************/
/* class Pen and Brush preview					  */
/******************************************************************/

/*==============================================================*/
PBPreview::PBPreview( QWidget* parent, const char* name, PaintType _paintType )
    : QFrame( parent, name )
{
    //FIXME zoom
    _zoomHandler=new KoZoomHandler();
    paintType = _paintType;
    pen = QPen( black, 1, SolidLine );
    brush = QBrush( white, SolidPattern );
    gradient = 0;

    setFrameStyle( WinPanel | Sunken );

    switch ( paintType ) {
    case Pen:
	setFixedHeight( 40 );
	break;
    default:
	setMinimumWidth( 230 );
	break;
    }
}

/*==============================================================*/
void PBPreview::resizeEvent( QResizeEvent *e )
{
    QFrame::resizeEvent( e );
    if ( gradient )
    {
	gradient->setSize( contentsRect().size() );
        repaint();
    }
}

/*==============================================================*/
void PBPreview::drawContents( QPainter *painter )
{
    painter->save();
    painter->translate( contentsRect().x(), contentsRect().y() );

    if ( paintType == Pen ) {
	painter->fillRect( 0, 0, contentsRect().width(), contentsRect().height(),
			   colorGroup().base() );
	KoSize diff1( 0, 0 ), diff2( 0, 0 );
        int _w = pen.width();

	if ( lineBegin != L_NORMAL )
	    diff1 = getBoundingSize( lineBegin, _w, _zoomHandler );

	if ( lineEnd != L_NORMAL )
	    diff2 = getBoundingSize( lineEnd, _w, _zoomHandler );

        double unzoom_diff1_width = _zoomHandler->unzoomItX( (int)diff1.width() );
        double unzoom_diff2_width = _zoomHandler->unzoomItX( (int)diff2.width() );

	if ( lineBegin != L_NORMAL )
	    drawFigure( lineBegin, painter, KoPoint( unzoom_diff1_width / 2, _zoomHandler->unzoomItY( contentsRect().height() ) / 2 ),
			pen.color(), _w, 180.0, _zoomHandler );

	if ( lineEnd != L_NORMAL )
	    drawFigure( lineEnd, painter, KoPoint( _zoomHandler->unzoomItX( contentsRect().width() ) - unzoom_diff2_width / 2,
                                                   _zoomHandler->unzoomItY( contentsRect().height() ) / 2 ),
                        pen.color(), _w, 0.0, _zoomHandler );

	painter->setPen( pen );
	painter->drawLine( (int)unzoom_diff1_width / 2,
                           contentsRect().height() / 2,
                           contentsRect().width() - (int)unzoom_diff2_width / 2,
                           contentsRect().height() / 2 );

    } else if ( paintType == Brush ) {
	painter->fillRect( 0, 0, contentsRect().width(), contentsRect().height(),
			   colorGroup().base() );
	painter->fillRect( 0, 0, contentsRect().width(), contentsRect().height(), brush );
    } else if ( paintType == Gradient && gradient ) {
	painter->drawPixmap( 0, 0, gradient->pixmap());
    }

    painter->restore();
}

/******************************************************************/
/* class StyleDia						  */
/******************************************************************/

/*==============================================================*/
StyleDia::StyleDia( QWidget* parent, const char* name, KPresenterDoc *_doc, int flags_, bool _stickyObj, bool _oneObject )
    : QTabDialog( parent, name, true ), flags( flags_ )
{
    m_doc=_doc;
    lockUpdate = true;
    stickyObj = _stickyObj;
    oneObject = _oneObject;
    setupTab1();
    setupTab2();
    if (stickyObj)
        setupTab3();

    setupTab4();

    lockUpdate = false;


    setCancelButton( i18n( "&Close" ) );
    setOkButton( i18n( "&OK" ) );
    setApplyButton( i18n( "&Apply" ) );
    setDefaultButton ( i18n("Reset") );
    slotReset();
    connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( styleDone() ) );
    connect( this, SIGNAL( cancelButtonPressed() ), this, SLOT( reject() ) );
    connect( this, SIGNAL( defaultButtonPressed () ), this, SLOT( slotReset() ) );

}

void StyleDia::slotReset()
{
    setPen( oldPen );
    setBrush( oldBrush );
    setLineBegin( oldLb );
    setLineEnd( oldLe );
    setFillType( oldFillType );
    setGradient( oldC1, oldC1, oldBCType,oldUnbalanced, oldXfactor, oldYfactor );
    setSticky( oldSticky );
    setProtected( oldProtect );
    setKeepRatio( oldKeepRatio );
    setSize( oldRect);
}

/*==============================================================*/
void StyleDia::setupTab1()
{
    QWidget *tab = new QWidget( this );
    QVBoxLayout *layout = new QVBoxLayout( tab );
    QHBoxLayout *config = new QHBoxLayout( layout );

    layout->setMargin( 5 );
    layout->setSpacing( 15 );
    config->setSpacing( 5 );

    QVBox *left = new QVBox( tab );
    config->addWidget( left );
    QVBox *right = new QVBox( tab );
    config->addWidget( right );

    left->setSpacing( 5 );
    right->setSpacing( 5 );

    QLabel *l = new QLabel( i18n( "Pen Color:" ), left );
    l->setFixedHeight( l->sizeHint().height() );

    choosePCol = new KColorButton( Qt::black, left );
    connect( choosePCol, SIGNAL( changed( const QColor& ) ),
	     this, SLOT( updatePenConfiguration() ) );

    l = new QLabel( i18n( "Pen Style:" ), left );
    l->setFixedHeight( l->sizeHint().height() );

    choosePStyle = new KComboBox( false, left, "PStyle" );
    choosePStyle->insertItem( i18n( "Solid Line" ) );
    choosePStyle->insertItem( i18n( "Dash Line ( ---- )" ) );
    choosePStyle->insertItem( i18n( "Dot Line ( **** )" ) );
    choosePStyle->insertItem( i18n( "Dash Dot Line ( -*-* )" ) );
    choosePStyle->insertItem( i18n( "Dash Dot Dot Line ( -**- )" ) );
    choosePStyle->insertItem( i18n( "No Pen" ) );
    connect( choosePStyle, SIGNAL( activated( int ) ),
	     this, SLOT( updatePenConfiguration() ) );

    l = new QLabel( i18n( "Pen Width:" ), left );
    l->setFixedHeight( l->sizeHint().height() );

    choosePWidth = new KIntNumInput( 1, left );
    choosePWidth->setRange( 1, 10, 1 );
    connect( choosePWidth, SIGNAL( valueChanged( int ) ),
	     this, SLOT( updatePenConfiguration() ) );

    l = new QLabel( i18n( "Line Begin:" ), right );
    l->setFixedHeight( l->sizeHint().height() );

    clineBegin = new KComboBox( false, right, "lineBegin" );
    clineBegin->insertItem( i18n("Normal") );
    clineBegin->insertItem( i18n("Arrow") );
    clineBegin->insertItem( i18n("Square") );
    clineBegin->insertItem( i18n("Circle") );
    clineBegin->insertItem( i18n("Line Arrow") );
    clineBegin->insertItem( i18n("Dimension Line") );
    clineBegin->insertItem( i18n("Double Arrow") );

    connect( clineBegin, SIGNAL( activated( int ) ),
	     this, SLOT( updatePenConfiguration() ) );

    l = new QLabel( i18n( "Line End:" ), right );
    l->setFixedHeight( l->sizeHint().height() );

    clineEnd = new KComboBox( false, right, "lineEnd" );
    clineEnd->insertItem( i18n("Normal") );
    clineEnd->insertItem( i18n("Arrow") );
    clineEnd->insertItem( i18n("Square") );
    clineEnd->insertItem( i18n("Circle") );
    clineEnd->insertItem( i18n("Line Arrow") );
    clineEnd->insertItem( i18n("Dimension Line") );
    clineEnd->insertItem( i18n("Double Arrow") );
    connect( clineEnd, SIGNAL( activated( int ) ),
	     this, SLOT( updatePenConfiguration() ) );

    if ( !(flags & SdEndBeginLine) )
    {
        clineEnd->setEnabled( false);
        clineBegin->setEnabled(false);
    }

    //hack for better layout
    l = new QLabel( " ", right );
    l->setFixedHeight( l->sizeHint().height() );
    l = new QLabel( " ", right );
    l->setFixedHeight( clineEnd->sizeHint().height() );

    layout->addWidget( new QWidget( tab ) );

    penPrev = new PBPreview( tab, "penPrev", PBPreview::Pen );
    layout->addWidget( penPrev );

    layout->addWidget( new QWidget( tab ) );

    if ( flags & SdPen )
	addTab( tab, i18n( "&Pen Configuration" ) );
    else
	tab->hide();
}

void StyleDia::setupTab3()
{
    PageConfigGeneral *w = new PageConfigGeneral( this );
    sticky = w->checkboxSticky;
    addTab( w , i18n( "&General" ) );
}

/*==============================================================*/
void StyleDia::setupTab2()
{
    QWidget *tab = new QWidget( this );
    QHBoxLayout *layout = new QHBoxLayout( tab );
    layout->setMargin( 5 );
    layout->setSpacing( 5 );

    QVBox *left = new QVBox( tab );
    layout->addWidget( left );

    left->setSpacing( 5 );

    QLabel *l = new QLabel( i18n( "Fill with:" ), left );
    l->setFixedHeight( l->sizeHint().height() );

    cFillType = new KComboBox( false, left );
    cFillType->insertItem( i18n( "Brush" ) );
    if(flags & SdGradient)
        cFillType->insertItem( i18n( "Gradient" ) );

    connect( cFillType, SIGNAL( activated( int ) ),
	     this, SLOT( updateBrushConfiguration() ) );

    (void)new QWidget( left );

    stack = new QWidgetStack( left );
    connect( cFillType, SIGNAL( activated( int ) ),
	     stack, SLOT( raiseWidget( int ) ) );

    QVBox *brushConfig = new QVBox( stack );
    brushConfig->setSpacing( 5 );
    stack->addWidget( brushConfig, 0 );

    l = new QLabel( i18n( "Brush Color:" ), brushConfig );
    l->setFixedHeight( l->sizeHint().height() );

    chooseBCol = new KColorButton( Qt::white, brushConfig );
    connect( chooseBCol, SIGNAL( changed( const QColor & ) ),
	     this, SLOT( updateBrushConfiguration() ) );

    l = new QLabel( i18n( "Brush Style:" ), brushConfig );
    l->setFixedHeight( l->sizeHint().height() );

    chooseBStyle = new KComboBox( false, brushConfig, "BStyle" );
    // xgettext:no-c-format
    chooseBStyle->insertItem( i18n( "100% Fill Pattern" ) );
    // xgettext:no-c-format
    chooseBStyle->insertItem( i18n( "94% Fill Pattern" ) );
    // xgettext:no-c-format
    chooseBStyle->insertItem( i18n( "88% Fill Pattern" ) );
    // xgettext:no-c-format
    chooseBStyle->insertItem( i18n( "63% Fill Pattern" ) );
    // xgettext:no-c-format
    chooseBStyle->insertItem( i18n( "50% Fill Pattern" ) );
    // xgettext:no-c-format
    chooseBStyle->insertItem( i18n( "37% Fill Pattern" ) );
    // xgettext:no-c-format
    chooseBStyle->insertItem( i18n( "12% Fill Pattern" ) );
    // xgettext:no-c-format
    chooseBStyle->insertItem( i18n( "6% Fill Pattern" ) );
    chooseBStyle->insertItem( i18n( "Horizontal Lines" ) );
    chooseBStyle->insertItem( i18n( "Vertical Lines" ) );
    chooseBStyle->insertItem( i18n( "Crossing Lines" ) );
    chooseBStyle->insertItem( i18n( "Diagonal Lines ( / )" ) );
    chooseBStyle->insertItem( i18n( "Diagonal Lines ( \\ )" ) );
    chooseBStyle->insertItem( i18n( "Diagonal Crossing Lines" ) );
    chooseBStyle->insertItem( i18n( "No Brush" ) );
    connect( chooseBStyle, SIGNAL( activated( int ) ),
	     this, SLOT( updateBrushConfiguration() ) );

    (void)new QWidget( brushConfig );

    QVBox *gradientConfig = new QVBox( left );
    stack->addWidget( gradientConfig, 1 );
    gradientConfig->setSpacing( 5 );

    l = new QLabel( i18n( "Gradient Colors:" ), gradientConfig );
    l->setFixedHeight( l->sizeHint().height() );

    gradient1 = new KColorButton( red, gradientConfig );
    connect( gradient1, SIGNAL( changed( const QColor & ) ),
	     this, SLOT( updateBrushConfiguration() ) );
    gradient2 = new KColorButton( green, gradientConfig );
    connect( gradient2, SIGNAL( changed( const QColor & ) ),
	     this, SLOT( updateBrushConfiguration() ) );

    l = new QLabel( i18n( "Gradient Style:" ), gradientConfig );
    l->setFixedHeight( l->sizeHint().height() );

    gradients = new KComboBox( false, gradientConfig );
    gradients->insertItem( i18n( "Horizontal Gradient" ), -1 );
    gradients->insertItem( i18n( "Vertical Gradient" ), -1 );
    gradients->insertItem( i18n( "Diagonal Gradient 1" ), -1 );
    gradients->insertItem( i18n( "Diagonal Gradient 2" ), -1 );
    gradients->insertItem( i18n( "Circle Gradient" ), -1 );
    gradients->insertItem( i18n( "Rectangle Gradient" ), -1 );
    gradients->insertItem( i18n( "PipeCross Gradient" ), -1 );
    gradients->insertItem( i18n( "Pyramid Gradient" ), -1 );
    connect( gradients, SIGNAL( activated( int ) ),
	     this, SLOT( updateBrushConfiguration() ) );

    unbalanced = new QCheckBox( i18n( "Unbalanced" ), gradientConfig );
    connect( unbalanced, SIGNAL( clicked() ),
	     this, SLOT( updateBrushConfiguration() ) );

    xfactorLabel = new QLabel( i18n( "X-Factor:" ), gradientConfig );
    xfactorLabel->setFixedHeight( xfactorLabel->sizeHint().height() );

    xfactor = new QSlider( -200, 200, 1, 100, QSlider::Horizontal, gradientConfig );
    connect( xfactor, SIGNAL( valueChanged( int ) ),
	     this, SLOT( updateBrushConfiguration() ) );

    yfactorLabel = new QLabel( i18n( "Y-Factor:" ), gradientConfig );
    yfactorLabel->setFixedHeight( yfactorLabel->sizeHint().height() );

    yfactor = new QSlider( -200, 200, 1, 100, QSlider::Horizontal, gradientConfig );
    connect( yfactor, SIGNAL( valueChanged( int ) ),
	     this, SLOT( updateBrushConfiguration() ) );

    gradient = new KPGradient( Qt::red, Qt::green, BCT_GHORZ, false, 100, 100 );

    (void)new QWidget( gradientConfig );
    (void)new QWidget( left );

    brushPrev = new PBPreview( tab, "", PBPreview::Brush );
    brushPrev->setGradient( gradient );

    layout->addWidget( brushPrev );

    stack->raiseWidget( 0 );

    if ( flags & SdBrush )
	addTab( tab, i18n( "&Brush Configuration" ) );
    else
	tab->hide();
}

void StyleDia::setupTab4()
{
    QWidget *tab = new QWidget( this );
    QVBoxLayout *layout = new QVBoxLayout( tab );
    layout->setMargin( 5 );
    layout->setSpacing( 5 );

    protect= new QCheckBox( i18n("Protect size and position"), tab);
    layout->addWidget(protect);
    connect( protect, SIGNAL(clicked ()), this, SLOT(protectChanged()));

    keepRatio= new QCheckBox( i18n("Keep ratio"), tab);
    layout->addWidget(keepRatio);

    QGroupBox *grp1 = new QGroupBox( i18n("Position in %1").arg(m_doc->getUnitName()), tab );
    layout->addWidget( grp1 );
    QGridLayout *pGrid = new QGridLayout( grp1, 8, 2, KDialog::marginHint(), KDialog::spacingHint() );

    QLabel * lab = new QLabel( i18n( "Left:" ), grp1 );
    lab->resize( lab->sizeHint() );
    pGrid->addWidget( lab, 1, 0 );

    m_lineLeft= new KDoubleNumInput( grp1 );

    pGrid->addWidget( m_lineLeft, 2, 0 );
    m_lineLeft->setRange ( 0, 9999, 1, false);

    lab = new QLabel( i18n( "Width:" ), grp1 );
    lab->resize( lab->sizeHint() );
    pGrid->addWidget( lab, 3, 0 );

    m_lineWidth= new KDoubleNumInput( grp1 );
    m_lineWidth->setRange ( 0, 9999, 1, false);

    pGrid->addWidget( m_lineWidth, 4, 0 );

    lab = new QLabel( i18n( "Top:" ), grp1 );
    lab->resize( lab->sizeHint() );
    pGrid->addWidget( lab, 1, 1 );

    m_lineTop= new KDoubleNumInput( grp1 );
    m_lineTop->setRange ( 0, 9999, 1, false);

    pGrid->addWidget( m_lineTop, 2, 1 );

    lab = new QLabel( i18n( "Height:" ), grp1 );
    lab->resize( lab->sizeHint() );
    pGrid->addWidget( lab, 3, 1 );

    m_lineHeight= new KDoubleNumInput( grp1 );
    m_lineHeight->setRange ( 0, 9999, 1, false);
    pGrid->addWidget( m_lineHeight, 4, 1 );


    addTab( tab, i18n( "&Geometry" ) );
}

void StyleDia::protectChanged()
{
    if ( lockUpdate )
	return;
    bool state=!(protect->isChecked())&&oneObject;
    m_lineTop->setEnabled( state);
    m_lineLeft->setEnabled( state);
    m_lineWidth->setEnabled( state);
    m_lineHeight->setEnabled( state);

}

/*==============================================================*/
StyleDia::~StyleDia()
{
    delete gradient;
    delete penPrev;
    delete brushPrev;
}

/*==============================================================*/
void StyleDia::updatePenConfiguration()
{
    if ( lockUpdate )
	return;

    QPen pen = getPen();
    penPrev->setPen( pen );
    penPrev->setLineBegin( getLineBegin() );
    penPrev->setLineEnd( getLineEnd() );
}

/*==============================================================*/
void StyleDia::updateBrushConfiguration()
{
    if ( lockUpdate )
	return;

    bool state =unbalanced->isChecked();
    xfactorLabel->setEnabled(state);
    yfactorLabel->setEnabled(state);
    xfactor->setEnabled( state );
    yfactor->setEnabled( state );

    if ( getFillType() == FT_BRUSH ) {
	brushPrev->setPaintType( PBPreview::Brush );
	brushPrev->setBrush( getBrush() );
	brushPrev->repaint( true );
    } else {
	brushPrev->setPaintType( PBPreview::Gradient );
	gradient->setColor1( getGColor1() );
	gradient->setColor2( getGColor2() );
	gradient->setBackColorType( getGType() );
	gradient->setUnbalanced( getGUnbalanced() );
	gradient->setXFactor( getGXFactor() );
	gradient->setYFactor( getGYFactor() );
	brushPrev->repaint( false );
    }
}

/*==============================================================*/
void StyleDia::setPen( const QPen &_pen )
{
    if ( lockUpdate )
	return;
    oldPen=_pen;
    switch ( _pen.style() ) {
    case NoPen: choosePStyle->setCurrentItem( 5 );
	break;
    case SolidLine: choosePStyle->setCurrentItem( 0 );
	break;
    case DashLine: choosePStyle->setCurrentItem( 1 );
	break;
    case DotLine: choosePStyle->setCurrentItem( 2 );
	break;
    case DashDotLine: choosePStyle->setCurrentItem( 3 );
	break;
    case DashDotDotLine: choosePStyle->setCurrentItem( 4 );
	break;
    }
    choosePWidth->setValue( _pen.width() );
    choosePCol->setColor( _pen.color() );
    updatePenConfiguration();
}

/*==============================================================*/
void StyleDia::setBrush( const QBrush &_brush )
{
    if ( lockUpdate )
	return;
    oldBrush =_brush;
    switch ( _brush.style() ) {
    case SolidPattern: chooseBStyle->setCurrentItem( 0 );
	break;
    case Dense1Pattern: chooseBStyle->setCurrentItem( 1 );
	break;
    case Dense2Pattern: chooseBStyle->setCurrentItem( 2 );
	break;
    case Dense3Pattern: chooseBStyle->setCurrentItem( 3 );
	break;
    case Dense4Pattern: chooseBStyle->setCurrentItem( 4 );
	break;
    case Dense5Pattern: chooseBStyle->setCurrentItem( 5 );
	break;
    case Dense6Pattern: chooseBStyle->setCurrentItem( 6 );
	break;
    case Dense7Pattern: chooseBStyle->setCurrentItem( 7 );
	break;
    case HorPattern: chooseBStyle->setCurrentItem( 8 );
	break;
    case VerPattern: chooseBStyle->setCurrentItem( 9 );
	break;
    case CrossPattern: chooseBStyle->setCurrentItem( 10 );
	break;
    case BDiagPattern: chooseBStyle->setCurrentItem( 11 );
	break;
    case FDiagPattern: chooseBStyle->setCurrentItem( 12 );
	break;
    case DiagCrossPattern: chooseBStyle->setCurrentItem( 13 );
	break;
    case NoBrush: chooseBStyle->setCurrentItem( 14 );
	break;
    case CustomPattern:
	break;
    }
    chooseBCol->setColor( _brush.color() );
    updateBrushConfiguration();
}

/*==============================================================*/
void StyleDia::setLineBegin( LineEnd lb )
{
    if ( lockUpdate )
	return;
    oldLb = lb;
    clineBegin->setCurrentItem( (int)lb );
    updatePenConfiguration();
}

/*==============================================================*/
void StyleDia::setLineEnd( LineEnd le )
{
    if ( lockUpdate )
	return;
    oldLe=le;
    clineEnd->setCurrentItem( (int)le );
    updatePenConfiguration();
}

/*==============================================================*/
void StyleDia::setFillType( FillType ft )
{
    if ( lockUpdate )
	return;

    cFillType->setCurrentItem( (int)ft );
    stack->raiseWidget( (int)ft );
    updateBrushConfiguration();
}

/*==============================================================*/
void StyleDia::setGradient( const QColor &_c1, const QColor &_c2, BCType _t,
			    bool _unbalanced, int _xfactor, int _yfactor )
{
    if ( lockUpdate )
	return;
    oldC1=_c1;
    oldC2=_c2;
    oldBCType=_t;
    oldUnbalanced=_unbalanced;
    oldXfactor=_xfactor;
    oldYfactor=_yfactor;
    gradient1->setColor( _c1 );
    gradient2->setColor( _c2 );
    gradients->setCurrentItem( (int)_t - 1 );
    unbalanced->setChecked( _unbalanced );
    xfactor->setValue( _xfactor );
    yfactor->setValue( _yfactor );
    updateBrushConfiguration();
}

/*==============================================================*/
QPen StyleDia::getPen() const
{
    QPen pen;

    switch ( choosePStyle->currentItem() ) {
    case 5: pen.setStyle( NoPen );
	break;
    case 0: pen.setStyle( SolidLine );
	break;
    case 1: pen.setStyle( DashLine );
	break;
    case 2: pen.setStyle( DotLine );
	break;
    case 3: pen.setStyle( DashDotLine );
	break;
    case 4: pen.setStyle( DashDotDotLine );
	break;
    }

    pen.setColor( choosePCol->color() );
    pen.setWidth( choosePWidth->value() );

    return pen;
}

/*==============================================================*/
QBrush StyleDia::getBrush() const
{
    QBrush brush;

    switch ( chooseBStyle->currentItem() ) {
    case 0: brush.setStyle( SolidPattern );
	break;
    case 1: brush.setStyle( Dense1Pattern );
	break;
    case 2: brush.setStyle( Dense2Pattern );
	break;
    case 3: brush.setStyle( Dense3Pattern );
	break;
    case 4: brush.setStyle( Dense4Pattern );
	break;
    case 5: brush.setStyle( Dense5Pattern );
	break;
    case 6: brush.setStyle( Dense6Pattern );
	break;
    case 7: brush.setStyle( Dense7Pattern );
	break;
    case 8: brush.setStyle( HorPattern );
	break;
    case 9: brush.setStyle( VerPattern );
	break;
    case 10: brush.setStyle( CrossPattern );
	break;
    case 11: brush.setStyle( BDiagPattern );
	break;
    case 12: brush.setStyle( FDiagPattern );
	break;
    case 13: brush.setStyle( DiagCrossPattern );
	break;
    case 14: brush.setStyle( NoBrush );
	break;
    }

    brush.setColor( chooseBCol->color() );

    return brush;
}

/*==============================================================*/
LineEnd StyleDia::getLineBegin() const
{
    return (LineEnd)clineBegin->currentItem();
}

/*==============================================================*/
LineEnd StyleDia::getLineEnd() const
{
    return (LineEnd)clineEnd->currentItem();
}

/*==============================================================*/
FillType StyleDia::getFillType() const
{
    return (FillType)cFillType->currentItem();
}

/*==============================================================*/
QColor StyleDia::getGColor1() const
{
    return gradient1->color();
}

/*==============================================================*/
QColor StyleDia::getGColor2() const
{
    return gradient2->color();
}

/*==============================================================*/
BCType StyleDia::getGType() const
{
    return (BCType)( gradients->currentItem() + 1 );
}

/*==============================================================*/
bool StyleDia::getGUnbalanced() const
{
    return unbalanced->isChecked();
}

/*==============================================================*/
int StyleDia::getGXFactor() const
{
    return xfactor->value();
}

/*==============================================================*/
int StyleDia::getGYFactor() const
{
    return yfactor->value();
}

void StyleDia::setSticky( bool s )
{
    oldSticky=s;
    if( stickyObj)
        sticky->setChecked( s );
}

bool StyleDia::isSticky() const
{
    if( stickyObj )
        return sticky->isChecked();
    return false;
}

void StyleDia::setProtected( bool p )
{
    oldProtect=p;
    protect->setChecked( p );
    protectChanged();
}

bool StyleDia::isProtected() const
{
    return protect->isChecked();
}

void StyleDia::setKeepRatio( bool p )
{
    oldKeepRatio = p;
    keepRatio->setChecked( p );
}

bool StyleDia::isKeepRatio()const
{
    return keepRatio->isChecked();
}

KoRect StyleDia::getNewSize() const
{
    double top=QMAX(0, KoUnit::ptFromUnit( m_lineTop->value(), m_doc->getUnit() ));
    double left=QMAX(0, KoUnit::ptFromUnit( m_lineLeft->value(), m_doc->getUnit() ));
    double width=QMAX(0, KoUnit::ptFromUnit( m_lineWidth->value(), m_doc->getUnit() ));
    double height=QMAX(0, KoUnit::ptFromUnit( m_lineHeight->value(), m_doc->getUnit() ));

    KoRect newRect = KoRect( left, top, width, height);
    return newRect;
}

void StyleDia::setSize(const KoRect & _rect)
{
    oldRect = _rect;
    m_lineTop->setValue(KoUnit::ptToUnit( QMAX(0.00, _rect.top()), m_doc->getUnit() ));
    m_lineLeft->setValue(KoUnit::ptToUnit( QMAX(0.00, _rect.left()), m_doc->getUnit() ));
    m_lineWidth->setValue(KoUnit::ptToUnit( QMAX(0.00, _rect.width()), m_doc->getUnit() ));
    m_lineHeight->setValue(KoUnit::ptToUnit( QMAX(0.00, _rect.height()), m_doc->getUnit() ));
}


#include <styledia.moc>
