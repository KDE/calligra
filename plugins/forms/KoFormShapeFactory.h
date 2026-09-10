/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <KoShapeFactoryBase.h>

class KoFormShapeFactory final : public KoShapeFactoryBase
{
public:
    KoFormShapeFactory();

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = nullptr) const override;
    KoShape *createShape(const KoProperties *properties, KoDocumentResourceManager *documentResources = nullptr) const override;
    bool supports(const KoXmlElement &element, KoShapeLoadingContext &context) const override;
};
