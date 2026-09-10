/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#pragma once

#include <KoOdfForm.h>

class QPainter;
class QImage;
class QSizeF;
class KoViewConverter;

void paintFormControl(QPainter &painter,
                      const KoViewConverter &converter,
                      const QSizeF &size,
                      const QString &kind,
                      const KoOdfForm::Control *control,
                      const QImage &image);
