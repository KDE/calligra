/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#include <kmainwindow.h>
#include <klocale.h>
#include <kdebug.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qdatetimeedit.h>
#include <qcursor.h>

#include <koproperty/property.h>
#include <koproperty/editor.h>

#include "test.h"

using namespace KoProperty;

test::test()
    : KMainWindow( 0, "test" )
{
    setXMLFile("testui.rc");

//  Simple
	m_set = new Set(this, "test");
	m_set->addProperty(new Property("Name", "Name"), "SimpleGroup");
	(*m_set)["Name"].setAutoSync(1);
	
	m_set->addProperty(new Property("Int", 2, "Int"), "SimpleGroup");
	m_set->addProperty(new Property("Double", 3.1415,"Double"), "SimpleGroup");
	m_set->addProperty(new Property("Bool", QVariant(true, 4), "Bool"), "SimpleGroup");
	m_set->addProperty(new Property("Date", QDate::currentDate(),"Date"), "SimpleGroup");
	m_set->addProperty(new Property("Time", QTime::currentTime(),"Time"), "SimpleGroup");
	m_set->addProperty(new Property("DateTime", QDateTime::currentDateTime(),"DateTime"), "SimpleGroup");

	QStringList list;//keys
	list.append("myitem");
	list.append("otheritem");
	list.append("3rditem");
	QStringList name_list; //strings
	name_list.append("My Item");
	name_list.append("Other Item");
	name_list.append("Third Item");
	m_set->addProperty(new Property("List", list, name_list, "otheritem", "List"), "SimpleGroup");

//  Complex
	m_set->addProperty(new Property("Rect", this->geometry(),"Rect"), "ComplexGroup");
	m_set->addProperty(new Property("Point", QPoint(3,4), "Point"), "ComplexGroup");
	m_set->addProperty(new Property("Size", QPoint(3,4), "Size"), "ComplexGroup");

//  Appearance
	m_set->addProperty(new Property("Color", this->paletteBackgroundColor(),"Color"), "AppearanceGroup");
	const QPixmap *pix = this->icon();
	m_set->addProperty(new Property("Pixmap", *pix,"Pixmap"), "AppearanceGroup");
	m_set->addProperty(new Property("Font", this->font(),"Font"), "AppearanceGroup");
	m_set->addProperty(new Property("Cursor", QCursor(Qt::WaitCursor),"Cursor"), "AppearanceGroup");
	m_set->addProperty(new Property("LineStyle", 3, "LineStyle", "", LineStyle), "AppearanceGroup");

	m_set->addProperty(new Property("SizePolicy", sizePolicy(), "SizePolicy"), "ComplexGroup");

	Editor *edit = new Editor(this,true/*autosync*/);
	setCentralWidget(edit);
	edit->changeSet(m_set);
	resize(500,500);
}

test::~test()
{
}

#include "test.moc"
