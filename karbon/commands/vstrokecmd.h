/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSTROKECMD_H__
#define __VSTROKECMD_H__

#include "vcommand.h"
#include "vcolor.h"

#include <qvaluevector.h>

// Stroke object(s)


class VPath;
class VSelection;
class VStroke;
class VGradient;

class VStrokeLineWidthCmd : public VCommand
{
public:
	VStrokeLineWidthCmd( VDocument *doc, double );
	virtual ~VStrokeLineWidthCmd();

	virtual void execute();
	virtual void unexecute();

private:
	VSelection* m_selection;
	double m_width;
	QValueVector<double> m_oldwidths;
};

class VStrokeCmd : public VCommand
{
public:
	VStrokeCmd( VDocument *doc,  const VStroke * );
	VStrokeCmd( VDocument *doc, VGradient * );
	virtual ~VStrokeCmd();

	virtual void execute();
	virtual void unexecute();

private:
	VSelection* m_selection;

	const VStroke *m_stroke;
	VGradient *m_gradient;

	QValueVector<VStroke> m_oldcolors;
};

#endif

