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
#include <qstring.h>
#include <qwidget.h>

#include <klocale.h>

#include "vcdlg_rectangle.h"

VCDlgRectangle::VCDlgRectangle()
	: KDialog( 0L, i18n( "Rectangle" ), true, Qt::WStyle_Customize |
	  Qt::WStyle_Dialog | Qt::WStyle_NormalBorder | Qt::WStyle_Title )
{
	setCaption( i18n( "Rectangle" ) );

	QBoxLayout* outerbox = new QHBoxLayout( this );

	// add input fields on the left:
	QGroupBox* group = new QGroupBox( 2, Qt::Horizontal, i18n( "Values" ), this );
 	outerbox->addWidget( group );

	// add width/height-input:
	new QLabel( i18n( "Width:" ), group );
	m_width = new QLineEdit( 0, group );
	new QLabel( i18n( "Height:" ), group );
	m_height = new QLineEdit( 0, group );
	new QLabel( i18n( "Edge Radius:" ), group );
	m_round = new QLineEdit( 0, group );

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
VCDlgRectangle::valueWidth()
{
	return m_width->text().toDouble();
}

double
VCDlgRectangle::valueHeight()
{
	return m_height->text().toDouble();
}

double
VCDlgRectangle::valueRound()
{
	return m_round->text().toDouble();
}

void
VCDlgRectangle::setValueWidth( const double value )
{
	QString s;
	s.setNum( value, 'f', 3 );
	m_width->setText( s );
}

void
VCDlgRectangle::setValueHeight( const double value )
{
	QString s;
	s.setNum( value, 'f', 3 );
	m_height->setText( s );
}

void
VCDlgRectangle::setValueRound( const double value )
{
	QString s;
	s.setNum( value, 'f', 3 );
	m_round->setText( s );
}

#include "vcdlg_rectangle.moc"
