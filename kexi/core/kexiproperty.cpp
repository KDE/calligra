/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004   Jaroslaw Staniek <js@iidea.pl>

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

#include "kexiproperty.h"

#include <qstringlist.h>
#include <kdebug.h>
#include <qsizepolicy.h>
#include <qsize.h>
#include <qrect.h>

#include <klocale.h>


class spHelper
{
	public:
	spHelper() {;}
	~spHelper() {;}

	static QStringList list();
	static QString valueToKey(int key);
	static QSizePolicy::SizeType keyToValue(const QString &key);
};

QString
spHelper::valueToKey(int key)
{
	switch(key)
	{
		case QSizePolicy::Fixed: return QString("Fixed");
		case QSizePolicy::Minimum: return QString("Minimum");
		case QSizePolicy::Maximum: return QString("Maximum");
		case QSizePolicy::Preferred: return QString("Preferred");
		case QSizePolicy::MinimumExpanding: return QString("MinimumExpanding");
		case QSizePolicy::Expanding: return QString("Expanding");
		case QSizePolicy::Ignored: return QString("Ignored");
		default: return QString();
	}
}

QStringList
spHelper::list()
{
	QStringList list;
	list << "Fixed" << "Maximum" << "Minimum" << "Preferred" << "Expanding" 
		<< "MinimumExpanding" << "Ignored";
	return list;
}

//===================================================

class KexiProperty::KexiPropertyListData
{
	public:
		KexiPropertyListData()
		{
		}
		QStringList keys, names;
};

//===================================================

KexiProperty::KexiProperty(const QString &name, QVariant value, const QString &desc)
{
	m_name = name;
//	m_value = value;
	m_desc = desc;
	init(value);
}

KexiProperty::KexiProperty(const QString &name, const QString &value, 
 const QStringList &key_list, const QStringList &name_list, 
 const QString &desc)
{
	m_name = name;
//	m_value = value;
	m_desc = desc;
	init(value);
	m_list = new KexiPropertyListData();
	m_list->keys = key_list;
	m_list->names = name_list;
	//kdDebug() << "creating stringlist property" << endl;
}

KexiProperty::KexiProperty()
{
//	m_name="";
//	m_value=QVariant();
	init(QVariant());
}

KexiProperty::~KexiProperty()
{
	delete m_list;
	delete m_children_list;
	delete m_children_dict;
}

void KexiProperty::init(QVariant value)
{
	m_changed = false;
	m_visible = true;
	m_list = 0;
	m_autosync = -1;
	m_children_dict = 0;
	m_children_list = 0;
	m_parent = 0;
	if (!m_desc.isEmpty())
		kdDebug() << "KexiProperty::KexiProperty(): labeled property (" << m_desc << ")" << endl;


	//automatically add children if necessary 
	switch(value.type())
	{
		case QVariant::Size:
		{
			QSize s = value.toSize();
			addChild( new KexiProperty("width", QVariant(s.width()), i18n("width")) );
			addChild( new KexiProperty("height", QVariant(s.height()), i18n("height")) );
			break;
		}
		case QVariant::Point:
		{
			QPoint p = value.toPoint();
			addChild( new KexiProperty("x", p.x(), i18n("x")) );
			addChild( new KexiProperty("y", p.y(), i18n("y")) );
			break;
		}
		case QVariant::Rect:
		{
			QRect r = value.toRect();
			addChild( new KexiProperty("x", r.x(), i18n("x")) );
			addChild( new KexiProperty("y", r.y(), i18n("y")) );
			addChild( new KexiProperty("width", r.width(), i18n("width")) );
			addChild( new KexiProperty("height", r.height(), i18n("height")) );
			break;
		}
		case QVariant::SizePolicy:
		{
			QSizePolicy p = value.toSizePolicy();

			addChild( new KexiProperty("horSizeType", spHelper::valueToKey(p.horData()), 
				spHelper::list(), i18n("horSizeType")) );

			addChild( new KexiProperty("verSizeType", 
				spHelper::valueToKey(p.verData()), spHelper::list(), i18n("verSizeType")) );

			addChild( new KexiProperty("hStretch", (int)p.horStretch(), i18n("hStretch") ) );
			addChild( new KexiProperty("vStretch", (int)p.verStretch(), i18n("vStretch") ) );
			break;
		}
		default:
		{
			break;
		}
	}

	setValue( value, false );
}

KexiProperty::KexiProperty(const KexiProperty &property)
{
	*this = property;
/*	m_name = property.m_name;
	m_value = property.m_value;
	m_changed = property.m_changed;
	m_visible = property.m_visible;
	m_desc = property.m_desc;
	m_autosync = property.m_autosync;
	if (property.m_children)
		m_children = new KexiProperty::Map(*property.m_children);
	else
		m_children = 0;

	m_parent = property.m_parent;

	if(property.m_list)
		m_list = new QStringList(*(property.m_list));
	else
		m_list=0;*/
}

const KexiProperty&
KexiProperty::operator=(const KexiProperty &property)
{
	if(&property==this)
		return *this;

	if(m_list) {
		delete m_list;
		m_list=0;
	}
	if(m_children_list) {
		m_children_list->clear();
		m_children_dict->clear();
	}

	m_name = property.m_name;
	m_value = property.m_value;
	m_changed = property.m_changed;
	m_visible = property.m_visible;
	m_desc = property.m_desc;
	m_autosync = property.m_autosync;

	if (property.m_children_list) {
		//copy children
//		m_children_dict = new KexiProperty::Dict(property.m_children_dict->size());
//		m_children_dict->setAutoDelete(true);
//		m_children_list = new KexiProperty::List();
		KexiProperty::ListIterator it(*property.m_children_list);
		for (;it.current();++it) {
			addChild( new KexiProperty(*it.current()) );
		}
	} else {
		m_children_dict = 0;
		m_children_list = 0;
	}

	m_parent = property.m_parent;

	if(property.m_list) {
		m_list = new KexiProperty::KexiPropertyListData();
		*m_list = *(property.m_list);
	} else {
		m_list=0;
	}
	return *this;
}

QVariant::Type  KexiProperty::type() const
{
	if(m_list)
		return QVariant::StringList;
	else
		return m_value.type();
}

void KexiProperty::setValue(const QVariant &v, bool saveOldValue)
{
	if (saveOldValue) {
		if (m_value != v) {
			if (!m_changed) {
				m_oldValue = m_value; //store old
			}
			m_changed = true;
			if (m_parent)
				m_parent->setChanged( true ); //inform the parent
			m_value = v;
		}
	}
	else {
		m_oldValue = QVariant(); //clear old
		m_changed = false;
		m_value = v;
	}


	//automatically update children's value if necessary
	switch(m_value.type())
	{
		case QVariant::Size:
		{
			QSize s = m_value.toSize();
			setValue("width",s.width(),saveOldValue);
			setValue("height",s.height(),saveOldValue);
			break;
		}
		case QVariant::Point:
		{
			QPoint p = m_value.toPoint();
			setValue("x",p.x(),saveOldValue);
			setValue("y",p.y(),saveOldValue);
			break;
		}
		case QVariant::Rect:
		{
			QRect r = m_value.toRect();
			setValue("x",r.x(),saveOldValue);
			setValue("y",r.y(),saveOldValue);
			setValue("width",r.width(),saveOldValue);
			setValue("height",r.height(),saveOldValue);
			break;
		}
		case QVariant::SizePolicy:
		{
			QSizePolicy p = m_value.toSizePolicy();
			setValue("horSizeType",spHelper::valueToKey(p.horData()),saveOldValue);
			setValue("verSizeType",spHelper::valueToKey(p.verData()),saveOldValue);
			setValue("hStretch",(int)p.horStretch(),saveOldValue);
			setValue("vStretch",(int)p.verStretch(),saveOldValue);
			break;
		}
		default:
			break;
	}
}

QVariant KexiProperty::value() const
{
	return m_value;
}

QString KexiProperty::valueText() const
{
	if (!m_list)
		return m_value.toString();
	//special case: return text
	int idx = m_list->keys.findIndex( m_value.toString() );
	if (idx<0) {
		kdDebug() << "KexiProperty::value(): NO SUCH KEY '" << m_value.toString() << "'" << endl;
		return m_value.toString();
	}
	return m_list->names[ idx ];
}

void KexiProperty::setValue(const QString& childName, const QVariant &v, bool saveOldValue)
{
	KexiProperty * prop = child(childName);
	if (!prop) {
		kdDebug()<< "KexiProperty::setValue() NO SUCH CHILD: " << childName << endl;
		return;
	}
	prop->setValue(v, saveOldValue);
}

bool KexiProperty::changed() const
{
	return m_changed;
}

void KexiProperty::setChanged(bool set)
{
	if (m_changed==set)
		return;
	m_changed=set;
	if (!m_changed)
		m_oldValue = QVariant();
	else {
		m_oldValue = m_value; //store
		if (m_parent)
			m_parent->setChanged( true );
	}
}

void KexiProperty::addChild(KexiProperty *prop)
{
	if (!m_children_dict) {
		m_children_dict = new KexiProperty::Dict(53);
		m_children_dict->setAutoDelete(true);
		m_children_list = new KexiProperty::List();
	}
	m_children_dict->insert(prop->name(), prop);
	m_children_list->append( prop );
}

bool KexiProperty::isVisible() const
{
	if (m_parent) {
		if (!m_parent->isVisible())
			return false;
	}
	return m_visible;
}

KexiProperty* KexiProperty::child(const QString& name)
{
	if (!m_children_dict)
		return 0;
	return m_children_dict->find(name);
}

void KexiProperty::debug()
{
	QString dbg = "KexiProperty( name='" + m_name + "' desc='" + m_desc 
		+ "' val=" + (m_value.isValid() ? m_value.toString() : "<INVALID>");
	if (!m_oldValue.isValid())
		dbg += (", oldVal='" + m_oldValue.toString() + "'");
	dbg += (QString(m_changed ? " " : " un") + "changed");
	dbg += (m_visible ? " visible" : " hidden");
	dbg+=" )";
	kdDebug() << dbg << endl;
}

QStringList* KexiProperty::keys() const
{
	return m_list ? &m_list->keys : 0;
}

QStringList* KexiProperty::names() const
{
	return m_list ? &m_list->names : 0;
}
