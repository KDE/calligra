/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 Matus Hanzes <matus.hanzes@ixonos.com>
 * SPDX-FileCopyrightText: 2013 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOANCHORINLINEOBJECT_H
#define KOANCHORINLINEOBJECT_H

#include "KoInlineObject.h"

#include "KoShapeAnchor.h"

#include "kotext_export.h"

class KoAnchorInlineObjectPrivate;

/**
 * This class connects KoShapeAnchor to an inline character in the text document.
 *
 * This class is used when the shape anchor is of type: as-char
 *
 * It has to be registered to the inlineobjectmanager and thus forms the connection between the text
 * and the KoShapeAnchor and by extension the so called 'anchored-shape' (any kind of shape)
 *
 * The KoAnchorInlineObject is placed as a character in text. As such it will move and be
 * editable like any other character, including deletion.
 *
 * Since this is a real character it will be positioned by the textlayout engine and anything that
 * will change the position of the text will thus also change the KoAnchorInlineObject character.
 *
 * The anchored-shape can be repositioned on the canvas if the text is relayouted (for example after
 * editing the text. This is dependent on how the text layout is implemented.
 *
 * Steps to use a KoAnchorInlineObject are
 * <ol>
 * <li> Create KoShapeAnchor *anchor = new KoShapeAnchor(shape);
 * <li> Use anchor->loadOdf() to load additional attributes like the "text:anchor-type"
 * <li> if type is as-char create KoAnchorInlineObject *anchorObj = new KoAnchorInlineObject(anchor);
 * </ol>
 */
class KOTEXT_EXPORT KoAnchorInlineObject : public KoInlineObject, public KoShapeAnchor::TextLocation
{
    Q_OBJECT
public:
    /**
     * Constructor for an as-char anchor.
     * @param parent the shapeanchor.
     */
    explicit KoAnchorInlineObject(KoShapeAnchor *parent);
    ~KoAnchorInlineObject() override;

    /// returns the parent anchor
    KoShapeAnchor *anchor() const;

    /// returns the cursor position in the document where this anchor is positioned.
    int position() const override;

    /// returns the document that this anchor is associated with.
    const QTextDocument *document() const override;

    /// reimplemented from KoInlineObject
    void updatePosition(const QTextDocument *document, int posInDocument, const QTextCharFormat &format) override;
    /// reimplemented from KoInlineObject
    void resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd) override;
    /// reimplemented from KoInlineObject
    void paint(QPainter &painter,
               QPaintDevice *pd,
               const QTextDocument *document,
               const QRectF &rect,
               const QTextInlineObject &object,
               int posInDocument,
               const QTextCharFormat &format) override;

    qreal inlineObjectAscent() const;

    qreal inlineObjectDescent() const;

    /// reimplemented from KoInlineObject - should not do anything
    bool loadOdf(const KoXmlElement &, KoShapeLoadingContext &) override;

    /// reimplemented from KoInlineObject
    void saveOdf(KoShapeSavingContext &context) override;

private:
    Q_DECLARE_PRIVATE(KoAnchorInlineObject)
};

#endif
