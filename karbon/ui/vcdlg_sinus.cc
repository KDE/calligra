/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

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

#include "vcdlg_sinus.h"

VCDlgSinus::VCDlgSinus()
	: KDialog( 0L, i18n( "Sinus" ), true, Qt::WStyle_Customize |
	  Qt::WStyle_Dialog | Qt::WStyle_NormalBorder | Qt::WStyle_Title )
{
	setCaption( i18n( "Sinus" ) );

	QBoxLayout* outerbox = new QHBoxLayout( this );

	// add input fields on the left:
	QGroupBox* group = new QGroupBox( 2, Qt::Horizontal, i18n( "Values" ), this );
 	outerbox->addWidget( group );

	// add width/height-input:
	new QLabel( i18n( "Width:" ), group );
	m_width = new QLineEdit( 0, group );
	new QLabel( i18n( "Height:" ), group );
	m_height = new QLineEdit( 0, group );
	new QLabel( i18n( "Periods:" ), group );
	m_periods = new QSpinBox( group );
	m_periods->setMinValue( 1 );

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
VCDlgSinus::valueWidth() const
{
	return m_width->text().toDouble();
}

double
VCDlgSinus::valueHeight() const
{
	return m_height->text().toDouble();
}

uint
VCDlgSinus::valuePeriods() const
{
	return m_periods->value();
}

void
VCDlgSinus::setValueWidth( const double value )
{
	QString s;
	s.setNum( value, 'f', 3 );
	m_width->setText( s );
}

void
VCDlgSinus::setValueHeight( const double value )
{
	QString s;
	s.setNum( value, 'f', 3 );
	m_height->setText( s );
}

void
VCDlgSinus::setValuePeriods( const uint value )
{
	m_periods->setValue( value );
}

#include "vcdlg_sinus.moc"
