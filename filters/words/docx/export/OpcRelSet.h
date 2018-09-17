/* This file is part of the KDE project

   Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>

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

#ifndef OPCRELSET_H
#define OPCRELSET_H

#include <QHash>

#include <KoFilter.h>

#include "FileCollector.h"


class QString;



/**
 * @brief this class manages a set of relationships in a file in the OPC format
 *
 * OPC is an abbreviation for the Open Package Conventions and is a
 * format that is the basis for several other file formats.  Examples
 * include MS OOXML file formats docx, xlsx and pptx but also many
 * others.
 *
 * This class handles the contents of one .rels file, i.e. the
 * relations for one file inside the OPC store.  Lines in that .rels
 * file looks like the following:
 *
 *   <Relationship
 *     Id="rId1"
 *     Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles"
 *     Target="styles.xml"
 *   />
 */
class OpcRelSet
{
public:
    OpcRelSet();
    ~OpcRelSet();

    // FIXME: So far we only design this for writing.  In the general
    //        case we should also lookup and reading.

    // Return the ID
    QString addRelationship(const QString &type, const QString &target);

private:
    // Privat functions

private:
    // data

    // FIXME: Hmmm, I want to use a QHash here but which of the 3
    //        attributes above should we use as key(s)?
};

#endif // OPCRELSET_H
