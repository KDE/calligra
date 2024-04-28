/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRPLACEHOLDERTEXTSTRATEGY_H
#define KPRPLACEHOLDERTEXTSTRATEGY_H

#include "KPrPlaceholderStrategy.h"

class KoShape;

class KPrPlaceholderTextStrategy : public KPrPlaceholderStrategy
{
public:
    ~KPrPlaceholderTextStrategy() override;

    KoShape *createShape(KoDocumentResourceManager *documentResources) override;

    void paint(QPainter &painter, const KoViewConverter &converter, const QRectF &rect, KoShapePaintingContext &paintcontext) override;

    void saveOdf(KoShapeSavingContext &context) override;

    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    void init(KoDocumentResourceManager *documentResources) override;

    KoShapeUserData *userData() const override;

protected:
    KPrPlaceholderTextStrategy(const QString &presentationClass);

    friend class KPrPlaceholderStrategy;

    KoShape *m_textShape;
};

#endif /* KPRPLACEHOLDERTEXTSTRATEGY_H */
