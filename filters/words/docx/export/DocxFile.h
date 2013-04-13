/* This file is part of the KDE project
   Copyright (C) 2012 Inge Wallin <inge@lysator.liu.se>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef DOCXFILE_H
#define DOCXFILE_H

#include <QHash>

#include <KoFilter.h>

#include "FileCollector.h"


class QString;
class QByteArray;

class KoStore;

class OdtTraverserDocxContext;

class DocxFile : public FileCollector
{
public:
    DocxFile();
    ~DocxFile();

    // When you have created all the content and added it using
    // addContentFile(), call this function once and it will write the
    // docx to the disk.
    KoFilter::ConversionStatus  writeDocx(const QString &fileName,
                                          const QByteArray &appIdentification,
                                          const OdtTraverserDocxContext &context);

private:
    // Privat functions

private:
    // data
};

#endif // DOCXFILE_H
