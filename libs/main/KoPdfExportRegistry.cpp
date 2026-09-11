/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPdfExportRegistry.h"

namespace
{
KoPdfFormExporter s_formExporter = nullptr;
}

void registerPdfFormExporter(KoPdfFormExporter exporter)
{
    s_formExporter = exporter;
}

bool exportPdfForms(const QString &fileName, KoDocument *document, QString *errorMessage)
{
    if (!s_formExporter) {
        return true;
    }
    return s_formExporter(fileName, document, errorMessage);
}
