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
#include <qpicture.h>
#include <qpixmap.h>

#include <kdebug.h>

#include "koPictureKey.h"
#include "koPictureBase.h"

KoPictureBase::KoPictureBase(void)
{
}

KoPictureBase::~KoPictureBase(void)
{
}

KoPictureBase* KoPictureBase::newCopy(void) const
{
    return new KoPictureBase(*this);
}

KoPictureType::Type KoPictureBase::getType(void) const
{
    return KoPictureType::TypeUnknown;
}


bool KoPictureBase::isNull(void) const
{
    return true;    // A KoPictureBase is always null.
}

void KoPictureBase::draw(QPainter& painter, int x, int y, int width, int height, int, int, int, int, bool /*fastMode*/)
{
    // Draw a light red box (easier DEBUG)
    kdWarning(30003) << "Drawing light red rectangle! (KoPictureBase::draw)" << endl;
    painter.save();
    painter.setBrush(QColor(128,0,0));
    painter.drawRect(x,y,width,height);
    painter.restore();
}

bool KoPictureBase::load(QIODevice*, const QString&)
{
    // Nothing to load!
    return false;
}

bool KoPictureBase::save(QIODevice*)
{
    // Nothing to save!
    return false;
}

QSize KoPictureBase::getOriginalSize(void) const
{
    return QSize(0,0);
}

QPixmap KoPictureBase::generatePixmap(const QSize&)
{
    return QPixmap();
}
