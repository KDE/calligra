// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C)  2002 Montel Laurent <lmontel@mandrakesoft.com>
   Copyright (C)  2005 Thorsten Zachmann <zachmann@kde.org>

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

#include "koGuideLineDia.h"

#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>

#include <klocale.h>
#include <koUnitWidgets.h>


KoGuideLineDia::KoGuideLineDia( QWidget *parent, double pos, double minPos, double maxPos,
                               KoUnit::Unit unit, const char *name )
: KDialogBase( parent, name , true, "", Ok | Cancel, Ok, true )
{
    setCaption( i18n("Set Guide Line Position") );
    QHBox *page = makeHBoxMainWidget();
    new QLabel( i18n( "Position:" ), page );
    m_position= new KoUnitDoubleSpinBox( page, QMAX( 0.00, minPos ), QMAX( 0.00, maxPos ), 1, QMAX( 0.00, pos ), unit );
    m_position->setFocus();
}

double KoGuideLineDia::pos() const
{
    return m_position->value();
}


#include "koGuideLineDia.moc"
