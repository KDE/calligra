/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KORECTANGLESHAPEFACTORY_H
#define KORECTANGLESHAPEFACTORY_H

#include "KoShapeFactoryBase.h"

class KoShape;

/// Factory for path shapes
class RectangleShapeFactory : public KoShapeFactoryBase
{
public:
    /// constructor
    RectangleShapeFactory();
    ~RectangleShapeFactory() override = default;
    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = nullptr) const override;
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;
    QList<KoShapeConfigWidgetBase *> createShapeOptionPanels() override;
};

#endif
