/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSELECTNODESTOOL_H__
#define __VSELECTNODESTOOL_H__

#include "vtool.h"
#include "vselection.h"

class VSelectNodesTool : public VTool
{
public:
	VSelectNodesTool( KarbonView* view );
	virtual ~VSelectNodesTool();

	virtual void activate();

	// draw the object while it is edited:
	void drawTemporaryObject();

protected:
	virtual void setCursor( const KoPoint & ) const;
	virtual void mousePressed( QMouseEvent * );
	virtual void mouseReleased( QMouseEvent * );

private:
	enum { normal, dragging, moving } m_state;
};

#endif

