/* This file is part of the KDE project

   Copyright (C) 2012 Inge Wallin            <inge@lysator.liu.se>

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


// Own
#include "OpcContentTypes.h"

// Qt
#include <QString>

// Calligra
#include <KoXmlWriter.h>
#include <KoStore.h>
#include <KoStoreDevice.h>

// This filter
#include "DocxExportDebug.h"


// ================================================================
//                         class OpcContentTypes

OpcContentTypes::OpcContentTypes()
{
}

OpcContentTypes::~OpcContentTypes()
{
}


void OpcContentTypes::addDefault(const QString &extension, const QString &contentType)
{
    defaults[extension] = contentType;
}

void OpcContentTypes::addFile(const QString &partName, const QString &contentType)
{
    parts[partName] = contentType;
}


KoFilter::ConversionStatus OpcContentTypes::writeToStore(KoStore *opcStore)
{
    // We can hardcode this one.
    if (!opcStore->open("[Content_Types].xml")) {
        debugDocx << "Can not to open [Content_Types].xml.";
        return KoFilter::CreationError;
    }

    KoStoreDevice metaDevice(opcStore);
    KoXmlWriter writer(&metaDevice);

    writer.startDocument(0, 0, 0);
    writer.startElement("Types");
    writer.addAttribute("xmlns", "http://schemas.openxmlformats.org/package/2006/content-types");

    // Write defaults
    foreach (const QString &def, defaults.keys()) {
        writer.startElement("Default");
        writer.addAttribute("Extension", def);
        writer.addAttribute("ContentType", defaults.value(def));
        writer.endElement(); // Default
    }

    // Write overrides.  Only write those which don't fit the defaults.
    foreach (const QString &part, parts.keys()) {
        bool found = false;
        foreach (const QString &extension, defaults.keys()) {
            if (part.endsWith(extension) && parts.value(part) == defaults.value(extension)) {
                found = true;
                break;
            }
        }
        if (found) {
            // No need to write this one
            continue;
        }

        writer.startElement("Override");
        writer.addAttribute("PartName", part);
        writer.addAttribute("ContentType", parts.value(part));
        writer.endElement(); // Override
    }


    writer.endElement();  // Types
    writer.endDocument();

    opcStore->close();
    return KoFilter::OK;
}
