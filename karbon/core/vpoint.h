#ifndef __VPOINT_H__
#define __VPOINT_H__

#include <qpoint.h>

/**
 * A VPoint acts like a vector and is used to describe coordinates.
 * It includes a QPoint for direct use in painting on a QPainter.
 * Before 
 */

// TODO: we really have to check if this inline-function excess for a point-class is a good idea

class VPoint {
public:
    VPoint();
    VPoint( const VPoint& p );
    VPoint( const double& x, const double& y );

    // convert to QPoint and recalculate if necessary:
    const QPoint& getQPoint();

    void moveTo( double& x, double& y ) { m_x = x; m_y = y; m_isDirty=true; }

    const double& x() const { return m_x; }
    void setX( double& x ) { m_x = x; m_isDirty=true; }
    const double& y() const { return m_y; }
    void setY( double& y ) { m_y = y; m_isDirty=true; }

//    VPoint& operator= (const VPoint& p) { return *this; }
    void operator +=( const VPoint& p ) { m_x+=p.m_x; m_y+=p.m_y; m_isDirty=true; }
    void operator -=( const VPoint& p ) { m_x-=p.m_x; m_y-=p.m_y; m_isDirty=true; }

    /**
     * we scale QPoint with fractScale, i.e. we consider fractBits bits
     * of the fraction of each double-coordinate.
     */
    static const char s_fractBits = 12;
    static const unsigned int s_fractScale = 1 << s_fractBits;
    static const double s_fractInvScale; // = 1/s_fractScale

private:
    double m_x;
    double m_y;
    QPoint m_QPoint;	// for painting

    bool m_isDirty;	// need to recalculate QPoint ?
};

#endif
