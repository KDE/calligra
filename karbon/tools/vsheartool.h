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

	virtual bool eventFilter( QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject();

protected:
	void setCursor( const QPoint & ) const;
	virtual void mousePressed( QMouseEvent * );
	virtual void mouseReleased( QMouseEvent * );

private:
	double m_s1, m_s2;

	VHandleNode m_activeNode;
};

#endif

