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

#include <kdebug.h>
#include <qstringlist.h>
#include <qstrlist.h>
#include <qmetaobject.h>
#include <qvariant.h>

#include "objecttree.h"
#include "form.h"
#include "formmanager.h"
#include "kexipropertyeditor.h"
#include "kexipropertyeditoritem.h"

#include "objpropbuffer.h"

namespace KFormDesigner {

ObjectPropertyBuffer::ObjectPropertyBuffer(FormManager *manager, QObject *parent, const char *name)
 : KexiPropertyBuffer(parent, name)
{
	m_object = 0;
	m_manager = manager;
}

void
ObjectPropertyBuffer::changeProperty(const QString &property, const QVariant &value)
{
	kdDebug() << "ObjPropBuffer::changeProperty(): changing: " << property << endl;
	KexiPropertyBuffer::changeProperty(property, value);
	
	if(property == "name")
		emit nameChanged(m_object->name(), value.toString());

	if(value.type() == QVariant::StringList)
	{
		QStrList list;
		QStringList StrList(value.toStringList());
		for(QStringList::iterator it = StrList.begin(); it != StrList.end(); ++it)
			list.append((*it).latin1());
		int count = m_object->metaObject()->findProperty(property.latin1(), true);
		const QMetaProperty *meta = m_object->metaObject()->property(count, true);
		QVariant val = meta->keysToValue(list);
		m_object->setProperty(property.latin1(), val);
	}
	m_object->setProperty(property.latin1(), value);
	emit propertyChanged(m_object, property, value);
}

void
ObjectPropertyBuffer::setObject(QWidget *widg)
{
	QObject *obj = (QObject*)widg; 
	if(obj==m_object)
		return;
	checkModifiedProp();
	kdDebug() << "loading object = " << widg->name() << endl;
	
	if(m_object)
		m_object->removeEventFilter(this);

	m_manager->editor()->reset(false);
	clear();

	m_object = obj;
	QStrList pList = obj->metaObject()->propertyNames(true);

	int count = 0;
	QStrListIterator it(pList);
	for(; it.current() != 0; ++it)
	{
		count = obj->metaObject()->findProperty(*it, true);
		const QMetaProperty *meta = obj->metaObject()->property(count, true);
		if(meta->designable(obj))
		{
			if(!showProperty(obj, meta->name()))
				continue;
			QStrList keys = meta->enumKeys();
			if(meta->isEnumType())
			{
				if(meta->isSetType())
				{
				kdDebug() << "set property" << endl;
				//	QStringList list = QStringList::fromStrList(keys);
				//	add(new KexiProperty(meta->name(), QStringList::fromStrList(meta->valueToKeys(obj->property(meta->name()).toInt())),
				//		list, list, meta->name()));
				}
				else
				{
					add(new KexiProperty(meta->name(), meta->valueToKey(obj->property(meta->name()).toInt()),
						QStringList::fromStrList(keys), QStringList::fromStrList(keys), meta->name()));
				}
			}
			else
				add(new KexiProperty(meta->name(), obj->property(meta->name()), meta->name()));
		}
	}

	m_manager->editor()->setBuffer(this);

	obj->installEventFilter(this);
}

bool
ObjectPropertyBuffer::showProperty(QObject *obj, const QString &property)
{
	QWidget *w = (QWidget*)obj;
	if(!m_manager->isTopLevel(w))
	{
		QStringList list;
		list << "caption" << "icon" << "sizeIncrement" << "iconText";
		if(!(list.grep(property)).isEmpty())
			return false;
	}
	return true;
}

bool
ObjectPropertyBuffer::eventFilter(QObject *o, QEvent *ev)
{
	if(o==m_object)
	{
	if((ev->type() == QEvent::Resize) || (ev->type() == QEvent::Move))
	{
		if((*this)["geometry"]->value() == o->property("geometry")) // to avoid infinite recursion
			return false;

		(*this)["geometry"]->setValue(((QWidget*)o)->geometry());

		QListViewItem *it = m_manager->editor()->findItem("geometry", 0, Qt::ExactMatch);
		KexiPropertyEditorItem *item = static_cast<KexiPropertyEditorItem*>(it);
		item->updateValue();
		//item->updateChildValue();
	}
	}
	return false;
}

void
ObjectPropertyBuffer::checkModifiedProp()
{
	if(m_object)
	{
		ObjectTreeItem *treeIt = m_manager->activeForm()->objectTree()->lookup(m_object->name());
		if(treeIt)
		{
			QListViewItem *it = m_manager->editor()->firstChild()->firstChild();
			while(it)
			{
				KexiPropertyEditorItem *item = static_cast<KexiPropertyEditorItem*>(it);
				if(item->property()->changed())
					treeIt->addModProperty(item->text(0));
				it = it->nextSibling();
			}
		}
	}
}

ObjectPropertyBuffer::~ObjectPropertyBuffer()
{
}

}

#include "objpropbuffer.moc"
