#include "koRect.h"

KoRect KoRect::normalize() const
{
    KoRect r;
    if ( right() < left() ) {                            // swap bad x values
        r.m_tl.setX( right() );
        r.m_br.setX( left() );
    } else {
        r.m_tl.setX( left() );
        r.m_br.setX( right() );
    }
    if ( bottom() < top() ) {                            // swap bad y values
        r.m_tl.setY( bottom() );
        r.m_br.setY( top() );
    } else {
        r.m_tl.setY( top() );
        r.m_br.setY( bottom() );
    }
    return r;
}

void KoRect::moveTopLeft(const KoPoint &topleft)
{
    m_br.rx() += topleft.x() - m_tl.x();
    m_br.ry() += topleft.y() - m_tl.y();
    m_tl = topleft;
}

void KoRect::moveBottomRight(const KoPoint &bottomright)
{
    m_tl.rx() += bottomright.x() - m_br.x();
    m_tl.ry() += bottomright.y() - m_br.y();
    m_br = bottomright;
}

void KoRect::moveTopRight(const KoPoint &topright)
{
    m_tl.rx() += topright.x() - m_br.x();
    m_br.ry() += topright.y() - m_tl.y();
    m_br.rx() = topright.x();
    m_tl.ry() = topright.y();
}

void KoRect::moveBottomLeft(const KoPoint &bottomleft)
{
    m_br.rx() += bottomleft.x() - m_tl.x();
    m_tl.ry() += bottomleft.y() - m_br.y();
    m_tl.rx() = bottomleft.x();
    m_br.ry() = bottomleft.y();
}

void KoRect::moveBy(const double &dx, const double &dy)
{
    m_tl.rx() += dx;
    m_tl.ry() += dy;
    m_br.rx() += dx;
    m_br.ry() += dy;
}

void KoRect::setRect(const double &x, const double &y, const double &width, const double &height)
{
    m_tl.setCoords( x, y );
    m_br.setCoords( x + width, y + height );
}

void KoRect::setCoords(const double &x1, const double &y1, const double &x2, const double &y2)
{
    m_tl.setCoords( x1, y1 );
    m_br.setCoords( x2, y2 );
}

