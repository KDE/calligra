/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qvbuttongroup.h>

#include <klocale.h>

#include <tkfloatspinbox.h>

#include "karbon_part.h"
#include "vcolor.h"
#include "vselection.h"
#include "vstrokecmd.h"
#include "vstrokedlg.h"

VStrokeDlg::VStrokeDlg( KarbonPart* part, QWidget* parent, const char* name )
	: KDialogBase ( parent, name, true, i18n( "Stroke" ), Ok | Cancel ), m_part( part )
{
	enableButtonSeparator( true );
	QWidget *mainWidget = new QWidget( this, "strokemain" );
	QHBoxLayout *mainLayout = new QHBoxLayout (mainWidget, 3);
	
	QVBoxLayout *leftLayout = new QVBoxLayout( mainLayout, 4 );
	
	QLabel* widthLabel = new QLabel( i18n ( "Width:" ), mainWidget );
	leftLayout->addWidget ( widthLabel );
	m_setLineWidth = new TKUFloatSpinBox( mainWidget );
	m_setLineWidth->setDecimals(1);
	m_setLineWidth->setMinValue(0.0);
	m_setLineWidth->setLineStep(0.5);
	leftLayout->addWidget ( m_setLineWidth );
		
	//Dashing ->
	QLabel* styleLabel = new QLabel( i18n ( "Style:" ), mainWidget );
	leftLayout->addWidget ( styleLabel );
	m_styleCombo = new QComboBox( mainWidget );
	m_styleCombo->setEnabled ( false );
	leftLayout->addWidget ( m_styleCombo );
	// <- Dashing - reserved for later
	
	QRadioButton* button;
	m_typeOption = new QVButtonGroup ( mainWidget );
	button = new QRadioButton ( i18n( "None" ), m_typeOption );
	m_typeOption->insert( button );
	button = new QRadioButton ( i18n( "Stroke" ), m_typeOption );
	m_typeOption->insert( button );
	button = new QRadioButton ( i18n( "Gradient" ), m_typeOption );
	m_typeOption->insert( button );
	m_typeOption->setTitle( i18n( "Type" ) );
	mainLayout->addWidget( m_typeOption );
	connect( m_typeOption, SIGNAL( clicked( int ) ), this, SLOT( slotTypeChanged( int ) ) );
	
	m_capOption = new QVButtonGroup ( mainWidget );
	button = new QRadioButton ( i18n( "Butt" ), m_capOption );
	m_capOption->insert( button );
	button = new QRadioButton ( i18n( "Round" ), m_capOption );
	m_capOption->insert( button );
	button = new QRadioButton ( i18n( "Square" ), m_capOption );
	m_capOption->insert( button );
	m_capOption->setTitle( i18n( "Cap" ) );
	mainLayout->addWidget( m_capOption );
	connect( m_capOption, SIGNAL( clicked( int ) ), this, SLOT( slotCapChanged( int ) ) );
	
	m_joinOption = new QVButtonGroup ( mainWidget );
	button = new QRadioButton ( i18n( "Mittel" ), m_joinOption );
	m_joinOption->insert( button );
	button = new QRadioButton ( i18n( "Round" ), m_joinOption );
	m_joinOption->insert( button );
	button = new QRadioButton ( i18n( "Bevel" ), m_joinOption );
	m_joinOption->insert( button );
	m_joinOption->setTitle( i18n( "Join" ) );
	mainLayout->addWidget( m_joinOption );
	connect( m_joinOption, SIGNAL( clicked( int ) ), this, SLOT( slotJoinChanged( int ) ) );
	
	mainLayout->activate();
	setMainWidget( mainWidget );
	disableResize();
	connect (this, SIGNAL( okClicked( void ) ), this, SLOT( slotOKClicked ( void ) ) );
}

void VStrokeDlg::slotTypeChanged( int ID )
{
	m_type = ID;
}

void VStrokeDlg::slotCapChanged( int ID )
{
	m_cap = ID;
}

void VStrokeDlg::slotJoinChanged( int ID )
{
	m_join = ID;
}

void VStrokeDlg::slotOKClicked()
{
	VStroke stroke;
	
	switch ( m_type ) {
	case 1:
		stroke.setType ( stroke_stroke ); break;
	case 2:
		stroke.setType ( stroke_gradient ); break;
	default:
		stroke.setType ( stroke_none );
	}

	switch ( m_cap ) {
	case 1:
		stroke.setLineCap ( cap_round ); break;
	case 2:
		stroke.setLineCap ( cap_square ); break;
	default:
		stroke.setLineCap ( cap_butt );
	}

	//switch ( m_join ) {
	//case 1:
//		stroke.setLineJoin ( join_round ); break;
//	case 2:
		stroke.setLineJoin ( join_bevel );// break;
//	default:
//		stroke.setLineJoin ( join_miter );
//	}

	float w = m_setLineWidth->value();
	stroke.setLineWidth ( w );

	if( m_part )
		m_part->addCommand( new VStrokeCmd( &m_part->document(), stroke ), true );

	emit strokeChanged( VStroke( stroke ) );
}

/*void VStrokeDlg::slotApplyButtonPressed()
{
	VColor color;
	float r = mRed->value() / 255.0, g = mGreen->value() / 255.0, b = mBlue->value() / 255.0;
	float op = mOpacity->value() / 100.0;
	color.setValues( &r, &g, &b, 0L );
	color.setOpacity( op );

	if( m_part )
		m_part->addCommand( new VStrokeCmd( &m_part->document(), VStroke( color ) ), true );

	emit strokeChanged( VStroke( color ) );
}*/

#include "vstrokedlg.moc"

