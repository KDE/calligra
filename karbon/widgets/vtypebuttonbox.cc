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
"8 8 7 1",
"c c #0a0000",
". c #0a0a0a",
"b c #330000",
"a c #331f1f",
"# c #333333",
"e c #ff0000",
"d c #ffffff",
".####abc",
"#ddddeeb",
"#dddeeea",
"#ddeeed#",
"#deeedd#",
"aeeeddd#",
"beedddd#",
"cba####."};

static const char* const buttonsolid[]={
"8 8 1 1",
". c #000000",
"........",
"........",
"........",
"........",
"........",
"........",
"........",
"........"};

static const char* const buttongradient[]={
"8 8 14 1",
"f c #000000",
"e c #040404",
". c #0a0a0a",
"d c #0f0f0f",
"c c #181818",
"b c #212121",
"a c #292929",
"# c #303030",
"g c #333333",
"l c #484848",
"k c #787878",
"j c #a7a7a7",
"i c #cdcdcd",
"h c #f1f1f1",
".#abcdef",
"ghijkl#f",
"ghijkl#f",
"ghijkl#f",
"ghijkl#f",
"ghijkl#f",
"ghijkl#f",
".#abcdef"};

static const char* const buttonpattern[]={
"8 8 4 1",
". c #0a0a0a",
"# c #333333",
"a c #a0a0a0",
"b c #ffffff",
".######.",
"#aabbaa#",
"#aabbaa#",
"#bbaabb#",
"#bbaabb#",
"#aabbaa#",
"#aabbaa#",
".######."};

#include <qtoolbutton.h>
#include <qpixmap.h>

#include "karbon_part.h"

#include "vtypebuttonbox.h"

VTypeButtonBox::VTypeButtonBox( KarbonPart *part, QWidget* parent, const char* name )
		: QHButtonGroup( parent, name ), m_part( part )
{
	setMaximumWidth( parent->width() );
	QToolButton* button = new QToolButton( this );
	button->setPixmap( QPixmap( (const char **) buttonnone ) );
	button->setMaximumWidth( 14 );
	button->setMaximumHeight( 14 );
	insert( button, none );
	button = new QToolButton( this );
	button->setPixmap( QPixmap( (const char **) buttonsolid ) );
	button->setMaximumWidth( 14 );
	button->setMaximumHeight( 14 );
	insert( button, solid );
	button = new QToolButton( this );
	button->setPixmap( QPixmap( (const char **) buttongradient ) );
	button->setMaximumWidth( 14 );
	button->setMaximumHeight( 14 );
	insert( button, gradient );
	button = new QToolButton( this );
	button->setPixmap( QPixmap( (const char **) buttonpattern ) );
	button->setMaximumWidth( 14 );
	button->setMaximumHeight( 14 );
	insert( button, pattern );
	setInsideMargin( 2 );
	setInsideSpacing( 2 );
}

#include "vtypebuttonbox.moc"

