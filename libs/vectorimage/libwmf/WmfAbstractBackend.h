/* This file is part of the KDE libraries

   SPDX-FileCopyrightText: 2003 thierry lorthiois (lorthioist@wanadoo.fr)
   SPDX-FileCopyrightText: 2011 Inge Wallin (inge@lysator.liu.se)

   SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef _WMFABSTRACTBACKEND_H_
#define _WMFABSTRACTBACKEND_H_

#include "kovectorimage_export.h"

#include <QPainter>
#include <QRect>
#include <QRegion>

class QString;
class QColor;
class QImage;
class QTransform;

/**
   Namespace for Windows Metafile (WMF) classes
*/
namespace Libwmf
{

class WmfParser;
class WmfDeviceContext;

/**
 * WmfAbstractBackend allows the redirection of the actions stored in a WMF file.
 * Most of the virtuals functions are compatible with QPainter format.
 *
 * How to use :
 *   inherit this class and define abstract functions
 *   then create an object and call @ref load() and @ref play()
 *
 */

class KOVECTORIMAGE_EXPORT WmfAbstractBackend
{
public:
    WmfAbstractBackend();
    virtual ~WmfAbstractBackend();

    /**
     * Load WMF file. Returns true on success.
     */
    virtual bool load(const QString &fileName);
    virtual bool load(const QByteArray &array);

    /**
     * play the WMF file => call virtuals functions
     */
    virtual bool play();

    /**
     * Returns true if the metafile is standard / placeable / enhanced / valid
     */
    bool isStandard(void) const;
    bool isPlaceable(void) const;
    bool isEnhanced(void) const;
    bool isValid(void) const;

    /**
     * Returns the bounding rectangle
     * Standard Meta File : return the bounding box from setWindowOrg and setWindowExt (slow)
     * Placeable Meta File : return the bounding box from header
     * always in logical coordinate
     */
    virtual QRect boundingRect(void) const;

    /**
     * Returns the default DotPerInch for placeable meta file,
     * return 0 for Standard meta file
     */
    int defaultDpi(void) const;

    /**
     * Activate debug mode.
     * nbFunc : number of functions to draw
     * nbFunc!=0 switch to debug mode with trace
     */
    void setDebug(int nbFunc);

    // -------------------------------------------------------------------------
    // A virtual QPainter: inherit those virtuals functions
    // for a good documentation : check QPainter documentation
    virtual bool begin(const QRect &boundingBox) = 0;
    virtual bool end() = 0;
    virtual void save() = 0;
    virtual void restore() = 0;

    // Drawing attributes/modes
    virtual void setCompositionMode(QPainter::CompositionMode) = 0;

    // Change logical Coordinate
    // some wmf files call those functions several times in the middle of a drawing
    // others doesn't call setWindow* at all
    virtual void setWindowOrg(int left, int top) = 0;
    virtual void setWindowExt(int width, int height) = 0;
    virtual void setViewportOrg(int left, int top) = 0;
    virtual void setViewportExt(int width, int height) = 0;

    // Graphics drawing functions
    virtual void setPixel(WmfDeviceContext &context, int x, int y, const QColor &color) = 0;
    virtual void lineTo(WmfDeviceContext &context, int x, int y) = 0;
    virtual void drawRect(WmfDeviceContext &context, int x, int y, int w, int h) = 0;
    virtual void drawRoundRect(WmfDeviceContext &context, int x, int y, int w, int h, int = 25, int = 25) = 0;
    virtual void drawEllipse(WmfDeviceContext &context, int x, int y, int w, int h) = 0;
    virtual void drawArc(WmfDeviceContext &context, int x, int y, int w, int h, int a, int alen) = 0;
    virtual void drawPie(WmfDeviceContext &context, int x, int y, int w, int h, int a, int alen) = 0;
    virtual void drawChord(WmfDeviceContext &context, int x, int y, int w, int h, int a, int alen) = 0;
    virtual void drawPolyline(WmfDeviceContext &context, const QPolygon &pa) = 0;
    virtual void drawPolygon(WmfDeviceContext &context, const QPolygon &pa) = 0;
    // drawPolyPolygon draw the XOR of a list of polygons
    virtual void drawPolyPolygon(WmfDeviceContext &context, QList<QPolygon> &listPa) = 0;
    virtual void drawImage(WmfDeviceContext &context, int x, int y, const QImage &, int sx = 0, int sy = 0, int sw = -1, int sh = -1) = 0;
    virtual void patBlt(WmfDeviceContext &context, int x, int y, int width, int height, quint32 rasterOperation) = 0;

    // Text drawing functions
    virtual void drawText(WmfDeviceContext &context, int x, int y, const QString &s) = 0;

    // matrix transformation : only used for bitmap manipulation
    virtual void setTransform(WmfDeviceContext &context, const QTransform &, bool combine = false) = 0;

protected:
    WmfParser *m_parser;
};

}

#endif
