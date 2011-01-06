/* This file is part of the KDE project
   Copyright (c) 2001 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

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

#include "PictureShared.h"

#include <QFile>
#include <QImageReader>
#include <QImageWriter>
#include <QPainter>
//Added by qt3to4:
#include <QPixmap>
#include <QMimeData>

#include <kdebug.h>
#include <kurl.h>
#include <kfilterdev.h>
#include <kio/netaccess.h>
#include <QBuffer>
#include "PictureKey.h"
#include "PictureBase.h"
#include "PictureImage.h"
#include "PictureEps.h"
#include "PictureClipart.h"

#include <kcodecs.h>


PictureShared::PictureShared(void) : m_base(NULL)
{
}

void PictureShared::assignPictureId(uint _id)
{
    m_pictureId = _id;
}

QString PictureShared::uniquePictureId() const
{
    return "Pictures" + QString::number(m_pictureId);
}

PictureShared::~PictureShared(void)
{
    delete m_base;
}

PictureShared::PictureShared(const PictureShared &other)
        : Shared() // Some compilers want it explicitly!
{
    // We need to use newCopy, because we want a real copy, not just a copy of the part of PictureBase
    if (other.m_base)
        m_base = other.m_base->newCopy();
    else
        m_base = NULL;
}

PictureShared& PictureShared::operator=(const PictureShared & other)
{
    clear();
    kDebug(30508) << "PictureShared::= before";
    if (other.m_base)
        m_base = other.m_base->newCopy();
    kDebug(30508) << "PictureShared::= after";
    return *this;
}

PictureType::Type PictureShared::getType(void) const
{
    if (m_base)
        return m_base->getType();
    return PictureType::TypeUnknown;
}

bool PictureShared::isNull(void) const
{
    if (m_base)
        return m_base->isNull();
    return true;
}

void PictureShared::draw(QPainter& painter, int x, int y, int width, int height, int sx, int sy, int sw, int sh, bool fastMode)
{
    if (m_base)
        m_base->draw(painter, x, y, width, height, sx, sy, sw, sh, fastMode);
    else {
        // Draw a red box (easier DEBUG)
        kWarning(30508) << "Drawing red rectangle! (PictureShared::draw)";
        painter.save();
        painter.setBrush(QColor(255, 0, 0));
        painter.drawRect(x, y, width, height);
        painter.restore();
    }
}

bool PictureShared::loadTmp(QIODevice* io)
// We have a temp file, probably from a downloaded file
//   We must check the file type
{
    kDebug(30508) << "PictureShared::loadTmp";
    if (!io) {
        kError(30508) << "No QIODevice!" << endl;
        return false;
    }

    QByteArray array(io->readAll());
    return identifyAndLoad(array);
}

bool PictureShared::identifyAndLoad(const QByteArray& _array)
{
    if (_array.size() < 5) {
        kError(30508) << "Picture is less than 5 bytes long!" << endl;
        return false;
    }

    QByteArray array = _array;

    QString strExtension;
    bool flag = false;

    // Try to find the file type by comparing magic on the first few bytes!
    // ### TODO: could not QImageIO::imageFormat do it too? (At least most of them?)
    if ((array[0] == char(0x89)) && (array[1] == 'P') && (array[2] == 'N') && (array[3] == 'G')) {
        strExtension = "png";
    } else if ((array[0] == char(0xff)) && (array[1] == char(0xd8)) && (array[2] == char(0xff)) && (array[3] == char(0xe0))) {
        strExtension = "jpeg";
    } else if ((array[0] == 'B') && (array[1] == 'M')) {
        strExtension = "bmp";
    } else if ((array[0] == '<') && (array[1] == '?') && (array[2] == 'x') && (array[3] == 'm') && (array[4] == 'l')) {
        strExtension = "svg";
    } else if ((array[0] == 'Q') && (array[1] == 'P') && (array[2] == 'I') && (array[3] == 'C')) {
        strExtension = "qpic";
    } else if ((array[0] == '%') && (array[1] == '!') && (array[2] == 'P') && (array[3] == 'S')) {
        strExtension = "eps";
    } else if ((array[0] == char(0xc5)) && (array[1] == char(0xd0)) && (array[2] == char(0xd3)) && (array[3] == char(0xc6))) {
        // So called "MS-DOS EPS file"
        strExtension = "eps";
    } else if ((array[0] == 'G') && (array[1] == 'I') && (array[2] == 'F') && (array[3] == '8')) {
        // GIF (87a or 89a)
        strExtension = "gif";
    } else if ((array[0] == char(0037)) && (array[1] == char(0213))) {
        // Gzip
        QBuffer buffer(&array);
        buffer.open(QIODevice::ReadOnly);

        const bool flag = loadCompressed(&buffer, "application/x-gzip", "tmp");
        buffer.close();
        return flag;
    } else if ((array[0] == 'B') && (array[1] == 'Z') && (array[2] == 'h')) {
        // BZip2
        QBuffer buffer(&array);
        buffer.open(QIODevice::ReadOnly);
        const bool flag = loadCompressed(&buffer, "application/x-bzip", "tmp");
        buffer.close();
        return flag;
    } else {
        kDebug(30508) << "Cannot identify the type of temp file!"
        << " Trying to convert to PNG! (in PictureShared::loadTmp" << endl;

        // Do not trust QBuffer and do not work directly on the QByteArray array
        // DF: It would be faster to work on array here, and to create a completely
        // different QBuffer for the writing code!
        QBuffer buf(&array);
        if (!buf.open(QIODevice::ReadOnly)) {
            kError(30508) << "Could not open read buffer!" << endl;
            return false;
        }

        QImageReader imageReader(&buf);
        QImage image = imageReader.read();
        if (image.isNull()) {
            kError(30508) << "Could not read image!" << endl;
            return false;
        }
        buf.close();

        if (!buf.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            kError(30508) << "Could not open write buffer!" << endl;
            return false;
        }

        QImageWriter imageWriter(&buf, "PNG");

        if (!imageWriter.write(image)) {
            kError(30508) << "Could not write converted image!" << endl;
            return false;
        }
        buf.close();

        array = buf.buffer();

        strExtension = "png";
    }

    kDebug(30508) << "Temp file considered to be" << strExtension;

    clearAndSetMode(strExtension);
    if (m_base)
        flag = m_base->loadData(array, strExtension);
    setExtension(strExtension);

    return flag;
}



bool PictureShared::loadXpm(QIODevice* io)
{
    kDebug(30508) << "PictureShared::loadXpm";
    if (!io) {
        kError(30508) << "No QIODevice!" << endl;
        return false;
    }

    clear();

    // Old KPresenter XPM files have char(1) instead of some "
    // Therefore we need to treat XPM separately

    QByteArray array = io->readAll();

    // As XPM files are normally only ASCII files, we can replace it without problems

    int pos = 0;

    while ((pos = array.indexOf(char(1), pos)) != -1) {
        array[pos] = '"';
    }

    // Now that the XPM file is corrected, we need to load it.

    m_base = new PictureImage();

    QBuffer buffer(&array);
    bool check = m_base->load(&buffer, "xpm");
    setExtension("xpm");
    return check;
}

bool PictureShared::save(QIODevice* io) const
{
    if (!io)
        return false;
    if (m_base)
        return m_base->save(io);
    return false;
}

bool PictureShared::saveAsBase64(KoXmlWriter& writer) const
{
    if (m_base)
        m_base->saveAsBase64(writer);
    return false;
}

void PictureShared::clear(void)
{
    // Clear does not reset the key m_key!
    delete m_base;
    m_base = NULL;
}

void PictureShared::clearAndSetMode(const QString& newMode)
{
    delete m_base;
    m_base = NULL;

    const QString mode = newMode.toLower();

    if ((mode == "svg") || (mode == "qpic")) {
        m_base = new PictureClipart();
    } else if ((mode == "eps") || (mode == "epsi") || (mode == "epsf")) {
        m_base = new PictureEps();
    } else {  // TODO: test if QImageIO really knows the file format
        m_base = new PictureImage();
    }
}

QString PictureShared::getExtension(void) const
{
    return m_extension;
}

void PictureShared::setExtension(const QString& extension)
{
    m_extension = extension;
}

QString PictureShared::getMimeType(void) const
{
    if (m_base)
        return m_base->getMimeType(m_extension);
    return QString(NULL_MIME_TYPE);
}


bool PictureShared::loadFromBase64(const QByteArray& str)
{
    clear();
    return identifyAndLoad(QByteArray::fromBase64(str));
}

bool PictureShared::load(QIODevice* io, const QString& extension)
{
    kDebug(30508) << "PictureShared::load(QIODevice*, const QString&)" << extension;
    bool flag = false;
    QString ext(extension.toLower());
    if (ext == "tmp") // ### TODO: also remote scripts need this, don't they?
        flag = loadTmp(io);
    else if (ext == "bz2") {
        flag = loadCompressed(io, "application/x-bzip", "tmp");
    } else if (ext == "gz") {
        flag = loadCompressed(io, "application/x-gzip", "tmp");
    } else if (ext == "svgz") {
        flag = loadCompressed(io, "application/x-gzip", "svg");
    } else {
        clearAndSetMode(ext);
        if (m_base)
            flag = m_base->load(io, ext);
        setExtension(ext);
    }
    if (!flag) {
        kError(30508) << "File was not loaded! (PictureShared::load)" << endl;
    }
    return flag;
}

bool PictureShared::loadFromFile(const QString& fileName)
{
    kDebug(30508) << "PictureShared::loadFromFile" << fileName;
    if (fileName.isEmpty()) {
        kError(30508) << "Cannot load file with empty name!" << endl;
        return false;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    bool flag = false;
    const int pos = fileName.lastIndexOf('.');
    if (pos == -1) {
        kDebug(30508) << "File with no extension!";
        // As we have no extension, consider it like a temporary file
        flag = loadTmp(&file);
    } else {
        const QString extension(fileName.mid(pos + 1));
        // ### TODO: check if the extension if gz or bz2 and find the previous extension
        flag = load(&file, extension);
    }
    file.close();
    return flag;
}

QSize PictureShared::getOriginalSize(void) const
{
    if (m_base)
        return m_base->getOriginalSize();
    return QSize(0, 0);
}

QPixmap PictureShared::generatePixmap(const QSize& size, bool smoothScale)
{
    if (m_base)
        return m_base->generatePixmap(size, smoothScale);
    return QPixmap();
}

QMimeData* PictureShared::dragObject(QWidget *dragSource, const char *name)
{
    if (m_base)
        return m_base->dragObject(dragSource, name);
    return 0;
}

QImage PictureShared::generateImage(const QSize& size)
{
    if (m_base)
        return m_base->generateImage(size);
    return QImage();
}

bool PictureShared::hasAlphaBuffer() const
{
    if (m_base)
        return m_base->hasAlphaBuffer();
    return false;
}

void PictureShared::setAlphaBuffer(bool enable)
{
    if (m_base)
        m_base->setAlphaBuffer(enable);
}

QImage PictureShared::createAlphaMask(Qt::ImageConversionFlags flags) const
{
    if (m_base)
        return m_base->createAlphaMask(flags);
    return QImage();
}

void PictureShared::clearCache(void)
{
    if (m_base)
        m_base->clearCache();
}

bool PictureShared::loadCompressed(QIODevice* io, const QString& mimeType, const QString& extension)
{
    // ### TODO: check that we do not have an endless recursion
    QIODevice* in = KFilterDev::device(io, mimeType, false);

    if (!in) {
        kError(30508) << "Cannot create device for uncompressing! Aborting!" << endl;
        return false;
    }


    if (!in->open(QIODevice::ReadOnly)) {
        kError(30508) << "Cannot open file for uncompressing! Aborting!" << endl;
        delete in;
        return false;
    }

    const bool flag = load(in, extension);

    in->close();
    delete in;

    return flag;
}
