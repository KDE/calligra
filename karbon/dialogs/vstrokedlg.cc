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
	button = new QRadioButton ( i18n( "Miter" ), m_joinOption );
	m_joinOption->insert( button );
	button = new QRadioButton ( i18n( "Round" ), m_joinOption );
	m_joinOption->insert( button );
	button = new QRadioButton ( i18n( "Bevel" ), m_joinOption );
	m_joinOption->insert( button );
	m_joinOption->setTitle( i18n( "Join" ) );
	mainLayout->addWidget( m_joinOption );
	connect( m_joinOption, SIGNAL( clicked( int ) ), this, SLOT( slotJoinChanged( int ) ) );
	
	if( part->document().selection()->objects().count() > 0 ) // there is a selection, so take the stroke of first selected object
	{
		m_stroke.setType ( part->document().selection()->objects().getFirst()->stroke()->type() );
		m_stroke.setColor ( part->document().selection()->objects().getFirst()->stroke()->color() );
		m_stroke.setLineWidth ( part->document().selection()->objects().getFirst()->stroke()->lineWidth() );
		m_stroke.setLineCap ( part->document().selection()->objects().getFirst()->stroke()->lineCap() );   
		m_stroke.setLineJoin ( part->document().selection()->objects().getFirst()->stroke()->lineJoin() );
		m_stroke.setMiterLimit ( part->document().selection()->objects().getFirst()->stroke()->miterLimit() );
	}
	
	slotUpdateDialog(); //Put the values of selected objects (or default)
	mainLayout->activate();
	setMainWidget( mainWidget );
	disableResize();
	connect (this, SIGNAL( okClicked( void ) ), this, SLOT( slotOKClicked ( void ) ) );
}

void VStrokeDlg::slotTypeChanged( int ID )
{
	switch ( ID ) {
	case 1:
		m_stroke.setType ( stroke_stroke ); break;
	case 2:
		m_stroke.setType ( stroke_gradient ); break;
	default:
		m_stroke.setType ( stroke_none );
	}
}

void VStrokeDlg::slotCapChanged( int ID )
{
	switch ( ID ) {
	case 1:
		m_stroke.setLineCap ( cap_round ); break;
	case 2:
		m_stroke.setLineCap ( cap_square ); break;
	default:
		m_stroke.setLineCap ( cap_butt );
	}
}

void VStrokeDlg::slotJoinChanged( int ID )
{
	switch ( ID ) {
	case 1:
		m_stroke.setLineJoin ( join_round ); break;
	case 2:
		m_stroke.setLineJoin ( join_bevel ); break;
	default:
		m_stroke.setLineJoin ( join_miter );
	}
}

void VStrokeDlg::slotOKClicked()
{
	m_stroke.setLineWidth ( m_setLineWidth->value() );

	if( m_part )
		m_part->addCommand( new VStrokeCmd( &m_part->document(), m_stroke ), true );

	emit strokeChanged( VStroke( m_stroke ) );
}

void VStrokeDlg::slotUpdateDialog()
{
	switch ( m_stroke.type() ){
	case stroke_stroke:
		m_typeOption->setButton(1); break;
	case stroke_gradient:
		m_typeOption->setButton(2); break;
	default:
		m_typeOption->setButton(0);
	}
	
	switch ( m_stroke.lineCap() ){
	case cap_round:
		m_capOption->setButton(1); break;
	case cap_square:
		m_capOption->setButton(2); break;
	default:
		m_capOption->setButton(0);
	}
	
	switch ( m_stroke.lineJoin() ){
	case join_round:
		m_joinOption->setButton(1); break;
	case join_bevel:
		m_joinOption->setButton(2); break;
	default:
		m_joinOption->setButton(0);
	}
	
	m_setLineWidth->setValue( ( m_stroke.lineWidth() ) );
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

