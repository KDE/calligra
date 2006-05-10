/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

#if 0

// 8x8 pixels

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

#else

// 16x16 pixels

static const char* const buttonnone[]={
"16 16 7 1",
"c c #0a0000",
". c #0a0a0a",
"b c #330000",
"a c #331f1f",
"# c #333333",
"e c #ff0000",
"d c #ffffff",
"..########aabbcc",
"..########aabbcc",
"##dddddddddeeebb",
"##ddddddddeeeebb",
"##dddddddeeeeeaa",
"##ddddddeeeeedaa",
"##dddddeeeeedd##",
"##ddddeeeeeddd##",
"##dddeeeeedddd##",
"##ddeeeeeddddd##",
"aaeeeeeedddddd##",
"aaeeeeeddddddd##",
"bbeeeedddddddd##",
"bbeeeddddddddd##",
"ccbbaa########..",
"ccbbaa########.."};

static const char* const buttonsolid[]={
"16 16 1 1",
". c #000000",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................",
"................"};


// FIXME: Smoother gradient button.

static const char* const buttongradient[]={
"16 16 14 1",
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
"..##aabbccddeeff",
"..##aabbccddeeff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"gghhiijjkkll##ff",
"..##aabbccddeeff",
"..##aabbccddeeff"};

static const char* const buttonpattern[]={
"16 16 4 1",
". c #0a0a0a",
"# c #333333",
"a c #a0a0a0",
"b c #ffffffff",
"..############..",
"..############..",
"##aaaabbbbaaaa##",
"##aaaabbbbaaaa##",
"##aaaabbbbaaaa##",
"##aaaabbbbaaaa##",
"##bbbbaaaabbbb##",
"##bbbbaaaabbbb##",
"##bbbbaaaabbbb##",
"##bbbbaaaabbbb##",
"##aaaabbbbaaaa##",
"##aaaabbbbaaaa##",
"##aaaabbbbaaaa##",
"##aaaabbbbaaaa##",
"..############..",
"..############.."};

#endif


#include <qpixmap.h>
#include <qtoolbutton.h>
#include <QToolTip>

#include <klocale.h>

#include "karbon_part.h"
#include "vfillcmd.h"
#include "vselection.h"
#include "vstrokecmd.h"

#include "vtypebuttonbox.h"

VTypeButtonBox::VTypeButtonBox( KarbonPart *part, 
								QWidget* parent, const char* name )
	: QHButtonGroup( parent, name ),
	  m_part( part ), m_isStrokeManipulator( false )
{
	setMaximumWidth( parent->width() - 2 );

	// The button for no fill
	QToolButton* button = new QToolButton( this );
	button->setPixmap( QPixmap( (const char **) buttonnone ) );
	button->setMaximumWidth( 14 );
	button->setMaximumHeight( 14 );
	button->setToolTip( i18n( "None" ) );
	insert( button, none );

	// The button for solid fill
	button = new QToolButton( this );
	button->setPixmap( QPixmap( (const char **) buttonsolid ) );
	button->setMaximumWidth( 14 );
	button->setMaximumHeight( 14 );
	button->setToolTip( i18n( "Solid" ) );
	insert( button, solid );

	// The button for gradient fill
	button = new QToolButton( this );
	button->setPixmap( QPixmap( (const char **) buttongradient ) );
	button->setMaximumWidth( 14 );
	button->setMaximumHeight( 14 );
	button->setToolTip( i18n( "Gradient" ) );
	insert( button, gradient );

	// The button for pattern fill
	button = new QToolButton( this );
	button->setPixmap( QPixmap( (const char **) buttonpattern ) );
	button->setMaximumWidth( 14 );
	button->setMaximumHeight( 14 );
	button->setToolTip( i18n( "Pattern" ) );
	insert( button, pattern );

	setInsideMargin( 1 );
	setInsideSpacing( 1 );
	connect( this, SIGNAL( clicked( int ) ),
			 this, SLOT( slotButtonPressed( int ) ) );
}

void
VTypeButtonBox::slotButtonPressed( int id )
{
	if( m_part && m_part->document().selection()->objects().count() > 0 ) {
		if ( m_isStrokeManipulator )
			manipulateStrokes( id );
		else
			manipulateFills( id );
	}
}

void
VTypeButtonBox::setStroke()
{
	m_isStrokeManipulator = true;
}

void
VTypeButtonBox::setFill()
{
	m_isStrokeManipulator = false;
}

void
VTypeButtonBox::manipulateFills( int id )
{
	VFill m_fill;
	m_fill = *m_part->document().selection()->objects().getFirst()->fill();
	switch( id ){
	case none:
		m_fill.setType( VFill::none );
		break;
	case solid:
		m_fill.setType( VFill::solid );
		break;
	case gradient:
		m_fill.setType( VFill::grad );
		break;
	case pattern:
		m_fill.setType( VFill::patt );
	}
	m_part->addCommand( new VFillCmd( &m_part->document(), m_fill ), true );
}

void
VTypeButtonBox::manipulateStrokes( int id )
{
	VStroke m_stroke;
	m_stroke = *m_part->document().selection()->objects().getFirst()->stroke();
	switch( id ){
	case none:
		m_stroke.setType( VStroke::none );
		break;
	case solid:
		m_stroke.setType( VStroke::solid );
		break;
	case gradient:
		m_stroke.setType( VStroke::grad );
		break;
	case pattern:
		m_stroke.setType( VStroke::patt );
	}
	m_part->addCommand( new VStrokeCmd( &m_part->document(), &m_stroke ), true );
}

#include "vtypebuttonbox.moc"

