/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <klocale.h>
#include <kpixmap.h>

#include "vtoolcontainer.h"

VToolContainer::VToolContainer( QObject* parent, const char* name )
{
	QButton *button;
	QVBoxLayout *layout = new QVBoxLayout(this, 1);

	setCaption(i18n("Tools"));
	btngroup = new QButtonGroup( 2, Horizontal, this );
	button = new QPushButton(btngroup);
	QPixmap pixmap("select");
	button->setPixmap(pixmap);
	btngroup->insert( button, Select );

	button = new QPushButton(btngroup);
	pixmap = QPixmap("scale");
	button->setPixmap(pixmap);
	btngroup->insert( button, Scale );

	button = new QPushButton(btngroup);
	pixmap = QPixmap("rotate");
	button->setPixmap(pixmap);
	btngroup->insert( button, Rotate );
	
	button = new QPushButton(btngroup);
	pixmap = QPixmap("shear");
	button->setPixmap(pixmap);
	btngroup->insert( button, Shear );
	
	button = new QPushButton(btngroup);
	pixmap = QPixmap("ellipse");
	button->setPixmap(pixmap);
	btngroup->insert( button, Ellipse );
	
	button = new QPushButton(btngroup);
	pixmap = QPixmap("rectangle");
	button->setPixmap(pixmap);
	btngroup->insert( button, Rectangle );

	button = new QPushButton(btngroup);
	pixmap = QPixmap("roundrect");
	button->setPixmap(pixmap);
	btngroup->insert( button, Roundrect );
	
	button = new QPushButton(btngroup);
	pixmap = QPixmap("polygon");
	button->setPixmap(pixmap);
	btngroup->insert( button, Polygon );
	
	button = new QPushButton(btngroup);
	pixmap = QPixmap("star");
	button->setPixmap(pixmap);
	btngroup->insert( button, Star );
	
	button = new QPushButton(btngroup);
	pixmap = QPixmap("sinus");
	button->setPixmap(pixmap);
	btngroup->insert( button, Sinus );
	
	button = new QPushButton(btngroup);
	pixmap = QPixmap("spiral");
	button->setPixmap(pixmap);
	btngroup->insert( button, Spiral );

	connect(btngroup, SIGNAL(clicked(int)), this, SLOT(slotButton(int)));
	layout->addWidget(btngroup);
	//resize(100,100);
	layout->activate();
}

void VToolContainer::slotButton( int ID)
{
}

VToolContainer::~VToolContainer() { }

#include "vtoolcontainer.moc"
