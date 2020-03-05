/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
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

#ifndef SVGOUTPUTDEV_H
#define SVGOUTPUTDEV_H

#include <Object.h>
#include <OutputDev.h>

#include <QString>

class GfxPath;
class QMatrix;
class GooString;

/**
 * Poppler svg output device base on code from popplers ArthurOutputDev
 *
 */
class SvgOutputDev : public OutputDev
{
public:
    explicit SvgOutputDev(const QString &fileName);
    ~SvgOutputDev() override;

    bool isOk();

    bool upsideDown() override;
    bool useDrawChar() override;
    bool interpretType3Chars() override;
    void startPage(int pageNum, GfxState *state, XRef *xref) override;
    void endPage() override;

    // path painting
    void stroke(GfxState * state) override;
    void fill(GfxState * state) override;
    void eoFill(GfxState *state) override;

    // text
#ifdef HAVE_POPPLER_PRE_0_64
    void drawString(GfxState * state, GooString * s) override;
#else
    void drawString(GfxState * state, const GooString * s) override;
#endif

    // images
#ifdef HAVE_POPPLER_PRE_0_82
    void drawImage(GfxState *state, Object *ref, Stream *str,
                           int width, int height, GfxImageColorMap *colorMap,
                           bool interpolate, int *maskColors, bool inlineImg) override;
#else
    void drawImage(GfxState *state, Object *ref, Stream *str,
                           int width, int height, GfxImageColorMap *colorMap,
                           bool interpolate, const int *maskColors, bool inlineImg) override;
#endif
    // styles
    void updateAll(GfxState *state) override;
    void updateFillColor(GfxState *state) override;
    void updateStrokeColor(GfxState *state) override;
    void updateFillOpacity(GfxState *state) override;
    void updateStrokeOpacity(GfxState *state) override;
    void updateLineJoin(GfxState *state) override;
    void updateLineCap(GfxState *state) override;
    void updateMiterLimit(GfxState *state) override;
    void updateLineWidth(GfxState *state) override;

    /// Dumps content to svg file
    void dumpContent();
private:
#ifdef HAVE_POPPLER_PRE_0_83
    QString convertPath(GfxPath *path);
#else
    QString convertPath(const GfxPath *path);
#endif
    QString convertMatrix(const QMatrix &matrix);
    QString convertMatrix(const double * matrix);
    QString printFill();
    QString printStroke();

    class Private;
    Private * const d;
};

#endif // SVGOUTPUTDEV_H
