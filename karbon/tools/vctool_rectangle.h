/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLRECTANGLE_H__
#define __VCTOOLRECTANGLE_H__

#include <qpoint.h>
#include <qrect.h>

#include "vtool.h"

class KarbonPart;

// A singleton state to create a rectangle.

class VCToolRectangle : public VTool
{
public:
	virtual ~VCToolRectangle() {}
	static VCToolRectangle* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

protected:
	VCToolRectangle( KarbonPart* part );

private:
	KarbonPart* m_part;

	bool m_isDragging;

	// temporary QRect:
	QRect m_QRect;

	static VCToolRectangle* s_instance;
};

#endif
