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

#ifndef OBJPROPBUFFER_H
#define OBJPROPBUFFER_H

#include "kexipropertybuffer.h"

class KexiPropertyEditor;

namespace KFormDesigner {

class Form;
class FormManager;

class KEXIPROPERTYEDITOR_EXPORT ObjectPropertyBuffer : public KexiPropertyBuffer
{
	Q_OBJECT

	public:
		ObjectPropertyBuffer(FormManager *manager, QObject *parent, const char *name=0);
		~ObjectPropertyBuffer();

		void	changeProperty(const QString &property, const QVariant &value);

		bool    eventFilter(QObject *o, QEvent *ev);
		void    checkModifiedProp();
	
	public slots:
		void	setObject(QWidget *obj);

	signals:
		void	nameChanged(const QString &oldname, const QString &newname);
		void	propertyChanged(QObject *, const QString &property, const QVariant &v);

	protected:
		bool    showProperty(QObject *obj, const QString &property);
	
	private:
		QObject		*m_object;
		FormManager     *m_manager;
};

}

#endif

