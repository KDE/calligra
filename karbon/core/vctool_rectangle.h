/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCTOOLRECTANGLE_H__
#define __VCTOOLRECTANGLE_H__

#include "vtool.h"

class KarbonPart;

// A singleton state to create a rectangle.

class VCToolRectangle : public VTool
{
public:
	virtual ~VCToolRectangle() {}
	static VCToolRectangle* instance( KarbonPart* part );

	virtual bool eventFilter( QEvent* event );

protected:
	VCToolRectangle( KarbonPart* part );

private:
	KarbonPart* m_part;
	static VCToolRectangle* s_instance;
};

#endif
