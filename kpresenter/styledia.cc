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
#include <confpiedia.h>
#include <confrectdia.h>
#include <confpolygondia.h>
#include <confpicturedia.h>

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
#include <kpresenter_doc.h>
#include <kpresenter_view.h>
#include <koUnit.h>
#include <knuminput.h>
#include <kprcanvas.h>

/******************************************************************/
/* class Pen and Brush preview					  */
/******************************************************************/

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

void PBPreview::resizeEvent( QResizeEvent *e )
{
    QFrame::resizeEvent( e );
    if ( gradient )
    {
	gradient->setSize( contentsRect().size() );
        repaint();
    }
}

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
/* class ConfPenDia                                               */
/******************************************************************/

ConfPenDia::ConfPenDia( QWidget* parent, const char* name, int flags)
    : QWidget( parent, name ), m_flags(flags)
{
    QVBoxLayout *layout = new QVBoxLayout( this );
    QHBoxLayout *config = new QHBoxLayout( layout );

    layout->setMargin( 5 );
    layout->setSpacing( 15 );
    config->setSpacing( 5 );

    QVBox *left = new QVBox( this );
    config->addWidget( left );
    QVBox *right = new QVBox( this );
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
    clineBegin->insertItem( i18n("Double Line Arrow") );

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
    clineEnd->insertItem( i18n("Double Line Arrow") );
    connect( clineEnd, SIGNAL( activated( int ) ),
	     this, SLOT( updatePenConfiguration() ) );

    if ( !(m_flags & StyleDia::SdEndBeginLine) )
    {
        clineEnd->setEnabled( false);
        clineBegin->setEnabled(false);
    }

    //hack for better layout
    l = new QLabel( " ", right );
    l->setFixedHeight( l->sizeHint().height() );
    l = new QLabel( " ", right );
    l->setFixedHeight( clineEnd->sizeHint().height() );

    layout->addWidget( new QWidget( this ) );

    penPrev = new PBPreview( this, "penPrev", PBPreview::Pen );
    layout->addWidget( penPrev );

    layout->addWidget( new QWidget( this ) );
}

ConfPenDia::~ConfPenDia()
{
    delete penPrev;
}

void ConfPenDia::setPen( const QPen &_pen )
{
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

void ConfPenDia::setLineBegin( LineEnd lb )
{
    oldLb = lb;
    clineBegin->setCurrentItem( (int)lb );
    updatePenConfiguration();
}

void ConfPenDia::setLineEnd( LineEnd le )
{
    oldLe=le;
    clineEnd->setCurrentItem( (int)le );
    updatePenConfiguration();
}

QPen ConfPenDia::getPen() const
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

LineEnd ConfPenDia::getLineBegin() const
{
    return (LineEnd)clineBegin->currentItem();
}

LineEnd ConfPenDia::getLineEnd() const
{
    return (LineEnd)clineEnd->currentItem();
}

void ConfPenDia::slotReset()
{
    setPen( oldPen );
    setLineBegin( oldLb );
    setLineEnd( oldLe );
}

void ConfPenDia::updatePenConfiguration()
{
    QPen pen = getPen();
    penPrev->setPen( pen );
    penPrev->setLineBegin( getLineBegin() );
    penPrev->setLineEnd( getLineEnd() );
}

/******************************************************************/
/* class ConfBrushDia                                             */
/******************************************************************/

ConfBrushDia::ConfBrushDia( QWidget* parent, const char* name, int flags)
    : QWidget( parent, name ), m_flags(flags)
{
    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->setMargin( 5 );
    layout->setSpacing( 5 );

    QVBox *left = new QVBox( this );
    layout->addWidget( left );

    left->setSpacing( 5 );

    QLabel *l = new QLabel( i18n( "Fill with:" ), left );
    l->setFixedHeight( l->sizeHint().height() );

    cFillType = new KComboBox( false, left );
    cFillType->insertItem( i18n( "Brush" ) );
    if(m_flags & StyleDia::SdGradient)
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

    brushPrev = new PBPreview( this, "", PBPreview::Brush );
    brushPrev->setGradient( gradient );

    layout->addWidget( brushPrev );

    stack->raiseWidget( 0 );
}

ConfBrushDia::~ConfBrushDia()
{
    delete gradient;
    delete brushPrev;
}

void ConfBrushDia::setBrush( const QBrush &_brush )
{
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

void ConfBrushDia::setFillType( FillType ft )
{
    cFillType->setCurrentItem( (int)ft );
    stack->raiseWidget( (int)ft );
    updateBrushConfiguration();
}

void ConfBrushDia::setGradient( const QColor &_c1, const QColor &_c2, BCType _t,
			    bool _unbalanced, int _xfactor, int _yfactor )
{
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

QBrush ConfBrushDia::getBrush() const
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

FillType ConfBrushDia::getFillType() const
{
    return (FillType)cFillType->currentItem();
}

QColor ConfBrushDia::getGColor1() const
{
    return gradient1->color();
}

QColor ConfBrushDia::getGColor2() const
{
    return gradient2->color();
}

BCType ConfBrushDia::getGType() const
{
    return (BCType)( gradients->currentItem() + 1 );
}

bool ConfBrushDia::getGUnbalanced() const
{
    return unbalanced->isChecked();
}

int ConfBrushDia::getGXFactor() const
{
    return xfactor->value();
}

int ConfBrushDia::getGYFactor() const
{
    return yfactor->value();
}

void ConfBrushDia::slotReset()
{
    setBrush( oldBrush );
    setFillType( oldFillType );
    setGradient( oldC1, oldC1, oldBCType,oldUnbalanced, oldXfactor, oldYfactor );
}

void ConfBrushDia::updateBrushConfiguration()
{
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

/******************************************************************/
/* class StyleDia						  */
/******************************************************************/

StyleDia::StyleDia( QWidget* parent, const char* name, KPresenterDoc *_doc, bool _stickyObj, bool _oneObject, bool _alltextobj )
    : QTabDialog( parent, name, true ),
      m_doc(_doc),
      m_confPenDia(0),
      m_confPieDia(0),
      m_confRectDia(0),
      m_confBrushDia(0),
      m_confPolygonDia(0),
      m_confPictureDia(0),
      stickyObj(_stickyObj),
      oneObject(_oneObject),
      allTextObj(_alltextobj)
{
    lockUpdate = true;
    m_canvas = m_doc->getKPresenterView()->getCanvas();
    flags = m_canvas->getPenBrushFlags();
    oldProtect = false;
    oldkeepRatiotripleState=false;
    oldProtectTripleState = false;
    oldKeepRatio=false;
    oldRect=KoRect();

    // allways create a pen- & brush-dialog or rewrite KPrPage::setPenBrush :-)
    setupTabPen();
    setupTabBrush();

    if (stickyObj)
        setupTabGeneral();

    setupTabGeometry();

    if (flags & SdPie && !(flags & (SdPolygon | SdPicture | SdRectangle | SdOther)))
        setupTabPie();

    if (flags & SdPolygon && !(flags & (SdPie | SdPicture | SdRectangle | SdOther)))
        setupTabPolygon();

    if (flags & SdPicture && !(flags & (SdPie | SdPolygon | SdRectangle | SdOther)))
        setupTabPicture();

    if (flags & SdRectangle && !(flags & (SdPie | SdPolygon | SdPicture | SdOther)))
        setupTabRectangle();

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
    if (stickyObj)
        setSticky( oldSticky );

    if ( !oldProtectTripleState )
        setProtected( oldProtect );
    else
        setProtectTripleState();

    if ( !oldkeepRatiotripleState )
        setKeepRatio( oldKeepRatio );
    else
        setKeepRatioTripleState();
    setSize( oldRect);
}

void StyleDia::styleDone()
{
    if (m_confRectDia)
        m_confRectDia->setPenBrush(m_confPenDia->getPen(),
                                   m_confBrushDia->getBrush());
    if (m_confPolygonDia)
        m_confPolygonDia->setPenBrush(m_confPenDia->getPen(),
                                      m_confBrushDia->getBrush());
    if (m_confPieDia)
        m_confPieDia->setPenBrush(m_confPenDia->getPen(),
                                  m_confBrushDia->getBrush());

    emit styleOk();
}

void StyleDia::setupTabPen()
{
    m_confPenDia = new ConfPenDia(this, 0, flags);
    m_confPenDia->setPen( m_canvas->activePage()->getPen( m_doc->getKPresenterView()->getPen() ) );
    m_confPenDia->setLineBegin( m_canvas->activePage()->getLineBegin( m_doc->getKPresenterView()->getLineBegin() ) );
    m_confPenDia->setLineEnd( m_canvas->activePage()->getLineEnd( m_doc->getKPresenterView()->getLineEnd() ) );

    connect( this, SIGNAL( defaultButtonPressed () ), m_confPenDia, SLOT( slotReset() ) );
    if (flags & SdPen)
        addTab( m_confPenDia, i18n( "&Pen" ) );
    else
        m_confPenDia->hide();
}

void StyleDia::setupTabBrush()
{
    m_confBrushDia = new ConfBrushDia(this, 0, flags);
    m_confBrushDia->setBrush( m_canvas->activePage()->getBrush( m_doc->getKPresenterView()->getBrush() ) );
    m_confBrushDia->setFillType( m_canvas->activePage()->getFillType( m_doc->getKPresenterView()->getFillType() ) );
    m_confBrushDia->setGradient( m_canvas->activePage()->getGColor1( m_doc->getKPresenterView()->getGColor1() ),
                                 m_canvas->activePage()->getGColor2( m_doc->getKPresenterView()->getGColor2() ),
                                 m_canvas->activePage()->getGType( m_doc->getKPresenterView()->getGType() ),
                                 m_canvas->activePage()->getGUnbalanced( m_doc->getKPresenterView()->getGUnbalanced() ),
                                 m_canvas->activePage()->getGXFactor( m_doc->getKPresenterView()->getGXFactor() ),
                                 m_canvas->activePage()->getGYFactor( m_doc->getKPresenterView()->getGYFactor() ) );

    connect( this, SIGNAL( defaultButtonPressed () ), m_confBrushDia, SLOT( slotReset() ) );
    if (flags & SdBrush)
        addTab( m_confBrushDia, i18n( "&Brush" ) );
    else
        m_confBrushDia->hide();
}

void StyleDia::setupTabGeneral()
{
    PageConfigGeneral *w = new PageConfigGeneral( this );
    sticky = w->checkboxSticky;
    addTab( w , i18n( "&General" ) );
}

void StyleDia::setupTabGeometry()
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

    if ( allTextObj )
    {
        protectContent = new QCheckBox( i18n("Protect Content"), tab);
        layout->addWidget(protectContent);
    }

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

void StyleDia::setupTabPie()
{
    m_confPieDia = new ConfPieDia( this, "ConfPageDia" );
    m_confPieDia->setType( m_canvas->activePage()->getPieType( m_doc->getKPresenterView()->getPieType() ) );
    m_confPieDia->setAngle( m_canvas->activePage()->getPieAngle( m_doc->getKPresenterView()->getPieAngle() ) );
    m_confPieDia->setLength( m_canvas->activePage()->getPieLength( m_doc->getKPresenterView()->getPieLength() ) );
    m_confPieDia->setPenBrush( m_canvas->activePage()->getPen( m_doc->getKPresenterView()->getPen() ),
                               m_canvas->activePage()->getBrush( m_doc->getKPresenterView()->getBrush() ) );

    connect( this, SIGNAL( defaultButtonPressed () ), m_confPieDia, SLOT( slotReset() ) );
    addTab( m_confPieDia, i18n( "P&ie" ) );
}

void StyleDia::setupTabPolygon()
{
    bool _checkConcavePolygon;
    int _cornersValue;
    int _sharpnessValue;

    if ( !m_canvas->activePage()->getPolygonSettings( &_checkConcavePolygon, &_cornersValue, &_sharpnessValue ) ) {
        _checkConcavePolygon = m_doc->getKPresenterView()->getCheckConcavePolygon();
        _cornersValue = m_doc->getKPresenterView()->getCornersValue();
        _sharpnessValue = m_doc->getKPresenterView()->getSharpnessValue();
    }

    m_confPolygonDia = new ConfPolygonDia( this, "ConfPolygonDia", _checkConcavePolygon, _cornersValue, _sharpnessValue );
    m_confPolygonDia->setPenBrush( m_canvas->activePage()->getPen( m_doc->getKPresenterView()->getPen() ),
                                   m_canvas->activePage()->getBrush( m_doc->getKPresenterView()->getBrush() ) );

    connect( this, SIGNAL( defaultButtonPressed () ), m_confPolygonDia, SLOT( slotReset() ) );
    addTab( m_confPolygonDia, i18n( "P&olygon" ) );
}

void StyleDia::setupTabPicture()
{
    PictureMirrorType _mirrorType;
    int _depth;
    bool _swapRGB;
    bool _grayscal;
    int _bright;
    QPixmap _origPixmap;

    if ( !m_canvas->activePage()->getPictureSettingsAndPixmap( &_mirrorType, &_depth, &_swapRGB, &_grayscal, &_bright, &_origPixmap ) ) {
        _mirrorType = m_doc->getKPresenterView()->getPictureMirrorType();
        _depth = m_doc->getKPresenterView()->getPictureDepth();
        _swapRGB= m_doc->getKPresenterView()->getPictureSwapRGB();
        _grayscal = m_doc->getKPresenterView()->getPictureGrayscal();
        _bright = m_doc->getKPresenterView()->getPictureBright();
        _origPixmap = QPixmap();
    }

    if ( _origPixmap.isNull() )
    {
        _origPixmap=BarIcon("kpresenter", KIcon::SizeMedium);
    }

    m_confPictureDia = new ConfPictureDia( this, "ConfPictureDia", _mirrorType, _depth, _swapRGB, _grayscal, _bright, _origPixmap );

    connect( this, SIGNAL( defaultButtonPressed () ), m_confPictureDia, SLOT( slotReset() ) );
    addTab( m_confPictureDia, i18n( "Pi&cture" ) );
}

void StyleDia::setupTabRectangle()
{
    m_confRectDia = new ConfRectDia( this, "ConfRectDia" );
    m_confRectDia->setRnds( m_canvas->activePage()->getRndX( m_doc->getKPresenterView()->getRndX() ),
                          m_canvas->activePage()->getRndY( m_doc->getKPresenterView()->getRndY()) );
    m_confRectDia->setPenBrush( m_canvas->activePage()->getPen( m_doc->getKPresenterView()->getPen() ),
                                m_canvas->activePage()->getBrush( m_doc->getKPresenterView()->getBrush() ) );

    connect( this, SIGNAL( defaultButtonPressed () ), m_confRectDia, SLOT( slotReset() ) );
    addTab( m_confRectDia, i18n( "&Rectangle" ) );
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

StyleDia::~StyleDia()
{
    delete m_confPenDia;
    delete m_confPieDia;
    delete m_confRectDia;
    delete m_confBrushDia;
    delete m_confPolygonDia;
    delete m_confPictureDia;
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

void StyleDia::setProtectTripleState()
{
    oldProtect = false;
    protect->setTristate( true );
    protect->setNoChange();
    oldProtectTripleState =true;
}

bool StyleDia::protectNoChange()const
{
    return protect->state()== QButton::NoChange;
}

void StyleDia::setKeepRatio( bool p )
{
    oldKeepRatio = p;
    keepRatio->setChecked( p );
    oldkeepRatiotripleState = false;
}

void StyleDia::setKeepRatioTripleState()
{
    oldKeepRatio = false;
    keepRatio->setTristate( true );
    keepRatio->setNoChange();
    oldkeepRatiotripleState =true;
}

bool StyleDia::keepRatioNoChange()const
{
    return keepRatio->state()== QButton::NoChange;
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

void StyleDia::setProtectContent( bool p )
{
    if ( isAllTextObject() &&protectContent )
    {
        protectContent->setChecked(p);
    }
}

bool StyleDia::isProtectContent()const
{
    if ( isAllTextObject()&& protectContent)
    {
        return protectContent->isChecked();
    }
    return false;
}


#include <styledia.moc>
