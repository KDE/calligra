/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <klocale.h>
#include <kpixmap.h>
#include <kstandarddirs.h>

#include "vtoolcontainer.h"

VToolContainer::VToolContainer( QObject* parent, const char* name )
{
	QButton *button;
	QVBoxLayout *layout = new QVBoxLayout(this, 1);

	setCaption(i18n("Tools"));
	btngroup = new QButtonGroup( 2, Horizontal, this );
	button = new QPushButton(btngroup);
	QPixmap pixmap(locate("appdata", "icons/hicolor/22x22/actions/select.png"));
	button->setPixmap(pixmap);
	btngroup->insert( button, Select );

	button = new QPushButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/16x16/actions/scale.png"));
	button->setPixmap(pixmap);
	btngroup->insert( button, Scale );

	button = new QPushButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/rotate.png"));
	button->setPixmap(pixmap);
	btngroup->insert( button, Rotate );
	
	button = new QPushButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/16x16/actions/shear.png"));
	button->setPixmap(pixmap);
	btngroup->insert( button, Shear );
	
	button = new QPushButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/ellipse.png"));
	button->setPixmap(pixmap);
	btngroup->insert( button, Ellipse );
	
	button = new QPushButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/rectangle.png"));
	button->setPixmap(pixmap);
	btngroup->insert( button, Rectangle );

	button = new QPushButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/roundrect.png"));
	button->setPixmap(pixmap);
	btngroup->insert( button, Roundrect );
	
	button = new QPushButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/polygon.png"));
	button->setPixmap(pixmap);
	btngroup->insert( button, Polygon );
	
	button = new QPushButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/star.png"));
	button->setPixmap(pixmap);
	btngroup->insert( button, Star );
	
	button = new QPushButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/sinus.png"));
	button->setPixmap(pixmap);
	btngroup->insert( button, Sinus );

	button = new QPushButton(btngroup);
	pixmap = QPixmap(locate("appdata", "icons/hicolor/22x22/actions/spiral.png"));
	button->setPixmap(pixmap);
	btngroup->setInsideSpacing(0);
	btngroup->setInsideMargin(0);
	btngroup->insert( button, Spiral );

	connect(btngroup, SIGNAL(clicked(int)), this, SLOT(slotButton(int)));
	layout->addWidget(btngroup);
	setFixedSize(baseSize());
	layout->activate();
}

void VToolContainer::slotButton( int ID)
{
}

VToolContainer::~VToolContainer() { }

#include "vtoolcontainer.moc"
