/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
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
class VPattern;

class VStrokeColorCmd : public VCommand
{
public:
	VStrokeColorCmd( VDocument *doc, VColor *color );
	virtual ~VStrokeColorCmd();

	virtual void execute();
	virtual void unexecute();

private:
	VSelection* m_selection;
	VColor* m_color;
	QValueVector<VColor> m_oldcolors;
};

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
	VStrokeCmd( VDocument *doc, VPattern * );
	virtual ~VStrokeCmd();

	virtual void execute();
	virtual void unexecute();

protected:
	VSelection* m_selection;

	const VStroke *m_stroke;
	VGradient *m_gradient;
	VPattern *m_pattern;

	QValueVector<VStroke> m_oldcolors;
};

#endif

