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

#include "vcdlg_polygon.h"

VCDlgPolygon::VCDlgPolygon()
	: KDialog( 0L, i18n( "Polygon" ), true, Qt::WStyle_Customize |
	  Qt::WStyle_Dialog | Qt::WStyle_NormalBorder | Qt::WStyle_Title )
{
	setCaption( i18n( "Polygon" ) );

	QBoxLayout* outerbox = new QHBoxLayout( this );

	// add input fields on the left:
	QGroupBox* group = new QGroupBox( 2, Qt::Horizontal, i18n( "Values" ), this );
 	outerbox->addWidget( group );

	new QLabel( i18n( "Radius:" ), group );
	m_radius = new QLineEdit( 0, group );
	new QLabel( i18n( "Edges:" ), group );
	m_edges = new QSpinBox( group );
	m_edges->setMinValue( 3 );

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
VCDlgPolygon::valueRadius() const
{
	return m_radius->text().toDouble();
}

uint
VCDlgPolygon::valueEdges() const
{
	return m_edges->value();
}

void
VCDlgPolygon::setValueRadius( const double value )
{
	QString s;
	s.setNum( value, 'f', 3 );
	m_radius->setText( s );
}

void
VCDlgPolygon::setValueEdges( const uint value )
{
	m_edges->setValue( value );
}

#include "vcdlg_polygon.moc"
