/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#include <qsize.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qstringlist.h>

#include <klocale.h>

#include "propertyeditoritem.h"

PropertyEditorItem::PropertyEditorItem(KListView *parent, const QString& name, QVariant::Type type, QVariant value, QObject *object)
 : KListViewItem(parent, name, format(value))
{
	m_name = name;
	m_type = type;
	m_value = value;
	m_object = object;
	m_list.clear();

	switch(m_type)
	{
		case QVariant::Size:
		{
			QSize s = value.toSize();
			new PropertyEditorItem(this, "width", QVariant(s.width()));
			new PropertyEditorItem(this, "height", QVariant(s.height()));

			break;
		}
		case QVariant::Rect:
		{
			break;
		}

		default:
		{
			return;
		}
	}
}

PropertyEditorItem::PropertyEditorItem(PropertyEditorItem *parent, const QString& name, QVariant value)
 : KListViewItem(parent, name, format(value))
{
	m_name = name;
	m_value = value;
	m_type = value.type();
}

PropertyEditorItem::PropertyEditorItem(KListView *parent, const QString& name, QVariant::Type type, QVariant value, QStringList l)
 : KListViewItem(parent, name, format(value))
{
	m_name = name;
	m_type = type;
	m_value = value;
	m_object = 0;
	m_list = l;
	setValue(value, false);
}

void
PropertyEditorItem::setValue(QVariant value, bool sync)
{
	if(m_type == QVariant::StringList)
	{
		setText(1, m_list[value.toInt()]);
	}
	else
	{
		setText(1, format(value));
	}
	m_value = value;

	if(sync && m_object)
	{
		m_object->setProperty(m_name.latin1(), value);
	}
}


void
PropertyEditorItem::paintCell(QPainter *p, const QColorGroup & cg, int column, int width, int align)
{
	if(column == 1)
	{
		switch(m_value.type())
		{
			case QVariant::Pixmap:
			{
				p->drawPixmap(1, 1, m_value.toPixmap());
				break;
			}
			case QVariant::Color:
			{
				QColor ncolor = m_value.toColor();
				p->setBrush(ncolor);
				p->drawRect(2, 2, width - 2, height() - 2);
				QColorGroup nGroup(cg);
//				nGroup.setColor(QColorGroup::Base, ncolor);
//				KListViewItem::paintCell(p, cg, column, width, align);
//				p->fillRect(2, 2, width - 2, height() - 2, QBrush(m_value.toColor()));
				return;
			}

			default:
			{
				KListViewItem::paintCell(p, cg, column, width, align);
				return;
			}
		}
	}
	KListViewItem::paintCell(p, cg, column, width, align);
}


QString
PropertyEditorItem::format(const QVariant &v)
{
	switch(v.type())
	{
		case QVariant::Size:
		{
			QSize s = v.toSize();
			return QString("[" + QString::number(s.width()) + "," + QString::number(s.height()) + "]");
		}
		case QVariant::Rect:
		{
			QRect r = v.toRect();
			QString x = QString::number(r.x());
			QString y = QString::number(r.y());
			QString w = QString::number(r.width());
			QString h = QString::number(r.height());

			return QString("[" + x + "," + y + "," + w + "," + h + "]");
		}
		case QVariant::Bool:
		{
			if(v.toBool())
			{
				return i18n("true");
			}
			
			return i18n("false");
		}
		default:
		{
			return v.toString();
		}
	}
}

PropertyEditorItem::~PropertyEditorItem()
{
}
