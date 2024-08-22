/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Vidhyapria Arunkumar <vidhyapria.arunkumar@nokia.com>
 * SPDX-FileCopyrightText: 2010 Amit Aggarwal <amit.5.aggarwal@nokia.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PLUGINSHAPEFACTORY_H
#define PLUGINSHAPEFACTORY_H

#include <KoShapeFactoryBase.h>

class KoShape;

class PluginShapeFactory : public KoShapeFactoryBase
{
public:
    PluginShapeFactory();
    ~PluginShapeFactory() override = default;

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = nullptr) const override;
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;
};

#endif
