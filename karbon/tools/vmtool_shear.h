/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMTOOLSHEAR_H__
#define __VMTOOLSHEAR_H__

#include "vtool.h"

class KarbonPart;
class KarbonView;

// A singleton state to scale object(s)

class VMToolShear : public VTool
{
public:
	virtual ~VMToolShear();
	static VMToolShear* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject( KarbonView* view );

protected:
	VMToolShear( KarbonPart* part );

	void setCursor( KarbonView* view ) const;

private:
	static VMToolShear* s_instance;

	// input (mouse coordinates):
	KoPoint m_fp;
	KoPoint m_lp;

	double m_s1, m_s2;

	bool m_isDragging;
};

#endif
