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
#include <kconfig.h>
#include <kglobal.h>

#include "koPictureKey.h"
#include "koPictureBase.h"

static int s_useSlowResizeMode = -1; // unset

KoPictureBase::KoPictureBase(void)
{
    // Slow mode can be very slow, especially at high zoom levels -> configurable
    if ( s_useSlowResizeMode == -1 )
    {
        KConfigGroup group( KGlobal::config(), "KOfficeImage" );
        s_useSlowResizeMode = group.readNumEntry( "HighResolution", 1 );
        kdDebug(30003) << "HighResolution = " << s_useSlowResizeMode << endl;
    }
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

bool KoPictureBase::saveAsKOffice1Dot1(QIODevice* io, const QString&)
{
    // The default is the normla save
    return save(io);
}

QSize KoPictureBase::getOriginalSize(void) const
{
    return QSize(0,0);
}

QPixmap KoPictureBase::generatePixmap(const QSize&, bool /*smoothScale*/)
{
    return QPixmap();
}

bool KoPictureBase::isClipartAsKOffice1Dot1(void) const
{
    return false;
}

QString KoPictureBase::getMimeType(const QString&) const
{
    return QString(NULL_MIME_TYPE);
}

bool KoPictureBase::isSlowResizeModeAllowed(void) const
{
    return s_useSlowResizeMode != 0;
}
