/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTOOL_H__
#define __VTOOL_H__

#include <math.h>
#include <qpoint.h>

#include "vglobal.h"

class QEvent;

class KarbonPart;
class KarbonView;
class VCommand;

class VTool
{
public:
	VTool( KarbonPart* part = 0L );
	virtual ~VTool() {}

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	KarbonPart* part() const { return m_part; }

protected:
	// that's our part:
	KarbonPart* m_part;
};

#endif
