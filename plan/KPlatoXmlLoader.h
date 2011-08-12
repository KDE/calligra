/* This file is part of the KDE project
 Copyright (C) 2010 Dag Andersen <danders@get2net.dk>

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
 Boston, MA 02110-1301, USA.
*/

#ifndef KPLATOXMLLOADER_H
#define KPLATOXMLLOADER_H

#include "KPlatoXmlLoaderBase.h"

#include "KoXmlReaderForward.h"

#include <QString>
#include <QObject>

namespace KPlato
{
    class Config;
    class Package;
    class XMLLoaderObject;
    class Project;

class KPlatoXmlLoader : public KPlatoXmlLoaderBase
{
    Q_OBJECT
public:
    KPlatoXmlLoader( XMLLoaderObject &loader, Project *project );

    QString errorMessage() const;
    Package *package() const;
    QString timeTag() const;

    using KPlatoXmlLoaderBase::load;
    bool load( const KoXmlElement& plan );
    bool loadWorkpackage( const KoXmlElement &plan );

private:
    XMLLoaderObject &m_loader;
    Project *m_project;
    QString m_message;
    Package *m_package;
    QString m_timeTag;
};

}

#endif // KPLATOXMLLOADER_H
