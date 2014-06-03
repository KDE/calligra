/* This file is part of the KDE project
   Copyright (C) 2011 Dag Andersen <danders@get2net.dk>

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

#include "kptcolorsconfigpanel.h"
#include "plansettings.h"

#include <kdebug.h>

namespace KPlato
{

ColorsConfigPanel::ColorsConfigPanel(QWidget *p )
    : QWidget(p)
{

    setupUi(this);

    kcfg_ColorGradientType->addItem( i18n( "Linear" ) );
    kcfg_ColorGradientType->addItem( i18n( "Flat" ) );
}

}  //KPlato namespace

#include "kptcolorsconfigpanel.moc"
