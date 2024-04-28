/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "OdfReaderDocxContext.h"

// Qt
#include <QBuffer>
#include <QByteArray>

// Calligra
#include <KoXmlWriter.h>
// #include <KoXmlReader.h>
// #include <KoOdfStyleManager.h>

// This filter
#include "DocxFile.h"

// ----------------------------------------------------------------
//                     class OdfReaderDocxContext

OdfReaderDocxContext::OdfReaderDocxContext(KoStore *store, DocxFile *dxf)
    : OdfReaderContext(store) // Collect metadata, manifest and styles
    , m_docxFile(dxf)
    , m_documentContent()
    , m_commentsContent()
    , m_documentIO(&m_documentContent)
    , m_commentsIO(&m_commentsContent)
{
    m_documentWriter = new KoXmlWriter(&m_documentIO);
    m_commentsWriter = new KoXmlWriter(&m_commentsIO);
}

OdfReaderDocxContext::~OdfReaderDocxContext()
{
    delete m_documentWriter;
}
