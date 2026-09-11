/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#pragma once

#include "KoFormShapeRenderer.h"

#include <QString>

/** Add the collected form fields to an already rendered PDF file. */
bool exportFormFieldsToPdf(const QString &fileName, const QList<FormPdfField> &fields, QString *errorMessage = nullptr);
