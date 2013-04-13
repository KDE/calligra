/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

// Own
#include "FormulaShapePlugin.h"

// KDE
#include <kpluginfactory.h>

// Calligra libs
#include <KoToolRegistry.h>
#include <KoShapeRegistry.h>

// This shape
#include "FormulaShapeFactory.h"
#include "FormulaToolFactory.h"


K_PLUGIN_FACTORY(FormulaShapePluginFactory, registerPlugin<FormulaShapePlugin>();)
K_EXPORT_PLUGIN(FormulaShapePluginFactory("FormulaShape"))

FormulaShapePlugin::FormulaShapePlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoShapeRegistry::instance()->add(new FormulaShapeFactory());

    KoToolRegistry::instance()->add(new FormulaToolFactory());
}


#include <FormulaShapePlugin.moc>
