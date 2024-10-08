/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Fela Winkelmolen <fela.kde@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KARBONCALLIGRAPHICSHAPEFACTORY_H
#define KARBONCALLIGRAPHICSHAPEFACTORY_H

#include "KoShapeFactoryBase.h"

class KoShape;

/// Factory for ellipse shapes
class KarbonCalligraphicShapeFactory : public KoShapeFactoryBase
{
public:
    /// constructor
    KarbonCalligraphicShapeFactory();
    ~KarbonCalligraphicShapeFactory() override;
    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = nullptr) const override;
    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;
    // virtual QList<KoShapeConfigWidgetBase*> createShapeOptionPanels();
};

#endif // KARBONCALLIGRAPHICSHAPEFACTORY_H
