/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGSHAPEFACTORY_H
#define SVGSHAPEFACTORY_H

#include "KoShapeFactoryBase.h"
#include "flake_export.h"

/// Use this shape factory to load embedded svg files from odf
class FLAKE_EXPORT SvgShapeFactory : public KoShapeFactoryBase
{
    Q_OBJECT
public:
    SvgShapeFactory();
    ~SvgShapeFactory() override;

    // reimplemented from KoShapeFactoryBase
    bool supports(const KoXmlElement &element, KoShapeLoadingContext &context) const override;
    // reimplemented from KoShapeFactoryBase
    KoShape *createShapeFromOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    /// Adds an instance of this factory to the shape registry, if not already registered
    static void addToRegistry();
};

#endif // SVGSHAPEFACTORY_H
