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

#include <poppler/Object.h>
#include <poppler/OutputDev.h>

#include <QtCore/QString>

class GfxPath;
class QColor;
class QMatrix;
class GooString;

/**
 * Poppler svg output device base on code from popplers ArthurOutputDev
 *
 */
class SvgOutputDev : public OutputDev
{
public:
    SvgOutputDev( const QString &fileName );
    virtual ~SvgOutputDev();

    GBool isOk();

    virtual GBool upsideDown();
    virtual GBool useDrawChar();
    virtual GBool interpretType3Chars();
    virtual void startPage( int pageNum, GfxState *state );
    virtual void endPage();

    // path painting
    virtual void stroke( GfxState * state );
    virtual void fill( GfxState * state );
    virtual void eoFill( GfxState *state );

    // text
    virtual void drawString( GfxState * state, GooString * s );

    // images
    virtual void drawImage(GfxState *state, Object *ref, Stream *str,
                int width, int height, GfxImageColorMap *colorMap,
                int *maskColors, GBool inlineImg);
    virtual void drawImage(GfxState *state, Object *ref, Stream *str,
                int width, int height, GfxImageColorMap *colorMap,
                GBool interpolate, int *maskColors, GBool inlineImg);
                                             
    // styles
    virtual void updateAll( GfxState *state );
    virtual void updateFillColor( GfxState *state );
    virtual void updateStrokeColor( GfxState *state );
    virtual void updateFillOpacity( GfxState *state );
    virtual void updateStrokeOpacity( GfxState *state );
    virtual void updateLineJoin( GfxState *state );
    virtual void updateLineCap( GfxState *state );
    virtual void updateMiterLimit( GfxState *state );
    virtual void updateLineWidth( GfxState *state );

    /// Dumps content to svg file
    void dumpContent();
private:
    QString convertPath( GfxPath *path );
    QString convertMatrix( const QMatrix &matrix );
    QString convertMatrix( double * matrix );
    QString printFill();
    QString printStroke();

    class Private;
    Private * const d;
};

#endif // SVGOUTPUTDEV_H
