/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PICTURESHAPEFACTORY_H
#define PICTURESHAPEFACTORY_H

#include <KoShapeFactoryBase.h>

class PictureShapeFactory : public KoShapeFactoryBase
{
public:
    PictureShapeFactory();
    ~PictureShapeFactory() override = default;

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = nullptr) const override;
    KoShape *createShape(const KoProperties *params, KoDocumentResourceManager *documentResources = nullptr) const override;
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;

    /// reimplemented
    void newDocumentResourceManager(KoDocumentResourceManager *manager) const override;
    /// reimplemented
    QList<KoShapeConfigWidgetBase *> createShapeOptionPanels() override;
};

#endif
