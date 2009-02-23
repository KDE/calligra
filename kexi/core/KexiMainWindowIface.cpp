/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2009 Jarosław Staniek <staniek@kde.org>

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

#include "KexiMainWindowIface.h"

#include <kdebug.h>

//K_GLOBAL_STATIC(KexiMainWindow, kexiMainWindow)
#ifdef __GNUC__
#warning not threadsafe
#else
#pragma WARNING( not threadsafe )
#endif
KexiMainWindowIface* kexiMainWindow = 0;

KexiMainWindowIface::KexiMainWindowIface()
        : KexiSharedActionHost(this)
{
    kexiMainWindow = this; //for singleton
}

KexiMainWindowIface::~KexiMainWindowIface()
{
}

KexiMainWindowIface* KexiMainWindowIface::global()
{
    return kexiMainWindow;
}
