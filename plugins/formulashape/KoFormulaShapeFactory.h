/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOFORMULASHAPEFACTORY_H
#define KOFORMULASHAPEFACTORY_H

#include <KoShapeFactoryBase.h>

class KoShape;

/**
 * @short Factory for the formula shape
 *
 * This class is a part of the FormulaShape plugin and provides a generic
 * way to obtain instances of the KoFormulaShape class.
 * It follows the factory design pattern and implements the two virtual methods
 * createDefaultShape() and createShape() of KoShapeFactoryBase.
 */
class KoFormulaShapeFactory : public KoShapeFactoryBase
{
public:
    /// The constructor - reimplemented from KoShapeFactoryBase
    explicit KoFormulaShapeFactory();

    /// The destructor - reimplemented from KoShapeFactoryBase
    ~KoFormulaShapeFactory() override;

    /// reimplemented
    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = nullptr) const override;

    bool supports(const KoXmlElement &e, KoShapeLoadingContext &context) const override;
};

#endif // KOFORMULASHAPEFACTORY_H
