/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoFormulaShapePlugin.h"
#include "KoFormulaShapeFactory.h"
#include "KoFormulaToolFactory.h"

#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>

#include <KPluginFactory>

#ifndef _MSC_VER
#include "KoM2MMLForumulaTool.h"
#endif

K_PLUGIN_FACTORY_WITH_JSON(KoFormulaShapePluginFactory, "calligra_shape_formula.json", registerPlugin<KoFormulaShapePlugin>();)

KoFormulaShapePlugin::KoFormulaShapePlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoToolRegistry::instance()->add(new KoFormulaToolFactory());
#ifndef _MSC_VER
    KoToolRegistry::instance()->add(new KoM2MMLFormulaToolFactory());
#endif
    KoShapeRegistry::instance()->add(new KoFormulaShapeFactory());
}

KoFormulaShapePlugin::~KoFormulaShapePlugin()
{
}

#include "KoFormulaShapePlugin.moc"
