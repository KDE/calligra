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

#ifndef FORMIO_H
#define FORMIO_H

#include <qobject.h>
#include <qstring.h>

class QString;
class QDomElement;
class QDomDocument;
class QVariant;

namespace KFormDesigner {

class ObjectPropertyBuffer;
class Form;
class ObjectTreeItem;

class KEXIPROPERTYEDITOR_EXPORT FormIO : public QObject
{
	Q_OBJECT
	
	public:
		FormIO(QObject *parent, ObjectPropertyBuffer *buffer, const char *name);
		~FormIO(){;}
		
		int saveForm(Form *form, const QString &filename=QString::null);
		int loadForm(Form *form, const QString &filename);
		
		void setPropertyBuffer(ObjectPropertyBuffer *buff) { m_buffer = buff;}

	protected:
		QDomElement  prop(QDomDocument &parent, const char *name, const QVariant &value);
		QDomElement  enumProp(QDomDocument &parent, const char *name, const QVariant &value);
		void         saveWidget(ObjectTreeItem *item, QDomElement &parent, QDomDocument &domDoc);
		QString      saveImage(QDomDocument &domDoc, const QPixmap &pixmap);

	private:
		ObjectPropertyBuffer	*m_buffer;
		int			m_count;
};

}

#endif


