/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers
*/

/* vreference.cc */

#include <qbuttongroup.h>
#include <qlayout.h>
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
	
	mButtonGroup->setButton( Center );
	layout->addWidget( mButtonGroup );
	layout->activate();
}

#include "vreference.moc"

