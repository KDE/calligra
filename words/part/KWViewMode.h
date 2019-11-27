/* This file is part of the KDE project
 * Copyright (C) 2001 David Faure <faure@kde.org>
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
 * Copyright (C) 2010-2011 Boudewijn Rempt <boud@kogmbh.com>
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

#ifndef KWVIEWMODE_H
#define KWVIEWMODE_H

class KWPageManager;
class KoViewConverter;
class KWDocument;

#include "KWPage.h"
#include "words_export.h"

#include <QPointF>
#include <QRectF>
#include <QSizeF>
#include <QObject>
#include <QRect>

/**
 * Abstract base class for KWCanvas's view modes.
 * A viewMode is a strategy inserted between the canvas and the document.  A document
 * normally is nothing but a very tall area for drawing with some artificial limitations
 * imposed by the pages (KWPage).  A viewmode can, for example, choose to paint page
 * outlines and translate the document coordinates so the pages get painted correctly
 * and mouseclicks on the canvas get converted into real document coordinates.
 *
 * On the implementation side the viewMode will not have the notion of zoom; its using
 * the viewconverter for that.  This means that to the user of this API zooming
 * is applied just like translation is.
 *
 * This class provides a layer on top of the KoViewConverter and Words should not use that
 * interface directly.
 */
class WORDS_EXPORT KWViewMode : public QObject
{
    Q_OBJECT
public:
    KWViewMode();
    ~KWViewMode() override {}

    /// a two value return type for mapExposedRects()
    struct ViewMap {
        QRect clipRect;   ///< the rectangle in the view coordinates showing (part of) the clip
        QPointF distance; ///< the displacement between the document and the view in view coordinates.
        KWPage page;      ///< The page that this section represents.
    };

    /** Document coord -> view coord */
    virtual QPointF documentToView(const QPointF &point, KoViewConverter *viewConverter) const = 0;

    /** Document coord -> view coord */
    QRectF documentToView(const QRectF &rect, KoViewConverter *viewConverter) const;

    /** View coord -> Document coord */
    virtual QPointF viewToDocument(const QPointF &point, KoViewConverter *viewConverter) const = 0;

    /** View coord -> Document coord */
    QRectF viewToDocument(const QRectF &rect, KoViewConverter *viewConverter) const;

    /** Size of the contents area, in pixels */
    virtual QSizeF contentsSize() const = 0;

    /** Does this viewmode know anything about pages? */
    virtual bool hasPages() {
        return true;
    }

    /** Return the name of the viewmode, used for loading/saving. */
    virtual const QString type() const = 0;

    /**
     * Create a new ViewMode based on the type string of that viewmode.
     * To create a specific viewMode you can use this;
     *  @code
     *    QString type = KWViewModeNormal::viewMode();
     *    KWViewMode *vm = KWViewMode::create(type, myCanvas);
     *  @endcode
     * @param viewModeType the type of viewMode
     * @param document
     */
    static KWViewMode *create(const QString& viewModeType, KWDocument *document);

    /**
     * This method converts a clip-rect of the view to a set of cliprects as they are
     * mirrored on the document space.
     * For a cliprect that overlaps various pages that same amount of arguments will
     * be created in the return value in the form of a ViewMap struct.
     * For example;
     *   Imagine a viewmode that shows two pages side by side. And a @p clipRect parameter
     * covering the top half of both pages.  To repaint this correctly in the Canvas
     * this needs to be split into two clip rects since the document internally does not
     * have those two pages next to each other, but below one another.
     * Note that just like everything in the viewMode; the input and output will be in the
     * same zoom-level. This means that adding all the output rects should have the same repaint
     * area as the input rect.
     * @param clipRect the clipping-rect as it was on the Canvas.
     * @param viewConverter An optional viewconverter to override the viewconverter set on the viewmode.
     * @return a list of clipping-rects as it maps to the internal document.
     */
    virtual QVector<ViewMap> mapExposedRects(const QRectF &clipRect, KoViewConverter *viewConverter) const = 0;

public Q_SLOTS:
    /**
     *  Notification that the page setup has changed expected when a page has been
     * added or removed or just resized.
     */
    void pageSetupChanged();
    void setPageManager(KWPageManager *pageManager) { m_pageManager = pageManager; updatePageCache(); }

protected:
    /**
     * Will be called when the pageSetupChanged() has been notified.
     */
    virtual void updatePageCache() = 0;

    KWPageManager *m_pageManager;
};

Q_DECLARE_TYPEINFO(KWViewMode::ViewMap, Q_MOVABLE_TYPE);

#endif
