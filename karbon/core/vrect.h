#ifndef __VRECT_H__
#define __VRECT_H__

#include "vpoint.h"

/**
 * This class is used for boundingboxes only.
 */

class VRect {
public:
	VRect();
	VRect( const double& l, const double& t, const double& r, const double& b );
	VRect( VPoint& tl, VPoint& br );

private:
	VPoint m_tl;
	VPoint m_br;
};

#endif
