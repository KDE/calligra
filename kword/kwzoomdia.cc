/* This file is part of the KDE project
   Copyright (C)  2001 Montel Laurent <lmontel@mandrakesoft.com>

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

#include <kapp.h>
#include <kdialogbase.h>
#include <klocale.h>

#include <qspinbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>

#include "kwzoomdia.h"
#include "kwview.h"


KWZoomDia::KWZoomDia( KWView* parent, const char *name )
  :  KDialogBase( Plain, i18n("Select Zoom"), Ok | Cancel, Ok , parent, name, true )
{
    setCaption(i18n("Select Zoom"));
    QWidget *page = plainPage();
    QGridLayout *grid = new QGridLayout( page, 1, 1, 15, 7 );

    zoom=new QSpinBox ( 10, 500, 10,page );
    grid->addWidget(zoom,0,0);


}

bool KWZoomDia::selectZoom( int & val)
{
    bool res = false;

    KWZoomDia *dlg = new KWZoomDia( 0L, "Select zoom");

    if ( dlg->exec() == Accepted )
    {
        val=dlg->getValue();
        res = true;
    }

    delete dlg;

    return res;
}

int KWZoomDia::getValue()
{
    return zoom->value();
}
#include "kwzoomdia.moc"
