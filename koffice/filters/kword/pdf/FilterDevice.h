/*
 * Copyright (c) 2002 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef FILTERDEVICE_H
#define FILTERDEVICE_H

#include <qimage.h>

class Object;
class Stream;

#include "gtypes.h"
#include "OutputDev.h"

#include "misc.h"


namespace PDFImport
{
class Page;
class Data;

class Device : public OutputDev
{
public:
    Device(Data &data);
    ~Device();

    void init();
    void dumpPage(uint i);

    //-----
    virtual GBool upsideDown() { return gTrue; }
    virtual GBool useDrawChar() { return gTrue; }
    virtual GBool interpretType3Chars() { return gFalse; }
    virtual GBool needNonText() { return gFalse; }

    //----- initialization
    virtual void startPage(int pageNum, GfxState *state);
    virtual void endPage();

    //----- update graphics state
    virtual void updateAll(GfxState *state);
//    virtual void updateLineDash(GfxState *state);
//    virtual void updateFlatness(GfxState *state);
//    virtual void updateLineJoin(GfxState *state);
//    virtual void updateLineCap(GfxState *state);
//    virtual void updateMiterLimit(GfxState *state);
//    virtual void updateLineWidth(GfxState *state);
    virtual void updateFillColor(GfxState *state);
    virtual void updateStrokeColor(GfxState *state);

    //----- update text state
    virtual void updateFont(GfxState *state);

    //----- text drawing
    virtual void beginString(GfxState *state, GString *s);
    virtual void endString(GfxState *state);
    virtual void drawChar(GfxState *state, double x, double y,
                          double dx, double dy,
                          double originX, double originY,
                          CharCode c, Unicode *u, int uLen);

    //----- link borders
    virtual void drawLink(::Link* link, Catalog *cat);

    //----- image drawing
    virtual void drawImageMask(GfxState *, Object *ref, Stream *,
                               int width, int height, GBool invert,
                               GBool inlineImg);
    virtual void drawImage(GfxState *, Object *ref, Stream *,
                           int width, int height, GfxImageColorMap *colorMap,
                           int *maskColors, GBool inlineImg);

    //----- path painting
    virtual void stroke(GfxState *state);
    virtual void fill(GfxState *state);
    virtual void eoFill(GfxState *state);

private:
    class Image;
    static void computeGeometry(GfxState *, Image &);
    uint initImage(GfxState *, int width, int height, bool mask);
    void addImage();
    void clear();
    static DPathVector convertPath(GfxState *);
    void doFill(const DPathVector &);
    Page *current() const { return _pages.getLast(); }

private:
    Data &_data;
    QPtrList<Page> _pages;
    QColor _fillColor, _strokeColor;

    class Image {
    public:
        QImage image;
        DRect  rect;
        bool   mask;
    };
    Image _currentImage;
    typedef QValueList<Image> ImageList;
    ImageList _images;
};

} // namespace

#endif
