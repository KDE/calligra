/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Michael Koch <koch@kde.org>
 
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

#include <qlayout.h>

#include "formpropertydlg.h"

// only for debug
#include <iostream.h>

FormPropertyDlg::FormPropertyDlg(  QWidget* _parent, const char* _name )
  : QDialog( _parent, _name )
{
  QGridLayout* grid = new QGridLayout( 3, 3 );

  // Michael Koch
  //
  // Vielleicht waere es am besten alle Form-weiten Einstellungen in
  // diesem Dialog zu machen ( dann aber von QTabDialog abgeleitet )
  //
  // Label mit Eingabezeile fuer den Namen des Formulars.
  // zwei Buttons: OK, Cancel.
}

FormPropertyDlg::~FormPropertyDlg()
{
}

#include "formpropertydlg.moc"

