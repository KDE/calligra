
/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2009-2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef MSOOXMLRELATIONSHIPS_H
#define MSOOXMLRELATIONSHIPS_H

#include "komsooxml_export.h"

struct KoOdfWriters;
class QString;

namespace MSOOXML
{
class MsooXmlImport;

//! Global document relationships handler.
/*! It supports delayed loading of any *.xml.rels file. */
//! @todo add write methods and saving support
class KOMSOOXML_EXPORT MsooXmlRelationships
{
public:
    explicit MsooXmlRelationships(MsooXmlImport &importer, KoOdfWriters *writers, QString &errorMessage);

    ~MsooXmlRelationships();

    QString target(const QString &path, const QString &file, const QString &id);

    QString targetForType(const QString &path, const QString &file, const QString &relType);

    unsigned targetCountWithWord(const QString &searchTerm);

private:
    class Private;
    Private *const d;
};

namespace Relationships
{
static const char comments[] = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/comments";
static const char commentAuthors[] = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/commentAuthors";
static const char tableStyles[] = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/tableStyles";
}
}

#endif // MSOOXMLRELATIONSHIPSREADER_H
