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

#include "styledia.h"
#include "styledia.moc"
#include "generalconfig.h"

#include <kapp.h>
#include <klocale.h>
#include <kcolordlg.h>
#include <kcolorbtn.h>

#include <qlayout.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qpainter.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qpen.h>
#include <qbrush.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qevent.h>

#include "kpresenter_utils.h"
#include "kpgradient.h"

/******************************************************************/
/* class Pen and Brush preview					  */
/******************************************************************/

/*==============================================================*/
PBPreview::PBPreview( QWidget* parent, const char* name, PaintType _paintType )
    : QFrame( parent, name )
{
    paintType = _paintType;
    pen = QPen( black, 1, SolidLine );
    brush = QBrush( white, SolidPattern );
    gradient = 0;

    setFrameStyle( WinPanel | Sunken );

    switch ( paintType ) {
    case Pen:
	setFixedHeight( 30 );
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
	gradient->setSize( QSize( contentsRect().width(),
				  contentsRect().height() ) );
}

/*==============================================================*/
void PBPreview::drawContents( QPainter *painter )
{
    painter->save();
    painter->translate( contentsRect().x(), contentsRect().y() );

    if ( paintType == Pen ) {
	painter->fillRect( 0, 0, contentsRect().width(), contentsRect().height(),
			   colorGroup().base() );
	QSize diff1( 0, 0 ), diff2( 0, 0 );
	int _w = pen.width();

	if ( lineBegin != L_NORMAL )
	    diff1 = getBoundingSize( lineBegin, _w );

	if ( lineEnd != L_NORMAL )
	    diff2 = getBoundingSize( lineEnd, _w );

	if ( lineBegin != L_NORMAL )
	    drawFigure( lineBegin, painter, QPoint( diff1.width() / 2, contentsRect().height() / 2 ),
			pen.color(), _w, 180.0 );

	if ( lineEnd != L_NORMAL )
	    drawFigure( lineEnd, painter, QPoint( contentsRect().width() - diff2.width() / 2,
						  contentsRect().height() / 2 ), pen.color(), _w, 0.0 );

	painter->setPen( pen );
	painter->drawLine( diff1.width() / 2, contentsRect().height() / 2,
			  contentsRect().width() - diff2.width() / 2, contentsRect().height()/2 );
    } else if ( paintType == Brush ) {
	painter->fillRect( 0, 0, contentsRect().width(), contentsRect().height(),
			   colorGroup().base() );
	painter->fillRect( 0, 0, contentsRect().width(), contentsRect().height(), brush );
    } else if ( paintType == Gradient && gradient ) {
	painter->drawPixmap( 0, 0, *gradient->getGradient() );
    }

    painter->restore();
}

/******************************************************************/
/* class StyleDia						  */
/******************************************************************/

/*==============================================================*/
StyleDia::StyleDia( QWidget* parent, const char* name, int flags_ )
    : QTabDialog( parent, name, true ), flags( flags_ )
{
    lockUpdate = true;
    setupTab1();
    setupTab2();
    setupTab3();
    lockUpdate = false;

    if ( flags & SdPen )
	updatePenConfiguration();
    if ( flags & SdBrush )
	updateBrushConfiguration();

    setCancelButton( i18n( "&Close" ) );
    setOkButton( i18n( "&OK" ) );
    setApplyButton( i18n( "&Apply" ) );

    connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( styleDone() ) );
    connect( this, SIGNAL( cancelButtonPressed() ), this, SLOT( reject() ) );
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

    choosePStyle = new QComboBox( false, left, "PStyle" );
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

    choosePWidth = new QSpinBox( 1, 10, 1, left );
    connect( choosePWidth, SIGNAL( valueChanged( int ) ),
	     this, SLOT( updatePenConfiguration() ) );

    l = new QLabel( i18n( "Line Begin:" ), right );
    l->setFixedHeight( l->sizeHint().height() );

    clineBegin = new QComboBox( false, right, "lineBegin" );
    clineBegin->insertItem( i18n("Normal") );
    clineBegin->insertItem( i18n("Arrow") );
    clineBegin->insertItem( i18n("Square") );
    clineBegin->insertItem( i18n("Circle") );
    connect( clineBegin, SIGNAL( activated( int ) ),
	     this, SLOT( updatePenConfiguration() ) );

    l = new QLabel( i18n( "Line End:" ), right );
    l->setFixedHeight( l->sizeHint().height() );

    clineEnd = new QComboBox( false, right, "lineEnd" );
    clineEnd->insertItem( i18n("Normal") );
    clineEnd->insertItem( i18n("Arrow") );
    clineEnd->insertItem( i18n("Square") );
    clineEnd->insertItem( i18n("Circle") );
    connect( clineEnd, SIGNAL( activated( int ) ),
	     this, SLOT( updatePenConfiguration() ) );

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

    cFillType = new QComboBox( false, left );
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

    chooseBStyle = new QComboBox( false, brushConfig, "BStyle" );
    chooseBStyle->insertItem( i18n( "100% fill Pattern" ) );
    chooseBStyle->insertItem( i18n( "94% fill Pattern" ) );
    chooseBStyle->insertItem( i18n( "88% fill Pattern" ) );
    chooseBStyle->insertItem( i18n( "63% fill Pattern" ) );
    chooseBStyle->insertItem( i18n( "50% fill Pattern" ) );
    chooseBStyle->insertItem( i18n( "37% fill Pattern" ) );
    chooseBStyle->insertItem( i18n( "12% fill Pattern" ) );
    chooseBStyle->insertItem( i18n( "6% fill Pattern" ) );
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

    gradients = new QComboBox( false, gradientConfig );
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

    l = new QLabel( i18n( "X-Factor:" ), gradientConfig );
    l->setFixedHeight( l->sizeHint().height() );

    xfactor = new QSlider( -200, 200, 1, 100, QSlider::Horizontal, gradientConfig );
    connect( xfactor, SIGNAL( valueChanged( int ) ),
	     this, SLOT( updateBrushConfiguration() ) );

    l = new QLabel( i18n( "Y-Factor:" ), gradientConfig );
    l->setFixedHeight( l->sizeHint().height() );

    yfactor = new QSlider( -200, 200, 1, 100, QSlider::Horizontal, gradientConfig );
    connect( yfactor, SIGNAL( valueChanged( int ) ),
	     this, SLOT( updateBrushConfiguration() ) );

    gradient = new KPGradient( Qt::red, Qt::green, BCT_GHORZ, QSize( chooseBCol->width(), 25 ),
			       false, 100, 100 );

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

/*==============================================================*/
StyleDia::~StyleDia()
{
    delete gradient;
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

    if ( !unbalanced->isChecked() ) {
	xfactor->setEnabled( false );
	yfactor->setEnabled( false );
    } else {
	xfactor->setEnabled( true );
	yfactor->setEnabled( true );
    }

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

    clineBegin->setCurrentItem( (int)lb );
    updatePenConfiguration();
}

/*==============================================================*/
void StyleDia::setLineEnd( LineEnd le )
{
    if ( lockUpdate )
	return;

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

    gradient1->setColor( _c1 );
    gradient2->setColor( _c2 );
    gradients->setCurrentItem( (int)_t - 1 );
    unbalanced->setChecked( _unbalanced );
    xfactor->setValue( _xfactor );
    yfactor->setValue( _yfactor );
    updateBrushConfiguration();
}

/*==============================================================*/
QPen StyleDia::getPen()
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
QBrush StyleDia::getBrush()
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
LineEnd StyleDia::getLineBegin()
{
    return (LineEnd)clineBegin->currentItem();
}

/*==============================================================*/
LineEnd StyleDia::getLineEnd()
{
    return (LineEnd)clineEnd->currentItem();
}

/*==============================================================*/
FillType StyleDia::getFillType()
{
    return (FillType)cFillType->currentItem();
}

/*==============================================================*/
QColor StyleDia::getGColor1()
{
    return gradient1->color();
}

/*==============================================================*/
QColor StyleDia::getGColor2()
{
    return gradient2->color();
}

/*==============================================================*/
BCType StyleDia::getGType()
{
    return (BCType)( gradients->currentItem() + 1 );
}

/*==============================================================*/
bool StyleDia::getGUnbalanced()
{
    return unbalanced->isChecked();
}

/*==============================================================*/
int StyleDia::getGXFactor()
{
    return xfactor->value();
}

/*==============================================================*/
int StyleDia::getGYFactor()
{
    return yfactor->value();
}

void StyleDia::setSticky( bool s )
{
    sticky->setChecked( s );
}


bool StyleDia::isSticky()
{
    return sticky->isChecked();
}
