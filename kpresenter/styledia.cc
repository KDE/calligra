// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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
#include "generalconfig.h"
#include "confpiedia.h"
#include "confrectdia.h"
#include "confpolygondia.h"
#include "confpicturedia.h"

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
#include "kpresenter_utils.h"
#include "kpgradient.h"
#include <qgroupbox.h>
#include "kpresenter_doc.h"
#include "kpresenter_view.h"
#include <koUnit.h>
#include "kprcanvas.h"
#include "kprcommand.h"
#include <kdeversion.h>

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
            diff1 = getOffset( lineBegin, _w, + 180.0 );

        if ( lineEnd != L_NORMAL )
            diff2 = getOffset( lineEnd, _w, 0 );

        if ( lineBegin != L_NORMAL )
            drawFigure( lineBegin, painter, KoPoint( 5, _zoomHandler->unzoomItY( contentsRect().height() ) / 2 ),
                        pen.color(), _w, 180.0, _zoomHandler );

        if ( lineEnd != L_NORMAL )
            drawFigure( lineEnd, painter, KoPoint( _zoomHandler->unzoomItX( contentsRect().width() - 7 ) ,
                                                   _zoomHandler->unzoomItY( contentsRect().height() ) / 2 ),
                        pen.color(), _w, 0.0, _zoomHandler );

        painter->setPen( pen );
        painter->drawLine( - (int)diff1.width() + 8,
                           contentsRect().height() / 2,
                           contentsRect().width() - (int)diff2.width() - 7,
                           contentsRect().height() / 2 );

    } else if ( paintType == Brush ) {
        painter->fillRect( 0, 0, contentsRect().width(), contentsRect().height(),
                           colorGroup().base() );
        painter->fillRect( 0, 0, contentsRect().width(), contentsRect().height(), brush );
    } else if ( paintType == Gradient && gradient )
        painter->drawPixmap( 0, 0, gradient->pixmap());

    painter->restore();
}

PBPreview::~PBPreview()
{
    delete _zoomHandler;
}

ConfPenDia::ConfPenDia( QWidget* parent, const char* name, int flags)
    : QWidget( parent, name ), m_flags(flags), m_bLineBeginChanged(false), m_bLineEndChanged(false),
      m_bColorChanged(false), m_bStyleChanged(false), m_bWidthChanged(false)
{
    QVBoxLayout *clayout = new QVBoxLayout( this );

    clayout->setMargin( KDialog::marginHint() );
    clayout->setSpacing( KDialog::spacingHint() );

    QLabel *l = new QLabel( i18n( "Color:" ), this );
    clayout->addWidget(l);

    choosePCol = new KColorButton( Qt::black,
                                   Qt::black,
                                   this );
    connect( choosePCol, SIGNAL( changed( const QColor& ) ),
             this, SLOT( slotColorChanged() ) );
    clayout->addWidget(choosePCol);

    l = new QLabel( i18n( "Style:" ), this);
    clayout->addWidget(l);

    choosePStyle = new KComboBox( false, this, "PStyle" );
    choosePStyle->insertItem( i18n( "No Outline" ) );
    choosePStyle->insertItem( i18n( "Solid Line" ) );
    choosePStyle->insertItem( i18n( "Dash Line ( ---- )" ) );
    choosePStyle->insertItem( i18n( "Dot Line ( **** )" ) );
    choosePStyle->insertItem( i18n( "Dash Dot Line ( -*-* )" ) );
    choosePStyle->insertItem( i18n( "Dash Dot Dot Line ( -**- )" ) );
    connect( choosePStyle, SIGNAL( activated( int ) ),
             this, SLOT( slotStyleChanged() ) );
    clayout->addWidget(choosePStyle);

    l = new QLabel( i18n( "Width:" ), this );
    clayout->addWidget(l);

    choosePWidth = new KIntNumInput( 1, this );
    choosePWidth->setSuffix(" pt"); // TODO use unit here, make it use i18n at the same time.
    choosePWidth->setRange( 1, 10, 1 );
    connect( choosePWidth, SIGNAL( valueChanged( int ) ),
             this, SLOT( slotWidthChanged() ) );
    clayout->addWidget(choosePWidth);

    QGroupBox *grp = new QGroupBox(2, Qt::Horizontal, i18n("Arrow Style"), this);
    QVBoxLayout *grplayout = new QVBoxLayout( grp );
    grplayout->setMargin( KDialog::marginHint() );
    grplayout->setSpacing( KDialog::spacingHint() );
    clayout->addWidget(grp);

    l = new QLabel( i18n( "Begin:" ), grp );
    grplayout->addWidget(l);

    clineBegin = new KComboBox( false, grp, "lineBegin" );
    clineBegin->insertItem( i18n("Normal") );
    clineBegin->insertItem( i18n("Arrow") );
    clineBegin->insertItem( i18n("Square") );
    clineBegin->insertItem( i18n("Circle") );
    clineBegin->insertItem( i18n("Line Arrow") );
    clineBegin->insertItem( i18n("Dimension Line") );
    clineBegin->insertItem( i18n("Double Arrow") );
    clineBegin->insertItem( i18n("Double Line Arrow") );
    grplayout->addWidget(clineBegin);

    connect( clineBegin, SIGNAL( activated( int ) ),
             this, SLOT( slotLineBeginChanged() ) );

    l = new QLabel( i18n( "End:" ), grp );
    grplayout->addWidget(l);

    clineEnd = new KComboBox( false, grp, "lineEnd" );
    clineEnd->insertItem( i18n("Normal") );
    clineEnd->insertItem( i18n("Arrow") );
    clineEnd->insertItem( i18n("Square") );
    clineEnd->insertItem( i18n("Circle") );
    clineEnd->insertItem( i18n("Line Arrow") );
    clineEnd->insertItem( i18n("Dimension Line") );
    clineEnd->insertItem( i18n("Double Arrow") );
    clineEnd->insertItem( i18n("Double Line Arrow") );
    connect( clineEnd, SIGNAL( activated( int ) ),
             this, SLOT( slotLineEndChanged() ) );
    grplayout->addWidget(clineEnd);

    if ( !(m_flags & StyleDia::SdEndBeginLine) )
        grp->setEnabled(false);
    penPrev = new PBPreview( this, "penPrev", PBPreview::Pen );
    clayout->addWidget(penPrev);
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    clayout->addItem( spacer );
}

ConfPenDia::~ConfPenDia()
{
    delete penPrev;
}

void ConfPenDia::setPen( const QPen &_pen )
{
    oldPen=_pen;
    switch ( _pen.style() ) {
    case NoPen: choosePStyle->setCurrentItem( 0 );
        break;
    case SolidLine: choosePStyle->setCurrentItem( 1 );
        break;
    case DashLine: choosePStyle->setCurrentItem( 2 );
        break;
    case DotLine: choosePStyle->setCurrentItem( 3 );
        break;
    case DashDotLine: choosePStyle->setCurrentItem( 4 );
        break;
    case DashDotDotLine: choosePStyle->setCurrentItem( 5 );
        break;
    case MPenStyle:  break; // not supported.
    }
    choosePWidth->setValue( _pen.width() );
    choosePCol->setColor( _pen.color() );
    penPrev->setPen(_pen);
    choosePWidth->setEnabled(_pen.style()!=NoPen);
}

void ConfPenDia::setLineBegin( LineEnd lb )
{
    oldLb = lb;
    clineBegin->setCurrentItem( (int)lb );
    penPrev->setLineBegin(lb);
}

void ConfPenDia::setLineEnd( LineEnd le )
{
    oldLe=le;
    clineEnd->setCurrentItem( (int)le );
    penPrev->setLineEnd(le);
}

void ConfPenDia::resetConfigChangedValues()
{
    m_bColorChanged = false;
    m_bStyleChanged = false;
    m_bWidthChanged = false;
    m_bLineBeginChanged = false;
    m_bLineEndChanged = false;
}

QPen ConfPenDia::getPen() const
{
    QPen pen;

    switch ( choosePStyle->currentItem() ) {
    case 0: pen.setStyle( NoPen );
        break;
    case 1: pen.setStyle( SolidLine );
        break;
    case 2: pen.setStyle( DashLine );
        break;
    case 3: pen.setStyle( DotLine );
        break;
    case 4: pen.setStyle( DashDotLine );
        break;
    case 5: pen.setStyle( DashDotDotLine );
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

int ConfPenDia::getPenConfigChange() const
{
    int flags = 0;
    if (m_bLineEndChanged)
        flags = flags | PenCmd::LineEnd;
    if (m_bLineBeginChanged)
        flags = flags | PenCmd::LineBegin;
    if (m_bColorChanged)
        flags = flags | PenCmd::Color;
    if (m_bStyleChanged)
        flags = flags | PenCmd::Style;
    if (m_bWidthChanged)
        flags = flags | PenCmd::Width;

    return flags;
}

void ConfPenDia::slotReset()
{
    setPen(oldPen);
    setLineBegin(oldLb);
    setLineEnd(oldLe);
    resetConfigChangedValues();
}

void ConfPenDia::slotColorChanged()
{
    m_bColorChanged = true;
    penPrev->setPen(getPen());
}

void ConfPenDia::slotStyleChanged()
{
    m_bStyleChanged = true;
    QPen pen = getPen();
    choosePWidth->setEnabled(pen.style()!=NoPen);
    penPrev->setPen(pen);
}

void ConfPenDia::slotWidthChanged()
{
    m_bWidthChanged = true;
    penPrev->setPen(getPen());
}

void ConfPenDia::slotLineBeginChanged()
{
    m_bLineBeginChanged = true;
    penPrev->setLineBegin(getLineBegin());
}

void ConfPenDia::slotLineEndChanged()
{
    m_bLineEndChanged = true;
    penPrev->setLineEnd(getLineEnd());
}

ConfBrushDia::ConfBrushDia( QWidget* parent, const char* name, int flags)
    : QWidget( parent, name ), m_bBrushColorChanged(false), m_bBrushStyleChanged(false),
      m_bFillTypeChanged(false), m_bGColor1Changed(false), m_bGColor2Changed(false),
      m_bGTypeChanged(false), m_bGUnbalancedChanged(false), m_flags(flags)
{
    QGridLayout *grid = new QGridLayout( this, 2, 2, KDialog::marginHint(), KDialog::spacingHint() );

    QVBox *left = new QVBox( this );
    grid->addWidget( left, 0, 0 );

    left->setSpacing( KDialog::spacingHint() );

    QLabel *l = new QLabel( i18n( "Fill type:" ), left );
    l->setFixedHeight( l->sizeHint().height() );

    cFillType = new KComboBox( false, left );
    cFillType->insertItem( i18n( "Fill" ) );
    if(m_flags & StyleDia::SdGradient)
        cFillType->insertItem( i18n( "Gradient" ) );

    connect( cFillType, SIGNAL( activated( int ) ),
             this, SLOT( slotFillTypeChanged() ) );

    (void)new QWidget( left );

    stack = new QWidgetStack( left );
    connect( cFillType, SIGNAL( activated( int ) ),
             stack, SLOT( raiseWidget( int ) ) );

    QVBox *brushConfig = new QVBox( stack );
    brushConfig->setSpacing( KDialog::spacingHint() );
    stack->addWidget( brushConfig, 0 );

    l = new QLabel( i18n( "Fill color:" ), brushConfig );
    l->setFixedHeight( l->sizeHint().height() );

    chooseBCol = new KColorButton( Qt::white,
                                   Qt::white,
                                   brushConfig );
    connect( chooseBCol, SIGNAL( changed( const QColor & ) ),
             this, SLOT( slotBrushColorChanged() ) );

    l = new QLabel( i18n( "Fill style:" ), brushConfig );
    l->setFixedHeight( l->sizeHint().height() );

    chooseBStyle = new KComboBox( false, brushConfig, "BStyle" );
    chooseBStyle->insertItem( i18n( "No Background Fill" ) );
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
    connect( chooseBStyle, SIGNAL( activated( int ) ),
             this, SLOT( slotBrushStyleChanged() ) );

    (void)new QWidget( brushConfig );

    QVBox *gradientConfig = new QVBox( left );
    stack->addWidget( gradientConfig, 1 );
    gradientConfig->setSpacing( KDialog::spacingHint() );

    l = new QLabel( i18n( "Gradient colors:" ), gradientConfig );
    l->setFixedHeight( l->sizeHint().height() );

    gradient1 = new KColorButton( red,
                                  red,
                                  gradientConfig );
    connect( gradient1, SIGNAL( changed( const QColor & ) ),
             this, SLOT( slotGColor1Changed() ) );
    gradient2 = new KColorButton( green,
                                  green,
                                  gradientConfig );
    connect( gradient2, SIGNAL( changed( const QColor & ) ),
             this, SLOT( slotGColor2Changed() ) );

    l = new QLabel( i18n( "Gradient style:" ), gradientConfig );
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
             this, SLOT( slotGTypeChanged() ) );

    unbalanced = new QCheckBox( i18n( "Unbalanced" ), gradientConfig );
    connect( unbalanced, SIGNAL( clicked() ),
             this, SLOT( slotGUnbalancedChanged() ) );

    xfactorLabel = new QLabel( i18n( "X-factor:" ), gradientConfig );
    xfactorLabel->setFixedHeight( xfactorLabel->sizeHint().height() );

    xfactor = new QSlider( -200, 200, 1, 100, QSlider::Horizontal, gradientConfig );
    connect( xfactor, SIGNAL( valueChanged( int ) ),
             this, SLOT( slotGXFactorChanged() ) );

    yfactorLabel = new QLabel( i18n( "Y-factor:" ), gradientConfig );
    yfactorLabel->setFixedHeight( yfactorLabel->sizeHint().height() );

    yfactor = new QSlider( -200, 200, 1, 100, QSlider::Horizontal, gradientConfig );
    connect( yfactor, SIGNAL( valueChanged( int ) ),
             this, SLOT( slotGYFactorChanged() ) );

    gradient = new KPGradient( Qt::red, Qt::green, BCT_GHORZ, false, 100, 100 );

    (void)new QWidget( gradientConfig );
    (void)new QWidget( left );

    brushPrev = new PBPreview( this, "", PBPreview::Brush );
    brushPrev->setGradient( gradient );

    grid->addWidget( brushPrev, 0, 1 );

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
    case NoBrush: chooseBStyle->setCurrentItem( 0 );
        break;
    case SolidPattern: chooseBStyle->setCurrentItem( 1 );
        break;
    case Dense1Pattern: chooseBStyle->setCurrentItem( 2 );
        break;
    case Dense2Pattern: chooseBStyle->setCurrentItem( 3 );
        break;
    case Dense3Pattern: chooseBStyle->setCurrentItem( 4 );
        break;
    case Dense4Pattern: chooseBStyle->setCurrentItem( 5 );
        break;
    case Dense5Pattern: chooseBStyle->setCurrentItem( 6 );
        break;
    case Dense6Pattern: chooseBStyle->setCurrentItem( 7 );
        break;
    case Dense7Pattern: chooseBStyle->setCurrentItem( 8 );
        break;
    case HorPattern: chooseBStyle->setCurrentItem( 9 );
        break;
    case VerPattern: chooseBStyle->setCurrentItem( 10 );
        break;
    case CrossPattern: chooseBStyle->setCurrentItem( 11 );
        break;
    case BDiagPattern: chooseBStyle->setCurrentItem( 12 );
        break;
    case FDiagPattern: chooseBStyle->setCurrentItem( 13 );
        break;
    case DiagCrossPattern: chooseBStyle->setCurrentItem( 14 );
        break;
    case CustomPattern:
        break;
    }
    chooseBCol->setColor( _brush.color() );
    brushPrev->setBrush(_brush);

    if(_brush.style()==NoBrush)
        brushPrev->hide();
    else
        brushPrev->show();
}

void ConfBrushDia::setFillType( FillType ft )
{
    cFillType->setCurrentItem( (int)ft );
    stack->raiseWidget( (int)ft );
    if (ft == FT_BRUSH)
    {
        brushPrev->setPaintType(PBPreview::Brush );
        brushPrev->setBrush(getBrush());
        brushPrev->repaint(true);
    }
    else
    {
        brushPrev->setPaintType(PBPreview::Gradient);
        gradient->setColor1(getGColor1());
        gradient->setColor2(getGColor2());
        gradient->setBackColorType(getGType());
        gradient->setUnbalanced(getGUnbalanced());
        gradient->setXFactor(getGXFactor());
        gradient->setYFactor(getGYFactor());
        brushPrev->repaint(true);
    }
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
    xfactorLabel->setEnabled(_unbalanced);
    yfactorLabel->setEnabled(_unbalanced);
    xfactor->setEnabled(_unbalanced);
    yfactor->setEnabled(_unbalanced);
    xfactor->setValue( _xfactor );
    yfactor->setValue( _yfactor );
    brushPrev->repaint(false);
}

void ConfBrushDia::resetConfigChangedValues()
{
    m_bBrushColorChanged = false;
    m_bBrushStyleChanged = false;
    m_bFillTypeChanged = false;
    m_bGColor1Changed = false;
    m_bGColor2Changed = false;
    m_bGTypeChanged = false;
    m_bGUnbalancedChanged = false;
}

QBrush ConfBrushDia::getBrush() const
{
    QBrush brush;

    switch ( chooseBStyle->currentItem() ) {
    case 0: brush.setStyle( NoBrush );
        break;
    case 1: brush.setStyle( SolidPattern );
        break;
    case 2: brush.setStyle( Dense1Pattern );
        break;
    case 3: brush.setStyle( Dense2Pattern );
        break;
    case 4: brush.setStyle( Dense3Pattern );
        break;
    case 5: brush.setStyle( Dense4Pattern );
        break;
    case 6: brush.setStyle( Dense5Pattern );
        break;
    case 7: brush.setStyle( Dense6Pattern );
        break;
    case 8: brush.setStyle( Dense7Pattern );
        break;
    case 9: brush.setStyle( HorPattern );
        break;
    case 10: brush.setStyle( VerPattern );
        break;
    case 11: brush.setStyle( CrossPattern );
        break;
    case 12: brush.setStyle( BDiagPattern );
        break;
    case 13: brush.setStyle( FDiagPattern );
        break;
    case 14: brush.setStyle( DiagCrossPattern );
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

int ConfBrushDia::getBrushConfigChange() const
{
    int flags = 0;
    if (m_bBrushColorChanged)
        flags = flags | BrushCmd::BrushColor;
    if (m_bBrushStyleChanged)
        flags = flags | BrushCmd::BrushStyle;
    if (m_bFillTypeChanged)
        flags = flags | BrushCmd::All;
    if (m_bGColor1Changed)
        flags = flags | BrushCmd::GradientColor1;
    if (m_bGColor2Changed)
        flags = flags | BrushCmd::GradientColor2;
    if (m_bGTypeChanged)
        flags = flags | BrushCmd::GradientType;
    if (m_bGUnbalancedChanged)
        flags = flags | BrushCmd::GradientBalanced;

    return flags;
}

void ConfBrushDia::slotReset()
{
    setBrush(oldBrush);
    setFillType(oldFillType);
    setGradient(oldC1, oldC1, oldBCType,oldUnbalanced, oldXfactor, oldYfactor);
    resetConfigChangedValues();
}

void ConfBrushDia::slotBrushColorChanged()
{
    m_bBrushColorChanged = true;
    brushPrev->setBrush(getBrush());
    brushPrev->repaint(true);
}

void ConfBrushDia::slotBrushStyleChanged()
{
    m_bBrushStyleChanged = true;
    QBrush b = getBrush();
    if(b.style()==NoBrush)
        brushPrev->hide();
    else {
        brushPrev->show();
        brushPrev->setBrush(b);
        brushPrev->repaint(true);
    }
}

void ConfBrushDia::slotFillTypeChanged()
{
    m_bFillTypeChanged = true;

    if (getFillType() == FT_BRUSH)
    {
        if(getBrush().style()==NoBrush) {
            brushPrev->hide();
            brushPrev->setBrush(getBrush());
            brushPrev->repaint(true);
        } else {
            brushPrev->show();
            brushPrev->repaint(true);
        }
        brushPrev->setPaintType(PBPreview::Brush );
    }
    else
    {
        brushPrev->show();
        brushPrev->setPaintType(PBPreview::Gradient);
        gradient->setColor1(getGColor1());
        gradient->setColor2(getGColor2());
        gradient->setBackColorType(getGType());
        gradient->setUnbalanced(getGUnbalanced());
        gradient->setXFactor(getGXFactor());
        gradient->setYFactor(getGYFactor());
        brushPrev->repaint(true);
    }
}

void ConfBrushDia::slotGColor1Changed()
{
    m_bGColor1Changed = true;
    gradient->setColor1(getGColor1());
    brushPrev->repaint(false);
}

void ConfBrushDia::slotGColor2Changed()
{
    m_bGColor2Changed = true;
    gradient->setColor2(getGColor2());
    brushPrev->repaint(false);
}

void ConfBrushDia::slotGTypeChanged()
{
    m_bGTypeChanged = true;
    gradient->setBackColorType(getGType());
    brushPrev->repaint(false);
}

void ConfBrushDia::slotGUnbalancedChanged()
{
    m_bGUnbalancedChanged = true;
    bool state = unbalanced->isChecked();
    xfactorLabel->setEnabled(state);
    yfactorLabel->setEnabled(state);
    xfactor->setEnabled(state);
    yfactor->setEnabled(state);
    gradient->setUnbalanced(state);
    brushPrev->repaint(false);
}

void ConfBrushDia::slotGXFactorChanged()
{
    gradient->setXFactor(getGXFactor());
    brushPrev->repaint(false);
}

void ConfBrushDia::slotGYFactorChanged()
{
    gradient->setYFactor(getGYFactor());
    brushPrev->repaint(false);
}

StyleDia::StyleDia( QWidget* parent, const char* name, KPresenterDoc *_doc,
                    bool _stickyObj, bool _oneObject, bool _alltextobj )
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
    m_canvas = m_doc->firstView()->getCanvas();
    flags = m_canvas->getPenBrushFlags();

    oldProtect=STATE_OFF;
    oldKeepRatio=STATE_OFF;

    oldLeft = 0.0;
    oldTop = 0.0;
    oldBottom = 0.0;
    oldRight = 0.0;


    oldRect=KoRect();

    if (flags & SdPen)
        setupTabPen();

    if (flags & SdBrush)
        setupTabBrush();

    if (flags & SdPie && !(flags & (SdPolygon | SdPicture | SdRectangle | SdOther)))
        setupTabPie();

    if (flags & SdPolygon && !(flags & (SdPie | SdPicture | SdRectangle | SdOther)))
        setupTabPolygon();

    if (flags & SdPicture && !(flags & (SdPie | SdPolygon | SdRectangle | SdOther)))
        setupTabPicture();

    if (flags & SdRectangle && !(flags & (SdPie | SdPolygon | SdPicture | SdOther)))
        setupTabRectangle();

    setupTabGeometry();

    if (stickyObj)
        setupTabGeneral();

    lockUpdate = false;

    setCancelButton( i18n( "&Cancel" ) );
    setOkButton( i18n( "&OK" ) );
    setApplyButton( i18n( "&Apply" ) );
    slotReset();
    connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( styleDone() ) );
    connect( this, SIGNAL( cancelButtonPressed() ), this, SLOT( reject() ) );
}

void StyleDia::slotReset()
{
    if (stickyObj)
        setSticky( oldSticky );

    setObjectName( oldObjectName );
    setProtected( oldProtect );
    setKeepRatio( oldKeepRatio );
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
    // if type is arc there is no m_confBrushDia
    if (m_confPieDia)
        m_confPieDia->setPenBrush(m_confPenDia->getPen(),
                                  (flags & SdBrush) ? m_confBrushDia->getBrush() : NoBrush );

    emit styleOk();
}

void StyleDia::setupTabPen()
{
    m_confPenDia = new ConfPenDia(this, 0, flags);
    m_confPenDia->setPen(m_canvas->getPen(m_doc->firstView()->getPen()));
    m_confPenDia->setLineBegin(m_canvas->getLineBegin(m_doc->firstView()->getLineBegin()));
    m_confPenDia->setLineEnd(m_canvas->getLineEnd(m_doc->firstView()->getLineEnd()));
    m_confPenDia->resetConfigChangedValues();

    addTab( m_confPenDia, i18n( "Out&line" ) );
}

void StyleDia::setupTabBrush()
{
    m_confBrushDia = new ConfBrushDia(this, 0, flags);
    m_confBrushDia->setBrush(m_canvas->getBrush( m_doc->firstView()->getBrush()));
    m_confBrushDia->setFillType(m_canvas->getFillType(m_doc->firstView()->getFillType()));
    m_confBrushDia->setGradient(m_canvas->getGColor1(m_doc->firstView()->getGColor1()),
                                m_canvas->getGColor2(m_doc->firstView()->getGColor2()),
                                m_canvas->getGType(m_doc->firstView()->getGType()),
                                m_canvas->getGUnbalanced(m_doc->firstView()->getGUnbalanced()),
                                m_canvas->getGXFactor(m_doc->firstView()->getGXFactor()),
                                m_canvas->getGYFactor(m_doc->firstView()->getGYFactor()));
    m_confBrushDia->resetConfigChangedValues();

    addTab( m_confBrushDia, i18n( "&Fill" ) );
}

void StyleDia::setupTabGeneral()
{
    PageConfigGeneral *w = new PageConfigGeneral( this );
    sticky = w->checkboxSticky;
    objectName = w->objectName;

    if ( ! oneObject ) {
        w->label->setEnabled( false );
        w->objectName->setEnabled( false );
    }

    addTab( w, i18n( "&General" ) );
}

void StyleDia::setupTabGeometry()
{
    QWidget *tab = new QWidget( this );
    QVBoxLayout *layout = new QVBoxLayout( tab );
    layout->setMargin( KDialog::marginHint() );
    layout->setSpacing( KDialog::spacingHint() );

    protect= new QCheckBox( i18n("Protect size and position"), tab);
    layout->addWidget(protect);
    connect( protect, SIGNAL(clicked ()), this, SLOT(protectChanged()));

    keepRatio= new QCheckBox( i18n("Keep ratio"), tab);
    layout->addWidget(keepRatio);
    connect( keepRatio, SIGNAL(toggled(bool)),
             this, SLOT(slotKeepRatioToggled(bool)));

    if ( allTextObj )
    {
        protectContent = new QCheckBox( i18n("Protect content"), tab);
        connect( protectContent, SIGNAL(toggled ( bool ) ), this, SLOT(slotProtectContentChanged( bool )));
        layout->addWidget(protectContent);
    }
    else
        protectContent = 0L;

    QGroupBox *grp1 = new QGroupBox( i18n("Position (%1)").arg(m_doc->getUnitName()), tab );
    layout->addWidget( grp1 );
    QGridLayout *pGrid = new QGridLayout( grp1, 8, 2, KDialog::marginHint(), KDialog::spacingHint() );
    pGrid->addRowSpacing(0, fontMetrics().height()/2);

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
    connect( m_lineWidth, SIGNAL(valueChanged(double)),
             this, SLOT(slotUpdateHeightForWidth(double)) );

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
    connect( m_lineHeight, SIGNAL(valueChanged(double)),
             this, SLOT(slotUpdateWidthForHeight(double)) );

    pGrid->addWidget( m_lineHeight, 4, 1 );


    QGroupBox *grp2 = new QGroupBox( i18n("Margins (%1)").arg(m_doc->getUnitName()), tab );
    QGridLayout *mGrid = new QGridLayout( grp2, 6, 2, KDialog::marginHint(), KDialog::spacingHint() );
    mGrid->addRowSpacing(0, fontMetrics().height()/2);

    layout->addWidget( grp2 );


    QLabel *lml = new QLabel( i18n( "Left:" ), grp2 );
    lml->resize( lml->sizeHint() );
    mGrid->addWidget( lml, 2, 0 );

    sml = new KDoubleNumInput( grp2 );

    sml->setValue( 0.0 );
    sml->resize( sml->sizeHint() );
    mGrid->addWidget( sml, 3, 0 );

    QLabel *lmr = new QLabel( i18n( "Right:" ), grp2 );
    lmr->resize( lmr->sizeHint() );
    mGrid->addWidget( lmr, 2, 1 );

    smr = new KDoubleNumInput( grp2 );

    smr->setValue( 0.0 );
    smr->resize( smr->sizeHint() );
    mGrid->addWidget( smr, 3, 1 );

    QLabel *lmt = new QLabel( i18n( "Top:" ), grp2 );
    lmt->resize( lmt->sizeHint() );
    mGrid->addWidget( lmt, 4, 0 );

    smt = new KDoubleNumInput( grp2 );

    smt->setValue( 0.0 );
    smt->resize( smt->sizeHint() );
    mGrid->addWidget( smt, 5, 0 );

    QLabel *lmb = new QLabel( i18n( "Bottom:" ), grp2 );
    lmb->resize( lmb->sizeHint() );
    mGrid->addWidget( lmb, 4, 1 );

    smb = new KDoubleNumInput( grp2 );

    smb->setValue( 0.0 );
    smb->resize( smb->sizeHint() );
    mGrid->addWidget( smb, 5, 1 );

    mGrid->addRowSpacing( 0, KDialog::spacingHint() + 5 );
    synchronize=new QCheckBox( i18n("Synchronize changes"), grp2 );
    mGrid->addMultiCellWidget( synchronize, 1, 1, 0, 1 );

    QSpacerItem *spacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addItem(spacer);

    connect( smb, SIGNAL( valueChanged(double)), this, SLOT( slotMarginsChanged( double )));
    connect( sml, SIGNAL( valueChanged(double)), this, SLOT( slotMarginsChanged( double )));
    connect( smr, SIGNAL( valueChanged(double)), this, SLOT( slotMarginsChanged( double )));
    connect( smt, SIGNAL( valueChanged(double)), this, SLOT( slotMarginsChanged( double )));

    if ( !allTextObj )
    {
        grp2->hide();
        synchronize->hide();
    }
    if (protectContent)
    {
        bool state = protectContent->isChecked();
        smb->setEnabled( !state );
        smt->setEnabled( !state );
        smr->setEnabled( !state );
        sml->setEnabled( !state );
        synchronize->setEnabled( !state );
    }
    addTab( tab, i18n( "Geo&metry" ) );
}

void StyleDia::setupTabPie()
{
    m_confPieDia = new ConfPieDia( this, "ConfPageDia" );
    m_confPieDia->setType(m_canvas->getPieType(m_doc->firstView()->getPieType()));
    m_confPieDia->setAngle(m_canvas->getPieAngle(m_doc->firstView()->getPieAngle()));
    m_confPieDia->setLength(m_canvas->getPieLength(m_doc->firstView()->getPieLength()));
    m_confPieDia->setPenBrush(m_canvas->getPen(m_doc->firstView()->getPen()),
                              m_canvas->getBrush(m_doc->firstView()->getBrush()));
    m_confPieDia->resetConfigChangedValues();

    addTab( m_confPieDia, i18n( "P&ie" ) );
}

void StyleDia::setupTabPolygon()
{
    m_confPolygonDia = new ConfPolygonDia(this, "ConfPolygonDia");
    m_confPolygonDia->setCheckConcavePolygon(m_canvas->getCheckConcavePolygon(m_doc->firstView()->getCheckConcavePolygon()));
    m_confPolygonDia->setCornersValue(m_canvas->getCornersValue(m_doc->firstView()->getCornersValue()));
    m_confPolygonDia->setSharpnessValue(m_canvas->getSharpnessValue(m_doc->firstView()->getSharpnessValue()));
    m_confPolygonDia->setPenBrush(m_canvas->getPen(m_doc->firstView()->getPen()),
                                  m_canvas->getBrush(m_doc->firstView()->getBrush()));
    m_confPolygonDia->resetConfigChangedValues();

    addTab( m_confPolygonDia, i18n( "P&olygon" ) );
}

void StyleDia::setupTabPicture()
{
    m_confPictureDia = new ConfPictureDia( this, "ConfPictureDia");
    m_confPictureDia->setPictureMirrorType(m_canvas->getPictureMirrorType(m_doc->firstView()->getPictureMirrorType()));
    m_confPictureDia->setPictureDepth(m_canvas->getPictureDepth(m_doc->firstView()->getPictureDepth()));
    m_confPictureDia->setPictureSwapRGB(m_canvas->getPictureSwapRGB(m_doc->firstView()->getPictureSwapRGB()));
    m_confPictureDia->setPictureGrayscal(m_canvas->getPictureGrayscal(m_doc->firstView()->getPictureGrayscal()));
    m_confPictureDia->setPictureBright(m_canvas->getPictureBright(m_doc->firstView()->getPictureBright()));
    m_confPictureDia->setPicturePixmap(m_canvas->getPicturePixmap());

    addTab( m_confPictureDia, i18n( "Pi&cture" ) );
}

void StyleDia::setupTabRectangle()
{
    m_confRectDia = new ConfRectDia( this, "ConfRectDia" );
    m_confRectDia->setRnds(m_canvas->getRndX(m_doc->firstView()->getRndX()),
                           m_canvas->getRndY(m_doc->firstView()->getRndY()));
    m_confRectDia->setPenBrush(m_canvas->getPen(m_doc->firstView()->getPen()),
                               m_canvas->getBrush(m_doc->firstView()->getBrush()));
    m_confRectDia->resetConfigChangedValues();

    addTab( m_confRectDia, i18n( "&Rectangle" ) );
}

void StyleDia::slotMarginsChanged( double val)
{
    if ( synchronize->isChecked() && !lockUpdate)
    {
        lockUpdate = true;
        sml->setValue( val );
        smb->setValue( val );
        smr->setValue( val );
        smt->setValue( val );
        lockUpdate = false;
    }
}

void StyleDia::slotUpdateWidthForHeight( double height )
{
    if ( !isKeepRatio() ) return;
    if ( heightByWidthRatio == 0 ) return; // avoid DBZ
    m_lineWidth->setValue( height / heightByWidthRatio );
}

void StyleDia::slotUpdateHeightForWidth( double width )
{
    if ( !isKeepRatio() ) return;
    m_lineHeight->setValue( width * heightByWidthRatio );
}

void StyleDia::slotKeepRatioToggled( bool on ) {
    if ( !on ) return;
    if ( m_lineWidth->value() == 0 )
        heightByWidthRatio = 1.0; // arbitrary
    else
        heightByWidthRatio = m_lineHeight->value() / m_lineWidth->value();
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

void StyleDia::setMargins( double left, double right, double top, double bottom)
{
    oldLeft = left;
    oldTop = top;
    oldBottom = bottom;
    oldRight = right;
    sml->setValue( KoUnit::toUserValue( QMAX(0.00, left), m_doc->getUnit() ) );
    smr->setValue( KoUnit::toUserValue( QMAX(0.00, right), m_doc->getUnit() ) );
    smt->setValue( KoUnit::toUserValue( QMAX(0.00, top), m_doc->getUnit() ) );
    smb->setValue( KoUnit::toUserValue( QMAX(0.00, bottom), m_doc->getUnit() ) );
}

double StyleDia::marginsLeft()
{
    return QMAX(KoUnit::fromUserValue( sml->value(), m_doc->getUnit() ),0);
}

double StyleDia::marginsRight()
{
    return QMAX(KoUnit::fromUserValue( smr->value(), m_doc->getUnit() ),0);
}

double StyleDia::marginsBottom()
{
    return QMAX(KoUnit::fromUserValue( smb->value(), m_doc->getUnit() ),0);
}

double StyleDia::marginsTop()
{
    return QMAX(KoUnit::fromUserValue( smt->value(), m_doc->getUnit() ),0);
}

void StyleDia::setSticky( PropValue p )
{
    oldSticky=p;
    if( stickyObj)
    {
        switch( p )
        {
        case STATE_ON:
            sticky->setChecked( true );
            break;
        case STATE_OFF:
            sticky->setChecked( false );
            break;
        case STATE_UNDEF:
            sticky->setTristate( true );
            sticky->setNoChange();
            break;
        default:
            sticky->setChecked( false );
            break;
        }
    }
}

bool StyleDia::stickyNoChange()const
{
    if( stickyObj)
        return sticky->state()== QButton::NoChange;
    return true;
}

bool StyleDia::isSticky() const
{
    if( stickyObj )
        return sticky->isChecked();
    return false;
}

void StyleDia::setObjectName( QString &name )
{
    oldObjectName = name;
    objectName->setText( oldObjectName );
}

QString StyleDia::getObjectName( ) const
{
    return objectName->text();
}

void StyleDia::setProtected( PropValue p )
{
    oldProtect=p;
    switch( oldProtect )
    {
    case STATE_ON:
        protect->setChecked( true );
        break;
    case STATE_OFF:
        protect->setChecked( false );
        break;
    case STATE_UNDEF:
        protect->setTristate( true );
        protect->setNoChange();
        break;
    default:
        protect->setChecked( false );
        break;
    }
    protectChanged();
}

bool StyleDia::isProtected() const
{
    return protect->isChecked();
}

bool StyleDia::protectNoChange()const
{
    return protect->state()== QButton::NoChange;
}

void StyleDia::setKeepRatio( PropValue p )
{
    oldKeepRatio = p;
    switch( p )
    {
    case STATE_ON:
        keepRatio->setChecked( true );
        break;
    case STATE_OFF:
        keepRatio->setChecked( false );
        break;
    case STATE_UNDEF:
        keepRatio->setTristate( true );
        keepRatio->setNoChange();
        break;
    default:
        keepRatio->setChecked( false );
        break;
    }
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
    double top=QMAX(0, KoUnit::fromUserValue( m_lineTop->value(), m_doc->getUnit() ));
    double left=QMAX(0, KoUnit::fromUserValue( m_lineLeft->value(), m_doc->getUnit() ));
    double width=QMAX(0, KoUnit::fromUserValue( m_lineWidth->value(), m_doc->getUnit() ));
    double height=QMAX(0, KoUnit::fromUserValue( m_lineHeight->value(), m_doc->getUnit() ));

    KoRect newRect = KoRect( left, top, width, height);
    return newRect;
}

void StyleDia::setSize(const KoRect & _rect)
{
    oldRect = _rect;
    m_lineTop->setValue(KoUnit::toUserValue( QMAX(0.00, _rect.top()), m_doc->getUnit() ));
    m_lineLeft->setValue(KoUnit::toUserValue( QMAX(0.00, _rect.left()), m_doc->getUnit() ));
    m_lineWidth->setValue(KoUnit::toUserValue( QMAX(0.00, _rect.width()), m_doc->getUnit() ));
    m_lineHeight->setValue(KoUnit::toUserValue( QMAX(0.00, _rect.height()), m_doc->getUnit() ));
    heightByWidthRatio = m_lineHeight->value() / m_lineWidth->value();
}

void StyleDia::setProtectContent( bool p )
{
    if ( isAllTextObject() &&protectContent )
        protectContent->setChecked(p);
}

bool StyleDia::isProtectContent()const
{
    if ( isAllTextObject()&& protectContent)
        return protectContent->isChecked();
    return false;
}

void StyleDia::slotProtectContentChanged( bool b)
{
    sml->setEnabled( !b );
    smt->setEnabled( !b );
    smb->setEnabled( !b );
    smr->setEnabled( !b );
    synchronize->setEnabled( !b );
}

#include "styledia.moc"
