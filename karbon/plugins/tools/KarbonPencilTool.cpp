/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KarbonPencilTool.h"
#include "KarbonCurveFit.h"

#include <KoPathShape.h>
#include <KoLineBorder.h>
#include <KoPointerEvent.h>
#include <KoCanvasBase.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>
#include <KoSelection.h>

#include <knuminput.h>
#include <klocale.h>

#include <QStackedWidget>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QPainter>

#include <math.h>

KarbonPencilTool::KarbonPencilTool(KoCanvasBase *canvas)
    : KoTool( canvas ),  m_mode( ModeRaw ), m_optimizeRaw( false )
    , m_optimizeCurve( false ), m_combineAngle( 15.0 ), m_fittingError( 4.0 )
    , m_close( false ), m_shape( 0 )

{
}

KarbonPencilTool::~KarbonPencilTool()
{
}

void KarbonPencilTool::paint( QPainter &painter, KoViewConverter &converter )
{
    if( ! m_shape )
        return;

    painter.save();

    painter.setMatrix( m_shape->transformationMatrix( &converter ) * painter.matrix() );

    //KoShape::applyConversion( painter, converter );

    painter.save();
    m_shape->paint( painter, converter );
    painter.restore();

    if( m_shape->border() )
    {
        painter.save();
        m_shape->border()->paintBorder( m_shape, painter, converter );
        painter.restore();
    }

    painter.restore();
}

void KarbonPencilTool::repaintDecorations()
{
}

void KarbonPencilTool::mousePressEvent( KoPointerEvent *event )
{
    if( ! m_shape )
    {
        m_shape = new KoPathShape();
        m_shape->setShapeId( KoPathShapeId );
        // TODO take properties from the resource provider
        m_shape->setBorder( new KoLineBorder( 1, Qt::black ) );
        m_points.clear();
        addPoint( event->point );
    }
}

void KarbonPencilTool::mouseMoveEvent( KoPointerEvent *event )
{
    if( event->buttons() & Qt::LeftButton )
        addPoint( event->point );
}

void KarbonPencilTool::mouseReleaseEvent( KoPointerEvent *event )
{
    addPoint( event->point );
    finish( event->modifiers() & Qt::ShiftModifier );
    delete m_shape;
    m_shape = 0;
    m_points.clear();
}

void KarbonPencilTool::keyPressEvent(QKeyEvent *event)
{
    if ( m_shape ) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

void KarbonPencilTool::activate( bool temporary )
{
    m_points.clear();
    m_close = false;
    useCursor(Qt::ArrowCursor, true);
}

void KarbonPencilTool::deactivate()
{
    m_points.clear();
    delete m_shape;
    m_shape = 0;
}

void KarbonPencilTool::resourceChanged( KoCanvasResource::EnumCanvasResource key, const QVariant & res )
{
}

void KarbonPencilTool::addPoint( const QPointF & point )
{
    if( ! m_shape )
        return;

    // do a moveTo for the first point added
    if( m_points.empty() )
        m_shape->moveTo( point );
    // do not allow coincident points
    else if( point != m_points.last() )
        m_shape->lineTo( point );
    else
        return;

    m_points.append( point );
    m_canvas->updateCanvas( m_shape->boundingRect() );
}

double KarbonPencilTool::lineAngle( const QPointF &p1, const QPointF &p2 )
{
    double angle = atan2( p2.y() - p1.y(), p2.x() - p1.x() );
    if( angle < 0.0 )
        angle += 2*M_PI;

    return angle * 180.0 / M_PI;
}

void KarbonPencilTool::finish( bool closePath )
{
    if( m_points.count() < 2 )
        return;

    KoPathShape * path = 0;
    QList<QPointF> complete;
    QList<QPointF> *points = &m_points;

    if( m_mode == ModeStraight || m_optimizeRaw || m_optimizeCurve )
    {
        float combineAngle;

        if( m_mode == ModeStraight )
            combineAngle = m_combineAngle;
        else
            combineAngle = 0.50f;

        //Add the first two points
        complete.append( m_points[0] );
        complete.append( m_points[1] );

        //Now we need to get the angle of the first line
        float lastAngle = lineAngle( complete[0], complete[1] );

        uint pointCount = m_points.count();
        for( uint i = 2; i < pointCount; ++i )
        {
            float angle = lineAngle( complete.last(), m_points[i] );
            if( qAbs( angle - lastAngle ) < combineAngle )
                complete.removeLast();
            complete.append( m_points[i] );
            lastAngle = angle;
        }

        m_points.clear();
        points = &complete;
    }

    switch( m_mode )
    {
        case ModeCurve:
        {
            path = bezierFit( *points, m_fittingError );
        }
        break;
        case ModeStraight:
        case ModeRaw:
        {
            path = new KoPathShape();
            uint pointCount = points->count();
            path->moveTo( points->at( 0 ) );
            for( uint i = 1; i < pointCount; ++i )
                path->lineTo( points->at(i) );
        }
        break;
    }

    if( ! path )
        return;

    if( closePath )
        path->close();

    // set the proper shape id
    path->setShapeId( KoPathShapeId );
    // TODO take properties from the resource provider
    path->setBorder( new KoLineBorder( 1, Qt::black ) );
    path->normalize();

    QUndoCommand * cmd = m_canvas->shapeController()->addShape( path );
    if( cmd )
    {
        KoSelection *selection = m_canvas->shapeManager()->selection();
        selection->deselectAll();
        selection->select( path );
        m_canvas->addCommand( cmd );
    }
    else
    {
        m_canvas->updateCanvas( path->boundingRect() );
        delete path;
    }
}

QWidget * KarbonPencilTool::createOptionWidget()
{
    QWidget *optionWidget = new QWidget();
    QVBoxLayout * layout = new QVBoxLayout( optionWidget );

    QComboBox * modeBox = new QComboBox( optionWidget );
    modeBox->addItem( i18n( "Raw" ) );
    modeBox->addItem( i18n( "Curve" ) );
    modeBox->addItem( i18n( "Straight" ) );
    layout->addWidget( modeBox );

    QStackedWidget * stackedWidget = new QStackedWidget( optionWidget );

    QGroupBox * rawBox = new QGroupBox( i18n( "Properties" ), stackedWidget );
    QVBoxLayout * rawLayout = new QVBoxLayout( rawBox );
    QCheckBox * optimizeRaw = new QCheckBox( i18n( "Optimize" ), rawBox );
    rawLayout->addWidget( optimizeRaw );

    QGroupBox * curveBox = new QGroupBox( i18n( "Properties" ), stackedWidget );
    QVBoxLayout * curveLayout = new QVBoxLayout( curveBox );
    QCheckBox * optimizeCurve = new QCheckBox( i18n( "Optimize" ), curveBox );
    KDoubleNumInput * fittingError = new KDoubleNumInput( 0.0, 400.0, m_fittingError, curveBox, 0.50, 3 );
    fittingError->setLabel( i18n( "Exactness:" ), Qt::AlignLeft|Qt::AlignVCenter );
    curveLayout->addWidget( optimizeCurve );
    curveLayout->addWidget( fittingError );

    QGroupBox * straightBox = new QGroupBox( i18n( "Properties" ), stackedWidget );
    QVBoxLayout * straightLayout = new QVBoxLayout( straightBox );
    KDoubleNumInput * combineAngle = new KDoubleNumInput( 0.0, 360.0, m_combineAngle, straightBox, 0.50, 3 );
    combineAngle->setSuffix( " deg" );
    combineAngle->setLabel( i18n( "Combine angle:" ), Qt::AlignLeft|Qt::AlignVCenter );
    straightLayout->addWidget( combineAngle );

    stackedWidget->addWidget( rawBox );
    stackedWidget->addWidget( curveBox );
    stackedWidget->addWidget( straightBox );
    layout->addWidget( stackedWidget, 1 );
    layout->addStretch( 1 );

    connect( modeBox, SIGNAL(activated(int)), stackedWidget, SLOT(setCurrentIndex(int)));
    connect( modeBox, SIGNAL(activated(int)), this, SLOT(selectMode(int)));
    connect( optimizeRaw, SIGNAL(stateChanged(int)), this, SLOT(setOptimize(int)));
    connect( optimizeCurve, SIGNAL(stateChanged(int)), this, SLOT(setOptimize(int)));
    connect( fittingError, SIGNAL(valueChanged(double)), this, SLOT(setDelta(double)));
    connect( combineAngle, SIGNAL(valueChanged(double)), this, SLOT(setDelta(double)));

    return optionWidget;
}

void KarbonPencilTool::selectMode( int mode )
{
    m_mode = static_cast<PencilMode>( mode );
}

void KarbonPencilTool::setOptimize( int state )
{
    if( m_mode == ModeRaw )
        m_optimizeRaw = state == Qt::Checked ? true : false;
    else
        m_optimizeCurve = state == Qt::Checked ? true : false;
}

void KarbonPencilTool::setDelta( double delta )
{
    if( m_mode == ModeCurve )
        m_fittingError = delta;
    else if( m_mode == ModeStraight )
        m_combineAngle = delta;
}

#include "KarbonPencilTool.moc"
