/*
 *  brushdialog.cc - part of KImageShop
 *
 *  A floating tabdialog showing a brushwidget and (later) some more properties
 *
 *  Copyright (c) 1999 Carsten Pfeiffer <pfeiffer@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <klocale.h>

#include "brusheswidget.h"
#include "brushdialog.h"


BrushDialog::BrushDialog( QWidget* parent, const char* name,WFlags /*flags*/ )
  : KFloatingTabDialog( parent, name )
{
  setCaption( i18n( "Brushes" ) );

  brushesTab = new BrushesWidget( this, "brushes widget" );
  // more tabs to come in the future

  addTab( brushesTab, i18n( "Brushes" ) );
}


BrushDialog::~BrushDialog()
{
  delete brushesTab;
}


#include "brushdialog.moc"
