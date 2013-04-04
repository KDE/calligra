/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTMYCOMBOBOX_P_H
#define KPTMYCOMBOBOX_P_H

#include "kplato_export.h"

#include <kcombobox.h>

namespace KPlato
{

class MyComboBox : public KComboBox
{
public:
    MyComboBox( QWidget *parent = 0 ) : KComboBox( parent ) {}

    void emitActivated( int i ) { emit activated( i ); }

};


} //KPlato namespace

#endif // KPTMYCOMBOBOX_P_H
