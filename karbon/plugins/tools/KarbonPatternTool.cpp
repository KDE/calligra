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

#include "KarbonPatternTool.h"

#include <karbon_factory.h>
#include <karbon_resourceserver.h>
#include <core/vpattern.h>

#include <KoResourceChooser.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoShape.h>
#include <KoShapeBackgroundCommand.h>
#include <KoPointerEvent.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kfiledialog.h>

#include <QPainter>
#include <QWidget>
#include <QGridLayout>
#include <QButtonGroup>
#include <QToolButton>
#include <QFileInfo>
#include <QUndoCommand>

#include <Q3PtrList>

/// The class used for editing a shapes pattern
class KarbonPatternTool::PatternStrategy
{
public:
    PatternStrategy( KoShape * shape )
    : m_shape( shape ),m_selectedHandle( -1 ), m_editing( false )
    {
        // cache the shapes transformation matrix
        m_matrix = m_shape->transformationMatrix( 0 );
        QSizeF size = m_shape->size();
        // the fixed length of half the average shape dimension
        m_normalizedLength = 0.25 * ( size.width() + size.height() );
        // the center handle at the center point of the shape
        QPointF center( 0.5 * size.width(), 0.5 * size.height() );
        // the direction handle with the length of half the average shape dimension
        QPointF dirVec = QPointF( m_normalizedLength, 0.0 );
        m_handles.append( center );
        m_handles.append( center + m_shape->background().matrix().map( dirVec ) );
    }

    ~PatternStrategy() {}

    /// painting of the pattern editing handles
    void paint( QPainter &painter, KoViewConverter &converter )
    {
        QPointF centerPoint = m_matrix.map( m_handles[center] );
        QPointF directionPoint = m_matrix.map( m_handles[direction] );

        m_shape->applyConversion( painter, converter );
        painter.drawLine( centerPoint, directionPoint );
        //paintHandle( painter, converter, centerPoint );
        paintHandle( painter, converter, directionPoint );
    }

    void paintHandle( QPainter &painter, KoViewConverter &converter, const QPointF &position )
    {
        QRectF handleRect = converter.viewToDocument( QRectF( m_handleRadius, m_handleRadius, 2*m_handleRadius, 2*m_handleRadius ) );
        handleRect.moveCenter( position );
        painter.drawRect( handleRect );
    }

    bool mouseInsideHandle( const QPointF &mousePos, const QPointF &handlePos )
    {
        QPointF handle = m_matrix.map( handlePos );
        if( mousePos.x() < handle.x()-m_handleRadius )
            return false;
        if( mousePos.x() > handle.x()+m_handleRadius )
            return false;
        if( mousePos.y() < handle.y()-m_handleRadius )
            return false;
        if( mousePos.y() > handle.y()+m_handleRadius )
            return false;
        return true;
    }

    /// selects handle at the given position
    bool selectHandle( const QPointF &mousePos )
    {
        int handleIndex = 0;
        foreach( QPointF handle, m_handles )
        {
            if( mouseInsideHandle( mousePos, handle ) )
            {
                m_selectedHandle = handleIndex;
                return true;
            }
            handleIndex++;
        }
        m_selectedHandle = -1;
        return false;
    }

    /// mouse position handling for moving handles
    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers)
    {
        Q_UNUSED( modifiers )

        if( m_selectedHandle == direction )
        {
            QPointF newPos = m_matrix.inverted().map( mouseLocation ) - m_handles[center];
            // calculate the temporary length after handle movement
            double newLength = sqrt( newPos.x()*newPos.x() + newPos.y()*newPos.y() );
            // set the new direction vector with the new direction and normalized length
            m_handles[m_selectedHandle] = m_handles[center] + m_normalizedLength / newLength * newPos;
        }

        m_newBackground = background();
        m_shape->setBackground( m_newBackground );
    }

    /// sets the strategy into editing mode
    void setEditing( bool on )
    {
        m_editing = on;
        // if we are going into editing mode, save the old background
        // for use inside the command emitted when finished
        if( on )
            m_oldBackground = m_shape->background();
    }

    /// checks if strategy is in editing mode
    bool isEditing() { return m_editing; }

    /// create the command for changing the shapes background
    QUndoCommand * createCommand()
    {
        m_shape->setBackground( m_oldBackground );
        QList<KoShape*> shapes;
        return new KoShapeBackgroundCommand( shapes << m_shape, m_newBackground, 0 );
    }

    /// schedules a repaint of the shape and gradient handles
    void repaint() const
    {
        m_shape->repaint();
    }

    /// returns the pattern handles bounding rect
    QRectF boundingRect()
    {
        // calculate the bounding rect of the handles
        QRectF bbox( m_matrix.map( m_handles[0] ), QSize(0,0) );
        for( int i = 1; i < m_handles.count(); ++i )
        {
            QPointF handle = m_matrix.map( m_handles[i] );
            bbox.setLeft( qMin( handle.x(), bbox.left() ) );
            bbox.setRight( qMax( handle.x(), bbox.right() ) );
            bbox.setTop( qMin( handle.y(), bbox.top() ) );
            bbox.setBottom( qMax( handle.y(), bbox.bottom() ) );
        }
        return bbox.adjusted( -m_handleRadius, -m_handleRadius, m_handleRadius, m_handleRadius );
    }

    QBrush background()
    {
        // the direction vector controls the rotation of the pattern
        QPointF dirVec = m_handles[direction]-m_handles[center];
        double angle = atan2( dirVec.y(), dirVec.x() ) * 180.0 / M_PI;
        QMatrix matrix;
        matrix.rotate( angle );

        QBrush newBrush( m_oldBackground );
        newBrush.setMatrix( matrix );
        return newBrush;
    }

    /// sets the handle radius used for painting the handles
    static void setHandleRadius( int radius ) { m_handleRadius = radius; }

    /// returns the actual handle radius
    static int handleRadius() { return m_handleRadius; }

private:
    enum Handles { center, direction };

    KoShape *m_shape;          ///< the shape we are working on
    int m_selectedHandle;      ///< index of currently deleted handle or -1 if none selected
    QBrush m_oldBackground;    ///< the old background brush
    QBrush m_newBackground;    ///< the new background brush
    QList<QPointF> m_handles;  ///< the list of handles
    QMatrix m_matrix;          ///< matrix to map handle into document coordinate system
    static int m_handleRadius; ///< the handle radius for all gradient strategies
    bool m_editing;            ///< the edit mode flag
    double m_normalizedLength; ///< the normalized direction vector length
};

int KarbonPatternTool::PatternStrategy::m_handleRadius = 3;



KarbonPatternTool::KarbonPatternTool(KoCanvasBase *canvas)
: KoTool( canvas ), m_patternChooser( 0 ), m_buttonGroup( 0 )
, m_pattern( 0 ), m_currentStrategy( 0 )
{
}

KarbonPatternTool::~KarbonPatternTool()
{
}

void KarbonPatternTool::paint( QPainter &painter, KoViewConverter &converter )
{
    painter.setBrush( Qt::green ); //TODO make configurable
    painter.setPen( Qt::blue ); //TODO make configurable

    // paint all the strategies
    foreach( PatternStrategy *strategy, m_patterns )
    {
        painter.save();
        strategy->paint( painter, converter );
        painter.restore();
    }

    // paint selected strategy with another color
    if( m_currentStrategy )
    {
        painter.setBrush( Qt::red ); //TODO make configurable
        m_currentStrategy->paint( painter, converter );
    }
}

void KarbonPatternTool::repaintDecorations()
{
    foreach( PatternStrategy *strategy, m_patterns )
        m_canvas->updateCanvas( strategy->boundingRect() );
}

void KarbonPatternTool::mousePressEvent( KoPointerEvent *event )
{
    Q_UNUSED( event )
    if( m_currentStrategy )
        m_currentStrategy->setEditing( true );
}

void KarbonPatternTool::mouseMoveEvent( KoPointerEvent *event )
{
    if( m_currentStrategy )
    {
        m_currentStrategy->repaint();
        if( m_currentStrategy->isEditing() )
        {
            m_currentStrategy->handleMouseMove( event->point, event->modifiers() );
            m_currentStrategy->repaint();
            return;
        }
    }
    foreach( PatternStrategy *strategy, m_patterns )
    {
        if( strategy->selectHandle( event->point ) )
        {
            m_currentStrategy = strategy;
            m_currentStrategy->repaint();
            useCursor(Qt::SizeAllCursor);
            return;
        }
    }
    m_currentStrategy = 0;
    useCursor(Qt::ArrowCursor);
}

void KarbonPatternTool::mouseReleaseEvent( KoPointerEvent *event )
{
    Q_UNUSED( event )
    // if we are editing, get out of edit mode and add a command to the stack
    if( m_currentStrategy )
    {
        m_currentStrategy->setEditing( false );
        m_canvas->addCommand( m_currentStrategy->createCommand() );
    }
}

void KarbonPatternTool::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_I:
        {
            uint handleRadius = PatternStrategy::handleRadius();
            if(event->modifiers() & Qt::ControlModifier)
                handleRadius--;
            else
                handleRadius++;
            m_canvas->resourceProvider()->setHandleRadius( handleRadius );
        }
        break;
        default:
            event->ignore();
            return;
    }
    event->accept();
}

void KarbonPatternTool::initialize()
{
    m_currentStrategy = 0;

    foreach( PatternStrategy* strategy, m_patterns )
    {
        strategy->repaint();
        delete strategy;
    }
    m_patterns.clear();

    foreach( KoShape *shape, m_canvas->shapeManager()->selection()->selectedShapes() )
    {
        const QBrush &background = shape->background();
        if( background.style() == Qt::TexturePattern )
        {
            m_patterns.append( new PatternStrategy( shape ) );
            m_patterns.last()->repaint();
        }
    }
}

void KarbonPatternTool::activate( bool temporary )
{
    Q_UNUSED(temporary);
    if( ! m_canvas->shapeManager()->selection()->count() )
    {
        emit sigDone();
        return;
    }

    initialize();

    PatternStrategy::setHandleRadius( m_canvas->resourceProvider()->handleRadius() );

    useCursor(Qt::ArrowCursor, true);
}

void KarbonPatternTool::deactivate()
{
    foreach( PatternStrategy* strategy, m_patterns )
    {
        strategy->repaint();
        delete strategy;
    }
    m_patterns.clear();
    foreach( KoShape *shape, m_canvas->shapeManager()->selection()->selectedShapes() )
        shape->repaint();
}

void KarbonPatternTool::resourceChanged( KoCanvasResource::EnumCanvasResource key, const QVariant & res )
{
    switch( key )
    {
        case KoCanvasResource::HandleRadius:
            foreach( PatternStrategy *strategy, m_patterns )
                strategy->repaint();
            PatternStrategy::setHandleRadius( res.toUInt() );
            foreach( PatternStrategy *strategy, m_patterns )
                strategy->repaint();
        break;
        default:
            return;
    }
}

QWidget * KarbonPatternTool::createOptionWidget()
{
    QWidget *optionWidget = new QWidget();
    QGridLayout* layout = new QGridLayout( optionWidget );

    m_patternChooser = new KoPatternChooser( KarbonFactory::rServer()->patterns(), optionWidget );
    //m_patternChooser->setFixedSize( 180, 120 );
    layout->addWidget( m_patternChooser, 0, 0, 1, 3 );

    m_buttonGroup = new QButtonGroup( optionWidget );
    m_buttonGroup->setExclusive( false );

    QToolButton *button = new QToolButton( optionWidget );
    button->setIcon( SmallIcon( "14_layer_newlayer" ) );
    button->setText( i18n( "Import" ) );
    button->setToolTip( i18n("Import pattern") );
    button->setEnabled( true );
    m_buttonGroup->addButton( button, Button_Import );
    layout->addWidget( button, 1, 0 );

    button = new QToolButton( optionWidget );
    button->setIcon( SmallIcon( "14_layer_deletelayer" ) );
    button->setText( i18n( "Delete" ) );
    button->setToolTip( i18n("Delete pattern") );
    button->setEnabled( false );
    m_buttonGroup->addButton( button, Button_Remove );
    layout->addWidget( button, 1, 1 );

    layout->setColumnStretch( 3, 1 );

    connect( m_buttonGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( slotButtonClicked( int ) ) );
    connect( m_patternChooser, SIGNAL( selected( QTableWidgetItem* ) ), this, SLOT( patternSelected( QTableWidgetItem* ) ) );

    m_pattern = (VPattern*)KarbonFactory::rServer()->patterns().first();

    return optionWidget;
}

void KarbonPatternTool::slotButtonClicked( int button )
{
    if( button == Button_Import )
        importPattern();
    else if( button == Button_Remove )
        deletePattern();
}

void KarbonPatternTool::patternSelected( QTableWidgetItem* item )
{
    m_pattern = dynamic_cast<VPattern*>( item );
    if( ! m_pattern || ! m_pattern->isValid() )
        return;

    QAbstractButton * removeButton = m_buttonGroup->button( Button_Remove );
    if( removeButton )
        removeButton->setEnabled( QFileInfo( m_pattern->tilename() ).isWritable() );

    QList<KoShape*> selectedShapes = m_canvas->shapeManager()->selection()->selectedShapes();
    QBrush newBrush( m_pattern->pixmap() );
    m_canvas->addCommand( new KoShapeBackgroundCommand( selectedShapes, newBrush ) );
    initialize();
}

void KarbonPatternTool::importPattern()
{
    QString filter( "*.jpg *.gif *.png *.tif *.xpm *.bmp" );
    VPattern* pattern = KarbonFactory::rServer()->addPattern(
        KFileDialog::getOpenFileName( KUrl(), filter, 0, i18n( "Choose Pattern to Add" ) ) );
    if( pattern )
        m_patternChooser->addPattern( pattern );
}

void KarbonPatternTool::deletePattern()
{
    KarbonFactory::rServer()->removePattern( m_pattern );
    m_pattern = static_cast<VPattern*>( m_patternChooser->currentPattern() );
}

#include "KarbonPatternTool.moc"
