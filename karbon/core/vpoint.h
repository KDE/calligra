#ifndef __VPOINT_H__
#define __VPOINT_H__

#include <qpoint.h>

/**
 * A VPoint acts like a vector and is used to describe coordinates.
 * It includes a QPoint for direct use in painting on a QPainter.
 * Before 
 */

class VPoint {
public:
    VPoint();
    VPoint( const VPoint& p );
    VPoint( const double& x, const double& y );

    // convert to QPoint and recalculate if necessary:
    const QPoint& getQPoint() const;

    void moveTo( double& x, double& y );
    void rmoveTo( double& x, double& y );

    const double& x() const { return m_x; }
    void setX( double& x );
    const double& y() const { return m_y; }
    void setY( double& y );

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
    mutable QPoint m_QPoint;	// for painting

    mutable bool m_isDirty;	// need to recalculate QPoint ?
};

#endif
