/*
 * Copyright (C) 2003 Cédric Pasteur <cedric.pasteur@free.fr>
 */


#include <kmainwindow.h>
#include <klocale.h>
#include <kdebug.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qdatetimeedit.h>

#include <kexipropertybuffer.h>
#include <kexiproperty.h>
#include "kexipropertyeditor.h"

#include "test.h"

test::test()
    : KMainWindow( 0, "test" )
{
    setXMLFile("testui.rc");

	m_buffer = new KexiPropertyBuffer(this, "test");
	m_buffer->add(new KexiProperty("Name", "Name"));
	(*m_buffer)["Name"]->setAutoSync(1);
	
	m_buffer->add(new KexiProperty("Int", 2, "Int"));
	m_buffer->add(new KexiProperty("Double", 3.1415,"Double"));
	m_buffer->add(new KexiProperty("Rect", this->geometry(),"Rect"));
	m_buffer->add(new KexiProperty("Color", this->paletteBackgroundColor(),"Color"));
	const QPixmap *pix = this->icon();
	m_buffer->add(new KexiProperty("Pixmap", *pix,"Pixmap") );
	m_buffer->add(new KexiProperty("Font", this->font(),"Font") );
	
	QStringList list;//keys
	list.append("myitem");
	list.append("otheritem");
	list.append("item");
	QStringList name_list;
	name_list.append("My Item");
	name_list.append("Other Item");
	name_list.append("Item");
	m_buffer->add(new KexiProperty("List", "myitem", list, name_list, "List"));
	
	bool boolean=false;
	m_buffer->add(new KexiProperty("Bool", QVariant(boolean, 4), "Bool"));
	
	m_buffer->add(new KexiProperty("Date", QDate::currentDate(),"Date"));
	m_buffer->add(new KexiProperty("Time", QTime::currentTime(),"Time"));
	m_buffer->add(new KexiProperty("DateTime", QDateTime::currentDateTime(),"DateTime"));
	m_buffer->add(new KexiProperty("SizePolicy", sizePolicy(), "Size"));
	
	KexiPropertyEditor *edit = new KexiPropertyEditor(this,true/*autosync*/);
	setCentralWidget(edit);
	edit->setBuffer(m_buffer);
	resize(500,500);
}

test::~test()
{
}

#include "test.moc"
