/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

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

static const char* const buttonnone[]={
"10 10 3 1",
". c #000000",
"a c #ff0000",
"# c #ffffff",
"..........",
".######aa.",
".#####aaa.",
".####aaa#.",
".###aaa##.",
".##aaa###.",
".#aaa####.",
".aaa#####.",
".aa######.",
".........."};

static const char* const buttonsolid[]={
"10 10 1 1",
". c #000000",
"..........",
"..........",
"..........",
"..........",
"..........",
"..........",
"..........",
"..........",
"..........",
".........."};

static const char* const buttongradient[]={
"10 10 8 1",
"a c None",
". c #000000",
"f c #303030",
"e c #585858",
"d c #808080",
"c c #a0a0a0",
"b c #c3c3c3",
"# c #ffffff",
"..........",
".#abcdef..",
".#abcdef..",
".#abcdef..",
".#abcdef..",
".#abcdef..",
".#abcdef..",
".#abcdef..",
".#abcdef..",
".........."};

static const char* const buttonpattern[]={
"10 10 2 1",
". c #000000",
"# c #ffffff",
"..........",
".########.",
".#..##..#.",
".#..##..#.",
".###..###.",
".###..###.",
".#..##..#.",
".#..##..#.",
".########.",
".........."};

#include <qbutton.h>
#include <qpixmap.h>

#include "vtypebuttonbox.h"

VTypeButtonBox::VTypeButtonBox( QWidget* parent, const char* name ) : QHButtonGroup( parent, name )
{
	QButton* button = new QButton( this );
	button->setPixmap( QPixmap( (const char **) buttonnone ) );
	insert( button, none );
	button = new QButton( this );
	button->setPixmap( QPixmap( (const char **) buttonsolid ) );
	insert( button, solid );
	button = new QButton( this );
	button->setPixmap( QPixmap( (const char **) buttongradient ) );
	insert( button, gradient );
	button = new QButton( this );
	button->setPixmap( QPixmap( (const char **) buttonpattern ) );
	insert( button, pattern );
}

#include "vtypebuttonbox.moc"

