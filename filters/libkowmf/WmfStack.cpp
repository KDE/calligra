/* This file is part of the KDE libraries
   Copyright (c) 1998 Stefan Taferner
                 2001/2003 thierry lorthiois (lorthioist@wanadoo.fr)
                 2011 Inge Wallin (inge@lysator.liu.se)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "WmfStack.h"
#include "WmfAbstractBackend.h"
#include "WmfDeviceContext.h"

/**
   Namespace for Windows Metafile (WMF) classes
*/
namespace Libwmf
{


void KoWmfBrushHandle::apply(WmfAbstractBackend *p, WmfDeviceContext *dc)
{
    p->setBrush(brush);
    dc->brush = brush;
    dc->changedItems |= DCBrush;
}

void KoWmfPenHandle::apply(WmfAbstractBackend *p, WmfDeviceContext *dc)
{
    p->setPen(pen);
    dc->pen = pen;
    dc->changedItems |= DCPen;
}

void KoWmfPatternBrushHandle::apply(WmfAbstractBackend *p, WmfDeviceContext *dc)
{
    p->setBrush(brush);
    dc->brush = brush;
    dc->changedItems |= DCBrush;
}

void KoWmfFontHandle::apply(WmfAbstractBackend *p, WmfDeviceContext *dc)
{
    p->setFont(font, rotation, height);
    dc->font = font;
    dc->changedItems |= DCFont;
}

}
