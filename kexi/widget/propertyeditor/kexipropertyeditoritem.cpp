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
#include <qcolor.h>
#include <qfont.h>
#include <kdebug.h>

#include <klocale.h>

#include "kexipropertyeditoritem.h"

KexiPropertyEditorItem::KexiPropertyEditorItem(KListView *parent, KexiProperty *property)
 : KListViewItem(parent, property->name(), format(property->value()))
{
	m_value = property->value();
	m_property=property;
	
	childprop.setAutoDelete(true);

	switch(property->type())
	{
		case QVariant::Size:
		{
			QSize s = m_value.toSize();
			KexiProperty *width = new KexiProperty(i18n("width"), s.width() );
			childprop.append(width);
			KexiProperty *height = new KexiProperty(i18n("height"), s.height() );
			childprop.append(height);
			new KexiPropertyEditorItem(this, width);
			new KexiPropertyEditorItem(this, height);
			break;
		}
		case QVariant::Rect:
		{
			QRect r = m_value.toRect();
			KexiProperty *x = new KexiProperty(i18n("x"), r.x() );
			childprop.append(x);
			KexiProperty *y = new KexiProperty(i18n("y"), r.y() );
			childprop.append(y);
			KexiProperty *wid = new KexiProperty(i18n("width"), r.width() );
			childprop.append(wid);
			KexiProperty *hei = new KexiProperty(i18n("height"), r.height() );
			childprop.append(hei);
			
			new KexiPropertyEditorItem(this, x);
			new KexiPropertyEditorItem(this, y);
			new KexiPropertyEditorItem(this, wid);
			new KexiPropertyEditorItem(this, hei);
			
			kdDebug() << "creating sub editors" << endl;
			break;
		}
		
	/*	case QVariant::Color:
		{
			QColor c = m_value.toColor();
			new KexiPropertyEditorItem(this, i18n("red"), QVariant(c.red()) );
			new KexiPropertyEditorItem(this, i18n("blue"), QVariant(c.blue()) );
			new KexiPropertyEditorItem(this, i18n("green"), QVariant(c.green()) );
			break;
		}*/

		default:
		{
			return;
		}
	}
}

KexiPropertyEditorItem::KexiPropertyEditorItem(KexiPropertyEditorItem *parent, KexiProperty *property)
 : KListViewItem(parent, property->name(), format(property->value() ))
{
	m_value = property->value();
	m_property=property;
}


void
KexiPropertyEditorItem::setValue(QVariant value)
{
	setText(1, format(value));
	m_value = value;
}


void
KexiPropertyEditorItem::paintCell(QPainter *p, const QColorGroup & cg, int column, int width, int align)
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
KexiPropertyEditorItem::format(const QVariant &v)
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
				return i18n("True");
			}
			
			return i18n("False");
		}
		case QVariant::Font:
		{
			QFont f = v.toFont();
			return QString(f.family() + " " + QString::number(f.pointSize()));
		}
		default:
		{
			return v.toString();
		}
	}
}

KexiPropertyEditorItem::~KexiPropertyEditorItem()
{
}
