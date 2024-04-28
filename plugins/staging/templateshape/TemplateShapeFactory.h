/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TEMPLATESHAPEFACTORY_H
#define TEMPLATESHAPEFACTORY_H

#include <KoShapeFactoryBase.h>

class TemplateShapeFactory : public KoShapeFactoryBase
{
public:
    TemplateShapeFactory();
    ~TemplateShapeFactory()
    {
    }

    virtual KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = 0) const;
    virtual KoShape *createShape(const KoProperties *params, KoDocumentResourceManager *documentResources = 0) const;
    virtual bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const;

    /// reimplemented from KoShapeFactoryBase
    virtual QList<KoShapeConfigWidgetBase *> createShapeOptionPanels();
};

#endif
