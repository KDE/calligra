/* This file is part of the KDE project
   Copyright (c) 2001 Davud Faure <faure@kde.org>

   $Id$

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
#ifndef __KoMainWindowIface_h__
#define __KoMainWindowIface_h__

#include <dcopobject.h>
#include <dcopref.h>

class KDCOPActionProxy;
class KoMainWindow;

class KoMainWindowIface : virtual public DCOPObject
{
    K_DCOP
public:
    KoMainWindowIface( KoMainWindow *mainwindow );
    virtual ~KoMainWindowIface();

k_dcop:
    DCOPRef action( const QCString &name );
    QCStringList actions();
    QMap<QCString,DCOPRef> actionMap();

protected:
    KoMainWindow *m_pMainWindow;
    KDCOPActionProxy *m_actionProxy;
};

#endif
