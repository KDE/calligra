/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSHEARTOOL_H__
#define __VSHEARTOOL_H__

#include "vtool.h"
#include "vselection.h"


class VShearTool : public VTool
{
public:
	VShearTool( KarbonView* view );
	virtual ~VShearTool();

	virtual void activate();

protected:
	virtual void draw();

	virtual void setCursor( const KoPoint& current ) const;
	virtual void mouseButtonPress( const KoPoint& current );
	virtual void mouseDrag( const KoPoint& current );
	virtual void mouseDragRelease( const KoPoint& current );

private:
	void recalc();

	double m_s1, m_s2;

	VHandleNode m_activeNode;

	// A list of temporary objects:
	VObjectList m_objects;
};

#endif

