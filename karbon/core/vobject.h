#ifndef __VOBJECT_H__
#define __VOBJECT_H__

#include "vrect.h"

class QPainter;

class VObject {
public:
	VObject();

	virtual void draw( QPainter& p, const QRect& rect, const double& zoomFactor ) = 0;

	virtual void translate( const double& dx, const double& dy ) = 0;
	//virtual void rotate( const double& deg ) = 0;
	//virtual void scale( const double& dx, const double& dy ) = 0;

private:
	VRect m_boundingBox;
};

#endif
