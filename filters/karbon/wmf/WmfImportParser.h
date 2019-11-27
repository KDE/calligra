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

#include <WmfAbstractBackend.h>
#include <QPainter>
#include <QMatrix>

class WmfDeviceContext;
class KoXmlWriter;

/**
 * WMFImportParser inherits WmfAbstractBackend and translates WMF functions
 */
class WMFImportParser : public Libwmf::WmfAbstractBackend
{
public:
    explicit WMFImportParser(KoXmlWriter &svgWriter);
    ~WMFImportParser() override;

private:
    // -------------------------------------------------------------------------
    // A virtual QPainter
    bool  begin(const QRect &boundingBox) override;
    bool  end() override;
    void  save() override;
    void  restore() override;

    // Drawing attributes/modes
    void  setCompositionMode(QPainter::CompositionMode) override;

    /**
     * Change logical Coordinate
     * some wmf files call those functions several times in the middle of a drawing
     * others wmf files doesn't call setWindow* at all
     * negative width and height are possible
     */
    void  setWindowOrg(int left, int top) override;
    void  setWindowExt(int width, int height) override;
    void  setViewportOrg(int left, int top) override;
    void  setViewportExt(int width, int height) override;

    // Clipping
    // the 'CoordinateMode' is omitted : always CoordPainter in wmf
    // setClipRegion() is often used with save() and restore() => implement all or none
    void  setClipRegion(Libwmf::WmfDeviceContext &context, const QRegion &rec);
    QRegion clipRegion();

    // Graphics drawing functions
    void  setPixel(Libwmf::WmfDeviceContext &context, int x, int y, const QColor &color) override;
    void  moveTo(Libwmf::WmfDeviceContext &context, int x, int y);
    void  lineTo(Libwmf::WmfDeviceContext &context, int x, int y) override;
    void  drawRect(Libwmf::WmfDeviceContext &context, int x, int y, int w, int h) override;
    void  drawRoundRect(Libwmf::WmfDeviceContext &context, int x, int y, int w, int h, int = 25, int = 25) override;
    void  drawEllipse(Libwmf::WmfDeviceContext &context, int x, int y, int w, int h) override;
    void  drawArc(Libwmf::WmfDeviceContext &context, int x, int y, int w, int h, int a, int alen) override;
    void  drawPie(Libwmf::WmfDeviceContext &context, int x, int y, int w, int h, int a, int alen) override;
    void  drawChord(Libwmf::WmfDeviceContext &context, int x, int y, int w, int h, int a, int alen) override;
    void  drawPolyline(Libwmf::WmfDeviceContext &context, const QPolygon &pa) override;
    void  drawPolygon(Libwmf::WmfDeviceContext &context, const QPolygon &pa) override;
    /**
     * drawPolyPolygon draw the XOR of a list of polygons
     * listPa : list of polygons
     */
    void  drawPolyPolygon(Libwmf::WmfDeviceContext &context, QList<QPolygon>& listPa) override;
    void  drawImage(Libwmf::WmfDeviceContext &context, int x, int y, const QImage &,
                    int sx = 0, int sy = 0, int sw = -1, int sh = -1) override;
    void  patBlt(Libwmf::WmfDeviceContext &context, int x, int y, int width, int height,
                 quint32 rasterOperation) override;

    // Text drawing
    // rotation = the degrees of rotation in counterclockwise
    // not yet implemented in KWinMetaFile
    void  drawText(Libwmf::WmfDeviceContext &context, int x, int y, const QString &s) override;

    // matrix transformation : only used in some bitmap manipulation
    void  setMatrix(Libwmf::WmfDeviceContext &context, const QMatrix &matrix, bool combine = false) override;

    //-----------------------------------------------------------------------------
    // Utilities
    // Add pen, brush and points to a path
    QString saveStroke(Libwmf::WmfDeviceContext &context);
    QString saveFill(Libwmf::WmfDeviceContext &context);

    // coordinate transformation
    QRectF boundBox(int left, int top, int width, int height);
    QPointF coord(const QPoint &p);
    QSizeF size(const QSize &s);

private:
    void updateTransform();

    KoXmlWriter &m_svgWriter;

    QSizeF m_pageSize;

    struct CoordData {
        CoordData() : org(0,0), ext(0,0), extIsValid(false) {}
        QPointF org;
        QSizeF ext;
        bool extIsValid;
    };

    CoordData m_window;
    CoordData m_viewport;
    qreal m_scaleX;
    qreal m_scaleY;
    QMatrix m_matrix;
};

#endif // _WMFIMPORTPARSER_H_
