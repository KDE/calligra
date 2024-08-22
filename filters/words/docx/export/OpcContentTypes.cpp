/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "OpcContentTypes.h"

// Qt
#include <QString>

// Calligra
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoXmlWriter.h>

// This filter
#include "DocxExportDebug.h"

// ================================================================
//                         class OpcContentTypes

OpcContentTypes::OpcContentTypes() = default;

OpcContentTypes::~OpcContentTypes() = default;

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

    writer.startDocument(nullptr, nullptr, nullptr);
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

    writer.endElement(); // Types
    writer.endDocument();

    opcStore->close();
    return KoFilter::OK;
}
