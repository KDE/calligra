/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPATHSHAPEFACTORY_H
#define KOPATHSHAPEFACTORY_H

#include "KoShapeFactoryBase.h"

#include "KoXmlReader.h"

class KoShape;

/// Factory for path shapes.
class FLAKE_EXPORT KoPathShapeFactory : public KoShapeFactoryBase
{
    Q_OBJECT
public:
    /// constructor
    explicit KoPathShapeFactory(const QStringList &);
    ~KoPathShapeFactory() override = default;
    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = nullptr) const override;
    bool supports(const KoXmlElement &element, KoShapeLoadingContext &context) const override;
    /// reimplemented
    void newDocumentResourceManager(KoDocumentResourceManager *manager) const override;
};

#endif
