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

	virtual bool eventFilter( QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject();

protected:
	void setCursor( const KoPoint & ) const;

private:
	enum { normal, dragging, moving } m_state;
};

#endif

