/* This file is part of the KDE project
   Copyright (C) 2001 Simon Hausmann <hausmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kpimage_h__
#define __kpimage_h__

#include <koImageCollection.h>

class QDomElement;
class QDomDocument;

// At the moment those classes are equivalent.
// But keeping a different name in kpresenter is a good idea,
// in case we need to extend KoImage later.
typedef KoImage KPImage;
typedef KoImageKey KPImageKey;

class KPImageCollection : public KoImageCollection
{
public:
    KPImageCollection()
        { m_tmpDate = QDate::currentDate(); m_tmpTime = QTime::currentTime(); }

    // KPresenter uses dateTime.isValid() for images in the collection and
    // !isValid() for images to be loaded from disk.
    // This method handles both cases.
    KPImage findOrLoad( const QString & fileName, const QDateTime & dateTime );

    // Special support for XPMs
    KPImage loadXPMImage( const KPImageKey &key, const QString &rawData );

    // this is ugly, but it was in KPPixmapCollection
    QDate tmpDate() const { return m_tmpDate; }
    QTime tmpTime() const { return m_tmpTime; }

private:
    QDate m_tmpDate;
    QTime m_tmpTime;
};

#endif
