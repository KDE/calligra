/* This file is part of the KDE project
   Copyright (C) 2003, The Karbon Developers

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __SVGGRAPHICSCONTEXT_H__
#define __SVGGRAPHICSCONTEXT_H__

#include <core/vfill.h>
#include <core/vstroke.h>
#include <core/vfillrule.h>

class SvgGraphicsContext
{
public:
	SvgGraphicsContext()
	{
		stroke.setType( VStroke::none ); // default is no stroke
		stroke.setLineWidth( 1.0 );
		stroke.setLineCap( VStroke::capButt );
		stroke.setLineJoin( VStroke::joinMiter );
		fill.setColor( VColor( Qt::black ) );
		fillRule = winding;
		color = Qt::black;
	}
	VFill		fill;
	VFillRule	fillRule;
	VStroke		stroke;
	QWMatrix	matrix;
	QFont		font;
	QColor		color;
};

#endif
