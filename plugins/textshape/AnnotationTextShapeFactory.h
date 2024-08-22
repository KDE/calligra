/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ANNOTATIONTEXTSHAPEFACTORY_H
#define ANNOTATIONTEXTSHAPEFACTORY_H

#include <KoShapeFactoryBase.h>

class KoShape;

class AnnotationTextShapeFactory : public KoShapeFactoryBase
{
public:
    AnnotationTextShapeFactory();
    ~AnnotationTextShapeFactory() override = default;

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources) const override;
    KoShape *createShape(const KoProperties *params, KoDocumentResourceManager *documentResources) const override;
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;
};

#endif // ANNOTATIONTEXTSHAPEFACTORY_H
