/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "Plugin.h"

#include "KoFormPdfExporter.h"
#include "KoFormShape.h"
#include "KoFormShapeFactory.h"
#include "KoFormShapeRenderer.h"
#include "KoFormTool.h"
#include <KoPdfExportRegistry.h>
#include <KoToolRegistry.h>

#include <KPluginFactory>
#include <KoShapeRegistry.h>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "calligra_shape_forms.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoShapeRegistry::instance()->add(new KoFormShapeFactory());
    KoToolRegistry::instance()->add(new KoFormToolFactory());
    registerPdfFormExporter([](const QString &fileName, KoDocument *document, QString *errorMessage) {
        const QList<KoShape *> shapes = registeredFormShapes();
        QList<KoShape *> documentShapes;
        for (KoShape *shape : shapes) {
            auto *formShape = dynamic_cast<KoFormShape *>(shape);
            if (formShape && formShape->document() == document) {
                documentShapes.append(shape);
            }
        }
        return exportFormFieldsToPdf(fileName, collectFormPdfFields(documentShapes), errorMessage);
    });
}

#include "Plugin.moc"
