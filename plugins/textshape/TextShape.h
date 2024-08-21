/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2008 Pierre Stirnweiss \pierre.stirnweiss_calligra@gadz.org>
 * SPDX-FileCopyrightText: 2010 KO GmbH <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTSHAPE_H
#define KOTEXTSHAPE_H

#include <KoFrameShape.h>
#include <KoShapeContainer.h>
#include <KoTextDocument.h>
#include <KoTextShapeData.h>

#include <QPainter>
#include <QTextDocument>

#define TextShape_SHAPEID "TextShapeID"

class KoInlineTextObjectManager;
class KoTextRangeManager;
class KoPageProvider;
class KoImageCollection;
class KoTextDocument;
class TextShape;
class KoTextDocumentLayout;
class KoParagraphStyle;

/**
 * A text shape.
 * The Text shape is capable of drawing structured text.
 * @see KoTextShapeData
 */
class TextShape : public KoShapeContainer, public KoFrameShape
{
public:
    TextShape(KoInlineTextObjectManager *inlineTextObjectManager, KoTextRangeManager *textRangeManager);
    ~TextShape() override;

    /// reimplemented
    void paintComponent(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;
    /// reimplemented
    void waitUntilReady(const KoViewConverter &converter, bool asynchronous) const override;

    /// helper method.
    QPointF convertScreenPos(const QPointF &point) const;

    /// reimplemented
    QPainterPath outline() const override;

    /// reimplemented
    QRectF outlineRect() const override;

    /// reimplemented
    ChildZOrderPolicy childZOrderPolicy() override
    {
        return KoShape::ChildZPassThrough;
    }

    /// set the image collection which is needed to draw bullet from images
    void setImageCollection(KoImageCollection *collection)
    {
        m_imageCollection = collection;
    }

    KoImageCollection *imageCollection();

    /**
     * From KoShape reimplemented method to load the TextShape from ODF.
     *
     * This method redirects the call to the KoTextShapeData::loadOdf() method which
     * in turn will call the KoTextLoader::loadBody() method that reads the element
     * into a QTextCursor.
     *
     * @param context the KoShapeLoadingContext used for loading.
     * @param element element which represents the shape in odf.
     * @return false if loading failed.
     */
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    /**
     * From KoShape reimplemented method to store the TextShape data as ODF.
     *
     * @param context the KoShapeSavingContext used for saving.
     */
    void saveOdf(KoShapeSavingContext &context) const override;

    KoTextShapeData *textShapeData()
    {
        return m_textShapeData;
    }

    void updateDocumentData();

    void update() const override;
    void update(const QRectF &shape) const override;

    // required for Stage hack
    void setPageProvider(KoPageProvider *provider)
    {
        m_pageProvider = provider;
    }

    /// reimplemented
    bool loadOdfFrame(const KoXmlElement &element, KoShapeLoadingContext &context) override;

protected:
    bool loadOdfFrameElement(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    /// reimplemented
    void loadStyle(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    /// reimplemented
    QString saveStyle(KoGenStyle &style, KoShapeSavingContext &context) const override;

private:
    void shapeChanged(ChangeType type, KoShape *shape = nullptr) override;

    KoTextShapeData *m_textShapeData;
    KoPageProvider *m_pageProvider;
    KoImageCollection *m_imageCollection;
    QRegion m_paintRegion;
    bool m_clip;
    KoTextDocumentLayout *m_layout;
};

#endif
