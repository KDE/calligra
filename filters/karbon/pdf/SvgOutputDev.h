/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGOUTPUTDEV_H
#define SVGOUTPUTDEV_H

#include <Object.h>
#include <OutputDev.h>

#include <QString>

#include <poppler-version.h>

#define POPPLER_VERSION_MACRO ((POPPLER_VERSION_MAJOR << 16) | (POPPLER_VERSION_MINOR << 8) | (POPPLER_VERSION_MICRO))

class GfxPath;
class QTransform;
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
    void stroke(GfxState *state) override;
    void fill(GfxState *state) override;
    void eoFill(GfxState *state) override;

    // text
    void drawString(GfxState *state, const GooString *s) override;

    // images
    void drawImage(GfxState *state,
                   Object *ref,
                   Stream *str,
                   int width,
                   int height,
                   GfxImageColorMap *colorMap,
                   bool interpolate,
                   const int *maskColors,
                   bool inlineImg) override;
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
    QString convertPath(const GfxPath *path);
    QString convertMatrix(const QTransform &matrix);
#if POPPLER_VERSION_MACRO < QT_VERSION_CHECK(26, 2, 0)
    QString convertMatrix(const double *matrix);
#else
    QString convertMatrix(const std::array<double, 6> &matrix);
#endif
    QString printFill();
    QString printStroke();

    class Private;
    Private *const d;
};

#endif // SVGOUTPUTDEV_H
