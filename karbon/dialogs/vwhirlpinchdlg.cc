/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qevent.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qwidget.h>

#include <klocale.h>

#include "vwhirlpinchdlg.h"

VWhirlPinchDlg::VWhirlPinchDlg( QWidget* parent, const char* name )
	: KDialog( parent, name, true, Qt::WStyle_Customize |
	  WType_Dialog | Qt::WStyle_NormalBorder | Qt::WStyle_Title )
{
	setCaption( i18n( "Whirl Pinch" ) );

	QBoxLayout* outerbox = new QHBoxLayout( this );

	// add input fields on the left:
	QGroupBox* group = new QGroupBox(
		2, Qt::Horizontal,
		i18n( "Properties" ), this );

 	outerbox->addWidget( group );

	new QLabel( i18n( "Angle:" ), group );
	m_angle = new QLineEdit( 0, group );
	new QLabel( i18n( "Pinch:" ), group );
	m_pinch = new QLineEdit( 0, group );
	new QLabel( i18n( "Radius:" ), group );
	m_radius = new QLineEdit( 0, group );

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
VWhirlPinchDlg::angle() const
{
	return m_angle->text().toDouble();
}

double
VWhirlPinchDlg::pinch() const
{
	return m_pinch->text().toDouble();
}

double
VWhirlPinchDlg::radius() const
{
	return m_radius->text().toDouble();
}

void
VWhirlPinchDlg::setAngle( double value )
{
	QString s;
	s.setNum( value, 'f', 3 );
	m_angle->setText( s );
}

void
VWhirlPinchDlg::setPinch( double value )
{
	QString s;
	s.setNum( value, 'f', 3 );
	m_pinch->setText( s );
}

void
VWhirlPinchDlg::setRadius( double value )
{
	QString s;
	s.setNum( value, 'f', 3 );
	m_radius->setText( s );
}

#include "vwhirlpinchdlg.moc"

