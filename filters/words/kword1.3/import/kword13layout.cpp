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
#include "kword13layout.h"

KWord13Layout::KWord13Layout(void) : m_outline(false)
{
}

KWord13Layout::~KWord13Layout(void)
{
}

void KWord13Layout::xmldump(QTextStream& iostream)
{
    iostream << "    <layout name=\"" << EscapeXmlDump(m_name)
    << "\" outline=\"" << (m_outline ? QString("true") : QString("false")) << "\">\n";

    for (QMap<QString, QString>::ConstIterator it = m_layoutProperties.constBegin();
            it != m_layoutProperties.constEnd();
            ++it) {
        iostream << "     <param key=\"" << it.key() << "\" data=\"" << EscapeXmlDump(it.data()) << "\"/>\n";
    }

    m_format.xmldump(iostream);

    iostream << "    </layout>\n";
}

QString KWord13Layout::key(void) const
{
    QString strKey;

    strKey += m_name;
    strKey += '@';

    // Use the number of properties as it is an easy sorting value
    strKey += QString::number(m_layoutProperties.count(), 16);
    strKey += ':';

    if (m_outline)
        strKey += "O1,";
    else
        strKey += "O0,";

    // use the worst key: the whole QMap (### FIXME)
    for (QMap<QString, QString>::const_iterator it = m_layoutProperties.constBegin() ;
            it != m_layoutProperties.constEnd(); ++it) {
        strKey += it.key();
        strKey += '=';
        strKey += it.data();
        strKey += ';';
    }

    strKey += '@';
    // At the end, the key from the <FORMAT id="1">
    strKey += m_format.key();

    return strKey;
}

QString KWord13Layout::getProperty(const QString& name) const
{
    QMap<QString, QString>::ConstIterator it(m_layoutProperties.find(name));
    if (it == m_layoutProperties.end()) {
        // Property does not exist
        return QString();
    } else {
        return it.data();
    }
}

