/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2000-2005 David Faure <faure@kde.org>
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2009 Boudewijn Rempt <boud@valdyas.org>

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
  Boston, MA 02110-1301, USA.
*/


#include "KoDocumentBase.h"

#include "KoOdfLoadingContext.h"
#include "OdfDebug.h"

#include <QtGlobal>

class Q_DECL_HIDDEN KoDocumentBase::Private {
public:
    Private()
    : storeInternal(false)
    {}

    bool storeInternal; // Store this doc internally even if url is external
};

KoDocumentBase::KoDocumentBase()
    : d( new Private )
{
}


KoDocumentBase::~KoDocumentBase()
{
    delete d;
}

void KoDocumentBase::setStoreInternal(bool i)
{
    d->storeInternal = i;
    //debugMain<<"="<<d->storeInternal<<" doc:"<<url().url();
}

bool KoDocumentBase::storeInternal() const
{
    return d->storeInternal;
}
