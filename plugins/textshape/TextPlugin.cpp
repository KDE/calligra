/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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
#include "TextPlugin.h"
#include "TextToolFactory.h"
#include "ReferencesToolFactory.h"
#include "ReviewToolFactory.h"
#ifdef CREATE_TEXTDOCUMENT_INSPECTOR
#include "TextDocumentInspectionDockerFactory.h"
#endif
#include "TextShapeFactory.h"
#include "AnnotationTextShapeFactory.h"

#include <KoShapeRegistry.h>
#include <KoDockRegistry.h>
#include <KoToolRegistry.h>

#ifdef CREATE_TEXTDOCUMENT_INSPECTOR
#include <KSharedConfig>
#include <KConfigGroup>
#endif
#include <kpluginfactory.h>

K_PLUGIN_FACTORY_WITH_JSON(TextPluginFactory, "calligra_shape_text.json",
                 registerPlugin<TextPlugin>();
)

TextPlugin::TextPlugin(QObject * parent, const QVariantList &)
        : QObject(parent)
{
    KoToolRegistry::instance()->add(new TextToolFactory());
    KoToolRegistry::instance()->add(new ReviewToolFactory());
    KoToolRegistry::instance()->add(new ReferencesToolFactory());
    KoShapeRegistry::instance()->add(new TextShapeFactory());
    KoShapeRegistry::instance()->add(new AnnotationTextShapeFactory());

#ifdef CREATE_TEXTDOCUMENT_INSPECTOR
    KConfigGroup debugConfigGroup( KSharedConfig::openConfig(), "Debug");
    const bool isInspectorEnabled  = (debugConfigGroup.readEntry("EnableTextDocumentInspector", QString()) == "true");

    if (isInspectorEnabled) {
        KoDockRegistry::instance()->add(new TextDocumentInspectionDockerFactory());
    }
#endif
}

#include <TextPlugin.moc>
