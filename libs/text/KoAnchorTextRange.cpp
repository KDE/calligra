/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009-2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 Ko Gmbh <cbo@kogmbh.com>
 * SPDX-FileCopyrightText: 2011 Matus Hanzes <matus.hanzes@ixonos.com>
 * SPDX-FileCopyrightText: 2013 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoAnchorTextRange.h"
#include "KoShapeAnchor.h"

#include <KoShape.h>
#include <KoShapeSavingContext.h>

#include "TextDebug.h"

class KoAnchorTextRangePrivate
{
public:
    KoAnchorTextRangePrivate(KoShapeAnchor *p)
        : parent(p)
    {
    }

    KoShapeAnchor *parent;
};

KoAnchorTextRange::KoAnchorTextRange(KoShapeAnchor *parent, const QTextCursor &cursor)
    : KoTextRange(cursor)
    , d_ptr(new KoAnchorTextRangePrivate(parent))
{
    Q_ASSERT(parent);
    parent->setTextLocation(this);
}

KoAnchorTextRange::KoAnchorTextRange(KoShapeAnchor *parent, QTextDocument *document, int position)
    : KoTextRange(document, position)
    , d_ptr(new KoAnchorTextRangePrivate(parent))
{
    Q_ASSERT(parent);
    parent->setTextLocation(this);
}
KoAnchorTextRange::~KoAnchorTextRange() = default;

KoShapeAnchor *KoAnchorTextRange::anchor() const
{
    Q_D(const KoAnchorTextRange);
    return d->parent;
}

const QTextDocument *KoAnchorTextRange::document() const
{
    return KoTextRange::document();
}

int KoAnchorTextRange::position() const
{
    return rangeStart();
}

void KoAnchorTextRange::updateContainerModel()
{
    Q_D(KoAnchorTextRange);

    if (!d->parent->shape()->isVisible()) {
        // Per default the shape this anchor presents is hidden and we only make it visible once an
        // explicit placement is made. This prevents shapes that are anchored at e.g. hidden
        // textboxes to not become visible.
        d->parent->shape()->setVisible(true);
    }

    if (d->parent->placementStrategy() != nullptr) {
        d->parent->placementStrategy()->updateContainerModel();
    }
}

bool KoAnchorTextRange::loadOdf(const KoXmlElement &, KoShapeLoadingContext &)
{
    return true;
}

void KoAnchorTextRange::saveOdf(KoShapeSavingContext &context, int position, KoTextRange::TagType tagType) const
{
    Q_UNUSED(position);
    Q_UNUSED(tagType);

    Q_D(const KoAnchorTextRange);
    if (tagType == KoTextRange::StartTag) {
        d->parent->saveOdf(context);
    }
}
