/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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

#ifndef KIVIO_PAGE_IFACE_H
#define KIVIO_PAGE_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>
#include <QString>

class KivioPage;

class KIvioPageIface : virtual public DCOPObject
{
    K_DCOP
public:
    KIvioPageIface( KivioPage *page_ );

k_dcop:
    DCOPRef firstLayer();
    DCOPRef layerAt( int );

    bool isHidden()const;
    void deleteSelectedStencils();
    void groupSelectedStencils();
    void ungroupSelectedStencils();
    QString pageName()const;
    void copy();
    void cut();
    void paste();
    void selectAllStencils();
    void unselectAllStencils();
    int nbLayer() const;
    void setHidePage(bool _hide);

private:
    KivioPage *m_page;

};

#endif
