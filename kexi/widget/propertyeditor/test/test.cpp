/*
 * Copyright (C) 2003 Cédric Pasteur <cedric.pasteur@free.fr>
 */


#include <kmainwindow.h>
#include <klocale.h>
#include <kdebug.h>
#include <qpixmap.h>
#include <qstringlist.h>

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
	list.append("Item 1");
	list.append("Item 2");
	list.append("Item 3");
	m_buffer->insert("list", KexiProperty("list","Item 1" , list));
	
	bool boolean=false;
	m_buffer->insert("bool", KexiProperty("bool", QVariant(boolean, 4)));
	
	KexiPropertyEditor *edit = new KexiPropertyEditor(this,true);
	setCentralWidget(edit);
	edit->setBuffer(m_buffer);
	edit->resize(edit->sizeHint());
}

test::~test()
{
}

#include "test.moc"
