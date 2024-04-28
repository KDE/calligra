/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ANNOTATIONTEXTSHAPE_H
#define ANNOTATIONTEXTSHAPE_H

#include "TextShape.h"
#include <KoShapeContainer.h>
#include <KoTextShapeData.h>

#include <QPainter>
#include <QTextDocument>

class KoInlineTextObjectManager;
class KoTextRangeManager;

#define AnnotationShape_SHAPEID "AnnotationTextShapeID"

class AnnotationTextShape : public TextShape
{
public:
    // Some constants
    static const qreal HeaderSpace; // The space needed for the annotation header.
    static const qreal HeaderFontSize;

    // For now we should give these parameters for TextShape.
    AnnotationTextShape(KoInlineTextObjectManager *inlineTextObjectManager, KoTextRangeManager *textRangeManager);
    ~AnnotationTextShape() override;

    void setAnnotationTextData(KoTextShapeData *textShape);

    // reimplemented
    void paintComponent(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;

    /**
     * From KoShape reimplemented method to load the TextShape from ODF.
     *
     * This method redirects the call to the KoTextShapeData::loadOdf() method which
     * in turn will call the KoTextLoader::loadBody() method that reads the element
     * into a QTextCursor.
     *
     * @param element element which represents the shape in odf.
     * @param context the KoShapeLoadingContext used for loading.
     * @return false if loading failed.
     */
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    /**
     * From KoShape reimplemented method to store the TextShape data as ODF.
     *
     * @param context the KoShapeSavingContext used for saving.
     */
    void saveOdf(KoShapeSavingContext &context) const override;

    void setCreator(const QString &creator);
    QString creator() const;
    void setDate(const QString &date);
    QString date() const;
    void setDateString(const QString &date);
    QString dateString() const;

private:
    KoTextShapeData *m_textShapeData;

    QString m_creator;
    QString m_date;
    QString m_dateString; // another way of storing the date. Not sure when it is used.
};

#endif // ANNOTATIONTEXTSHAPE_H
