/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcombobox.h>
#include <qevent.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qwidget.h>

#include <klocale.h>

#include "vcdlg_spiral.h"

VCDlgSpiral::VCDlgSpiral()
	: KDialog( 0L, 0, true, Qt::WStyle_Customize |
		WType_Dialog | Qt::WStyle_NormalBorder | Qt::WStyle_Title )
{
	setCaption( i18n( "Insert Spiral" ) );

	QBoxLayout* outerbox = new QHBoxLayout( this );

	// add input fields on the left:
	QGroupBox* group = new QGroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );
	outerbox->addWidget( group );

	new QLabel( i18n( "Radius:" ), group );
	m_radius = new QLineEdit( 0, group );
	new QLabel( i18n( "Segments:" ), group );
	m_segments = new QSpinBox( group );
	m_segments->setMinValue( 1 );
	new QLabel( i18n( "Fade:" ), group );
	m_fade = new QLineEdit( 0, group );
	new QLabel( i18n( "Orientation:" ), group );
	m_clockwise = new QComboBox( false,group );
	m_clockwise->insertItem( i18n( "Clockwise" ), 0 );
	m_clockwise->insertItem( i18n( "Counter Clockwise" ), 1 );

	outerbox->addSpacing( 2 );

	// add buttons on the right side:
	QBoxLayout* innerbox = new QVBoxLayout( outerbox );

	innerbox->addStretch();

	QPushButton* okbutton = new QPushButton( i18n( "&Ok" ), this );
	QPushButton* cancelbutton = new QPushButton( i18n( "&Cancel" ), this );

	okbutton->setMaximumSize( okbutton->sizeHint() );
	cancelbutton->setMaximumSize( cancelbutton->sizeHint() );

	okbutton->setFocus();

	innerbox->addWidget( okbutton );
	innerbox->addSpacing( 2 );
	innerbox->addWidget( cancelbutton );

	// signals and slots:
	connect( okbutton, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( cancelbutton, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

double
VCDlgSpiral::valueRadius() const
{
	return m_radius->text().toDouble();
}

uint
VCDlgSpiral::valueSegments() const
{
	return m_segments->value();
}

double
VCDlgSpiral::valueFade() const
{
	return m_fade->text().toDouble();
}

bool
VCDlgSpiral::valueClockwise() const
{
	if( m_clockwise->currentItem() == 0 )
		return true;
	else
		return false;
}

void
VCDlgSpiral::setValueRadius( const double value )
{
	QString s;
	s.setNum( value, 'f', 3 );
	m_radius->setText( s );
}

void
VCDlgSpiral::setValueSegments( const uint value )
{
	m_segments->setValue( value );
}

void
VCDlgSpiral::setValueFade( const double value )
{
	QString s;
	s.setNum( value, 'f', 3 );
	m_fade->setText( s );
}

void
VCDlgSpiral::setValueClockwise( const bool value )
{
	if( value )
		m_clockwise->setCurrentItem( 0 );
	else
		m_clockwise->setCurrentItem( 1 );
}

#include "vcdlg_spiral.moc"
