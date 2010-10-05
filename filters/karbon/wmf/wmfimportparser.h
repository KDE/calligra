/* This file is part of the KDE project
 * Copyright (c) 2003 thierry lorthiois <lorthioist@wanadoo.fr>
 * Copyright (c) 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
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
#ifndef _WMFIMPORTPARSER_H_
#define _WMFIMPORTPARSER_H_

#include <QPainter>
#include <kowmfread.h>

class KoShape;
class KoPathShape;
class KarbonDocument;

/**
 * WMFImportParser inherit KoWmfRead
 * and translate WMF functions
 *
 */

class WMFImportParser : public KoWmfRead
{
public:
    WMFImportParser();
    ~WMFImportParser() { }

    /**
     * play WMF file on a KarbonDocument. Return true on success.
     */
    using KoWmfRead::play;
    bool play(KarbonDocument& doc);


private:
    // -------------------------------------------------------------------------
    // A virtual QPainter
    bool  begin();
    bool  end();
    void  save();
    void  restore();

    // Drawing tools
    void  setFont(const QFont &font);
    // the pen : the width of the pen is in logical coordinate
    void  setPen(const QPen &pen);
    void  setTextPen(const QPen &pen);
    const QPen &pen() const;
    void  setBrush(const QBrush &brush);

    // Drawing attributes/modes
    void  setBackgroundColor(const QColor &c);
    void  setBackgroundMode(Qt::BGMode mode);

    void  setCompositionMode(QPainter::CompositionMode);

    /**
     * Change logical Coordinate
     * some wmf files call those functions several times in the middle of a drawing
     * others wmf files doesn't call setWindow* at all
     * negative width and height are possible
     */
    void  setWindowOrg(int left, int top);
    void  setWindowExt(int width, int height);
    void  setViewportOrg(int left, int top);
    void  setViewportExt(int width, int height);

    // Clipping
    // the 'CoordinateMode' is ommitted : always CoordPainter in wmf
    // setClipRegion() is often used with save() and restore() => implement all or none
    void  setClipRegion(const QRegion &rec);
    QRegion clipRegion();

    // Graphics drawing functions
    void  moveTo(int x, int y);
    void  lineTo(int x, int y);
    void  drawRect(int x, int y, int w, int h);
    void  drawRoundRect(int x, int y, int w, int h, int = 25, int = 25);
    void  drawEllipse(int x, int y, int w, int h);
    void  drawArc(int x, int y, int w, int h, int a, int alen);
    void  drawPie(int x, int y, int w, int h, int a, int alen);
    void  drawChord(int x, int y, int w, int h, int a, int alen);
    void  drawPolyline(const QPolygon &pa);
    void  drawPolygon(const QPolygon &pa, bool winding = false);
    /**
     * drawPolyPolygon draw the XOR of a list of polygons
     * listPa : list of polygons
     */
    void  drawPolyPolygon(QList<QPolygon>& listPa, bool winding = false);
    void  drawImage(int x, int y, const QImage &, int sx = 0, int sy = 0, int sw = -1, int sh = -1);
    void  patBlt(int x, int y, int width, int height, quint32 rasterOperation);

    // Text drawing
    // rotation = the degrees of rotation in counterclockwise
    // not yet implemented in KWinMetaFile
    void  drawText(int x, int y, int w, int h, int flags, const QString &s, double rotation);

    // matrix transformation : only used in some bitmap manipulation
    void  setMatrix(const QMatrix &matrix, bool combine = false);

    //-----------------------------------------------------------------------------
    // Utilities
    // Add pen, brush and points to a path
    void appendPen(KoShape& obj);
    void appendBrush(KoShape& obj);
    void appendPoints(KoPathShape& path, const QPolygon& pa);
    // coordinate transformation
    // translate wmf to (0,0) -> scale to document size
    double coordX(int left);
    double coordY(int top);
    double scaleW(int width);
    double scaleH(int height);

private:
    KoShape * createShape(const QString &shapeID);

    KarbonDocument *mDoc;
    QRegion   mClippingRegion;
    QPen      mPen;
    QPen      mTextPen;
    QBrush    mBrush;
    Qt::BGMode mBackgroundMode;
    QColor    mBackgroundColor;
    // current origin of WMF logical coordinate
    QPoint    mCurrentOrg;
    double    mScaleX;
    double    mScaleY;
    // current position in WMF coordinate (INT16)
    QPoint    mCurrentPoint;
    QFont     mFont;
};

#endif
