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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qgroupbox.h>
#include <qlabel.h>

#include <klocale.h>
#include <knuminput.h>

#include "karbon_part.h"
#include "vroundrectdlg.h"

VRoundRectDlg::VRoundRectDlg( KarbonPart*part,QWidget* parent, const char* name )
	: KDialogBase( parent, name, true,  i18n( "Insert Round Rectangle"), Ok | Cancel ), m_part( part)
{
	// add input fields:
	QGroupBox* group = new QGroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );

	// add width/height-input:
	m_widthLabel=new QLabel( i18n( "Width(%1):" ).arg(m_part->getUnitName()), group );

	m_width = new KDoubleNumInput(0,group);
	m_width->setRange(0, 1000, 0.1);

	m_heightLabel = new QLabel( i18n( "Height(%1):" ).arg(m_part->getUnitName()), group );
	m_height = new KDoubleNumInput(0,group);
	m_height->setRange(0, 1000, 0.1);

	new QLabel( i18n( "Edge Radius:" ), group );
	m_round = new KDoubleNumInput(0,group);
	m_round->setRange(0, 1000, 0.1);

	group->setMinimumWidth( 300 );
	
	// signals and slots:
	connect( this, SIGNAL( okClicked() ), this, SLOT( accept() ) );
	connect( this, SIGNAL( cancelClicked() ), this, SLOT( reject() ) );
	
	setMainWidget( group );
	setFixedSize( baseSize() );
}

double
VRoundRectDlg::width() const
{
	return KoUnit::ptFromUnit(m_width->value(),m_part->getUnit()) ;
}

double
VRoundRectDlg::height() const
{
       return KoUnit::ptFromUnit(m_height->value(),m_part->getUnit()) ;
}

double
VRoundRectDlg::round() const
{
	return m_round->value();
}

void
VRoundRectDlg::setWidth( double value )
{
    m_width->setValue(KoUnit::ptToUnit( value, m_part->getUnit() ));
}

void
VRoundRectDlg::setHeight( double value )
{
    m_height->setValue( KoUnit::ptToUnit( value, m_part->getUnit() ) );
}

void
VRoundRectDlg::setRound( double value )
{
    m_round->setValue( value);
}

void VRoundRectDlg::refreshUnit ()
{
    m_widthLabel->setText(i18n( "Width(%1):" ).arg(m_part->getUnitName()));
    m_heightLabel->setText( i18n( "Height(%1):" ).arg(m_part->getUnitName()));
}

#include "vroundrectdlg.moc"

