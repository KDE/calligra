/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2002-2006 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2005-2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2010-2011 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KWCANVASBASE_H
#define KWCANVASBASE_H

#include <QCache>
#include <QDebug>

#include "KWDocument.h"
#include "KWPage.h"
#include "KWViewMode.h"
#include "words_export.h"

#include <KoCanvasBase.h>

#include <QImage>
#include <QQueue>
#include <QRectF>

class QRect;
class QPainter;

class KoToolProxy;
class KoShape;
class KoViewConverter;
class KWPageCacheManager;

class WORDS_EXPORT KWCanvasBase : public KoCanvasBase
{
public:
    static const qreal AnnotationAreaWidth;

    explicit KWCanvasBase(KWDocument *document, QObject *parent = nullptr);
    ~KWCanvasBase() override;

public: // KoCanvasBase interface methods.
    /// reimplemented method from superclass
    void gridSize(qreal *horizontal, qreal *vertical) const override;

    /// reimplemented method from superclass
    void addCommand(KUndo2Command *command) override;

    /// reimplemented method from superclass
    KoShapeManager *shapeManager() const override;

    /// reimplemented method from superclass
    void updateCanvas(const QRectF &rc) override;

    /// reimplemented method from superclass
    KoUnit unit() const override;

    /// reimplemented method from superclass
    KoToolProxy *toolProxy() const override;

    /// reimplemented method from superclass
    void clipToDocument(const KoShape *shape, QPointF &move) const override;

    /// reimplemented method from superclass
    KoGuidesData *guidesData() override;

    /// reimplemented method from superclass
    KoViewConverter *viewConverter() const override;

    /// return the document that this canvas works on
    KWDocument *document() const;

    /// return the viewMode currently associated with this canvas
    KWViewMode *viewMode() const;

    /// reimplemented method from superclass
    void ensureVisible(const QRectF &rect) override;

    /**
     * Enable or disable the page cache. The cache stores the rendered pages. It is
     * emptied when the zoomlevel changes.
     *
     * @param enabled: if true, we cache the contents of the document for this canvas,
     *  for the current zoomlevel
     * @param cacheSize: the maximum size for the cache. The cache will throw away
     *  pages once this size is reached. Depending on Qt's implementation of QCache, the
     *  unit is pages.
     * @param maxZoom above this zoomlevel we'll paint a scaled version of the cache, instead
     *  of creating a new cache
     */
    virtual void setCacheEnabled(bool enabled, int cacheSize = 50, qreal maxZoom = 2.0);

    /**
     * return whether annotations are shown in the canvas.
     */
    bool showAnnotations() const;
    void setShowAnnotations(bool doShow);

    /// @return the offset of the document in this canvas
    QPoint documentOffset() const;

protected:
    void paint(QPainter &painter, const QRectF &paintRect);

    void paintBackgrounds(QPainter &painter, KWViewMode::ViewMap &viewMap);
    void paintPageDecorations(QPainter &painter, KWViewMode::ViewMap &viewMap);
    void paintBorder(QPainter &painter, KWViewMode::ViewMap &viewMap);
    void doPaintBorder(QPainter &painter, const KoBorder &border, const QRectF &borderRect) const;

    void paintGrid(QPainter &painter, KWViewMode::ViewMap &viewMap);

    /**
     * paint one border along one of the 4 sides.
     * @param painter the Qt painter
     * @param borderData the border data
     * @param lineStart the line start
     * @param lineEnd the line end
     * @param zoom the zoom quotient
     * @param inwardsX is the horizontal vector (with value -1, 0 or 1) for the vector
     * pointing inwards for the border part nearest the center of the page.
     * @param inwardsY is the vertical vector (with value -1, 0 or 1) for the vector
     * pointing inwards for the border part nearest the center of the page.
     */
    void paintBorderSide(QPainter &painter,
                         const KoBorder::BorderData &borderData,
                         const QPointF &lineStart,
                         const QPointF &lineEnd,
                         qreal zoom,
                         int inwardsX,
                         int inwardsY) const;

    virtual void updateCanvasInternal(const QRectF &clip) = 0;

protected:
    KWDocument *m_document;
    KoShapeManager *m_shapeManager;
    KoToolProxy *m_toolProxy;
    KWViewMode *m_viewMode;
    QPoint m_documentOffset;
    KoViewConverter *m_viewConverter;
    bool m_showAnnotations; //< true if annotations should be shown in the canvas

    bool m_cacheEnabled;
    qreal m_currentZoom;
    qreal m_maxZoom; //< above this zoomlevel we scale the cached image, instead of recreating the cache.
    KWPageCacheManager *m_pageCacheManager;
    int m_cacheSize;
};

#endif // KWCANVASBASE_H
