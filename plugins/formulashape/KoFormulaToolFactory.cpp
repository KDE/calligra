/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFormulaToolFactory.h"
#include "KoFormulaTool.h"
#include "KoFormulaShape.h"

#include <KoIcon.h>
#include <KLocalizedString>

KoFormulaToolFactory::KoFormulaToolFactory()
           : KoToolFactoryBase("KoFormulaToolFactoryId")
{
    setToolTip( i18n( "Formula editing" ) );
    setToolType( dynamicToolType() );
    setIconName(koIconName("edittext"));
    setPriority( 1 );
    setActivationShapeId( KoFormulaShapeId );
}

KoFormulaToolFactory::~KoFormulaToolFactory()
{}

KoToolBase* KoFormulaToolFactory::createTool( KoCanvasBase* canvas )
{
    return new KoFormulaTool( canvas );
}

