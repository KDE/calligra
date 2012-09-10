/* This file is part of the KDE project
   Copyright (C) 2012 Stuart Dickson <stuartmd@kogmbh.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "ManifestParser.h"


bool ManifestParser::startDocument()
{
    m_currentType = QString();
    m_currentPath = QString();
    return true;
}

bool ManifestParser::endElement(const QString &, const QString &, const QString &name)
{
    m_currentType = QString();
    m_currentPath = QString();
    return true;
}

bool ManifestParser::startElement(const QString &, const QString &, const QString &name, const QXmlAttributes &attrs)
{
    if (name == "manifest:file-entry") {
        QString attrName;

        for (int i=0; i<attrs.count(); i++) {
            attrName = attrs.localName(i);
            if (attrName == "media-type") {
                m_currentType = attrs.value(i);
            } else if(attrName == "full-path") {
                m_currentPath = attrs.value(i);
            }
        }
        if (!m_currentType.isEmpty() && !m_currentPath.isEmpty()) {
            if (m_currentType == "image/png") {
                // We support this file type
                m_fileList += m_currentPath;

                m_currentType = QString();
                m_currentPath = QString();
            }
        }
    }

    return true;
}

QStringList ManifestParser::fileList() const
{
    return m_fileList;
}