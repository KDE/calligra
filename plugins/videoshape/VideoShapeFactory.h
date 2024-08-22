/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef VIDEOSHAPEFACTORY_H
#define VIDEOSHAPEFACTORY_H

#include <KoShapeFactoryBase.h>

class KoShape;

class VideoShapeFactory : public KoShapeFactoryBase
{
public:
    VideoShapeFactory();
    ~VideoShapeFactory() = default;

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = nullptr) const override;
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;

    /// reimplemented
    QList<KoShapeConfigWidgetBase *> createShapeOptionPanels() override;
    void newDocumentResourceManager(KoDocumentResourceManager *manager) const override;
};

#endif
