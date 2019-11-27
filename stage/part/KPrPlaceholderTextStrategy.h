/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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

    void paint( QPainter & painter, const KoViewConverter &converter, const QRectF & rect, KoShapePaintingContext &paintcontext) override;

    void saveOdf( KoShapeSavingContext & context ) override;

    bool loadOdf( const KoXmlElement & element, KoShapeLoadingContext & context ) override;

    void init(KoDocumentResourceManager *documentResources) override;

    KoShapeUserData * userData() const override;

protected:
    KPrPlaceholderTextStrategy( const QString & presentationClass );

    friend class KPrPlaceholderStrategy;

    KoShape * m_textShape;
};

#endif /* KPRPLACEHOLDERTEXTSTRATEGY_H */
