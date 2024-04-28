/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2012-2014 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "OdsReader.h"

// Qt
#include <QBuffer>
#include <QStringList>

// KF5
#include <KLocalizedString>

// Calligra
#include <KoOdfReadStore.h>
#include <KoStore.h>
#include <KoXmlNS.h>
#include <KoXmlStreamReader.h>
#include <KoXmlWriter.h> // For copyXmlElement

// Reader library
#include "OdfReaderContext.h"
#include "OdfReaderDebug.h"
#include "OdfTextReader.h"
#include "OdsReaderBackend.h"

#if 0
static int debugIndent = 0;
#define DEBUGSTART()                                                                                                                                           \
    ++debugIndent;                                                                                                                                             \
    DEBUG_READING("entering")
#define DEBUGEND()                                                                                                                                             \
    DEBUG_READING("exiting");                                                                                                                                  \
    --debugIndent
#define DEBUG_READING(param)                                                                                                                                   \
    debugOdfReader << QString("%1").arg(" ", debugIndent * 2) << param << ": "                                                                                 \
                   << (reader.isStartElement() ? "start" : (reader.isEndElement() ? "end" : "other")) << reader.qualifiedName().toString()
#else
#define DEBUGSTART() // NOTHING
#define DEBUGEND() // NOTHING
#define DEBUG_READING(param) // NOTHING
#endif

OdsReader::OdsReader()
    : OdfReader()
{
}

OdsReader::~OdsReader()
{
}

#if 0
// This is a template function for the reader library.
// Copy this one and change the name and fill in the code.
void OdsReader::readElementNamespaceTagname(KoXmlStreamReader &reader)
{ 
   DEBUGSTART();

    // <namespace:tagname> has the following children in ODF 1.2:
    //   FILL IN THE CHILDREN LIKE THIS EXAMPLE (taken from office:document-content):
    //          <office:automatic-styles> 3.15.3
    //          <office:body> 3.3
    //          <office:font-face-decls> 3.14
    //          <office:scripts> 3.12.
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "office:automatic-styles") {
            // FIXME: NYI
        }
        else if (tagName == "office:body") {
            readElementOfficeBody(reader);
        }
        ...  MORE else if () HERE
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementNamespaceTagname(reader, m_context);
    DEBUGEND();
}
#endif

// Reimplemented from OdfReader
void OdsReader::readElementOfficeSpreadsheet(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    OdsReaderBackend *backend = dynamic_cast<OdsReaderBackend *>(m_backend);
    backend->elementOfficeSpreadsheet(reader, m_context);

    // <office:spreadsheet> has the following children in ODF 1.2:
    //
    //          <table:calculation-settings> 9.4.1
    //          <table:consolidation> 9.7
    //          <table:content-validations> 9.4.4
    //          <table:database-ranges> 9.4.14
    //          <table:data-pilot-tables> 9.6.2
    //          <table:dde-links> 9.8
    //          <table:label-ranges> 9.4.10
    //          <table:named-expressions> 9.4.11
    //   [done] <table:table> 9.1.2
    //          <table:tracked-changes> 9.9.2
    //          <text:alphabetical-index-auto-mark-file> 8.8.3
    //          <text:dde-connection-decls> 14.6.2
    //          <text:sequence-decls> 7.4.11
    //          <text:user-field-decls> 7.4.7
    //          <text:variable-decls> 7.4.2

    //
    // FIXME: For now, only very few of these are handled.
    while (reader.readNextStartElement()) {
        DEBUG_READING("loop-start");

        QString tagName = reader.qualifiedName().toString();
        if (tagName == "table:table") {
            if (m_textReader) {
                // <table:table> is handled in the text reader even in spreadsheets.
                m_textReader->readElementTableTable(reader);
            } else {
                reader.skipCurrentElement();
            }
        } else if (tagName == "table:calculation-settings") {
            // FIXME: NYI
            reader.skipCurrentElement();
        } else if (tagName == "table:consolidation") {
            reader.skipCurrentElement();
        } else if (tagName == "table:content-validation") {
            reader.skipCurrentElement();
        } else if (tagName == "table:database-ranges") {
            reader.skipCurrentElement();
        } else if (tagName == "table:data-pilot-tables") {
            reader.skipCurrentElement();
        } else if (tagName == "table:dde-links") {
            reader.skipCurrentElement();
        } else if (tagName == "table:label-ranges") {
            reader.skipCurrentElement();
        } else if (tagName == "table:named-expressions") {
            reader.skipCurrentElement();
        } else if (tagName == "table:tracked-changes") {
            reader.skipCurrentElement();
        } else if (tagName == "text:alphabetical-index-auto-mark-file") {
            reader.skipCurrentElement();
        } else if (tagName == "text:dde-connection-decls") {
            reader.skipCurrentElement();
        } else if (tagName == "text:sequence-decls") {
            reader.skipCurrentElement();
        } else if (tagName == "text:user-field-decls") {
            reader.skipCurrentElement();
        } else if (tagName == "text:variable-decls") {
            reader.skipCurrentElement();
        } else {
            reader.skipCurrentElement();
        }
        DEBUG_READING("loop-end");
    }

    backend->elementOfficeSpreadsheet(reader, m_context);
    DEBUGEND();
}

// ----------------------------------------------------------------
//                             Other functions
