/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>

#include "propertybuffer.h"
//#include "propertybufferitem.h"

PropertyBuffer::PropertyBuffer(QObject *parent, const char *name)
 : QObject(parent, name),
   QPtrList<PropertyBufferItem>()
{
}

void
PropertyBuffer::changeProperty(QObject *o, const char *property, const QVariant &value)
{
	kdDebug() << "PropertyBuffer::changeProperty(): changing: " << property << endl;
	PropertyBufferItem *i;
	for(i = first(); i; i = next())
	{
		if(i->object() == o && i->name() == property)
		{
			if(i->name() == "name")
			{
				emit nameChanged(o, value.toString().latin1());
			}
			i->setValue(value);
			emit propertyChanged(o, property, value);
			return;
		}
	}

	i = new PropertyBufferItem(o, QString(property), value);
	append(i);
	emit propertyChanged(o, property, value);
	if(i->name() == "name")
		emit nameChanged(i->object(), value.toString().latin1());
}

PropertyBuffer::~PropertyBuffer()
{
}

#include "propertybuffer.moc"
