/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef THREEDSHAPEFACTORY_H
#define THREEDSHAPEFACTORY_H

#include <KoShapeFactoryBase.h>

class ThreedShapeFactory : public KoShapeFactoryBase
{
public:
    ThreedShapeFactory();
    ~ThreedShapeFactory() override = default;

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = nullptr) const override;
    KoShape *createShape(const KoProperties *params, KoDocumentResourceManager *documentResources = nullptr) const override;
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;

    /// reimplemented
    QList<KoShapeConfigWidgetBase *> createShapeOptionPanels() override;
};

#endif
