/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
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

#include <qlabel.h>
#include <qgrid.h>
#include <qpixmap.h>

#include <klocale.h>
#include <knuminput.h>
#include <koMainWindow.h>
#include <koPoint.h>
#include <koRect.h>
#include <koView.h>

#include <tkfloatspinbox.h>

#include "karbon_part.h"

#include "vobjectdlg.h"
#include "vtransformcmd.h"
#include "vselection.h"
#include "vstrokecmd.h"

static const char* width_xpm[] = {
  "13 11 3 1",
  "  c Gray0",
  ". c #808080",
  "X c None",
  "XXXXXXXXXXXXX",
  "XXXXXXXXXXXXX",
  ".XXXXXXXXXXX.",
  ".XX XXXXX XX.",
  ".X  XXXXX  X.",
  ".           .",
  ".X  XXXXX  X.",
  ".XX XXXXX XX.",
  ".XXXXXXXXXXX.",
  "XXXXXXXXXXXXX",
  "XXXXXXXXXXXXX"
};

static const char* height_xpm[] = {
  "13 11 3 1",
  "  c Gray0",
  ". c #808080",
  "X c None",
  "XXX.......XXX",
  "XXXXXX XXXXXX",
  "XXXXX   XXXXX",
  "XXXX     XXXX",
  "XXXXXX XXXXXX",
  "XXXXXX XXXXXX",
  "XXXXXX XXXXXX",
  "XXXX     XXXX",
  "XXXXX   XXXXX",
  "XXXXXX XXXXXX",
  "XXX.......XXX"
};

static const char* xpos_xpm[] = {
  "13 11 2 1",
  "  c Gray0",
  ". c None",
  ".. .. .......",
  ".. .. .......",
  "...  ........",
  "...  ........",
  ".. .. .......",
  ".. .. .     .",
  "....... ... .",
  "....... ... .",
  "....... ... .",
  "....... ... .",
  ".......     ."
};

static const char* ypos_xpm[] = {
  "13 11 2 1",
  "  c Gray0",
  ". c None",
  "... . .......",
  "... . .......",
  "... . .......",
  ".... ........",
  ".... ........",
  "..  ...     .",
  "....... ... .",
  "....... ... .",
  "....... ... .",
  "....... ... .",
  ".......     ."
};

static const char* rotate_xpm[]={
"16 16 3 1",
". c None",
"# c #000000",
"a c #ffffff",
"................",
"................",
".......####.....",
".....##aaaa##...",
"##..#aaaaaaaa#..",
"#a##aaaaaaaaaa#.",
"#aaaaaaa####aa#.",
"#aaaaaa#....#aa#",
"#aaaaa#......#a#",
"#aaaaa#......#a#",
"#aaaaaa#.....#a#",
".#######....#a#.",
"...........#a#..",
"..........###...",
"........###.....",
"................"};

static const char *stroke_xpm[] = {
"16 16 4 1",
"  c Gray0",
". c #808080",
"X c None",
"o c Gray100",
"XXXXXXXXXXXXXXXX",
"XXXXXXX. .XXXXXX",
"XXXXXXX   XXXXXX",
"XXXXXX o o XXXXX",
"XXXXXX o o XXXXX",
"XXXXX oo oo XXXX",
"XXXXX ooooo XXXX",
"XXXXX oo oo XXXX",
"XXXXX. ooo .XXXX",
"XXXXXX ooo XXXXX",
"XXXXXX. o .XXXXX",
"XXXXX       XXXX",
"XXXXXX.   .XXXXX",
"XXXXXX.   .XXXXX",
"XXXXXXXXXXXXXXXX",
"XXXXXXXXXXXXXXXX"
};

VObjectDlg::VObjectDlg( KarbonPart* part, KoView* parent, const char* /*name*/ )
	: QDockWindow( QDockWindow::OutsideDock, parent->shell() ), m_part ( part )
{
	setCaption( i18n( "Geometry Panel" ) );
	setCloseMode( QDockWindow::Always );
	
	//Widgets layout:
	mainLayout = new QGrid( 2, Vertical, this );
	mainLayout->setSpacing( 5 );
	
	QLabel *lx = new QLabel( mainLayout );
	lx->setPixmap( QPixmap((const char **)xpos_xpm) );
	QLabel *ly = new QLabel( mainLayout );
	ly->setPixmap( QPixmap((const char **)ypos_xpm) );
	m_X = new KDoubleNumInput( 0.00, mainLayout );
	m_X->setMinimumWidth( 75 );
	m_Y = new KDoubleNumInput( 0.00, mainLayout );
	m_Y->setMinimumWidth( 75 );
	
	QLabel *lw = new QLabel( mainLayout );
	lw->setPixmap( QPixmap((const char **)width_xpm) );
	QLabel *lh = new QLabel( mainLayout );
	lh->setPixmap( QPixmap((const char **)height_xpm) );
	m_Width = new KDoubleNumInput( 0.00, mainLayout );
	m_Width->setMinimumWidth( 75 );
	m_Height = new KDoubleNumInput( 0.00, mainLayout );
	m_Height->setMinimumWidth( 75 );
	
	QLabel *lrotate = new QLabel( mainLayout );
	lrotate->setPixmap( QPixmap((const char **)rotate_xpm) );
	QLabel *lstroke = new QLabel( mainLayout );
	lstroke->setPixmap( QPixmap((const char **)stroke_xpm) );
	m_Rotation = new KDoubleNumInput( 0, mainLayout );
	m_setLineWidth = new TKUFloatSpinBox( mainLayout );
	m_setLineWidth->setDecimals(1);
	m_setLineWidth->setMinValue(0.0);
	m_setLineWidth->setLineStep(0.5);
	
	connect (m_X, SIGNAL( valueChanged( double ) ), this, SLOT( xChanged ( double ) ) );
	connect (m_Y, SIGNAL( valueChanged( double ) ), this, SLOT( yChanged ( double ) ) );                                                                                    
	connect (m_Width, SIGNAL( valueChanged( double ) ), this, SLOT( widthChanged ( double ) ) );
	connect (m_Height, SIGNAL( valueChanged( double ) ), this, SLOT( heightChanged ( double ) ) );
	connect( m_setLineWidth, SIGNAL( valueChanged( value() ) ), this, SLOT( lineWidthChanged( double ) ) );
	
	setWidget( mainLayout );
	setFixedSize( baseSize() );
}

VObjectDlg::~VObjectDlg()
{
}

void
VObjectDlg::enable()
{
	mainLayout->setEnabled ( true ); //Needed to enable the main widget, but not the entire docker
}

void
VObjectDlg::disable()
{
	mainLayout->setEnabled ( false ); //Needed to disable the main widget, but not the entire docker
}

void
VObjectDlg::reset() //Show default values
{
	m_X->setValue( 0.00 );
	m_Y->setValue( 0.00 );
	m_Width->setValue( 0.00 );
	m_Height->setValue( 0.00 );
	m_setLineWidth->setValue( 0.0 );
}

void
VObjectDlg::update( KarbonPart* part )
{
	if( part->document().selection()->objects().count() > 0 ) // there is a selection, so take the stroke of first selected object
	{
		m_X->setValue ( 0.00 );
		m_Y->setValue ( 0.00 );
		m_Width->setValue ( part->document().selection()->objects().getFirst()->boundingBox().width() );
		m_Height->setValue ( part->document().selection()->objects().getFirst()->boundingBox().height() );
		
		m_stroke.setType ( part->document().selection()->objects().getFirst()->stroke()->type() );
		m_stroke.setColor ( part->document().selection()->objects().getFirst()->stroke()->color() );
		m_stroke.setLineWidth ( part->document().selection()->objects().getFirst()->stroke()->lineWidth() );
		m_stroke.setLineCap ( part->document().selection()->objects().getFirst()->stroke()->lineCap() );   
		m_stroke.setLineJoin ( part->document().selection()->objects().getFirst()->stroke()->lineJoin() );
		m_stroke.setMiterLimit ( part->document().selection()->objects().getFirst()->stroke()->miterLimit() );
		m_setLineWidth->setValue( m_stroke.lineWidth() );
	}
}

void
VObjectDlg::xChanged ( double x )
{
	if( m_part && m_part->document().selection()->objects().count() > 0 && x !=0.00)
		m_part->addCommand( new VTranslateCmd( &m_part->document(), x, 0 ), true );
	m_X->setValue( 0.00 );
}

void
VObjectDlg::yChanged ( double y )
{
	if( m_part && m_part->document().selection()->objects().count() > 0 && y !=0.00)
		m_part->addCommand( new VTranslateCmd( &m_part->document(), 0, y ), true );
	m_Y->setValue( 0.00 );
}

void
VObjectDlg::widthChanged( double width )
{
	double height = m_Height->value();
	KoPoint current = KoPoint ( m_part->document().selection()->objects().getFirst()->boundingBox().left(), 
		m_part->document().selection()->objects().getFirst()->boundingBox().top() );
	if( m_part && m_part->document().selection()->objects().count() > 0 )
		m_part->addCommand( new VScaleCmd( &m_part->document(), current , width, height ), true );
}

void
VObjectDlg::heightChanged( double height )
{
	double width = m_Width->value();
	KoPoint current = KoPoint ( m_part->document().selection()->objects().getFirst()->boundingBox().left(), 
		m_part->document().selection()->objects().getFirst()->boundingBox().top() );
	if( m_part && m_part->document().selection()->objects().count() > 0 )
		m_part->addCommand( new VScaleCmd( &m_part->document(), current , width, height ), true );
}

void
VObjectDlg::lineWidthChanged ( double width )
{
	m_part->addCommand( new VStrokeLineWidthCmd( &m_part->document(), m_setLineWidth->value() ), true );
}
#include "vobjectdlg.moc"

