/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSHAPETOOL_H__
#define __VSHAPETOOL_H__

#include <qstring.h>

#include <koPoint.h>

#include "vtool.h"


class VPath;


class VShapeTool : public VTool
{
public:
	VShapeTool( KarbonView* view, const QString& name, bool polar = false );

	const QString& name() const { return m_name; }

protected:
	virtual void showDialog() const {}

	virtual void draw();

	virtual void mouseDrag( const KoPoint& current );
	virtual void mouseDragRelease( const KoPoint& current );
	virtual void mouseDragShiftPressed( const KoPoint& current );
	virtual void mouseDragCtrlPressed( const KoPoint& current );
	virtual void mouseDragShiftReleased( const KoPoint& current );
	virtual void mouseDragCtrlReleased( const KoPoint& current );

	virtual void cancel();

	// Make it "abstract":
	virtual ~VShapeTool() {}

	virtual VPath* shape( bool decide = false ) const = 0;

	/// Output coordinates.
	KoPoint m_p;
	double m_d1;
	double m_d2;

private:
	void recalc();

	QString m_name;
	
	/// Calculate wiidth/height or radius/angle?
	bool m_isPolar;

	/// States:
	bool m_isSquare;
	bool m_isCentered;
};

#endif
