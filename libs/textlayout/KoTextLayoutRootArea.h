/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTLAYOUTROOTAREA_H
#define KOTEXTLAYOUTROOTAREA_H

#include "kotextlayout_export.h"

#include "KoTextLayoutArea.h"

class KoShape;
class KoTextPage;

/**
 * When laying out text it happens in areas that can occupy space of various size.
 */
class KOTEXTLAYOUT_EXPORT KoTextLayoutRootArea : public KoTextLayoutArea
{
public:
    /// constructor
    explicit KoTextLayoutRootArea(KoTextDocumentLayout *documentLayout);
    ~KoTextLayoutRootArea() override;

    /// Layouts as much as it can
    /// Returns true if it has reached the end of the frame
    bool layoutRoot(FrameIterator *cursor);

    /// Sets an associated shape which can be retrieved with associatedShape()
    /// KoTextLayoutRootArea doesn't use it for anything.
    void setAssociatedShape(KoShape *shape);

    /// Returns the shape set with setAssociatedShape()
    KoShape *associatedShape() const;

    /**
     * Set the \p page this root area is on.
     *
     * The root-area takes over the ownership of the KoTextPage and will take
     * care to delete the KoTextPage if not needed any longer.
     */
    void setPage(KoTextPage *textpage);

    /// Returns the page this root area is on.
    KoTextPage *page() const;

    void setDirty();

    bool isDirty() const;

    /// Returns the cursor position of the following root frame
    FrameIterator *nextStartOfArea() const;

    KoText::Direction parentTextDirection() const override;

    void setBottom(qreal b);

private:
    class Private;
    Private *const d;
};

#endif
