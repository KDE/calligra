/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 Matus Hanzes <matus.hanzes@ixonos.com>
 * SPDX-FileCopyrightText: 2013 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOANCHORTEXTRANGE_H
#define KOANCHORTEXTRANGE_H

#include "KoTextRange.h"

#include "KoShapeAnchor.h"

#include "kotext_export.h"

class KoAnchorTextRangePrivate;
class QTextCursor;

/**
 * This class connects KoShapeAnchor to a position in the text document.
 *
 * This class is used when the shape anchor is of type: char or paragraph
 *
 * It has to be registered to the textrange manager and thus forms the connection between the text
 * and the KoShapeAnchor and by extension the so called 'anchored-shape' (any kind of shape)
 *
 * The KoAnchorTextRange is placed at a position in text. As with all KoTextRange it will change
 * it's position in the text when the user edits text before it. The user is also able to delete it if
 * deleting the text where it is positioned.
 *
 * The anchored-shape can be repositioned on the canvas if the text is relayouted (for example after
 * editing the text. This is dependent on how the text layout is implemented.
 *
 * Steps to use a KoAnchorTextRange are
 * <ol>
 * <li> Create KoShapeAnchor *anchor = new KoShapeAnchor(shape);
 * <li> Use anchor->loadOdf() to load additional attributes like the "text:anchor-type"
 * <li> if type is char or paragraph create KoAnchorTextRange *anchorRange = new KoAnchorTextRange(anchor);
 * </ol>
 */
class KOTEXT_EXPORT KoAnchorTextRange : public KoTextRange, public KoShapeAnchor::TextLocation
{
    Q_OBJECT
public:
    /**
     * Constructor for a char or paragraph anchor.
     * @param parent the shapeanchor.
     * @param cursor the cursor.
     */
    KoAnchorTextRange(KoShapeAnchor *parent, const QTextCursor &cursor);
    KoAnchorTextRange(KoShapeAnchor *parent, QTextDocument *document, int position);
    ~KoAnchorTextRange() override;

    /// returns the parent anchor
    KoShapeAnchor *anchor() const;

    /// reimplemented from KoShapeAnchor::TextLocation
    const QTextDocument *document() const override;

    /// reimplemented from KoShapeAnchor::TextLocation
    int position() const override;

    void updateContainerModel();

    /// reimplemented from KoTextRange - should not do anything
    bool loadOdf(const KoXmlElement &, KoShapeLoadingContext &) override;

    /// reimplemented from KoTextRange
    void saveOdf(KoShapeSavingContext &context, int position, KoTextRange::TagType tagType) const override;

private:
    KoAnchorTextRangePrivate *const d_ptr;
    Q_DECLARE_PRIVATE(KoAnchorTextRange)
};

#endif
