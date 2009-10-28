/*  -*- C++ -*-
    This file is part of the KDE libraries
    Copyright (C) 2003 Jason Harris <kstars@30doradus.org>

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
    Boston, MA 02110-1301, USA.
*/

#include "kplotwidget.h"
#include "kplotwidget.moc"

#include <math.h>
#include <kdebug.h>

#include <QtGui/QActionEvent>
#include <QHash>
#include <QPainter>
#include <QPixmap>
#include <QToolTip>
#include <QtAlgorithms>

#include "kplotaxis.h"
#include <kplotpoint.h>
#include "kplotobject.h"

#define XPADDING 20
#define YPADDING 20
#define BIGTICKSIZE 10
#define SMALLTICKSIZE 4
#define TICKOFFSET 0

#define EXTRALABELSPACE 4
#define MINIMUMLABELSPACE 10

namespace KPlato
{

class KPlotWidget::Private
{
public:
    Private( KPlotWidget *qq )
      : q( qq ),
        cBackground( Qt::black ), cForeground( Qt::white ), cGrid( Qt::gray ),
        showGrid( false ), showObjectToolTip( true ), useAntialias( false )
    {
        // create the axes and setting their default properties
        KPlotAxis *leftAxis = new KPlotAxis();
        leftAxis->setTickLabelsShown( true );
        axes.insert( LeftAxis, leftAxis );
        KPlotAxis *bottomAxis = new KPlotAxis();
        bottomAxis->setTickLabelsShown( true );
        axes.insert( BottomAxis, bottomAxis );
        KPlotAxis *rightAxis = new KPlotAxis();
        axes.insert( RightAxis, rightAxis );
        KPlotAxis *topAxis = new KPlotAxis();
        axes.insert( TopAxis, topAxis );
    }

    ~Private()
    {
        qDeleteAll( objectList );
        qDeleteAll( axes );
    }

    KPlotWidget *q;

    void calcDataRectLimits( double x1, double x2, double y1, double y2 );
    /**
     * @return a value indicating how well the given rectangle is 
     * avoiding masked regions in the plot.  A higher returned value 
     * indicates that the rectangle is intersecting a larger portion 
     * of the masked region, or a portion of the masked region which 
     * is weighted higher.
     * @param r The rectangle to be tested
     */
    float rectCost( const QRectF &r ) const;

    //Colors
    QColor cBackground, cForeground, cGrid;
    //draw options
    bool showGrid : 1;
    bool showObjectToolTip : 1;
    bool useAntialias : 1;
    //padding
    int leftPadding, rightPadding, topPadding, bottomPadding;
    // hashmap with the axes we have
    QHash<Axis, KPlotAxis*> axes;
    // List of KPlotObjects
    QList<KPlotObject*> objectList;
    // Limits of the plot area in data units
    QRectF dataRect, secondDataRect;
    // Limits of the plot area in pixel units
    QRect pixRect;
    // Grid of bools to mask "used" regions of the plot
    float plotMask[100][100];
    double px[100], py[100];

    int bottomTickLabelHeight;
    int bottomLabelHeight;
    int topTickLabelHeight;
    int topLabelHeight;
    int leftTickLabelWidth;
    int leftLabelWidth;
    int rightTickLabelWidth;
    int rightLabelWidth;

};

KPlotWidget::KPlotWidget( QWidget * parent )
    : QFrame( parent ), d( new Private( this ) )
{
    setAttribute( Qt::WA_OpaquePaintEvent );
    setAttribute( Qt::WA_NoSystemBackground );

    d->secondDataRect = QRectF(); //default: no secondary data rect
    // sets the default limits
    d->calcDataRectLimits( 0.0, 1.0, 0.0, 1.0 );

    setDefaultPaddings();

    setMinimumSize( 150, 150 );
    resize( minimumSizeHint() );
}

KPlotWidget::~KPlotWidget()
{
    delete d;
}

QSize KPlotWidget::minimumSizeHint() const
{
    return QSize( 150, 150 );
}

QSize KPlotWidget::sizeHint() const
{
    return size();
}

void KPlotWidget::setLimits( double x1, double x2, double y1, double y2 )
{
    d->calcDataRectLimits( x1, x2, y1, y2 );
    update();
}

void KPlotWidget::Private::calcDataRectLimits( double x1, double x2, double y1, double y2 )
{
    double XA1, XA2, YA1, YA2;
    if (x2<x1) { XA1=x2; XA2=x1; }
    else { XA1=x1; XA2=x2; }
    if ( y2<y1) { YA1=y2; YA2=y1; }
    else { YA1=y1; YA2=y2; }

    if ( XA2 == XA1 ) {
        kWarning() << "x1 and x2 cannot be equal. Setting x2 = x1 + 1.0";
        XA2 = XA1 + 1.0;
    }
    if ( YA2 == YA1 ) {
        kWarning() << "y1 and y2 cannot be equal. Setting y2 = y1 + 1.0";
        YA2 = YA1 + 1.0;
    }
    dataRect = QRectF( XA1, YA1, XA2 - XA1, YA2 - YA1 );

    q->axis( LeftAxis )->setTickMarks( dataRect.y(), dataRect.height() );
    q->axis( BottomAxis )->setTickMarks( dataRect.x(), dataRect.width() );

    if ( secondDataRect.isNull() )
    {
        q->axis( RightAxis )->setTickMarks( dataRect.y(), dataRect.height() );
        q->axis( TopAxis )->setTickMarks( dataRect.x(), dataRect.width() );
    }
}

void KPlotWidget::setSecondaryLimits( double x1, double x2, double y1, double y2 ) {
    double XA1, XA2, YA1, YA2;
    if (x2<x1) { XA1=x2; XA2=x1; }
    else { XA1=x1; XA2=x2; }
    if ( y2<y1) { YA1=y2; YA2=y1; }
    else { YA1=y1; YA2=y2; }

    if ( XA2 == XA1 ) {
        kWarning() << "x1 and x2 cannot be equal. Setting x2 = x1 + 1.0";
        XA2 = XA1 + 1.0;
    }
    if ( YA2 == YA1 ) {
        kWarning() << "y1 and y2 cannot be equal. Setting y2 = y1 + 1.0";
        YA2 = YA1 + 1.0;
    }
    d->secondDataRect = QRectF( XA1, YA1, XA2-XA1, YA2-YA1 );

    axis(RightAxis)->setTickMarks( d->secondDataRect.y(), d->secondDataRect.height() );
    axis(TopAxis)->setTickMarks( d->secondDataRect.x(), d->secondDataRect.width() );

    update();
}

void KPlotWidget::clearSecondaryLimits() {
    d->secondDataRect = QRectF();
    axis(RightAxis)->setTickMarks( d->dataRect.y(), d->dataRect.height() );
    axis(TopAxis)->setTickMarks( d->dataRect.x(), d->dataRect.width() );

    update();
}

QRectF KPlotWidget::dataRect() const
{
    return d->dataRect;
}

QRectF KPlotWidget::secondaryDataRect() const
{
    return d->secondDataRect;
}

void KPlotWidget::addPlotObject( KPlotObject *object )
{
    // skip null pointers
    if ( !object )
        return;
    d->objectList.append( object );
    update();
}

void KPlotWidget::addPlotObjects( const QList< KPlotObject* >& objects )
{
    bool addedsome = false;
    foreach ( KPlotObject *o, objects )
    {
        if ( !o )
            continue;

        d->objectList.append( o );
        addedsome = true;
    }
    if ( addedsome )
        update();
}

QList< KPlotObject* > KPlotWidget::plotObjects() const
{
    return d->objectList;
}

void KPlotWidget::removeAllPlotObjects()
{
    if ( d->objectList.isEmpty() )
        return;

    qDeleteAll( d->objectList );
    d->objectList.clear();
    update();
}

void KPlotWidget::resetPlotMask() {
    for (int ix=0; ix<100; ++ix ) 
        for ( int iy=0; iy<100; ++iy ) 
            d->plotMask[ix][iy] = 0.0;
}

void KPlotWidget::resetPlot() {
    qDeleteAll( d->objectList );
    d->objectList.clear();
    clearSecondaryLimits();
    d->calcDataRectLimits( 0.0, 1.0, 0.0, 1.0 );
    KPlotAxis *a = axis( RightAxis );
    a->setLabel( QString() );
    a->setTickLabelsShown( false );
    a = axis( TopAxis );
    a->setLabel( QString() );
    a->setTickLabelsShown( false );
    axis(KPlotWidget::LeftAxis)->setLabel( QString() );
    axis(KPlotWidget::BottomAxis)->setLabel( QString() );
    resetPlotMask();
}

void KPlotWidget::replacePlotObject( int i, KPlotObject *o )
{
    // skip null pointers and invalid indexes
    if ( !o || i < 0 || i >= d->objectList.count() )
        return;
    d->objectList.replace( i, o );
    update();
}

QColor KPlotWidget::backgroundColor() const
{
    return d->cBackground;
}

QColor KPlotWidget::foregroundColor() const
{
    return d->cForeground;
}

QColor KPlotWidget::gridColor() const
{
    return d->cGrid;
}

void KPlotWidget::setBackgroundColor( const QColor &bg ) {
    d->cBackground = bg;
    update();
}

void KPlotWidget::setForegroundColor( const QColor &fg )
{
    d->cForeground = fg;
    update();
}

void KPlotWidget::setGridColor( const QColor &gc )
{
    d->cGrid = gc;
    update();
}

bool KPlotWidget::isGridShown() const
{
    return d->showGrid;
}

bool KPlotWidget::isObjectToolTipShown() const
{
    return d->showObjectToolTip;
}

bool KPlotWidget::antialiasing() const
{
    return d->useAntialias;
}

void KPlotWidget::setAntialiasing( bool b )
{
    d->useAntialias = b;
    update();
}

void KPlotWidget::setShowGrid( bool show ) {
    d->showGrid = show;
    update();
}

void KPlotWidget::setObjectToolTipShown( bool show )
{
    d->showObjectToolTip = show;
}


KPlotAxis* KPlotWidget::axis( Axis type )
{
    QHash<Axis, KPlotAxis*>::Iterator it = d->axes.find( type );
    return it != d->axes.end() ? it.value() : 0;
}

const KPlotAxis* KPlotWidget::axis( Axis type ) const
{
    QHash<Axis, KPlotAxis*>::ConstIterator it = d->axes.constFind( type );
    return it != d->axes.constEnd() ? it.value() : 0;
}

QRect KPlotWidget::pixRect() const
{
    return d->pixRect;
}

QList<KPlotPoint*> KPlotWidget::pointsUnderPoint( const QPoint& p ) const {
    QList<KPlotPoint*> pts;
    foreach ( KPlotObject *po, d->objectList ) {
        foreach ( KPlotPoint *pp, po->points() ) {
            if ( ( p - mapToWidget( pp->position() ).toPoint() ).manhattanLength() <= 4 )
                pts << pp;
        }
    }

    return pts;
}


bool KPlotWidget::event( QEvent* e ) {
    if ( e->type() == QEvent::ToolTip ) {
        if ( d->showObjectToolTip )
        {
            QHelpEvent *he = static_cast<QHelpEvent*>( e );
            const QList<KPlotPoint*> pts = pointsUnderPoint( he->pos() - QPoint( contentsFrame() + leftPadding(), contentsFrame() + topPadding() ) - contentsRect().topLeft() );
            if ( pts.count() > 0 ) {
                QToolTip::showText( he->globalPos(), pts.front()->label(), this );
            }
        }
        e->accept();
        return true;
    }
    else
        return QFrame::event( e );
}

void KPlotWidget::resizeEvent( QResizeEvent* e ) {
    QFrame::resizeEvent( e );
}

void KPlotWidget::setPixRect( QPainter *p ) {
    p->save();
    //set small font for tick labels
    QFont f = p->font();
    int s = f.pointSize();
    f.setPointSize( s - 2 );
    p->setFont( f );

    d->bottomTickLabelHeight = 0;
    d->bottomLabelHeight = 0;
    d->topTickLabelHeight = 0;
    d->topLabelHeight = 0;
    d->leftTickLabelWidth = 0;
    d->leftLabelWidth = 0;
    d->rightTickLabelWidth = 0;
    d->rightLabelWidth = 0;
    KPlotAxis *a = axis(BottomAxis);
    if ( a && a->areTickLabelsShown() ) {
        foreach( double xx, a->majorTickMarks() ) {
            int h = p->boundingRect( QRect(0,0,0,0), Qt::AlignLeft | Qt::TextDontClip, a->tickLabel( xx ) ).height();
            d->bottomTickLabelHeight = qMax( h + EXTRALABELSPACE, d->bottomTickLabelHeight );
        }
        if ( ! a->label().isEmpty() ) {
            int h = p->boundingRect( QRect(0,0,0,0), Qt::AlignLeft | Qt::TextDontClip, a->label() ).height();
            d->bottomLabelHeight = h + EXTRALABELSPACE;
        }
    }
    a = axis(TopAxis);
    if ( a && a->areTickLabelsShown() ) {
        foreach( double xx, a->majorTickMarks() ) {
            int h = p->boundingRect( QRect(0,0,0,0), Qt::AlignLeft | Qt::TextDontClip, a->tickLabel( xx ) ).height();
            d->topTickLabelHeight = qMax( h + EXTRALABELSPACE, d->topTickLabelHeight );
        }
        if ( ! a->label().isEmpty() ) {
            int h = p->boundingRect( QRect(0,0,0,0), Qt::AlignLeft | Qt::TextDontClip, a->label() ).height();
            d->topLabelHeight = h + EXTRALABELSPACE;
        }
    }
    a = axis(LeftAxis);
    if ( a && a->areTickLabelsShown() ) {
        foreach( double xx, a->majorTickMarks() ) {
            int w = p->boundingRect( QRect(0,0,0,0), Qt::AlignLeft | Qt::TextDontClip, a->tickLabel( xx ) ).width();
            d->leftTickLabelWidth = qMax( w + EXTRALABELSPACE, d->leftTickLabelWidth );
        }
        if ( ! a->label().isEmpty() ) {
            p->rotate( -90.0 );
            int w = p->boundingRect( QRect(0,0,0,0), Qt::AlignLeft | Qt::TextDontClip, a->label() ).height();
            d->leftLabelWidth = w + EXTRALABELSPACE;
            p->rotate( 90.0 );
        }
    }
    a = axis(RightAxis);
    if ( a && a->areTickLabelsShown() ) {
        foreach( double xx, a->majorTickMarks() ) {
            int w = p->boundingRect( QRect(0,0,0,0), Qt::AlignLeft | Qt::TextDontClip, a->tickLabel( xx ) ).width();
            d->rightTickLabelWidth = qMax( w + EXTRALABELSPACE, d->rightTickLabelWidth );
        }
        if ( ! a->label().isEmpty() ) {
            p->rotate( -90.0 );
            int w = p->boundingRect( QRect(0,0,0,0), Qt::AlignLeft | Qt::TextDontClip, a->label() ).height();
            d->rightLabelWidth = w + EXTRALABELSPACE;
            p->rotate( 90.0 );
        }
    }
    int padWidth = leftPadding() + rightPadding();
    int padHeight = bottomPadding() + topPadding();
    int newWidth = paintRect().width() - padWidth;
    int newHeight = paintRect().height() - padHeight;
    // PixRect starts at (0,0) because we will translate by leftPadding(), topPadding()
    d->pixRect = QRect( 0, 0, newWidth, newHeight );
    for ( int i=0; i<100; ++i ) {
        d->px[i] = double(i*d->pixRect.width())/100.0 + double(d->pixRect.x());
        d->py[i] = double(i*d->pixRect.height())/100.0 + double(d->pixRect.y());
    }
    p->restore();
}

QPointF KPlotWidget::mapToWidget( const QPointF& p ) const
{
    float px = d->pixRect.left() + d->pixRect.width() * ( p.x() - d->dataRect.x() ) / d->dataRect.width();
    float py = d->pixRect.top() + d->pixRect.height() * ( d->dataRect.y() + d->dataRect.height() - p.y() ) / d->dataRect.height();
    return QPointF( px, py );
}

void KPlotWidget::maskRect( const QRectF& r, float value ) {
    //Loop over Mask grid points that are near the target rectangle.
    int ix1 = int( 100.0*(r.x() - d->pixRect.x())/d->pixRect.width() );
    int iy1 = int( 100.0*(r.y() - d->pixRect.y())/d->pixRect.height() );
    if ( ix1 < 0 ) ix1 = 0;
    if ( iy1 < 0 ) iy1 = 0;
    int ix2 = int( 100.0*(r.right() - d->pixRect.x())/d->pixRect.width() ) + 2;
    int iy2 = int( 100.0*(r.bottom() - d->pixRect.y())/d->pixRect.height() ) + 2;
    if ( ix1 > 99 ) ix1 = 99;
    if ( iy1 > 99 ) iy1 = 99;

    for ( int ix=ix1; ix<ix2; ++ix ) 
        for ( int iy=iy1; iy<iy2; ++iy ) 
            d->plotMask[ix][iy] += value;
}

void KPlotWidget::maskAlongLine( const QPointF &p1, const QPointF &p2, float value ) {
    //Determine slope and zeropoint of line
    double m = (p2.y() - p1.y())/(p2.x() - p1.x());
    double y0 = p1.y() - m*p1.x();
 
    //Make steps along line from p1 to p2, computing the nearest 
    //gridpoint position at each point.
    double x1 = p1.x();
    double x2 = p2.x();
    if ( x1 > x2 ) {
        x1 = p2.x(); 
        x2 = p1.x();
    }
    for ( double x=x1; x<x2; x+=0.01*(x2-x1) ) {
        double y = y0 + m*x;
        int ix = int( 100.0*( x - d->pixRect.x() )/d->pixRect.width() );
        int iy = int( 100.0*( y - d->pixRect.y() )/d->pixRect.height() );

        if ( ix >= 0 && ix < 100 && iy >= 0 && iy < 100 )
          d->plotMask[ix][iy] += value;

    }
}

void KPlotWidget::placeLabel( QPainter *painter, KPlotPoint *pp ) {
    int textFlags = Qt::TextSingleLine | Qt::AlignCenter;

    float rbest = 100;
    float bestCost = 1.0e7;
    QPointF pos = mapToWidget( pp->position() );
    QRectF bestRect;
    int ix0 = int( 100.0*( pos.x() - d->pixRect.x() )/d->pixRect.width() );
    int iy0 = int( 100.0*( pos.y() - d->pixRect.y() )/d->pixRect.height() );

    QFontMetricsF fm( painter->font(), painter->device() );
    int xmin = qMax( ix0 - 20, 0 );
    int xmax = qMin( ix0 + 20, 100 );
    int ymin = qMax( iy0 - 20, 0 );
    int ymax = qMin( iy0 + 20, 100 );
    for ( int ix = xmin; ix < xmax; ++ix )
    {
        for ( int iy = ymin; iy < ymax; ++iy )
        {
            QRectF labelRect = fm.boundingRect( QRectF( d->px[ix], d->py[iy], 1, 1 ), textFlags, pp->label() );
                //Add some padding to labelRect
                labelRect.adjust( -2, -2, 2, 2 );

                float r = sqrt( (float)(ix-ix0)*(ix-ix0) + (iy-iy0)*(iy-iy0) );
                float cost = d->rectCost( labelRect ) + 0.1*r;

                if ( cost < bestCost ) {
                    bestRect = labelRect;
                    bestCost = cost;
                    rbest = r;
                }
        }
    }

    if ( ! bestRect.isNull() ) {
        painter->drawText( bestRect, textFlags, pp->label() );

        //Is a line needed to connect the label to the point?
        if ( rbest > 2.0 ) {
            //Draw a rectangle around the label 
            painter->setBrush( QBrush() );
            //QPen pen = painter->pen();
            //pen.setStyle( Qt::DotLine );
            //painter->setPen( pen );
            painter->drawRoundRect( bestRect );
    
            //Now connect the label to the point with a line.
            //The line is drawn from the center of the near edge of the rectangle
            float xline = bestRect.center().x();
            if ( bestRect.left() > pos.x() )
                xline = bestRect.left();
            if ( bestRect.right() < pos.x() )
                xline = bestRect.right();
    
            float yline = bestRect.center().y();
            if ( bestRect.top() > pos.y() )
                yline = bestRect.top();
            if ( bestRect.bottom() < pos.y() )
                yline = bestRect.bottom();
    
            painter->drawLine( QPointF( xline, yline ), pos );
        }
                                                
        //Mask the label's rectangle so other labels won't overlap it.
        maskRect( bestRect );
    }
}

float KPlotWidget::Private::rectCost( const QRectF &r ) const
{
    int ix1 = int( 100.0 * ( r.x() - pixRect.x() ) / pixRect.width() );
    int ix2 = int( 100.0 * ( r.right() - pixRect.x() ) / pixRect.width() );
    int iy1 = int( 100.0 * ( r.y() - pixRect.y() ) / pixRect.height() );
    int iy2 = int( 100.0 * ( r.bottom() - pixRect.y() ) / pixRect.height() );
    float cost = 0.0;

    for ( int ix=ix1; ix<ix2; ++ix ) {
        for ( int iy=iy1; iy<iy2; ++iy ) {
            if ( ix >= 0 && ix < 100 && iy >= 0 && iy < 100 ) {
                cost += plotMask[ix][iy];
            } else {
                cost += 100.;
            }
        }
    }

    return cost;
}

void KPlotWidget::paintEvent( QPaintEvent *e ) {
    // let QFrame draw its default stuff (like the frame)
    QFrame::paintEvent( e );
    QPainter p;

    p.begin( this );
    p.setRenderHint( QPainter::Antialiasing, d->useAntialias );
    p.fillRect( rect(), backgroundColor() );
    p.translate( contentsFrame(), contentsFrame() );
    setPixRect( &p );

    p.translate( leftPadding() + 0.5, topPadding() + 0.5 );

    p.setClipRect( d->pixRect );
    p.setClipping( true );

    resetPlotMask();

    foreach( KPlotObject *po, d->objectList )
        po->draw( &p, this );

    //DEBUG_MASK
    /*
    p.setPen( Qt::magenta );
    p.setBrush( Qt::magenta );
    for ( int ix=0; ix<100; ++ix ) {
        for ( int iy=0; iy<100; ++iy ) {
            if ( PlotMask[ix][iy] > 0.0 ) {
                double x = d->pixRect.x() + double(ix*d->pixRect.width())/100.;
                double y = d->pixRect.y() + double(iy*d->pixRect.height())/100.;

                p.drawRect( QRectF(x-1, y-1, 2, 2 ) );
            }
        }
    }
  */

    p.setClipping( false );
    drawAxes( &p );

    p.end();
}

void KPlotWidget::drawAxes( QPainter *p ) {
    if ( d->showGrid ) {
        p->setPen( gridColor() );

        //Grid lines are placed at locations of primary axes' major tickmarks
        //vertical grid lines
        foreach ( double xx, axis(BottomAxis)->majorTickMarks() ) {
            double px = d->pixRect.width() * (xx - d->dataRect.x()) / d->dataRect.width();
            p->drawLine( QPointF( px, 0.0 ), QPointF( px, double(d->pixRect.height()) ) );
        }
        //horizontal grid lines
        foreach( double yy, axis(LeftAxis)->majorTickMarks() ) {
            double py = d->pixRect.height() * (yy - d->dataRect.y()) / d->dataRect.height();
            p->drawLine( QPointF( 0.0, py ), QPointF( double(d->pixRect.width()), py ) );
        }
    }

    p->setPen( foregroundColor() );
    p->setBrush( Qt::NoBrush );

    //set small font for tick labels
    QFont f = p->font();
    int s = f.pointSize();
    f.setPointSize( s - 2 );
    p->setFont( f );

    /*** BottomAxis ***/
    KPlotAxis *a = axis(BottomAxis);
    if (a->isVisible()) {
        //Draw axis line
        p->drawLine( 0, d->pixRect.height(), d->pixRect.width(), d->pixRect.height() );

        // Draw major tickmarks
        foreach( double xx, a->majorTickMarks() ) {
            double px = d->pixRect.width() * (xx - d->dataRect.x()) / d->dataRect.width();
            if ( px >= 0 && px < d->pixRect.width() ) {
                p->drawLine( QPointF( px, double(d->pixRect.height() - TICKOFFSET)), 
                        QPointF( px, double(d->pixRect.height() - BIGTICKSIZE - TICKOFFSET)) );

                //Draw ticklabel
                if ( a->areTickLabelsShown() ) {
                    QRect r( int(px) - BIGTICKSIZE, d->pixRect.height(), d->bottomTickLabelHeight, d->bottomTickLabelHeight );
                    p->drawText( r, Qt::AlignCenter | Qt::TextDontClip, a->tickLabel( xx ) );
                }
            }
        }

        // Draw minor tickmarks
        foreach ( double xx, a->minorTickMarks() ) {
            double px = d->pixRect.width() * (xx - d->dataRect.x()) / d->dataRect.width();
            if ( px > 0 && px < d->pixRect.width() ) {
                p->drawLine( QPointF( px, double(d->pixRect.height() - TICKOFFSET)), 
                        QPointF( px, double(d->pixRect.height() - SMALLTICKSIZE -TICKOFFSET)) );
            }
        }

        // Draw BottomAxis Label
        if ( ! a->label().isEmpty() ) {
            int y = d->pixRect.height() + d->bottomTickLabelHeight;
            QRect r( d->pixRect.x(), y, d->pixRect.width(), d->bottomLabelHeight );
            p->drawText( r, Qt::AlignCenter, a->label() );
        }
    }  //End of BottomAxis

    /*** LeftAxis ***/
    a = axis(LeftAxis);
    if (a->isVisible()) {
        //Draw axis line
        p->drawLine( 0, 0, 0, d->pixRect.height() );

        // Draw major tickmarks
        foreach( double yy, a->majorTickMarks() ) {
            double py = d->pixRect.height() * ( 1.0 - (yy - d->dataRect.y()) / d->dataRect.height() );
            if ( py > 0 && py < d->pixRect.height() ) {
                p->drawLine( QPointF( TICKOFFSET, py ), QPointF( double(TICKOFFSET + BIGTICKSIZE), py ) );

                //Draw ticklabel
                if ( a->areTickLabelsShown() ) {
                    QRect r( -d->leftTickLabelWidth-2, int(py)-SMALLTICKSIZE, d->leftTickLabelWidth, 2*SMALLTICKSIZE );
                    p->drawText( r, Qt::AlignRight | Qt::AlignVCenter | Qt::TextDontClip, a->tickLabel( yy ) );
                }
            }
        }

        // Draw minor tickmarks
        foreach ( double yy, a->minorTickMarks() ) {
            double py = d->pixRect.height() * ( 1.0 - (yy - d->dataRect.y()) / d->dataRect.height() );
            if ( py > 0 && py < d->pixRect.height() ) {
                p->drawLine( QPointF( TICKOFFSET, py ), QPointF( double(TICKOFFSET + SMALLTICKSIZE), py ) );
            }
        }

        //Draw LeftAxis Label.  We need to draw the text sideways.
        if ( ! a->label().isEmpty() ) {
            //store current painter translation/rotation state
            p->save();
    
            //translate coord sys to left corner of axis label rectangle, then rotate 90 degrees.
            p->translate( -leftPadding(), d->pixRect.height() );
            p->rotate( -90.0 );
    
            QRect r( 0, 0, d->pixRect.height(), d->leftLabelWidth );
            p->drawText( r, Qt::AlignCenter, a->label() ); //draw the label, now that we are sideways
    
            p->restore();  //restore translation/rotation state
        }
    }  //End of LeftAxis

    //Prepare for top and right axes; we may need the secondary data rect
    double x0 = d->dataRect.x();
    double y0 = d->dataRect.y();
    double dw = d->dataRect.width();
    double dh = d->dataRect.height();
    if ( secondaryDataRect().isValid() ) {
        x0 = secondaryDataRect().x();
        y0 = secondaryDataRect().y();
        dw = secondaryDataRect().width();
        dh = secondaryDataRect().height();
    }

    /*** TopAxis ***/
    a = axis(TopAxis);
    if (a->isVisible()) {
        p->drawLine( 0, 0, d->pixRect.width(), 0 );

        // Draw major tickmarks
        foreach( double xx, a->majorTickMarks() ) {
            double px = d->pixRect.width() * (xx - x0) / dw;
            if ( px > 0 && px < d->pixRect.width() ) {
                p->drawLine( QPointF( px, TICKOFFSET ), QPointF( px, double(BIGTICKSIZE + TICKOFFSET)) );

                //Draw ticklabel
                if ( a->areTickLabelsShown() ) {
                    QRect r( int(px) - BIGTICKSIZE, (int)-1.5*BIGTICKSIZE, 2*BIGTICKSIZE, BIGTICKSIZE );
                    p->drawText( r, Qt::AlignCenter | Qt::TextDontClip, a->tickLabel( xx ) );
                }
            }
        }

        // Draw minor tickmarks
        foreach ( double xx, a->minorTickMarks() ) {
            double px = d->pixRect.width() * (xx - x0) / dw;
            if ( px > 0 && px < d->pixRect.width() ) {
                p->drawLine( QPointF( px, TICKOFFSET ), QPointF( px, double(SMALLTICKSIZE + TICKOFFSET)) );
            }
        }

        // Draw TopAxis Label
        if ( ! a->label().isEmpty() ) {
            QRect r( 0, 0 - topPadding(), d->pixRect.width(), d->topLabelHeight );
            p->drawText( r, Qt::AlignCenter, a->label() );
        }
    }  //End of TopAxis

    /*** RightAxis ***/
    a = axis(RightAxis);
    if (a->isVisible()) {
        p->drawLine( d->pixRect.width(), 0, d->pixRect.width(), d->pixRect.height() );

        // Draw major tickmarks
        foreach( double yy, a->majorTickMarks() ) {
            double py = d->pixRect.height() * ( 1.0 - (yy - y0) / dh );
            if ( py > 0 && py < d->pixRect.height() ) {
                p->drawLine( QPointF( double(d->pixRect.width() - TICKOFFSET), py ), 
                        QPointF( double(d->pixRect.width() - TICKOFFSET - BIGTICKSIZE), py ) );

                //Draw ticklabel
                if ( a->areTickLabelsShown() ) {
                    QRect r( d->pixRect.width() + SMALLTICKSIZE, int(py)-SMALLTICKSIZE, 2*BIGTICKSIZE, 2*SMALLTICKSIZE );
                    p->drawText( r, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip, a->tickLabel( yy ) );
                }
            }
        }

        // Draw minor tickmarks
        foreach ( double yy, a->minorTickMarks() ) {
            double py = d->pixRect.height() * ( 1.0 - (yy - y0) / dh );
            if ( py > 0 && py < d->pixRect.height() ) {
                p->drawLine( QPointF( double(d->pixRect.width() - 0.0), py ), 
                        QPointF( double(d->pixRect.width() - 0.0 - SMALLTICKSIZE), py ) );
            }
        }

        //Draw RightAxis Label.  We need to draw the text sideways.
        if ( ! a->label().isEmpty() ) {
            //store current painter translation/rotation state
            p->save();
    
            //translate coord sys to left corner of axis label rectangle, then rotate 90 degrees.
            p->translate( d->pixRect.width() + rightPadding() - d->rightLabelWidth, d->pixRect.height() );
            p->rotate( -90.0 );
    
            QRect r( 0, 0, d->pixRect.height(), d->rightLabelWidth );
            p->drawText( r, Qt::AlignCenter, a->label() ); //draw the label, now that we are sideways
    
            p->restore();  //restore translation/rotation state
        }
    }  //End of RightAxis
}

int KPlotWidget::leftPadding() const
{
    return d->leftLabelWidth + d->leftTickLabelWidth;
/*    if ( d->leftPadding >= 0 )
        return d->leftPadding;
    const KPlotAxis *a = axis( LeftAxis );
    if ( a && a->isVisible() && a->areTickLabelsShown() )
    {
        return !a->label().isEmpty() ? 3 * XPADDING : 2 * XPADDING;
    }
    return XPADDING;*/
}

int KPlotWidget::rightPadding() const
{
    return qMax( d->rightLabelWidth + d->rightTickLabelWidth, MINIMUMLABELSPACE ) ;
/*    if ( d->rightPadding >= 0 )
        return d->rightPadding;
    const KPlotAxis *a = axis( RightAxis );
    if ( a && a->isVisible() && a->areTickLabelsShown() )
    {
        return !a->label().isEmpty() ? 3 * XPADDING : 2 * XPADDING;
    }
    return XPADDING;*/
}

int KPlotWidget::topPadding() const
{
    return qMax( d->topLabelHeight + d->topTickLabelHeight, MINIMUMLABELSPACE );
/*    if ( d->topPadding >= 0 )
        return d->topPadding;
    const KPlotAxis *a = axis( TopAxis );
    if ( a && a->isVisible() && a->areTickLabelsShown() )
    {
        return !a->label().isEmpty() ? 3 * YPADDING : 2 * YPADDING;
    }
    return YPADDING;*/
}

int KPlotWidget::bottomPadding() const
{
    return d->bottomLabelHeight + d->bottomTickLabelHeight;
/*    if ( d->bottomPadding >= 0 )
        return d->bottomPadding;
    const KPlotAxis *a = axis( BottomAxis );
    if ( a && a->isVisible() && a->areTickLabelsShown() )
    {
        return !a->label().isEmpty() ? 3 * YPADDING : 2 * YPADDING;
    }
    return YPADDING;*/
}

void KPlotWidget::setLeftPadding( int padding )
{
    d->leftPadding = padding;
}

void KPlotWidget::setRightPadding( int padding )
{
    d->rightPadding = padding;
}

void KPlotWidget::setTopPadding( int padding )
{
    d->topPadding = padding;
}

void KPlotWidget::setBottomPadding( int padding )
{
    d->bottomPadding = padding;
}

void KPlotWidget::setDefaultPaddings()
{
   d->leftPadding = -1;
   d->rightPadding = -1;
   d->topPadding = -1;
   d->bottomPadding = -1;
}

int KPlotWidget::contentsFrame() const
{
    return 2;
}

QRect KPlotWidget::paintRect() const
{
    return contentsRect().adjusted( 0, 0, -2*contentsFrame(), -2*contentsFrame() );
}

} //namespace KPlato
