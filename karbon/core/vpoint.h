#ifndef __VPOINT_H__
#define __VPOINT_H__

#include "vobject.h"

class VPoint {
public:
    VPoint()
	: m_x(0.0), m_y(0.0), m_refCount(0) {}
    VPoint( const VPoint& p )
	: m_x(p.m_x), m_y(p.m_y), m_refCount(0) {}
    VPoint( const double& x, const double& y )
	: m_x(x), m_y(y), m_refCount(0) {}

    void ref() { ++m_refCount; }
    void unref() { --m_refCount; }
	
    double x() const { return m_x; }
    void setX( double& x ) { m_x = x; }
    double y() const { return m_y; }
    void setY( double& y ) { m_y = y; }

//    VPoint& operator= (const VPoint& p) { return *this; }
    void operator +=( const VPoint& p ) { m_x+=p.m_x; m_y+=p.m_y; }
    void operator -=( const VPoint& p ) { m_x-=p.m_x; m_y-=p.m_y; }
        
private:
    double m_x;
    double m_y;
    unsigned int m_refCount;
    
    // m_referers
};

#endif
