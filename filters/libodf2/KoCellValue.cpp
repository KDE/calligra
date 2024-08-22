/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoCellValue.h"

#include <KoXmlWriter.h>

#include <QMap>

KoCellValue::KoCellValue() = default;

KoCellValue::~KoCellValue() = default;

void KoCellValue::saveOdf(KoXmlWriter &writer) const
{
    if (!type().isEmpty()) {
        writer.addAttribute("office:value-type", type());
    }

    typedef QPair<QString, QString> Attribute;
    foreach (const Attribute &attribute, attributes()) {
        // TODO is this safe? I think that it didn't use to be
        writer.addAttribute(attribute.first.toLatin1(), attribute.second);
    }
}
