#ifndef __VOBJECT_H__
#define __VOBJECT_H__

#include <qrect.h>

class QPainter;

class VObject {
public:
	VObject();

	virtual void draw( QPainter& p, const QRect& rect, const double& zoomFactor ) = 0;

	virtual void translate( const double& dx, const double& dy ) = 0;
	//virtual void rotate( const double& deg ) = 0;
	//virtual void scale( const double& dx, const double& dy ) = 0;

	// QRect as boundingBox is sufficent since it's not used for calculating intersections
	virtual const QRect& boundingBox() const = 0;

protected:
	QRect m_boundingBox;
};

#endif
