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
#include "koPicture.h"

KoPicture::KoPicture(void) : m_base(NULL)
{
}

KoPicture::~KoPicture(void)
{
    if (m_base)
        delete m_base;
}

KoPicture::KoPicture(const KoPicture &other)
{
    // We need to use newCopy, because we want a real copy, not just a copy of the part of KoPictureBase
    if (other.m_base)
        m_base=other.m_base->newCopy();
    else
        m_base=NULL;
    m_key=other.m_key;
}

KoPicture& KoPicture::operator=( const KoPicture &other )
{
    clear();
    kdDebug(30003) << "KoPicture::= before" << endl;
    if (other.m_base)
        m_base=other.m_base->newCopy();
    m_key=other.m_key;
    kdDebug(30003) << "KoPicture::= after" << endl;
    return *this;
}

KoPictureType::Type KoPicture::getType(void) const
{
    if (m_base)
        return m_base->getType();
    return KoPictureType::TypeUnknown;
}

KoPictureKey KoPicture::getKey(void) const
{
    return m_key;
}

void KoPicture::setKey(const KoPictureKey& key)
{
    m_key=key;
}

bool KoPicture::isNull(void) const
{
    if (m_base)
        return m_base->isNull();
    return true;
}

void KoPicture::draw(QPainter& painter, int x, int y, int width, int height, int sx, int sy, int sw, int sh)
{
    if (m_base)
        m_base->draw(painter, x, y, width, height, sx, sy, sw, sh);
    else
    {
        // Draw a red box (easier DEBUG)
        kdWarning(30003) << "Drawing red rectangle! (KoPicture::draw)" << endl;
        painter.save();
        painter.setBrush(QColor(255,0,0));
        painter.drawRect(x,y,width,height);
        painter.restore();
    }
}

bool KoPicture::load(QIODevice* io)
{
    kdDebug(30003) << "KoPicture::load(QIODevice*)" << endl;
    if (!io)
    {
        kdError(30003) << "No QIODevice!" << endl;
        return false;
    }

    if (m_base)
        return m_base->load(io);
    kdError(30003) << "KoPIcture was not prepared for loading!" << endl;
    return false;
}

bool KoPicture::loadWmf(QIODevice* io)
{
    kdDebug(30003) << "KoPicture::loadWmf" << endl;
    if (!io)
    {
        kdError(30003) << "No QIODevice!" << endl;
        return false;
    }

    clear();

    // The extension .wmf was used (KOffice 1.1.x) for QPicture files
    // For an extern file or in future in the storage, .wmf can mean a real Windows Meta File.

    QByteArray array=io->readAll();
    QPicture picture(KoPictureType::formatVersionQPicture);
    KoPictureClipart* picClip;
    m_base=picClip=new KoPictureClipart();

    // "QPIC" at start of the file?
    if ((array[0]=='Q') && (array[1]=='P') &&(array[2]=='I') && (array[3]=='C'))
    {
        // We have found the signature of a QPicture file
        kdDebug(30003) << "QPicture file format!" << endl;
        QBuffer buffer(array);
        buffer.open(IO_ReadWrite);
        bool check = false;
        if (picture.load(&buffer,NULL)) {
            check = picClip->loadQPicture(picture);
            m_base->setExtension("wmf");
        }
        buffer.close();
        return check;
    }
    else
    {
        // real WMF
        // TODO: create KoPictureWmf and give the control to that class
        kdDebug(30003) << "Real WMF file format!" << endl;
        QBuffer buffer(array);
        buffer.open(IO_ReadWrite);
        bool check = false;
        QWinMetaFile wmf;
        if (wmf.load(buffer))
        {
            wmf.paint(&picture);
            check = picClip->loadQPicture(picture);
            m_base->setExtension("wmf");
        }
        buffer.close();
        return check;
    }
}

bool KoPicture::loadXpm(QIODevice* io)
{
    kdDebug(30003) << "KoPicture::loadXpm" << endl;
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
    bool check = m_base->load(&buffer);
    m_base->setExtension("xpm");
    return check;
}

bool KoPicture::save(QIODevice* io)
{
    if (!io)
        return false;
    if (m_base)
        return m_base->save(io);
    return false;
}

void KoPicture::clear(void)
{
    // Clear does not reset the key m_key!
    if (m_base)
        delete m_base;
    m_base=NULL;
}

void KoPicture::clearAndSetMode(const QString& newMode)
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

QString KoPicture::getExtension(void) const
{
    if (m_base)
        return m_base->getExtension();
    return "null"; // Just a dummy
}

bool KoPicture::load(QIODevice* io, const QString& extension)
{
    kdDebug(30003) << "KoPicture::load(QIODevice*, const QString&) " << extension << endl;
    bool flag;
    QString ext=extension.lower();
    if (ext=="wmf")
        flag=loadWmf(io);
    else
    {
        clearAndSetMode(extension);
        flag=load(io);
        m_base->setExtension(extension.lower());
    }
    if (!flag)
    {
        kdError(30003) << "File was not loaded! (KoPicture::load)" << endl;
    }
    return flag;
}

bool KoPicture::loadFromFile(const QString& fileName)
{
    kdDebug(30003) << "KoPicture::loadFromFile " << fileName << endl;
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

QSize KoPicture::getOriginalSize(void) const
{
    if (m_base)
        return m_base->getOriginalSize();
    return QSize(0,0);
}

QSize KoPicture::getSize(void) const
{
    if (m_base)
        return m_base->getSize();
    return QSize(0,0);
}

void KoPicture::setSize(const QSize& size)
{
    if (m_base)
        return m_base->setSize(size);
}

QPixmap KoPicture::generatePixmap(const QSize& size)
{
    if (m_base)
        return m_base->generatePixmap(size);
    return QPixmap();
}
