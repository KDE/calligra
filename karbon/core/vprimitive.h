#ifndef __VPRIMITIVE_H__
#define __VPRIMITIVE_H__

#include "vpoint.h"


class VPrimitive {
public:

protected:
    // protected ctor to forbid instantiation:
    VPrimitive( VPoint* fp, VPoint* lp )
	: m_firstPoint(fp), m_lastPoint(lp) {}
    
private:
    VPoint* m_firstPoint;
    VPoint* m_lastPoint;

};


class VLine : public VPrimitive {
public:
    VLine( VPoint* fp, VPoint* lp )
	: VPrimitive(fp,lp) {}
	    
};


class VBezier : public VPrimitive {
public:
    VBezier( VPoint* fp, VPoint* fcp, VPoint* lcp, VPoint* lp )
	: VPrimitive(fp,lp), m_firstCtrlPoint(fcp), m_lastCtrlPoint(lcp) {}
	    
private:
    VPoint* m_firstCtrlPoint;
    VPoint* m_lastCtrlPoint;
};


#endif
