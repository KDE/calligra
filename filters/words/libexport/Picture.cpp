/* This file is part of the KDE project
   Copyright (c) 2001 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2002, 2003 Nicolas GOUTTE <goutte@kde.org>

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

#include "Picture.h"

#include "PictureKey.h"
#include "PictureShared.h"
#include "PictureBase.h"

#include <QPainter>
#include <QFile>
//Added by qt3to4:
#include <QPixmap>

#include <kdebug.h>
#include <kurl.h>
#include <kio/netaccess.h>

uint Picture::uniqueValue = 0;


Picture::Picture(void) : m_sharedData(NULL)
{
    m_uniqueName = "Pictures" + QString::number(uniqueValue++);
}

Picture::~Picture(void)
{
    unlinkSharedData();
}

QString Picture::uniqueName() const
{
    return m_uniqueName;
}

Picture::Picture(const Picture &other)
{
    m_sharedData = NULL;
    (*this) = other;
}

void Picture::assignPictureId(uint _id)
{
    if (m_sharedData)
        m_sharedData->assignPictureId(_id);
}

QString Picture::uniquePictureId() const
{
    if (m_sharedData)
        return m_sharedData->uniquePictureId();
    else
        return QString();
}

Picture& Picture::operator=(const Picture & other)
{
    //kDebug(30508) <<"Picture::= before";
    if (other.m_sharedData)
        other.linkSharedData();
    if (m_sharedData)
        unlinkSharedData();
    m_sharedData = other.m_sharedData;
    m_key = other.m_key;
    //kDebug(30508) <<"Picture::= after";
    return *this;
}

void Picture::unlinkSharedData(void)
{
    if (m_sharedData && m_sharedData->deref())
        delete m_sharedData;

    m_sharedData = NULL;
}

void Picture::linkSharedData(void) const
{
    if (m_sharedData)
        m_sharedData->ref();
}

void Picture::createSharedData(void)
{
    if (!m_sharedData) {
        m_sharedData = new PictureShared();
        // Do not call m_sharedData->ref()
    }
}

PictureType::Type Picture::getType(void) const
{
    if (m_sharedData)
        return m_sharedData->getType();
    return PictureType::TypeUnknown;
}

PictureKey Picture::getKey(void) const
{
    return m_key;
}

void Picture::setKey(const PictureKey& key)
{
    m_key = key;
}


bool Picture::isNull(void) const
{
    if (m_sharedData)
        return m_sharedData->isNull();
    return true;
}

void Picture::draw(QPainter& painter, int x, int y, int width, int height, int sx, int sy, int sw, int sh, bool fastMode)
{
    if (m_sharedData)
        m_sharedData->draw(painter, x, y, width, height, sx, sy, sw, sh, fastMode);
    else {
        // Draw a white box
        kWarning(30508) << "Drawing white rectangle! (Picture::draw)";
        painter.save();
        painter.setBrush(QColor(255, 255, 255));
        painter.drawRect(x, y, width, height);
        painter.restore();
    }
}

bool Picture::loadXpm(QIODevice* io)
{
    kDebug(30508) << "Picture::loadXpm";
    if (!io) {
        kError(30508) << "No QIODevice!" << endl;
        return false;
    }
    createSharedData();
    return m_sharedData->loadXpm(io);
}

bool Picture::save(QIODevice* io) const
{
    if (!io)
        return false;
    if (m_sharedData)
        return m_sharedData->save(io);
    return false;
}

bool Picture::saveAsBase64(KoXmlWriter& writer) const
{
    if (m_sharedData)
        return m_sharedData->saveAsBase64(writer);
    return false;
}

void Picture::clear(void)
{
    unlinkSharedData();
}

void Picture::clearAndSetMode(const QString& newMode)
{
    createSharedData();
    m_sharedData->clearAndSetMode(newMode);
}

QString Picture::getExtension(void) const
{
    if (m_sharedData)
        return m_sharedData->getExtension();
    return "null"; // Just a dummy
}

QString Picture::getMimeType(void) const
{
    if (m_sharedData)
        return m_sharedData->getMimeType();
    return QString(NULL_MIME_TYPE);
}

bool Picture::load(QIODevice* io, const QString& extension)
{
    kDebug(30508) << "Picture::load(QIODevice*, const QString&)" << extension;
    createSharedData();

    return m_sharedData->load(io, extension);
}

bool Picture::loadFromFile(const QString& fileName)
{
    kDebug(30508) << "Picture::loadFromFile" << fileName;
    createSharedData();
    return m_sharedData->loadFromFile(fileName);
}

bool Picture::loadFromBase64(const QByteArray& str)
{
    createSharedData();
    return m_sharedData->loadFromBase64(str);
}

QSize Picture::getOriginalSize(void) const
{
    if (m_sharedData)
        return m_sharedData->getOriginalSize();
    return QSize(0, 0);
}

QPixmap Picture::generatePixmap(const QSize& size, bool smoothScale)
{
    if (m_sharedData)
        return m_sharedData->generatePixmap(size, smoothScale);
    return QPixmap();
}

bool Picture::setKeyAndDownloadPicture(const KUrl& url, QWidget *window)
{
    bool result = false;

    QString tmpFileName;
    if (KIO::NetAccess::download(url, tmpFileName, window)) {
        PictureKey key;
        key.setKeyFromFile(tmpFileName);
        setKey(key);
        result = loadFromFile(tmpFileName);
        KIO::NetAccess::removeTempFile(tmpFileName);
    }

    return result;
}

QMimeData* Picture::dragObject(QWidget *dragSource, const char *name)
{
    if (m_sharedData)
        return m_sharedData->dragObject(dragSource, name);
    return 0;
}

QImage Picture::generateImage(const QSize& size)
{
    if (m_sharedData)
        return m_sharedData->generateImage(size);
    return QImage();
}

bool Picture::hasAlphaBuffer() const
{
    if (m_sharedData)
        return m_sharedData->hasAlphaBuffer();
    return false;
}

void Picture::setAlphaBuffer(bool enable)
{
    if (m_sharedData)
        m_sharedData->setAlphaBuffer(enable);
}

QImage Picture::createAlphaMask(Qt::ImageConversionFlags flags) const
{
    if (m_sharedData)
        return m_sharedData->createAlphaMask(flags);
    return QImage();
}

void Picture::clearCache(void)
{
    if (m_sharedData)
        m_sharedData->clearCache();
}
