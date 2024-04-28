/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "TextPlugin.h"
#include "ReferencesToolFactory.h"
#include "ReviewToolFactory.h"
#include "TextToolFactory.h"
#ifdef CREATE_TEXTDOCUMENT_INSPECTOR
#include "TextDocumentInspectionDockerFactory.h"
#endif
#include "AnnotationTextShapeFactory.h"
#include "TextShapeFactory.h"

#include <KoDockRegistry.h>
#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>

#ifdef CREATE_TEXTDOCUMENT_INSPECTOR
#include <KConfigGroup>
#include <KSharedConfig>
#endif
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(TextPluginFactory, "calligra_shape_text.json", registerPlugin<TextPlugin>();)

TextPlugin::TextPlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoToolRegistry::instance()->add(new TextToolFactory());
    KoToolRegistry::instance()->add(new ReviewToolFactory());
    KoToolRegistry::instance()->add(new ReferencesToolFactory());
    KoShapeRegistry::instance()->add(new TextShapeFactory());
    KoShapeRegistry::instance()->add(new AnnotationTextShapeFactory());

#ifdef CREATE_TEXTDOCUMENT_INSPECTOR
    KConfigGroup debugConfigGroup(KSharedConfig::openConfig(), "Debug");
    const bool isInspectorEnabled = (debugConfigGroup.readEntry("EnableTextDocumentInspector", QString()) == "true");

    if (isInspectorEnabled) {
        KoDockRegistry::instance()->add(new TextDocumentInspectionDockerFactory());
    }
#endif
}

#include <TextPlugin.moc>
