/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSELECTNODESTOOL_H__
#define __VSELECTNODESTOOL_H__

#include "vtool.h"


class VSelectNodesTool : public VTool
{
public:
	VSelectNodesTool( KarbonView* view );
	virtual ~VSelectNodesTool();

	virtual void activate();

protected:
	virtual void draw();

	virtual void setCursor( const KoPoint& current ) const;
	virtual void mouseDragRelease( const KoPoint& current );

private:
	enum { normal, dragging, moving } m_state;
};

#endif

