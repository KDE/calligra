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

	m_buffer = new KexiPropertyBuffer(this);
	m_buffer->insert("Name", KexiProperty("Name", "Name"));
	m_buffer->insert("Int", KexiProperty("Int", 2));
	m_buffer->insert("Double", KexiProperty("Double", 3.1415));
	m_buffer->insert("Rect", KexiProperty("Rect", this->geometry()));
	m_buffer->insert("Color", KexiProperty("Color", this->paletteBackgroundColor()));
	const QPixmap *pix = this->icon();
	m_buffer->insert("pixmap", KexiProperty("pixmap", *pix) );
	m_buffer->insert("font", KexiProperty("font", this->font()) );
	
	QStringList list;
	list.append("MyItem");
	list.append("OtherItem");
	list.append("Item");
	m_buffer->insert("list", KexiProperty("list","Item 1" , list));
	
	bool boolean=false;
	m_buffer->insert("bool", KexiProperty("bool", QVariant(boolean, 4)));
	
	m_buffer->insert("date", KexiProperty("date", QDate::currentDate()));
	m_buffer->insert("time", KexiProperty("time", QTime::currentTime()));
	m_buffer->insert("datetime", KexiProperty("datetime", QDateTime::currentDateTime()));
	
	KexiPropertyEditor *edit = new KexiPropertyEditor(this,true);
	setCentralWidget(edit);
	edit->setBuffer(m_buffer);
	resize(500,500);
}

test::~test()
{
}

#include "test.moc"
