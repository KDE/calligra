/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOFORMULATOOLFACTORY_H
#define KOFORMULATOOLFACTORY_H

#include <KoToolFactoryBase.h>

/**
 * @short The factory for KoFormulaTool
 *
 * This reimplements the KoToolFactoryBase class from the flake library in order
 * to provide a factory for the KoToolBase based class KoFormulaTool. This is the
 * KoToolBase that is used to edit a KoFormulaShape.
 * This class is part of the FormulaShape plugin and follows the factory design
 * pattern.
 */
class KoFormulaToolFactory : public KoToolFactoryBase
{
public:
    /// The constructor - reimplemented from KoToolFactoryBase
    explicit KoFormulaToolFactory();

    /// The destructor - reimplemented from KoToolFactoryBase
    ~KoFormulaToolFactory() override;

    /// @return an instance of KoFormulaTool
    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif
