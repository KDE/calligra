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

#include "pdfdocument.h"

#include <math.h>

#include <kdebug.h>
#include <kglobal.h>

#include "GlobalParams.h"
#include "PDFDoc.h"

#include "FilterDevice.h"
#include "misc.h"

using namespace PDFImport;

PdfDocument::PdfDocument(const QString &name, const QString &ownerPassword,
               const QString &userPassword, KoFilter::ConversionStatus &result)
    : _object(0), _fileStream(0), _document(0), _device(0), _imageIndex(1)
{
    _file = new QFile(name);
    if( !_file->open(IO_ReadOnly) ) {
        kdError(30516) << "Unable to open input file!" << endl;
        result =  KoFilter::FileNotFound;
        return;
    }

    FILE *fd = fdopen(_file->handle(), "r");
    if ( fd==0 ) {
        kdError(30516) << "Unable to obtain FILE*" << endl;
        result =  KoFilter::InternalError;
        return;
    }

    globalParams = new GlobalParams(0);
    globalParams->setErrQuiet(gFalse);

    _object = new Object;
    _object->initNull();
    _fileStream = new FileStream(fd, 0, gFalse, 0, _object);
    GString *owner =
        (ownerPassword.isEmpty() ? 0 : new GString(ownerPassword.latin1()));
    GString *user =
        (userPassword.isEmpty() ? 0 : new GString(userPassword.latin1()));
    _document = new PDFDoc(_fileStream, owner, user);
    delete user;
    delete owner;

    if ( !_document->isOk() ) {
        kdError(30516) << "Unrecognized file!" << endl;
        result = KoFilter::WrongFormat;
        return;
    }

    result = KoFilter::OK;
    FilterFont::defaultFont = new FilterFont;
}

PdfDocument::~PdfDocument()
{
    delete FilterFont::defaultFont;
    delete _device;
    delete _document;
    // _fileStream is deleted by PDFDoc
    // _object is free()ed by FileStream
    delete _object;
    delete globalParams;
    delete _file;
}

QString PdfDocument::info(const QCString &key) const
{
    QString res;
    Object info;
    _document->getDocInfo(&info);
    if ( info.isDict() ) {
        Object tmp;
        if ( info.getDict()->lookup(key.data(), &tmp)->isString() ) {
            GString *s = tmp.getString();
            bool isUnicode = false;
            int i = 0;
            Unicode u;
            if ( (s->getChar(0) & 0xff)==0xfe &&
                 (s->getChar(1) & 0xff)==0xff ) {
                isUnicode = true;
                i = 2;
            }
            while ( i<s->getLength() ) {
                if (isUnicode) {
                    u = ( (s->getChar(i) & 0xff)<<8 ) |
                        (s->getChar(i+1) & 0xff);
                    i++;
                } else u = s->getChar(i) & 0xff;
                i++;
                res += QChar(u);
            }
            tmp.free();
        }
    }
    info.free();
    return res;
}

uint PdfDocument::nbPages() const
{
    return _document->getNumPages();
}

KoOrientation PdfDocument::paperOrientation() const
{
    if ( nbPages()==0 ) return PG_PORTRAIT;
    return ( _document->getPageWidth(1)<_document->getPageHeight(1) ?
             PG_PORTRAIT : PG_LANDSCAPE );
}

DRect PdfDocument::paperSize(KoFormat &format) const
{
    KoOrientation orientation = paperOrientation();

    DRect rect;
    rect.top = 0;
    rect.left = 0;
    double w, h;
    if ( nbPages()==0 ) {
        format = PG_DIN_A4;
        w = toPoint(KoPageFormat::width(format, orientation));
        h = toPoint(KoPageFormat::height(format, orientation));
        rect.right = w;
        rect.bottom = h;
        return rect;
    }

    w = _document->getPageWidth(1);
    h = _document->getPageHeight(1);
    format = PG_CUSTOM;
    double min = kMin(w, h);
    double max = kMax(w, h);
    double best = 2;
    double width = w;
    double height = h;
    for (uint i=0; i<22/*PG_LAST_FORMAT*/; i++) { // #### koffice 1.2
        if ( i==PG_CUSTOM || i==PG_SCREEN ) continue; // #### koffice 1.2
        w = toPoint(KoPageFormat::width(KoFormat(i), orientation));
        h = toPoint(KoPageFormat::height(KoFormat(i), orientation));
        double v = fabs(min/w - 1) + fabs(max/h - 1);
        if ( v<best ) {
            best = v;
            if ( best<0.1 ) {
                format = KoFormat(i);
                width = w;
                height = h;
            }
        }
    }

    rect.bottom = height;
    rect.right = width;
    return rect;
}

void PdfDocument::initDevice(FilterData &data)
{
    Q_ASSERT( _device==0 );
    _device = new FilterDevice(data);
}

void PdfDocument::treatPage(uint i)
{
    _document->displayPage(_device, i, static_cast<int>(72*1), 0, gTrue);
}

bool PdfDocument::isEncrypted() const
{
    return _document->getXRef()->isEncrypted();
}
