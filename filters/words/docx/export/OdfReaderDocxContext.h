/* This file is part of the KDE project

   Copyright (C) 2013-2014 Inge Wallin <inge@lysator.liu.se>

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

#ifndef ODFREADERDOCXCONTEXT_H
#define ODFREADERDOCXCONTEXT_H

// Qt
#include <QBuffer>
#include <QHash>
#include <QTextStream>

// Calligra
#include <KoFilter.h>

// libodfreader
#include "OdfReaderContext.h"


class QByteArray;

class KoStore;
class KoXmlWriter;

class DocxFile;


class OdfReaderDocxContext : public OdfReaderContext
{
 public:
    OdfReaderDocxContext(KoStore *store, DocxFile *dxf);
    ~OdfReaderDocxContext() override;

    QByteArray documentContent() const { return m_documentContent; }
    QByteArray commentsContent() const { return m_commentsContent; }

 private:

    // These members should be accessible to the backend but nobody else.
    // Exception: see getter above.
    friend class OdtReaderDocxBackend;
    friend class OdfTextReaderDocxBackend;

    DocxFile    *m_docxFile;        //!< Collect embedded files here
    QByteArray   m_documentContent; //!< m_documentWriter writes here;
    QByteArray   m_commentsContent;
    KoXmlWriter *m_documentWriter;  //!< XML writer for the document contents
    KoXmlWriter *m_commentsWriter;  //!< XML writer for the comments

    // These members should not be accessed from the outside at all.
    QBuffer      m_documentIO;      //!< IODevice for the XML writer
    QBuffer      m_commentsIO;        //!< IODevice for the Comments XML Writer
};


#endif // ODFREADERDOCXCONTEXT_H
