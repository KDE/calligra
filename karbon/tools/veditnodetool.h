/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VEDITNODETOOL_H__
#define __VEDITNODETOOL_H__

#include "vtool.h"
#include "vselection.h"

class VEditNodeTool : public VTool
{
public:
	VEditNodeTool( KarbonView* view );
	virtual ~VEditNodeTool();

	virtual void activate();

	virtual bool eventFilter( QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject();

protected:
	void setCursor( const KoPoint & ) const;

private:
	enum { normal, dragging, moving } m_state;

	// input (mouse coordinates):
	KoPoint m_fp;
	KoPoint m_lp;

	bool m_isDragging;
};

#endif

