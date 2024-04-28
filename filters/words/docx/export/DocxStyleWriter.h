/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2014 Lassi Nieminen <lassniem@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DOCXSTYLEWRITER_H
#define DOCXSTYLEWRITER_H

// Qt
#include <QBuffer>

class QByteArray;

class KoXmlWriter;
class OdfReaderContext;

class DocxStyleWriter
{
public:
    explicit DocxStyleWriter(OdfReaderContext *context);
    virtual ~DocxStyleWriter();

    void read();

    QByteArray documentContent() const;

private:
    OdfReaderContext *m_readerContext;
    QByteArray m_documentContent; // m_documentWriter writes here;
    KoXmlWriter *m_documentWriter; // XML writer for the document contents
    QBuffer m_documentIO; // IODevice for the XMl writer
};

#endif // DOCXSTYLEWRITER_H
