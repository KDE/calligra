/* This file is part of the KDE libraries

   SPDX-FileCopyrightText: 2003 thierry lorthiois (lorthioist@wanadoo.fr)
   SPDX-FileCopyrightText: 2011 Inge Wallin (inge@lysator.liu.se)

   SPDX-License-Identifier: LGPL-2.0-only
*/
#ifndef _WMFWRITER_H_
#define _WMFWRITER_H_

#include "kovectorimage_export.h"

#include <QPainter>

class QPen;
class QBrush;
class QColor;
class QFont;
class QRegion;
class QString;
class QImage;

/**
   Namespace for Windows Metafile (WMF) classes
*/
namespace Libwmf
{

class WmfWriterPrivate;

/**
 * WmfWriter allows to create a windows placeable meta file (WMF).
 * Most of the functions are compatible with QPainter format.
 *
 * sample of utilization:
 *   <pre>
 *   WmfWriter  wmf("/home/test.wmf");
 *   wmf.begin();
 *   wmf.setWindow(0, 0, 200, 200);
 *   wmf.drawRect(10, 20, 50, 120);
 *   wmf.end();
 *   </pre>
 */
class KOVECTORIMAGE_EXPORT WmfWriter
{
public:
    explicit WmfWriter(const QString &fileName);
    virtual ~WmfWriter();

    // -------------------------------------------------------------------------
    // virtual Painter
    // for a good documentation : check QPainter documentation
    /**
     * Open the file. Returns true on success.
     */
    bool begin();
    /**
     * Close the file. Returns true on success.
     */
    bool end();
    void save();
    void restore();

    /**
     * Placeable WMF's use logical coordinates and have a default DPI.
     * This function set the dot per inch ratio.
     * If not specified the dpi is 1024.
     */
    void setDefaultDpi(int dpi);

    // Drawing tools
    void setFont(const QFont &f);
    // the width of the pen is in logical coordinate
    void setPen(const QPen &p);
    void setBrush(const QBrush &b);

    // Drawing attributes/modes
    void setBackgroundColor(const QColor &r);
    void setBackgroundMode(Qt::BGMode);
    void setCompositionMode(QPainter::CompositionMode);

    // Change logical Coordinate
    void setWindow(int left, int top, int width, int height);

    // Clipping
    // the 'CoordinateMode' parameter is omitted : always CoordPainter in wmf
    // not yet implemented
    void setClipRegion(const QRegion &r);
    void clipping(bool enable);

    // Graphics drawing functions
    void moveTo(int left, int top);
    void lineTo(int left, int top);
    void drawRect(int left, int top, int width, int height);
    void drawRoundRect(int left, int top, int width, int height, int = 25, int = 25);
    void drawEllipse(int left, int top, int width, int height);
    void drawArc(int left, int top, int width, int height, int a, int alen);
    void drawPie(int left, int top, int width, int height, int a, int alen);
    void drawChord(int left, int top, int width, int height, int a, int alen);
    void drawPolyline(const QPolygon &pa);
    void drawPolygon(const QPolygon &pa, bool winding = false);
    // drawPolyPolygon draw the XOR of a list of polygons
    // listPa : list of polygons
    void drawPolyPolygon(QList<QPolygon> &listPa, bool winding = false);
    void drawImage(int left, int top, const QImage &, int sx = 0, int sy = 0, int sw = -1, int sh = -1);

    // Text drawing functions
    // rotation = the degrees of rotation in counterclockwise
    // not yet implemented
    void drawText(int x, int y, int w, int h, int flags, const QString &s, double rotation);

private:
    //-----------------------------------------------------------------------------
    // Utilities and conversion Qt --> Wmf

    /** Convert QPointArray into qint16 position (result in mSt) */
    void pointArray(const QPolygon &pa);

    /** Conversion between windows color and QColor */
    quint32 winColor(const QColor &color);

    /** Convert angle a and alen in coordinate (xStart,yStart) and (xEnd, yEnd) */
    void angleToxy(int &xStart, int &yStart, int &xEnd, int &yEnd, int a, int alen);

    /** Convert windows rasterOp in QT rasterOp */
    quint16 qtRasterToWin16(QPainter::CompositionMode op) const;
    quint32 qtRasterToWin32(QPainter::CompositionMode op) const;

private:
    WmfWriterPrivate *const d;
};
}

#endif
