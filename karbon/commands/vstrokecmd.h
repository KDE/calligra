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
class VSelection;


class VStrokeCmd : public VCommand
{
public:
	VStrokeCmd( VDocument *doc, const VStroke& );
	virtual ~VStrokeCmd();

	virtual void execute();
	virtual void unexecute();

private:
	VSelection* m_selection;

	VStroke m_stroke;

	QValueVector<VStroke> m_oldcolors;
};

#endif

