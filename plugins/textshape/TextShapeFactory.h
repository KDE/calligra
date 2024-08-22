/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TEXTSHAPEFACTORY_H
#define TEXTSHAPEFACTORY_H

#include <KoShapeFactoryBase.h>

class KoShape;

class TextShapeFactory : public KoShapeFactoryBase
{
public:
    /// constructor
    TextShapeFactory();
    ~TextShapeFactory() override = default;

    KoShape *createShape(const KoProperties *params, KoDocumentResourceManager *documentResources = nullptr) const override;
    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = nullptr) const override;
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;

    void newDocumentResourceManager(KoDocumentResourceManager *manager) const override;
};

#endif
