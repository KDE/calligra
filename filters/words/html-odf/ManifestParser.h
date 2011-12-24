/* This file is part of the KDE project
   Copyright (C) 2012 Stuart Dickson <stuart@kogmbh.com>

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

#ifndef MANIFESTPARSER_H_H
#define MANIFESTPARSER_H

#include "qxml.h"


class ManifestParser : public QXmlDefaultHandler
{
public:    
    bool startDocument();
    bool endElement( const QString&, const QString&, const QString &name );
    bool startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs );
    
    
    QStringList m_fileList;
    
protected:
    QString     m_currentType;
    QString     m_currentPath;
};

#endif