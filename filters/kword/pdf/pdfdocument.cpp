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

#include "pdfdocument.h"

#include <math.h>

#include <kdebug.h>
#include <kglobal.h>

#include "GlobalParams.h"
#include "PDFDoc.h"

#include "FilterDevice.h"
#include "misc.h"


namespace PDFImport
{

Document::Document()
    : _file(0), _object(0), _fileStream(0), _document(0), _device(0)
{}

KoFilter::ConversionStatus
Document::init(const QString &name, const QString &ownerPassword,
               const QString &userPassword)
{
    clear();

    _file = new QFile(name);
    if( !_file->open(IO_ReadOnly) ) return KoFilter::FileNotFound;

    FILE *fd = fdopen(_file->handle(), "r");
    if ( fd==0 ) return KoFilter::InternalError;

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

    if ( !_document->isOk() ) return KoFilter::WrongFormat;
    Font::init();
    return KoFilter::OK;
}

void Document::clear()
{
    Font::cleanup();
    delete _device;
    _device = 0;
    delete _document;
    _document = 0;
    // _fileStream is deleted by PDFDoc
    // _object is free()ed by FileStream
    _object = 0;
    _fileStream = 0;
    delete globalParams;
    globalParams = 0;
    delete _file;
    _file = 0;
    _imageIndex = 1;
}

QString Document::info(const QCString &key) const
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

uint Document::nbPages() const
{
    return _document->getNumPages();
}

KoOrientation Document::paperOrientation() const
{
    if ( nbPages()==0 ) return PG_PORTRAIT;
    return ( _document->getPageWidth(1)<_document->getPageHeight(1) ?
             PG_PORTRAIT : PG_LANDSCAPE );
}

DRect Document::paperSize(KoFormat &format) const
{
    KoOrientation orientation = paperOrientation();

    double w, h;
    if ( nbPages()==0 ) {
        format = PG_DIN_A4;
        w = mmToPoint(KoPageFormat::width(format, orientation));
        h = mmToPoint(KoPageFormat::height(format, orientation));
        return DRect(0, w, 0, h);
    }

    w = _document->getPageWidth(1);
    h = _document->getPageHeight(1);
    format = PG_CUSTOM;
    double min = kMin(w, h);
    double max = kMax(w, h);
    double best = 2;
    double width = w;
    double height = h;
    for (uint i=0; i<=PG_LAST_FORMAT; i++) {
        if ( i==PG_CUSTOM || i==PG_SCREEN ) continue; // #### koffice 1.2
        w = mmToPoint(KoPageFormat::width(KoFormat(i), orientation));
        h = mmToPoint(KoPageFormat::height(KoFormat(i), orientation));
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
    return DRect(0, width, 0, height);
}

void Document::initDevice(Data &data)
{
    Q_ASSERT( _device==0 );
    _device = new Device(data);
}

void Document::init()
{
    _device->init();
}

void Document::treatPage(uint i)
{
    if (i == 0)
        i = 1;
    _document->displayPage(_device, i, int(72*1), 0, gTrue);
}

bool Document::isEncrypted() const
{
    return _document->getXRef()->isEncrypted();
}

void Document::dumpPage(uint i)
{
    _device->dumpPage(i);
}

} // namespace
