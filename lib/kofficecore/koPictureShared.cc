/* This file is part of the KDE project
   Copyright (c) 2001 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2002 Nicolas GOUTTE <nicog@snafu.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qpainter.h>
#include <qfile.h>

#include <kdebug.h>

#include <qwmf.h>
#include "koPictureKey.h"
#include "koPictureBase.h"
#include "koPictureImage.h"
#include "koPictureClipart.h"
#include "koPictureShared.h"

KoPictureShared::KoPictureShared(void) : m_base(NULL)
{
}

KoPictureShared::~KoPictureShared(void)
{
    if (m_base)
        delete m_base;
}

KoPictureShared::KoPictureShared(const KoPictureShared &other)
    : QShared() // Some compilers want it explicitely!
{
    // We need to use newCopy, because we want a real copy, not just a copy of the part of KoPictureBase
    if (other.m_base)
        m_base=other.m_base->newCopy();
    else
        m_base=NULL;
}

KoPictureShared& KoPictureShared::operator=( const KoPictureShared &other )
{
    clear();
    kdDebug(30003) << "KoPictureShared::= before" << endl;
    if (other.m_base)
        m_base=other.m_base->newCopy();
    kdDebug(30003) << "KoPictureShared::= after" << endl;
    return *this;
}

KoPictureType::Type KoPictureShared::getType(void) const
{
    if (m_base)
        return m_base->getType();
    return KoPictureType::TypeUnknown;
}

bool KoPictureShared::isNull(void) const
{
    if (m_base)
        return m_base->isNull();
    return true;
}

void KoPictureShared::draw(QPainter& painter, int x, int y, int width, int height, int sx, int sy, int sw, int sh, bool fastMode)
{
    if (m_base)
        m_base->draw(painter, x, y, width, height, sx, sy, sw, sh, fastMode);
    else
    {
        // Draw a red box (easier DEBUG)
        kdWarning(30003) << "Drawing red rectangle! (KoPictureShared::draw)" << endl;
        painter.save();
        painter.setBrush(QColor(255,0,0));
        painter.drawRect(x,y,width,height);
        painter.restore();
    }
}

bool KoPictureShared::loadWmf(QIODevice* io)
{
    kdDebug(30003) << "KoPictureShared::loadWmf" << endl;
    if (!io)
    {
        kdError(30003) << "No QIODevice!" << endl;
        return false;
    }

    clear();

    // The extension .wmf was used (KOffice 1.1.x) for QPicture files
    // For an extern file or in the storage, .wmf can mean a real Windows Meta File.

    QByteArray array=io->readAll();
    QPicture picture(KoPictureType::formatVersionQPicture);
    KoPictureClipart* picClip;
    m_base=picClip=new KoPictureClipart();

    QString extension(picClip->loadWmfFromArray(picture, array));
    if (extension.isEmpty())
        return false;
    else
    {
        setExtension(extension);
        return true;
    }
}

bool KoPictureShared::loadXpm(QIODevice* io)
{
    kdDebug(30003) << "KoPictureShared::loadXpm" << endl;
    if (!io)
    {
        kdError(30003) << "No QIODevice!" << endl;
        return false;
    }

    clear();

    // Old KPresenter XPM files have char(1) instead of some "
    // Therefore we need to treat XPM separately

    QByteArray array=io->readAll();

    // As XPM files are normally only ASCII files, we can replace it without problems

    int pos=0;

    while ((pos=array.find(char(1),pos))!=-1)
    {
        array[pos]='"';
    }

    // Now that the XPM file is corrected, we need to load it.

    m_base=new KoPictureImage();

    QBuffer buffer(array);
    bool check = m_base->load(&buffer,"xpm");
    setExtension("xpm");
    return check;
}

bool KoPictureShared::save(QIODevice* io)
{
    if (!io)
        return false;
    if (m_base)
        return m_base->save(io);
    return false;
}

void KoPictureShared::clear(void)
{
    // Clear does not reset the key m_key!
    if (m_base)
        delete m_base;
    m_base=NULL;
}

void KoPictureShared::clearAndSetMode(const QString& newMode)
{
    if (m_base)
        delete m_base;
    m_base=NULL;

    const QString mode=newMode.lower();

    // TODO: WMF need to be alone!
    if ((mode=="svg") || (mode=="qpic") || (mode=="wmf"))
    {
        m_base=new KoPictureClipart();
    }
    else
    {   // TODO: test if QImageIO really knows the file format
        m_base=new KoPictureImage();
    }
}

QString KoPictureShared::getExtension(void) const
{
    return m_extension;
}

void KoPictureShared::setExtension(const QString& extension)
{
    m_extension = extension;
}

bool KoPictureShared::load(QIODevice* io, const QString& extension)
{
    kdDebug(30003) << "KoPictureShared::load(QIODevice*, const QString&) " << extension << endl;
    bool flag;
    QString ext(extension.lower());
    if (ext=="wmf")
        flag=loadWmf(io);
    else
    {
        clearAndSetMode(ext);
        if (m_base)
            flag=m_base->load(io,ext);
        setExtension(ext);
    }
    if (!flag)
    {
        kdError(30003) << "File was not loaded! (KoPictureShared::load)" << endl;
    }
    return flag;
}

bool KoPictureShared::loadFromFile(const QString& fileName)
{
    kdDebug(30003) << "KoPictureShared::loadFromFile " << fileName << endl;
    QFile file(fileName);
    const int pos=fileName.findRev('.');
    if (pos==-1)
    {
        kdDebug(30003) << "File with no extension! Not supported!" << endl;
        return false;
    }
    QString extension=fileName.mid(pos+1);
    if (!file.open(IO_ReadOnly))
        return false;
    const bool flag=load(&file,extension);
    file.close();
    return flag;
}

QSize KoPictureShared::getOriginalSize(void) const
{
    if (m_base)
        return m_base->getOriginalSize();
    return QSize(0,0);
}

QPixmap KoPictureShared::generatePixmap(const QSize& size)
{
    if (m_base)
        return m_base->generatePixmap(size);
    return QPixmap();
}

