/*
 * Copyright (c) 2002 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include <qfile.h>
#include <qdom.h>

#include <koFilter.h>
#include <koGlobal.h>


class Object;
class BaseStream;
class PDFDoc;
class FilterDevice;
class FilterData;
class DRect;

class PdfDocument
{
 public:
    PdfDocument(const QString &name, const QString &ownerPassword,
                const QString &userPassword, KoFilter::ConversionStatus &);
    ~PdfDocument();

    QString info(const QCString &key) const;
    uint nbPages() const;
    KoOrientation paperOrientation() const;
    DRect paperSize(KoFormat &format) const;
    bool isEncrypted() const;

    void initDevice(FilterData &data);
    void treatPage(uint i);

 private:
    QFile        *_file;
    Object       *_object;
    BaseStream   *_fileStream;
    PDFDoc       *_document;
    FilterDevice *_device;
    uint          _imageIndex;
};

#endif
