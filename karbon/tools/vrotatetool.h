/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VROTATETOOL_H__
#define __VROTATETOOL_H__

#include "vtool.h"
#include "vselection.h"

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
	virtual void mousePressed( QMouseEvent * );
	virtual void mouseReleased( QMouseEvent * );

private:
	KoPoint m_sp;

	double m_angle;

	VHandleNode m_activeNode;
};

#endif

