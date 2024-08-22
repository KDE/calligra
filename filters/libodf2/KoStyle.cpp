/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoStyle.h"

#include <KoGenStyles.h>

KoStyle::KoStyle()
    : m_autoStyleInStylesDotXml(false)
{
}

KoStyle::~KoStyle() = default;

void KoStyle::setName(const QString &name)
{
    m_name = name;
}

QString KoStyle::name() const
{
    return m_name;
}

void KoStyle::setAutoStyleInStylesDotXml(bool b)
{
    m_autoStyleInStylesDotXml = b;
}

bool KoStyle::autoStyleInStylesDotXml() const
{
    return m_autoStyleInStylesDotXml;
}

KoGenStyles::InsertionFlags KoStyle::insertionFlags() const
{
    if (m_name.isEmpty()) {
        return KoGenStyles::NoFlag;
    } else {
        return KoGenStyles::DontAddNumberToName | KoGenStyles::AllowDuplicates;
    }
}

QString KoStyle::saveOdf(KoGenStyles &styles) const
{
    KoGenStyle::Type type;
    if (m_name.isEmpty()) {
        type = automaticstyleType();
    } else {
        type = styleType();
    }
    KoGenStyle style(type, styleFamilyName());
    prepareStyle(style);
    style.setAutoStyleInStylesDotXml(m_autoStyleInStylesDotXml);

    QString styleName = m_name;
    if (styleName.isEmpty()) {
        styleName = defaultPrefix();
    }

    return styles.insert(style, styleName, insertionFlags());
}
