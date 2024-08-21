/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KWCOPYSHAPE_H
#define KWCOPYSHAPE_H

#include "words_export.h"

#include <KoShape.h>

class KWPageManager;

/**
 * This shape allows the concept of copy-of-frame by nesting a frame in another.
 * This shape has a position and size, but all the rest of its settings will be
 * reused from the original, as passed on the constructor.  The shapes appearance
 * will be the same as the original, just in a different place in the document.
 */
class WORDS_TEST_EXPORT KWCopyShape : public KoShape
{
public:
    /**
     * Constructor
     * @param original the original shape this one will look like.
     * @param pageManager the page manager
     */
    explicit KWCopyShape(KoShape *original, const KWPageManager *pageManager = nullptr);
    ~KWCopyShape() override;

    /// reimplemented from KoShape
    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;
    /// reimplemented from KoShape
    QPainterPath outline() const override;
    /// reimplemented from KoShape
    QRectF outlineRect() const override;
    /// reimplemented from KoShape
    QRectF boundingRect() const override;
    /// reimplemented
    void saveOdf(KoShapeSavingContext &context) const override;
    /// reimplemented
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    KoShape *original() const;

private:
    KoShape *m_original;
    const KWPageManager *m_pageManager;
};

#endif
