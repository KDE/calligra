/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextLayoutRootArea.h"

#include "FrameIterator.h"

#include <KoShapeContainer.h>
#include <KoTextPage.h>
#include <KoTextShapeData.h>

class Q_DECL_HIDDEN KoTextLayoutRootArea::Private
{
public:
    Private()
        : shape(nullptr)
        , dirty(true)
        , textpage(nullptr)
        , nextStartOfArea(nullptr)
    {
    }
    KoShape *shape;
    bool dirty;
    KoTextPage *textpage;
    FrameIterator *nextStartOfArea;
};

KoTextLayoutRootArea::KoTextLayoutRootArea(KoTextDocumentLayout *documentLayout)
    : KoTextLayoutArea(nullptr, documentLayout)
    , d(new Private)
{
}

KoTextLayoutRootArea::~KoTextLayoutRootArea()
{
    if (d->shape) {
        KoTextShapeData *data = qobject_cast<KoTextShapeData *>(d->shape->userData());
        if (data)
            data->setRootArea(nullptr);
    }
    delete d->nextStartOfArea;
    delete d->textpage;
    delete d;
}

bool KoTextLayoutRootArea::layoutRoot(FrameIterator *cursor)
{
    d->dirty = false;

    setVirginPage(true);

    bool retval = KoTextLayoutArea::layout(cursor);

    delete d->nextStartOfArea;
    d->nextStartOfArea = new FrameIterator(cursor);
    return retval;
}

void KoTextLayoutRootArea::setAssociatedShape(KoShape *shape)
{
    d->shape = shape;
}

KoShape *KoTextLayoutRootArea::associatedShape() const
{
    return d->shape;
}

void KoTextLayoutRootArea::setPage(KoTextPage *textpage)
{
    delete d->textpage;
    d->textpage = textpage;
}

KoTextPage *KoTextLayoutRootArea::page() const
{
    if (d->textpage) {
        return d->textpage;
    }
    // If this root area has no KoTextPage then walk up the shape-hierarchy and look if we
    // have a textshape-parent that has a valid KoTextPage. This handles the in Words valid
    // case that the associatedShape is nested in another shape.
    KoTextPage *p = nullptr;
    for (KoShape *shape = associatedShape() ? associatedShape()->parent() : nullptr; shape; shape = shape->parent()) {
        if (KoTextShapeData *data = qobject_cast<KoTextShapeData *>(shape->userData())) {
            if (KoTextLayoutRootArea *r = data->rootArea())
                p = r->page();
            break;
        }
    }
    return p;
}

void KoTextLayoutRootArea::setDirty()
{
    d->dirty = true;
    documentLayout()->emitLayoutIsDirty();
}

bool KoTextLayoutRootArea::isDirty() const
{
    return d->dirty;
}

FrameIterator *KoTextLayoutRootArea::nextStartOfArea() const
{
    return d->nextStartOfArea;
}

KoText::Direction KoTextLayoutRootArea::parentTextDirection() const
{
    return KoText::LeftRightTopBottom;
}

void KoTextLayoutRootArea::setBottom(qreal b)
{
    KoTextLayoutArea::setBottom(b);
}
