/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qbuttongroup.h>
#include <qlayout.h>
#include <kpixmap.h>
#include <kstandarddirs.h>
#include <qtoolbutton.h>
#include <kdualcolorbutton.h>

#include "vtoolcontainer.h"

VToolContainer::VToolContainer( KarbonView* parent, const char* name )
{
	QToolButton *button;
	QHBoxLayout *layout = new QHBoxLayout(this, 1);

	setLabel("");
	btngroup = new QButtonGroup( 2, Horizontal, this );
	btngroup->setExclusive( true );
	button = new QToolButton(btngroup);
	QPixmap pixmap(locate("appdata", "icons/hicolor/22x22/actions/select.png"));
	button->setPixmap(pixmap);
	button->setToggleButton( true );
	button->toggle();
	btngroup->insert( button, Select );

	button = new QToolButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/16x16/actions/scale.png"));
	button->setPixmap(pixmap);
	button->setToggleButton( true );
	btngroup->insert( button, Scale );

	button = new QToolButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/rotate.png"));
	button->setPixmap(pixmap);
	button->setToggleButton( true );
	btngroup->insert( button, Rotate );
	
	button = new QToolButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/16x16/actions/shear.png"));
	button->setPixmap(pixmap);
	button->setToggleButton( true );
	btngroup->insert( button, Shear );
	
	button = new QToolButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/ellipse.png"));
	button->setPixmap(pixmap);
	button->setToggleButton( true );
	btngroup->insert( button, Ellipse );
	
	button = new QToolButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/rectangle.png"));
	button->setPixmap(pixmap);
	button->setToggleButton( true );
	btngroup->insert( button, Rectangle );

	button = new QToolButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/roundrect.png"));
	button->setPixmap(pixmap);
	button->setToggleButton( true );
	btngroup->insert( button, Roundrect );
	
	button = new QToolButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/polygon.png"));
	button->setPixmap(pixmap);
	button->setToggleButton( true );
	btngroup->insert( button, Polygon );
	
	button = new QToolButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/star.png"));
	button->setPixmap(pixmap);
	button->setToggleButton( true );
	btngroup->insert( button, Star );

	button = new QToolButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/sinus.png"));
	button->setPixmap(pixmap);
	button->setToggleButton( true );
	btngroup->insert( button, Sinus );

	button = new QToolButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/spiral.png"));
	button->setPixmap(pixmap);
	button->setToggleButton( true );
	btngroup->insert( button, Spiral );

	btngroup->setInsideSpacing(2);
	btngroup->setInsideMargin(5);
	connect(btngroup, SIGNAL(clicked(int)), this, SLOT(slotButton(int)));
	layout->addWidget(btngroup);
	//setFixedSize(btngroup->baseSize());
	layout->activate();
}

void VToolContainer::slotButton( int ID)
{ //if ( ! ID = Scale) btngroup(Select)->toggle();
}

VToolContainer::~VToolContainer() { }

#include "vtoolcontainer.moc"
