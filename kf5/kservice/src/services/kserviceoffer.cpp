/* This file is part of the KDE libraries
   Copyright (C) 2000 Torben Weis <weis@kde.org>
   Copyright (C) 2006 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kserviceoffer.h"

class KServiceOffer::Private
{
public:
    Private()
        : preference(-1),
          mimeTypeInheritanceLevel(0),
          bAllowAsDefault( false ),
          pService( 0 )
    {
    }

    int preference;
    int mimeTypeInheritanceLevel;
    bool bAllowAsDefault;
    KService::Ptr pService;
};

KServiceOffer::KServiceOffer()
    : d( new Private )
{
}

KServiceOffer::KServiceOffer( const KServiceOffer& _o )
    : d( new Private )
{
    *d = *_o.d;
}

KServiceOffer::KServiceOffer( const KService::Ptr& _service, int _pref, int mimeTypeInheritanceLevel, bool _default )
    : d( new Private )
{
    d->pService = _service;
    d->preference = _pref;
    d->mimeTypeInheritanceLevel = mimeTypeInheritanceLevel;
    d->bAllowAsDefault = _default;
}

KServiceOffer::~KServiceOffer()
{
    delete d;
}

KServiceOffer& KServiceOffer::operator=( const KServiceOffer& rhs )
{
    if ( this == &rhs ) {
        return *this;
    }

    *d = *rhs.d;
    return *this;
}

bool KServiceOffer::operator< ( const KServiceOffer& _o ) const
{
    // First check mimetype inheritance level.
    // Direct mimetype association is preferred above association via parent mimetype
    // So, the smaller the better.
    if (d->mimeTypeInheritanceLevel != _o.d->mimeTypeInheritanceLevel)
        return d->mimeTypeInheritanceLevel < _o.d->mimeTypeInheritanceLevel;

    // Put offers allowed as default FIRST.
    if ( _o.d->bAllowAsDefault && !d->bAllowAsDefault )
        return false; // _o is default and not 'this'.
    if ( !_o.d->bAllowAsDefault && d->bAllowAsDefault )
        return true; // 'this' is default but not _o.
    // Both offers are allowed or not allowed as default

    // Finally, use preference to sort them
    // The bigger the better, but we want the better FIRST
    return _o.d->preference < d->preference;
}

bool KServiceOffer::allowAsDefault() const
{
    return d->bAllowAsDefault;
}

int KServiceOffer::preference() const
{
    return d->preference;
}

void KServiceOffer::setPreference( int p )
{
    d->preference = p;
}

KService::Ptr KServiceOffer::service() const
{
    return d->pService;
}

bool KServiceOffer::isValid() const
{
    return d->preference >= 0;
}

void KServiceOffer::setMimeTypeInheritanceLevel(int level)
{
    d->mimeTypeInheritanceLevel = level;
}

int KServiceOffer::mimeTypeInheritanceLevel() const
{
    return d->mimeTypeInheritanceLevel;
}

