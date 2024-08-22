/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoDummyCellValue.h"

KoDummyCellValue::KoDummyCellValue() = default;

KoDummyCellValue::~KoDummyCellValue() = default;

QString KoDummyCellValue::type() const
{
    return QString();
}

QList<QPair<QString, QString>> KoDummyCellValue::attributes() const
{
    return QList<QPair<QString, QString>>();
}
