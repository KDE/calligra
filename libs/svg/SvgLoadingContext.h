/* This file is part of the KDE project
 * Copyright (C) 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SVGLOADINGCONTEXT_H
#define SVGLOADINGCONTEXT_H

#include "kosvg_export.h"
#include <KoXmlReader.h>

class SvgGraphicsContext;

class KOSVG_EXPORT SvgLoadingContext
{
public:
    SvgLoadingContext();
    ~SvgLoadingContext();

    /// Returns the current graphics context
    SvgGraphicsContext *currentGC();

    /// Pushes a new graphics context to the stack
    SvgGraphicsContext *pushGraphicsContext(const KoXmlElement &element = KoXmlElement(), bool inherit = true);

    /// Pops the current graphics context from the stack
    void popGraphicsContext();

    /// Sets the initial xml base dir, i.e. the directory the svg file is read from
    void setInitialXmlBaseDir(const QString &baseDir);

    /// Returns the current xml base dir
    QString xmlBaseDir();

    /// Constructs an absolute file path from the given href and current xml base directory
    QString absoluteFilePath(const QString &href);

    /// Returns the next z-index
    int nextZIndex();

private:
    class Private;
    Private * const d;
};

#endif // SVGLOADINGCONTEXT_H
