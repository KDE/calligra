/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRPLACEHOLDERSTRATEGY_H
#define KPRPLACEHOLDERSTRATEGY_H

#include <QMap>

class QString;
class QRectF;
class QPainter;
#include "KoXmlReaderForward.h"
class KoShape;
class KoShapeLoadingContext;
class KoShapeSavingContext;
class KoShapeUserData;
class KoViewConverter;
class PlaceholderData;
class KoDocumentResourceManager;
class KoShapePaintingContext;

class KPrPlaceholderStrategy
{
public:
    /**
     * Factory method to create a KPrPlaceholderStrategy
     *
     * @param presentationClass The presentation:class attribute of the placeholder
     */
    static KPrPlaceholderStrategy *create(const QString &presentationClass);

    /**
     * Check if the presentation class is supported
     *
     * @param presentationClass The presentation:class attribute of the placeholder
     */
    static bool supported(const QString &presentationClass);

    virtual ~KPrPlaceholderStrategy();

    virtual KoShape *createShape(KoDocumentResourceManager *documentResources);

    virtual void paint(QPainter &painter, const KoViewConverter &converter, const QRectF &rect, KoShapePaintingContext &paintcontext);

    virtual void saveOdf(KoShapeSavingContext &context);

    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);

    virtual void init(KoDocumentResourceManager *documentResources);

    virtual KoShapeUserData *userData() const;

protected:
    /**
     * @param presentationClass The presentation:class of the shape used for creating a shape of that type
     */
    KPrPlaceholderStrategy(const QString &presentationClass);

    /**
     * Get the text that is displayed
     */
    virtual QString text() const;

    const PlaceholderData *m_placeholderData;
};

#endif /* KPRPLACEHOLDERSTRATEGY_H */
