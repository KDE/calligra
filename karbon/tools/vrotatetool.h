/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VROTATETOOL_H__
#define __VROTATETOOL_H__

#include "vtool.h"


class VRotateTool : public VTool
{
public:
	VRotateTool( KarbonView* view );
	virtual ~VRotateTool();

	virtual void activate();

	virtual bool eventFilter( QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject();

protected:
	void setCursor( const QPoint & ) const;

private:
	// input (mouse coordinates):
	KoPoint m_fp;
	KoPoint m_lp;
	KoPoint m_sp;

	double m_angle;

	bool m_isDragging;
};

#endif

