/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013-2014 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
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

    QByteArray documentContent() const
    {
        return m_documentContent;
    }
    QByteArray commentsContent() const
    {
        return m_commentsContent;
    }

private:
    // These members should be accessible to the backend but nobody else.
    // Exception: see getter above.
    friend class OdtReaderDocxBackend;
    friend class OdfTextReaderDocxBackend;

    DocxFile *m_docxFile; //!< Collect embedded files here
    QByteArray m_documentContent; //!< m_documentWriter writes here;
    QByteArray m_commentsContent;
    KoXmlWriter *m_documentWriter; //!< XML writer for the document contents
    KoXmlWriter *m_commentsWriter; //!< XML writer for the comments

    // These members should not be accessed from the outside at all.
    QBuffer m_documentIO; //!< IODevice for the XML writer
    QBuffer m_commentsIO; //!< IODevice for the Comments XML Writer
};

#endif // ODFREADERDOCXCONTEXT_H
