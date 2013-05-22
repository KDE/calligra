/* This file is part of the KDE project

   Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>

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
class QSizeF;
class QFile;
class QStringList;

class KoStore;
class KoXmlWriter;

class DocxFile;


class OdfReaderDocxContext : public OdfReaderContext 
{
 public: 
    OdfReaderDocxContext(KoStore *store, DocxFile *dxf);
    virtual ~OdfReaderDocxContext();

    DocxFile    *m_docxFile;        // Collect embedded files here
    QByteArray   m_documentContent; // m_documentWriter writes here;
    KoXmlWriter *m_documentWriter;  // XML writer for the document contents

private:
    QBuffer      m_documentIO;      // IODevice for the XMl writer
};


#endif // ODFREADERDOCXCONTEXT_H
