/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 1998 Stefan Taferner
                 2001/2003 thierry lorthiois (lorthioist@wanadoo.fr)
                 2011 Inge Wallin (inge@lysator.liu.se)

   SPDX-License-Identifier: LGPL-2.0-only
*/

// Own
#include "WmfStack.h"

// Local
#include "WmfAbstractBackend.h"
#include "WmfDeviceContext.h"

// lib
#include "VectorImageDebug.h"

/**
   Namespace for Windows Metafile (WMF) classes
*/
namespace Libwmf
{

void KoWmfBrushHandle::apply(WmfDeviceContext *dc)
{
    dc->brush = brush;
    dc->changedItems |= DCBrush;
}

void KoWmfPenHandle::apply(WmfDeviceContext *dc)
{
    debugVectorImage << "Setting pen" << pen;
    dc->pen = pen;
    dc->changedItems |= DCPen;
}

void KoWmfPatternBrushHandle::apply(WmfDeviceContext *dc)
{
    dc->brush = brush;
    dc->changedItems |= DCBrush;
}

void KoWmfFontHandle::apply(WmfDeviceContext *dc)
{
    dc->font = font;
    dc->escapement = escapement;
    dc->orientation = orientation;
    dc->height = height;
    dc->changedItems |= DCFont; // Includes the font itself, the rotation and the height;
}

}
