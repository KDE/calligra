/* This file is part of the KDE project
   Copyright (c) 2001 David Faure <faure@kde.org>

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

#include "koClipart.h"
#include <qshared.h>
#include <qpicture.h>
#include <assert.h>

class KoClipartPrivate : public QShared
{
public:
    QPicture m_picture;
    KoClipartKey m_key;
};


KoClipart::KoClipart()
{
    d = 0;
}

KoClipart::KoClipart( const KoClipartKey &key, const QPicture &pic )
{
    d = new KoClipartPrivate;
    d->m_picture.setData( pic.data(), pic.size() ); // make a copy
    d->m_key = key;
}

KoClipart::KoClipart( const KoClipart &other )
{
    d = 0;
    (*this) = other;
}

KoClipart::~KoClipart()
{
    if ( d && d->deref() )
        delete d;
}

KoClipart &KoClipart::operator=( const KoClipart &_other )
{
    KoClipart &other = const_cast<KoClipart &>( _other );

    if ( other.d )
        other.d->ref();

    if ( d && d->deref() )
        delete d;

    d = other.d;

    return *this;
}

QPicture * KoClipart::picture() const
{
    if ( !d ) return 0L;
    return &d->m_picture;
}

KoClipartKey KoClipart::key() const
{
    if ( !d ) return KoClipartKey();

    return d->m_key;
}

bool KoClipart::isNull() const
{
    return d == 0 || d->m_picture.isNull();
}
