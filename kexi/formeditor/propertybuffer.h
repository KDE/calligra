/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

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

#ifndef PROPERTYBUFFER_H
#define PROPERTYBUFFER_H

#include <qptrlist.h>
#include <qobject.h>
#include "propertybufferitem.h"

//class PropertyBufferItem;

//typedef QMap<QObject *, PropertyBufferItem *> PropertyBuffer;

class KFORMEDITOR_EXPORT PropertyBuffer : public QObject, public QPtrList<PropertyBufferItem>
{
	Q_OBJECT

	public:
		PropertyBuffer(QObject *parent, const char *name=0);
		~PropertyBuffer();

		void	changeProperty(QObject *o, const char *property, const QVariant &value);

	signals:
		void	propertyChanged(QObject *o, const char *property, const QVariant &value);
		void	nameChanged(QObject *o, QString newName);
};

#endif
