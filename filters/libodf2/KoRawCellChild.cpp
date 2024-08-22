/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoRawCellChild.h"

#include <KoGenStyles.h>
#include <KoXmlWriter.h>

KoRawCellChild::KoRawCellChild(const QByteArray &content)
    : m_content(content)
{
}

KoRawCellChild::~KoRawCellChild() = default;

void KoRawCellChild::saveOdf(KoXmlWriter &writer, KoGenStyles &styles) const
{
    Q_UNUSED(styles)

    writer.addCompleteElement(m_content.data());
}
