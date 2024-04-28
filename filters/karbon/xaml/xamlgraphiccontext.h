/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 The Karbon Developers

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __XAMLGRAPHICSCONTEXT_H__
#define __XAMLGRAPHICSCONTEXT_H__

#include <core/vfill.h>
#include <core/vfillrule.h>
#include <core/vstroke.h>

class XAMLGraphicsContext
{
public:
    XAMLGraphicsContext()
    {
        stroke.setType(VStroke::none); // default is no stroke
        stroke.setLineWidth(1.0);
        stroke.setLineCap(VStroke::capButt);
        stroke.setLineJoin(VStroke::joinMiter);
        fill.setColor(VColor(Qt::black));
        fillRule = winding;
    }
    VFill fill;
    VFillRule fillRule;
    VStroke stroke;
    QTransform matrix;
    QFont font;
};

#endif
