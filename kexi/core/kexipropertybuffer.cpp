/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>

#include <qdatetime.h>

#include "kexipropertybuffer.h"

KexiProperty KexiPropertyBuffer::m_nonConstNull;

KexiPropertyBuffer::KexiPropertyBuffer(QObject *parent, const QString &type_name)
 : QObject(parent, type_name.latin1())
	,QAsciiDict<KexiProperty>(101, false)
	,m_typeName( type_name )
	,m_collection(0)
{
	setAutoDelete( true );
}

KexiPropertyBuffer::~KexiPropertyBuffer()
{
	emit destroying();
}

void
KexiPropertyBuffer::changeProperty(const QCString &property, const QVariant &value)
{
	KexiProperty& prop = this->property(property);
	if (prop.isNull())
		return;

	kdDebug() << "KexiPropertyBuffer::changeProperty(): changing: " << property
		<< " from '" << (prop.value().toString().isNull() ? QString("NULL") : prop.value().toString())
		<< "' to '" << (value.toString().isNull() ? QString("NULL") : value.toString()) << "'" << endl;
/*
	bool ch = false;
	if (prop->value().type()==QVariant::DateTime
		|| prop->value().type()==QVariant::Time) {
		//for date and datetime types: compare with strings, because there
		//can be miliseconds difference
		ch = prop->value().toString() != value.toString();
	}
	else {
		ch = prop->value() != value;
	}

	if (prop->value().type()==QVariant::String) {
		//property is also changed for string type, if one of value is empty and other isn't
		if (prop->value().toString().isEmpty() != value.toString().isEmpty())
			ch = true;
	}

	if (ch) {
		prop->setValue(value);
		emit propertyChanged(*this, *prop);
	}*/
	prop = value;
}

void
KexiPropertyBuffer::add(KexiProperty *property)
{
	property->m_buf = this;
	insert(property->name(), property);
	m_list.append( property );
}

void KexiPropertyBuffer::clear()
{
	m_list.clear();
	QAsciiDict<KexiProperty>::clear();
}

void KexiPropertyBuffer::debug()
{
	kdDebug() << "KexiPropertyBuffer: typeName='" << m_typeName << "'" << endl;
	if (isEmpty()) {
		kdDebug() << "<EMPTY>" << endl;
		return;
	}
	kdDebug() << count() << " properties:" << endl;

	KexiProperty::ListIterator it(m_list);
	for (;it.current();++it) {
		it.current()->debug();
	}
}

void
KexiPropertyBuffer::addCollectionPixmap(KexiProperty *prop, const QString pixmapName)
{
	if(!prop)  return;
	prop->setPixmapName(pixmapName);
	emit collectionItemChoosed(*this, *prop);
}

QString
KexiPropertyBuffer::pixmapName(const char *name)
{
	if(property(name))
		return property(name).pixmapName();
	return QString::null;
}

#include "kexipropertybuffer.moc"

