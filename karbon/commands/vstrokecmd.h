/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSTROKECMD_H__
#define __VSTROKECMD_H__

#include "vcommand.h"
#include "vcolor.h"
#include "vstroke.h"

#include <qvaluevector.h>

// Stroke object(s)

class VPath;

class VStrokeCmd : public VCommand
{
public:
	VStrokeCmd( VDocument *doc, const VColor& color, float opacity = -1 );
	virtual ~VStrokeCmd() {}

	virtual void execute();
	virtual void unexecute();

private:
	VSelection m_objects;

	VColor m_color;
	float m_opacity;

	QValueVector<VStroke> m_oldcolors;
};

#endif

