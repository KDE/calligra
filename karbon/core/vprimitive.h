#ifndef __VPRIMITIVE_H__
#define __VPRIMITIVE_H__

#include "vobject.h"
#include "vpoint.h"

class VPrimitive : public VObject {
public:
    VPrimitive();

protected:    
    
private:

};

class VLine : public VPrimitive {
public:
    VLine( VPoint& p0, VPoint& p1 )
	: m_p0(p0), m_p1(p1) {}
	    
private:
    VPoint& m_p0, m_p1;
};

class VBezier : public VPrimitive {
public:
    VBezier( VPoint& p0, VPoint& p1, VPoint& p2, VPoint& p3 )
	: m_p0(p0), m_p1(p1), m_p2(p2), m_p3(p3) {}
	    
private:
    VPoint& m_p0, m_p1, m_p2, m_p3;
};


#endif
