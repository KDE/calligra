/* This file is part of the KDE project

   Copyright (C) 2014 Lassi Nieminen <lassniem@gmail.com>

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
    QByteArray   m_documentContent; // m_documentWriter writes here;
    KoXmlWriter *m_documentWriter;  // XML writer for the document contents
    QBuffer      m_documentIO;      // IODevice for the XMl writer
};


#endif // DOCXSTYLEWRITER_H
