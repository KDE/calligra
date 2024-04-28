/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef VECTORSHAPE_FACTORY_H
#define VECTORSHAPE_FACTORY_H

// Calligra
#include <KoShapeFactoryBase.h>

class KoShape;

class VectorShapeFactory : public KoShapeFactoryBase
{
public:
    /// constructor
    VectorShapeFactory();
    ~VectorShapeFactory() override
    {
    }

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = 0) const override;

    /// Reimplemented
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;

    QList<KoShapeConfigWidgetBase *> createShapeOptionPanels() override;
};

#endif
