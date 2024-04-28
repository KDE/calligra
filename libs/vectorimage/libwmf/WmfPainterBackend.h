/* This file is part of the KDE libraries
 *
 * SPDX-FileCopyrightText: 2003 thierry lorthiois (lorthioist@wanadoo.fr)
 *               2009-2011 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-only
 */
#ifndef _WMFPAINTERBACKEND_H_
#define _WMFPAINTERBACKEND_H_

#include "WmfAbstractBackend.h"
#include "kovectorimage_export.h"

#include <QPainter>
#include <QTransform>

class QPolygon;

/**
   Namespace for Windows Metafile (WMF) classes
*/
namespace Libwmf
{

class WmfDeviceContext;

/**
 * WmfPainterBackend inherits the abstract class WmfAbstractbackend
 * and redirects WMF actions onto a QPaintDevice.
 * Uses relative or absolute coordinate.
 *
 * how to use:
 * <pre>
 *   QPixmap  pix(100, 100);
 *   QPainter painter(pix);
 *   WmfPainterBackend wmf(painter, pix.size());
 *   if (wmf.load("/home/test.wmf" )) {
 *      wmf.play(pix);
 *   }
 *   paint.drawPixmap(0, 0, pix);
 * </pre>
 *
 */

class KOVECTORIMAGE_EXPORT WmfPainterBackend : public WmfAbstractBackend
{
public:
    WmfPainterBackend(QPainter *painter, const QSizeF &outputSize);
    ~WmfPainterBackend() override;

    using WmfAbstractBackend::play;

    /**
     * Play a WMF file on a QPaintDevice. Return true on success.
     */
    // bool play(QPaintDevice& target);
    // bool play(QPainter &painter);
    bool play() override;

private:
    // -------------------------------------------------------------------------
    // A virtual QPainter
    bool begin(const QRect &boundingBox) override;
    bool end() override;
    void save() override;
    void restore() override;

    /// Recalculate the world transform and then apply it to the painter
    /// This must be called at the end of every function that changes the transform.
    void recalculateWorldTransform();

    // Drawing attributes/modes
    void setCompositionMode(QPainter::CompositionMode mode) override;

    /**
     * Change logical Coordinate
     * some wmf files call those functions several times in the middle of a drawing
     * others wmf files doesn't call setWindow* at all
     * negative width and height are possible
     */
    void setWindowOrg(int left, int top) override;
    void setWindowExt(int width, int height) override;
    void setViewportOrg(int left, int top) override;
    void setViewportExt(int width, int height) override;

    // Graphics drawing functions
    void setPixel(WmfDeviceContext &context, int x, int y, const QColor &color) override;
    void lineTo(WmfDeviceContext &context, int x, int y) override;
    void drawRect(WmfDeviceContext &context, int x, int y, int w, int h) override;
    void drawRoundRect(WmfDeviceContext &context, int x, int y, int w, int h, int = 25, int = 25) override;
    void drawEllipse(WmfDeviceContext &context, int x, int y, int w, int h) override;
    void drawArc(WmfDeviceContext &context, int x, int y, int w, int h, int a, int alen) override;
    void drawPie(WmfDeviceContext &context, int x, int y, int w, int h, int a, int alen) override;
    void drawChord(WmfDeviceContext &context, int x, int y, int w, int h, int a, int alen) override;
    void drawPolyline(WmfDeviceContext &context, const QPolygon &pa) override;
    void drawPolygon(WmfDeviceContext &context, const QPolygon &pa) override;
    /**
     * drawPolyPolygon draw the XOR of a list of polygons
     * listPa : list of polygons
     */
    void drawPolyPolygon(WmfDeviceContext &context, QList<QPolygon> &listPa) override;
    void drawImage(WmfDeviceContext &context, int x, int y, const QImage &, int sx = 0, int sy = 0, int sw = -1, int sh = -1) override;
    void patBlt(WmfDeviceContext &context, int x, int y, int width, int height, quint32 rasterOperation) override;

    // Text drawing functions
    // rotation = the degrees of rotation in counterclockwise
    // not yet implemented in KWinMetaFile
    void drawText(WmfDeviceContext &context, int x, int y, const QString &s) override;

    // matrix transformation : only used in some bitmap manipulation
    void setTransform(WmfDeviceContext &context, const QTransform &, bool combine = false) override;

private:
    void updateFromDeviceContext(WmfDeviceContext &context);

protected:
    bool mIsInternalPainter; // True if the painter wasn't externally provided.
    QPainter *mPainter;
    QSizeF mOutputSize;
    QPaintDevice *mTarget;
    bool mRelativeCoord;
    // QPoint mLastPos;

    // Everything that has to do with window and viewport calculation
    QPoint mWindowOrg;
    QSize mWindowExt;
    QPoint mViewportOrg;
    QSize mViewportExt;
    bool mWindowExtIsSet;
    bool mViewportExtIsSet;
    QTransform mOutputTransform;
    QTransform mWorldTransform;

    int mSaveCount; // number of times Save() was called without Restore()
};
}

#endif
