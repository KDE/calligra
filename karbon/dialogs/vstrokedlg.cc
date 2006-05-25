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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <qtabwidget.h>
#include <qradiobutton.h>
#include <Q3ButtonGroup>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

#include <kiconloader.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include "KoUnitWidgets.h"

#include "karbon_part.h"
#include "vcolor.h"
#include "vselection.h"
#include "vstrokecmd.h"
#include "vstroke.h"
#include "vstrokedlg.h"
#include "vcolortab.h"

VStrokeDlg::VStrokeDlg( KarbonPart* part, QWidget* parent, const char* name )
	: KDialogBase ( parent, name, true, i18n( "Stroke" ), Ok | Cancel ), m_part( part )
{
	enableButtonSeparator( true );
	QTabWidget *mainWidget = new QTabWidget( this, "strokemain" );
	Q3HBoxLayout *mainLayout = new Q3HBoxLayout (mainWidget, 3);

	Q3VBoxLayout *leftLayout = new Q3VBoxLayout( mainLayout, 4 );

	QLabel* widthLabel = new QLabel( i18n ( "Width:" ), mainWidget );
	leftLayout->addWidget ( widthLabel );
	m_setLineWidth = new KoUnitDoubleSpinBox( mainWidget, 0.0, 1000.0, 0.5, 1.0, KoUnit::U_PT, 1 );
	leftLayout->addWidget ( m_setLineWidth );

	//Dashing ->
	QLabel* styleLabel = new QLabel( i18n ( "Style:" ), mainWidget );
	leftLayout->addWidget ( styleLabel );
	m_styleCombo = new QComboBox( mainWidget );
	m_styleCombo->setEnabled ( false );
	leftLayout->addWidget ( m_styleCombo );
	// <- Dashing - reserved for later

	QRadioButton* button;
	m_typeOption = new Q3VButtonGroup ( mainWidget );
	button = new QRadioButton ( i18n( "None" ), m_typeOption );
	m_typeOption->insert( button );
	button = new QRadioButton ( i18n( "Stroke" ), m_typeOption );
	m_typeOption->insert( button );
	button = new QRadioButton ( i18n( "Gradient" ), m_typeOption );
	m_typeOption->insert( button );
	m_typeOption->setTitle( i18n( "Type" ) );
	mainLayout->addWidget( m_typeOption );
	connect( m_typeOption, SIGNAL( clicked( int ) ), this, SLOT( slotTypeChanged( int ) ) );

	m_capOption = new Q3VButtonGroup ( mainWidget );
	//button = new QRadioButton ( i18n( "Butt" ), m_capOption );	                                                          
	button = new QRadioButton ( m_capOption );
	button->setPixmap( DesktopIcon( "cap_butt" ) );
	m_capOption->insert( button );
	button = new QRadioButton ( m_capOption );
	button->setPixmap( DesktopIcon( "cap_round" ) );
	m_capOption->insert( button );
	button = new QRadioButton ( m_capOption );
	button->setPixmap( DesktopIcon( "cap_square" ) );
	m_capOption->insert( button );
	m_capOption->setTitle( i18n( "Cap" ) );
	mainLayout->addWidget( m_capOption );
	connect( m_capOption, SIGNAL( clicked( int ) ), this, SLOT( slotCapChanged( int ) ) );

	m_joinOption = new Q3VButtonGroup ( mainWidget );
	button = new QRadioButton ( m_joinOption );
	button->setPixmap( DesktopIcon( "join_miter" ) );
	m_joinOption->insert( button );
	button = new QRadioButton ( m_joinOption );
	button->setPixmap( DesktopIcon( "join_round" ) );
	m_joinOption->insert( button );
	button = new QRadioButton ( m_joinOption );
	button->setPixmap( DesktopIcon( "join_bevel" ) );
	m_joinOption->insert( button );
	m_joinOption->setTitle( i18n( "Join" ) );
	mainLayout->addWidget( m_joinOption );
	connect( m_joinOption, SIGNAL( clicked( int ) ), this, SLOT( slotJoinChanged( int ) ) );

	VSelection *sel = part->document().selection();
	if( sel && sel->objects().count() > 0 ) // there is a selection, so take the stroke of first selected object
	{
		m_stroke.setType ( sel->objects().getFirst()->stroke()->type() );
		m_stroke.setColor ( sel->objects().getFirst()->stroke()->color() );
		m_stroke.setLineWidth ( sel->objects().getFirst()->stroke()->lineWidth() );
		m_stroke.setLineCap ( sel->objects().getFirst()->stroke()->lineCap() );   
		m_stroke.setLineJoin ( sel->objects().getFirst()->stroke()->lineJoin() );
		m_stroke.setMiterLimit ( sel->objects().getFirst()->stroke()->miterLimit() );
	}

	slotUpdateDialog(); //Put the values of selected objects (or default)
	mainLayout->activate();

	//setMainWidget( mainWidget );

	m_colortab = new VColorTab( sel->objects().count() == 0 ? sel->stroke()->color() :
								sel->objects().getFirst()->stroke()->color(), this);
	m_colortab->insertTab( mainWidget, i18n("Stroke"), 0 );
	m_colortab->setCurrentPage( 0 );

	setMainWidget( m_colortab );

	disableResize();
	connect (this, SIGNAL( okClicked( void ) ), this, SLOT( slotOKClicked ( void ) ) );
}

void VStrokeDlg::slotTypeChanged( int ID )
{
	switch ( ID ) {
		case 1:
			m_stroke.setType ( VStroke::solid ); break;
		case 2:
			m_stroke.setType ( VStroke::grad ); break;
		default:
			m_stroke.setType ( VStroke::none );
	}
}

void VStrokeDlg::slotCapChanged( int ID )
{
	switch ( ID ) {
		case 1:
			m_stroke.setLineCap ( VStroke::capRound ); break;
		case 2:
			m_stroke.setLineCap ( VStroke::capSquare ); break;
		default:
			m_stroke.setLineCap ( VStroke::capButt );
	}
}

void VStrokeDlg::slotJoinChanged( int ID )
{
	switch ( ID ) {
		case 1:
			m_stroke.setLineJoin ( VStroke::joinRound ); break;
		case 2:
			m_stroke.setLineJoin ( VStroke::joinBevel ); break;
		default:
			m_stroke.setLineJoin ( VStroke::joinMiter );
	}
}

void VStrokeDlg::slotOKClicked()
{
	m_stroke.setLineWidth ( m_setLineWidth->value() );

	m_stroke.setColor( m_colortab->Color() );

	if( m_part && m_part->document().selection()->objects().count() > 0 )
		m_part->addCommand( new VStrokeCmd( &m_part->document(), &m_stroke ), true );

	emit strokeChanged( VStroke( m_stroke ) );
}

void VStrokeDlg::slotUpdateDialog()
{
	switch( m_stroke.type() )
	{
		case VStroke::solid:
			m_typeOption->setButton( 1 ); break;
		case VStroke::grad:
			m_typeOption->setButton( 2 ); break;
		default:
			m_typeOption->setButton( 0 );
	}

	switch( m_stroke.lineCap() )
	{
		case VStroke::capRound:
			m_capOption->setButton( 1 ); break;
		case VStroke::capSquare:
			m_capOption->setButton( 2 ); break;
		default:
			m_capOption->setButton( 0 );
	}

	switch( m_stroke.lineJoin() )
	{
		case VStroke::joinRound:
			m_joinOption->setButton( 1 ); break;
		case VStroke::joinBevel:
			m_joinOption->setButton( 2 ); break;
		default:
			m_joinOption->setButton( 0 );
	}
	
	m_setLineWidth->setValue( m_stroke.lineWidth() );
}

#include "vstrokedlg.moc"

