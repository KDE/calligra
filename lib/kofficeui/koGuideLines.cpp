// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright ( C ) 2005 Thorsten Zachmann <zachmann@kde.org>
   based on work done by Dave Marotti <landshark_666@hotmail.com>
*/

#include "koGuideLines.h"
#include <qpainter.h>
#include <qbitmap.h>
#include <koDocument.h>
#include <koView.h>

QPixmap * KoGuideLines::m_pattern = 0;
QPixmap * KoGuideLines::m_selectedPattern = 0;

KoGuideLineData::KoGuideLineData( Qt::Orientation o, double pos, const QSize &size )
: m_orient( o )
, m_pos( pos )    
, m_hasBuffer( false )    
, m_selected( true )    
{
    resize( size );
}

KoGuideLineData::~KoGuideLineData()
{
}
        
void KoGuideLineData::resize( const QSize &size )
{
    int width = 1;
    int height = 1;
    
    if ( m_orient == Qt::Vertical )
    {
        height = size.height();
    }
    else
    {
        width = size.width();
    }

    m_buffer.resize( width, height );
    m_hasBuffer = false;
}

KoGuideLines::KoGuideLines( KoView *view, KoZoomHandler *zoomHandler, QPixmap *buffer )
: m_view( view )    
, m_buffer( buffer )    
, m_zoomHandler( zoomHandler )    
, m_mouseSelected( false )
, m_insertGuide( false )
{
    m_guides.setAutoDelete( true );
    if ( !m_pattern ) 
    {
        static const char* const image_data[] = 
        {
            "15 1 2 1",
            "  c None",
            "# c blue",
            "## ## ## ## ## "
        };
        m_pattern = new QPixmap( (const char**)image_data );
        
        static const char* const image_data_selected[] = 
        {
            "15 1 2 1",
            "  c None",
            "# c red",
            "## ## ## ## ## "
        };
        m_selectedPattern = new QPixmap( (const char**)image_data_selected );
    }
}


KoGuideLines::~KoGuideLines()
{
}


bool KoGuideLines::mousePressEvent( QMouseEvent* e )
{
    //kdDebug(33001) << "KoGuideLines::mousePressEvent" << endl;
    bool eventProcessed = true;
    bool changed = false;
    m_mouseSelected = false;
    
    KoPoint p( mapFromScreen( e->pos() ) );
    KoGuideLineData * gd = find( p.x(), p.y(), m_zoomHandler->unzoomItY(2) );
    if ( gd )
    {
        m_lastPoint = e->pos();
        if ( e->button() == Qt::LeftButton )
        {
            m_mouseSelected = true;
            if ( e->state() & Qt::ControlButton )
            {
                if ( gd->isSelected() )
                {
                    unselect( gd );
                    m_mouseSelected = false;
                }
                else
                {
                    select( gd );
                }
                changed = true;
            }
            else if ( ! gd->isSelected() )
            {
                unselectAll();
                select( gd );
                changed = true;
            }
        }
    }
    else 
    {   
        if ( !( e->state() && Qt::ControlButton ) )
        {
            changed = unselectAll();
        }
        eventProcessed = false;
    }
    
    if ( changed )
    {
        erase();
        paint();
    }
    
    return eventProcessed;
}


bool KoGuideLines::mouseMoveEvent( QMouseEvent* e )
{
    //kdDebug(33001) << "KoGuideLines::mouseMoveEvent" << endl;
    bool eventProcessed = false;
    if ( m_mouseSelected )
    {
        erase();
        QPoint p( e->pos() );
        p -= m_lastPoint;
        m_lastPoint = e->pos();
        moveSelectedBy( p );
        paint();
        eventProcessed = true;
    }
    else if ( e->button() == Qt::NoButton )
    {
        KoPoint p( mapFromScreen( e->pos() ) );
        KoGuideLineData * gd = find( p.x(), p.y(), m_zoomHandler->unzoomItY(2) );
        if ( gd )
        {
            m_view->canvas()->setCursor( gd->orientation() == Qt::Vertical ? Qt::sizeHorCursor : Qt::sizeVerCursor );
            eventProcessed = true;
        }
    }
    return eventProcessed;
}


bool KoGuideLines::mouseReleaseEvent( QMouseEvent* e )
{
    //kdDebug(33001) << "KoGuideLines::mouseReleaseEvent" << endl;
    bool eventProcessed = false;
    if ( m_mouseSelected )
    {
        KoPoint p( mapFromScreen( e->pos() ) );
        if ( m_selectedGuides.count() == 1 )
        {
            int x1, y1, x2, y2;
            m_view->canvas()->rect().coords( &x1, &y1, &x2, &y2 );
            QPoint gp( m_view->canvas()->mapFromGlobal( e->globalPos() ) );
            if ( m_selectedGuides.first()->orientation() == Qt::Vertical )
            {
                if ( gp.x() < x1 || gp.x() > x2 )
                    removeSelected();
            }
            else
            {
                if ( gp.y() < y1 || gp.y() > y2 )
                    removeSelected();
            }
        }
        KoGuideLineData * gd = find( p.x(), p.y(), m_zoomHandler->unzoomItY(2) );
        if ( gd )
        {
            m_view->canvas()->setCursor( gd->orientation() == Qt::Vertical ? Qt::sizeHorCursor : Qt::sizeVerCursor );
        }
        m_mouseSelected = false;
        eventProcessed = true;
    }
    return eventProcessed;
}


bool KoGuideLines::keyPressEvent( QKeyEvent *e )
{
    //kdDebug( 33001 ) << "KoGuideLines::keyPressEvent" << endl;
    bool eventProcessed = false;
    switch( e->key() )
    {
        case Qt::Key_Delete: 
            if ( hasSelected() )
            {
                removeSelected();
                eventProcessed = true;
            }
            break;
        default:
            break;
    }
    return eventProcessed;
}


void KoGuideLines::resize()
{
    const QSize &size( m_view->canvas()->size() );
    resizeLinesPixmap( size, m_vGuideLine, m_hGuideLine, *m_pattern);
    resizeLinesPixmap( size, m_selectedVGuideLine, m_selectedHGuideLine, *m_selectedPattern );
    for ( KoGuideLineData *gd = m_guides.first(); gd; gd = m_guides.next() )
    {
        gd->resize( size );
    }
}


void KoGuideLines::erase( bool updateCanvas )
{
    QSize size( m_view->canvas()->size() );
    if ( m_buffer == 0 && !updateCanvas )
    {
        return;
    }
    for ( KoGuideLineData *gd = m_selectedGuides.last(); gd; gd = m_selectedGuides.prev() ) 
    {
        eraseGuide( gd, size, updateCanvas );
    }
    for ( KoGuideLineData *gd = m_guides.last(); gd; gd = m_guides.prev() ) 
    {
        eraseGuide( gd, size, updateCanvas );
    }
}


void KoGuideLines::eraseGuide( KoGuideLineData *gd, const QSize &size, bool updateCanvas )
{
    if ( gd->hasBuffer() )
    {
        if ( gd->orientation() == Qt::Vertical )
        {
            KoPoint p( gd->position(), 0 );
            int x = mapToScreen( p ).x();
            if ( x >= 0 && x < size.width() ) 
            {
                if ( m_buffer )
                {
                    bitBlt( m_buffer, x, 0, gd->buffer() );
                }
                if ( updateCanvas )
                {
                    bitBlt( m_view->canvas(), x, 0, gd->buffer() );
                }
            }
        } 
        else 
        {
            KoPoint p( 0, gd->position() );
            int y = mapToScreen( p ).y();
            if ( y >= 0 && y < size.height() ) 
            {
                if ( m_buffer )
                {
                    bitBlt( m_buffer, 0, y, gd->buffer() );
                }
                if ( updateCanvas )
                {
                    bitBlt( m_view->canvas(), 0, y, gd->buffer() );
                }
            }
        }
    }
    gd->setHasBuffer( false );
}


void KoGuideLines::paint( bool updateCanvas )
{
    if ( m_buffer == 0 && !updateCanvas )
    {
        return;
    }
    int d = m_pattern->width();
    int dx = m_view->canvasXOffset() % d;
    int dy = m_view->canvasYOffset() % d;
    if ( m_view->canvasXOffset() < 0 ) {
        dx += d;
    }
    if ( m_view->canvasYOffset() < 0 ) {
        dy += d;
    }
    QSize size( m_view->canvas()->size() );
    // paint selected guides after the unselect once so that they are in front
    for ( KoGuideLineData *gd = m_guides.first(); gd; gd = m_guides.next() ) 
    {
        if ( ! gd->isSelected() )
        {
            paintGuide( gd, size, updateCanvas, dx, dy );
        }
    }
    for ( KoGuideLineData *gd = m_selectedGuides.first(); gd; gd = m_selectedGuides.next() ) 
    {
        paintGuide( gd, size, updateCanvas, dx, dy );
    }
}


void KoGuideLines::paintGuide( KoGuideLineData *gd, const QSize &size, bool updateCanvas, int dx, int dy )
{
    if ( gd->orientation() == Qt::Vertical ) 
    {
        KoPoint p( gd->position(), 0 );
        int x = mapToScreen( p ).x();
        if ( x >= 0 && x < size.width() ) 
        {
            if ( m_buffer )
            {
                bitBlt( gd->buffer(), 0, 0, m_buffer, x, 0, 1, gd->buffer()->height() );
                bitBlt( m_buffer, x, 0, gd->isSelected() ? &m_selectedVGuideLine : &m_vGuideLine, 0, dy, 1, size.height() );
            }
            if ( updateCanvas )
            {
                bitBlt( gd->buffer(), 0, 0, m_view->canvas(), x, 0, 1, gd->buffer()->height() );
                bitBlt( m_view->canvas(), x, 0, gd->isSelected() ? &m_selectedVGuideLine : &m_vGuideLine, 0, dy, 1, size.height() );
            }
            gd->setHasBuffer( true );
        } 
        else 
        {
            gd->setHasBuffer( false );
        }
    } 
    else 
    {
        KoPoint p( 0, gd->position() );
        int y = mapToScreen( p ).y();
        if ( y >= 0 && y < size.height() ) 
        {
            if ( m_buffer )
            {
                bitBlt( gd->buffer(), 0, 0, m_buffer, 0, y, gd->buffer()->width(), 1 );
                bitBlt( m_buffer, 0, y, gd->isSelected() ? &m_selectedHGuideLine : &m_hGuideLine, dx, 0, size.width(), 1 );
            }
            if ( updateCanvas )
            {
                bitBlt( gd->buffer(), 0, 0, m_view->canvas(), 0, y, gd->buffer()->width(), 1 );
                bitBlt( m_view->canvas(), 0, y, gd->isSelected() ? &m_selectedHGuideLine : &m_hGuideLine, dx, 0, size.width(), 1 );
            }
            gd->setHasBuffer( true );
        } 
        else 
        {
            gd->setHasBuffer( false );
        }
    }
}


void KoGuideLines::add( QPoint &pos, Qt::Orientation o )
{
    KoPoint p( mapFromScreen( pos ) );
    erase();
    unselectAll();
    KoGuideLineData *gd = new KoGuideLineData( o, o == Qt::Vertical ? p.x(): p.y(), m_view->canvas()->size() );
    m_guides.append( gd );
    m_selectedGuides.append( gd );
    paint();
}


void KoGuideLines::moveSelectedBy( QPoint &p )
{
    KoPoint point( m_zoomHandler->unzoomPoint( p ) );
    if ( m_selectedGuides.count() > 1 )
    {
        const KoPageLayout& pl = m_view->koDocument()->pageLayout();
        double right = QMAX( pl.ptWidth, m_zoomHandler->unzoomItX( m_view->canvas()->size().width() + m_view->canvasXOffset() - 1 ) );
        double bottom = QMAX( pl.ptHeight, m_zoomHandler->unzoomItY( m_view->canvas()->size().height() + m_view->canvasYOffset() - 1 ) );
        for ( KoGuideLineData *gd = m_selectedGuides.first(); gd; gd = m_selectedGuides.next() )
        {
            if ( gd->orientation() == Qt::Vertical )
            {
                double tmp = gd->position() + point.x();
                if ( tmp < 0 )
                {
                    point.setX( point.x() - tmp );
                }
                else if ( tmp > right )
                {
                    point.setX( point.x() - ( tmp - right ) );
                }
            }
            else
            {
                double tmp = gd->position() + point.y();
                if ( tmp < 0 )
                {
                    point.setY( point.y() - tmp );
                }
                else if ( tmp > bottom )
                {
                    point.setY( point.y() - ( tmp - bottom ) );
                }
            }
        }
    }
    for ( KoGuideLineData *gd = m_selectedGuides.first(); gd; gd = m_selectedGuides.next() )
    {
        if ( gd->orientation() == Qt::Vertical && p.x() != 0 )
        {
            gd->setPosition( gd->position() + point.x() );
        }
        else if ( gd->orientation() == Qt::Horizontal && p.y() != 0 )
        {
            gd->setPosition( gd->position() + point.y() );
        }
    }
}


KoGuideLineData * KoGuideLines::find( double x, double y, double diff )
{
    for ( KoGuideLineData *gd = m_guides.first(); gd; gd = m_guides.next() )
    {
        if ( gd->orientation() == Qt::Horizontal && QABS( gd->position() - y ) < diff )
        {
            return gd;
        }
        if ( gd->orientation() == Qt::Vertical && QABS( gd->position() - x ) < diff )
        {
            return gd;
        }
    }    
    return 0;
}


// erase/paint
void KoGuideLines::removeSelected()
{
    erase();
    for ( KoGuideLineData *gd = m_selectedGuides.first(); gd; gd = m_selectedGuides.next() )
    {
        m_guides.remove( gd );
    }
    m_selectedGuides.clear();
    paint();
}


void KoGuideLines::select( KoGuideLineData *gd )
{
    gd->setSelected( true );
    m_selectedGuides.append( gd );
}


void KoGuideLines::unselect( KoGuideLineData *gd )
{
    gd->setSelected( false );
    m_selectedGuides.remove( gd );
}


bool KoGuideLines::unselectAll()
{
    bool selected = m_selectedGuides.count() > 0;
    
    for ( KoGuideLineData *gd = m_selectedGuides.first(); gd; gd = m_selectedGuides.next() )
    {
        gd->setSelected( false );
    }
    m_selectedGuides.clear();
    
    return selected;
}


bool KoGuideLines::hasSelected()
{
    return m_selectedGuides.count() > 0;
}


void KoGuideLines::moveGuide( const QPoint &pos, bool horizontal, int rulerWidth )
{
    int x = pos.x() - rulerWidth;
    int y = pos.y() - rulerWidth;
    QPoint p( x, y );
    if ( !m_insertGuide )
    {
        if ( ! horizontal && x > 0 )
        {
            m_insertGuide = true;
            add( p, Qt::Vertical );
        }
        else if ( horizontal && y > 0 )
        {
            m_insertGuide = true;
            add( p, Qt::Horizontal );
        }
        if ( m_insertGuide )
        {
            QMouseEvent e( QEvent::MouseButtonPress, p, Qt::LeftButton, Qt::LeftButton );
            mousePressEvent( &e );
        }
    }
    else
    {
        QMouseEvent e( QEvent::MouseMove, p, Qt::NoButton, Qt::LeftButton );
        mouseMoveEvent( &e );
    }
}


void KoGuideLines::addGuide( const QPoint &pos, bool /*horizontal*/, int rulerWidth )
{
    int x = pos.x() - rulerWidth;
    int y = pos.y() - rulerWidth;
    QPoint p( x, y );
    m_insertGuide = false;
    QMouseEvent e( QEvent::MouseButtonRelease, p, Qt::LeftButton, Qt::LeftButton );
    mouseReleaseEvent( &e );
}


KoPoint KoGuideLines::mapFromScreen( const QPoint & pos )
{
    int x = pos.x() + m_view->canvasXOffset();
    int y = pos.y() + m_view->canvasYOffset();
    double xf = m_zoomHandler->unzoomItX( x );
    double yf = m_zoomHandler->unzoomItY( y );
    //kdDebug( 33001 ) << "KoGuideLines::mapFromScreen pos = " << pos << ", xf = " << xf << ", yf = " << yf << endl;
    return KoPoint( xf, yf );
}


QPoint KoGuideLines::mapToScreen( const KoPoint & pos )
{
    int x = m_zoomHandler->zoomItX( pos.x() ) - m_view->canvasXOffset();
    int y = m_zoomHandler->zoomItY( pos.y() ) - m_view->canvasYOffset();
    return QPoint( x, y );
}


void KoGuideLines::resizeLinesPixmap( const QSize &s, QPixmap &vLine, QPixmap &hLine, const QPixmap &linePattern )
{
    int d = linePattern.width();
    hLine.resize( s.width() + d, 1 );
    vLine.resize( 1, s.height() + d );
    QPainter p;
    p.begin( &hLine );
    p.drawTiledPixmap( 0, 0, hLine.width(), 1, linePattern );
    p.end();
    QBitmap hmask( hLine.size() );
    p.begin( &hmask );
    p.drawTiledPixmap( 0, 0, hmask.width(), 1, *( linePattern.mask() ) );
    p.end();
    hLine.setMask( hmask );
    QWMatrix m;
    m.rotate( 90 );
    QPixmap rpattern( linePattern.xForm(m) );
    p.begin( &vLine );
    p.drawTiledPixmap( 0, 0, 1, vLine.height(), rpattern );
    p.end();
    QBitmap vmask( vLine.size() );
    p.begin( &vmask );
    p.drawTiledPixmap( 0, 0, 1, vmask.height(), *( rpattern.mask() ) );
    p.end();
    vLine.setMask( vmask );
}


#include "koGuideLines.moc"
