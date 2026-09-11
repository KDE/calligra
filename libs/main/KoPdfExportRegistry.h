/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#pragma once

#include "komain_export.h"

#include <QString>

class KoDocument;

using KoPdfFormExporter = bool (*)(const QString &fileName, KoDocument *document, QString *errorMessage);

KOMAIN_EXPORT void registerPdfFormExporter(KoPdfFormExporter exporter);
KOMAIN_EXPORT bool exportPdfForms(const QString &fileName, KoDocument *document, QString *errorMessage = nullptr);
