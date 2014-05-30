/* This file is part of the KDE project

   Copyright (C) 2013 Inge Wallin  <inge@lysator.liu.se>

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


// Own
#include "OdfReaderDocxContext.h"

// Qt
#include <QByteArray>
#include <QBuffer>

// Calligra
#include <KoXmlWriter.h>
//#include <KoXmlReader.h>
//#include <KoOdfStyleManager.h>

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
