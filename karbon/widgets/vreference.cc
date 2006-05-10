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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

/* vreference.cc */

#include <qbuttongroup.h>
#include <QLayout>
#include <qradiobutton.h>

#include "vreference.h"

VReference::VReference( QWidget *parent, const char *name ) : QFrame ( parent, name )
{
	QVBoxLayout* layout = new QVBoxLayout( this );
	mButtonGroup = new QButtonGroup (3, Vertical, this );
	QRadioButton* radio = new QRadioButton ( mButtonGroup );
	mButtonGroup->insert( radio, TopLeft );
	radio = new QRadioButton ( mButtonGroup );
	mButtonGroup->insert( radio, Left );
	radio = new QRadioButton ( mButtonGroup );
	mButtonGroup->insert( radio, BottomLeft );
	radio = new QRadioButton ( mButtonGroup );
	mButtonGroup->insert( radio, Top );
	radio = new QRadioButton ( mButtonGroup );
	mButtonGroup->insert( radio, Center );
	radio = new QRadioButton ( mButtonGroup );
	mButtonGroup->insert( radio, Bottom );
	radio = new QRadioButton ( mButtonGroup );
	mButtonGroup->insert( radio, TopRight );
	radio = new QRadioButton ( mButtonGroup );
	mButtonGroup->insert( radio, Right );
	radio = new QRadioButton ( mButtonGroup );
	mButtonGroup->insert( radio, BottomRight );
	
	connect(
		mButtonGroup, SIGNAL( clicked( int ) ),
		this, SLOT( setReferencePoint( int ) ) );
	
	mButtonGroup->setButton( Center );
	layout->addWidget( mButtonGroup );
	layout->activate();
	setReferencePoint( Center );
}

void VReference::setReferencePoint ( int i )
{
	m_referencePoint = i;
	emit referencePointChanged( m_referencePoint );
}

int VReference::referencePoint()
{
	return( m_referencePoint );
}

#include "vreference.moc"

