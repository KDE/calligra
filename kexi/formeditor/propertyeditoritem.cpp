/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#include <qsize.h>
#include <qpainter.h>
#include <qpixmap.h>

#include "propertyeditoritem.h"

PropertyEditorItem::PropertyEditorItem(KListView *parent, QString name, QVariant::Type type, QVariant value, QObject *object)
 : KListViewItem(parent, name, format(value))
{
	m_name = name;
	m_type = type;
	m_value = value;
	m_object = object;

	switch(value.type())
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

PropertyEditorItem::PropertyEditorItem(PropertyEditorItem *parent, QString name, QVariant value)
 : KListViewItem(parent, name, format(value))
{
	m_name = name;
	m_value = value;
}

void
PropertyEditorItem::setValue(QVariant value, bool sync)
{
	setText(1, value.toString());

	if(sync)
		m_object->setProperty(m_name.latin1(), value);
}

/*
void
PropertyEditorItem::paintCell(QPainter *p, const QColorGroup & cg, int column, int width, int align)
{
	KListViewItem::paintCell(p, cg, column, width, align);
	if(column == 1)
	{
		switch(m_value.type())
		{
			case QVariant::Pixmap:
			{
				p->drawPixmap(1, 1, m_value.toPixmap());
			}

			default:
			{
				return;
			}
		}
	}

}
*/

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

		default:
		{
			return v.toString();
		}
	}
}

PropertyEditorItem::~PropertyEditorItem()
{
}
