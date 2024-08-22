/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ARTISTICTEXTSHAPEFACTORY_H
#define ARTISTICTEXTSHAPEFACTORY_H

#include <KoShapeFactoryBase.h>

class KoShape;

class ArtisticTextShapeFactory : public KoShapeFactoryBase
{
public:
    ArtisticTextShapeFactory();
    ~ArtisticTextShapeFactory() override = default;

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = nullptr) const override;
    // reimplemented from KoShapeFactoryBase
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;
};

#endif // ARTISTICTEXTSHAPEFACTORY_H
