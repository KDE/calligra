/* This file is part of the KDE project
   Copyright (C) 2001, Laurent MONTEL <lmontel@mandrakesoft.com>

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

#ifndef KIVIO_DOC_IFACE_H
#define KIVIO_DOC_IFACE_H

#include <KoDocumentIface.h>
#include <dcopref.h>

#include <QString>

class KivioDoc;

class KIvioDocIface : virtual public KoDocumentIface
{
    K_DCOP
public:
    KIvioDocIface(  KivioDoc *doc_ );

k_dcop:
    virtual DCOPRef map();
    void initConfig();
    void saveConfig();
    void setShowGrid( bool b );
    bool showGrid() const;
    int undoRedoLimit() const;
    void setUndoRedoLimit(int val);
    void snapToGrid( bool b );
    bool isSnapToGrid() const;
private:
    KivioDoc *doc;

};

#endif
