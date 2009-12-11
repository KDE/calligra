/*
   This file is part of the KDE project
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <QTextStream>

#include "kword13utils.h"
#include "kword13formatone.h"

KWord13FormatOneData::KWord13FormatOneData(void)
{
}

KWord13FormatOneData::~KWord13FormatOneData(void)
{
}

void KWord13FormatOneData::xmldump(QTextStream& iostream)
{
    iostream << "     <formatone>"  << "\">\n";

    for (QMap<QString, QString>::ConstIterator it = m_properties.constBegin();
            it != m_properties.constEnd();
            ++it) {
        iostream << "       <param key=\"" << it.key() << "\" data=\"" << EscapeXmlDump(it.data()) << "\"/>\n";
    }

    iostream << "    </formatone>\n";
}

QString KWord13FormatOneData::key(void) const
{
    QString strKey;

    // At first, use the number of properties as it is an easy sorting value
    strKey += QString::number(m_properties.count(), 16);
    strKey += ':';

    // use the worst key: the whole QMap (### FIXME)
    for (QMap<QString, QString>::const_iterator it = m_properties.constBegin() ;
            it != m_properties.constEnd(); ++it) {
        strKey += it.key();
        strKey += '=';
        strKey += it.data();
        strKey += ';';
    }

    return strKey;
}

QString KWord13FormatOneData::getProperty(const QString& name) const
{
    QMap<QString, QString>::ConstIterator it(m_properties.find(name));
    if (it == m_properties.end()) {
        // Property does not exist
        return QString();
    } else {
        return it.data();
    }
}

//
//
//

KWord13FormatOne::KWord13FormatOne(void) : m_length(1)
{
    m_id = 1;
}

KWord13FormatOne::~KWord13FormatOne(void)
{
}

int KWord13FormatOne::length(void)
{
    return m_length;
}

KWord13FormatOneData* KWord13FormatOne::getFormatOneData(void)
{
    return &m_formatOne;
}

