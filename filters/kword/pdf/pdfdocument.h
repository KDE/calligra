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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include <QFile>
#include <qdom.h>
//Added by qt3to4:
#include <QByteArray>

#include <KoFilter.h>
#include <KoPageLayout.h>

class Object;
class BaseStream;
class PDFDoc;


namespace PDFImport
{
class DRect;
class Device;
class Data;

class Document
{
 public:
    Document();
    ~Document() { clear(); }

    KoFilter::ConversionStatus init(const QString &name,
                    const QString &ownerPassword, const QString &userPassword);
    void clear();

    QString info(const QByteArray &key) const;
    uint nbPages() const;
    KoOrientation paperOrientation() const;
    DRect paperSize(KoFormat &format) const;
    bool isEncrypted() const;

    void initDevice(Data &);
    void treatPage(uint i);
    void init();
    void dumpPage(uint i);

 private:
    QFile      *_file;
    Object     *_object;
    BaseStream *_fileStream;
    PDFDoc     *_document;
    Device     *_device;
    uint        _imageIndex;

    Document(const Document &);
    Document &operator =(const Document &);
};

} // namespace

#endif
