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
#include <qpoint.h>
#include <kdebug.h>
#include <kiconloader.h>

#include <kglobal.h>
#include <klocale.h>

#include "kexipropertyeditoritem.h"

KexiPropertyEditorItem::KexiPropertyEditorItem(KListView *parent, KexiProperty *property)
 : KListViewItem(parent, property->name(), format(property->value()))
{
	m_value = property->value();
	m_oldvalue = m_value;
	m_property=property;
	m_childprop = 0;
	m_children = 0;
	
	switch(property->type())
	{
		case QVariant::Size:
		{
			m_childprop = new QPtrList<KexiProperty>();
			QSize s = m_value.toSize();
			KexiProperty *width = new KexiProperty(i18n("width"), s.width() );
			m_childprop->append(width);
			KexiProperty *height = new KexiProperty(i18n("height"), s.height() );
			m_childprop->append(height);
			
			m_children = new ChildDict();
			m_children->insert("width", new KexiPropertyEditorItem(this, width));
			m_children->insert("height", new KexiPropertyEditorItem(this, height));
			break;
		}
		case QVariant::Point:
		{
			m_childprop = new QPtrList<KexiProperty>();
			QPoint p = m_value.toPoint();
			KexiProperty *x = new KexiProperty(i18n("x"), p.x() );
			m_childprop->append(x);
			KexiProperty *y = new KexiProperty(i18n("y"), p.y() );
			m_childprop->append(y);
			
			m_children = new ChildDict();
			m_children->insert("x", new KexiPropertyEditorItem(this, x));
			m_children->insert("y", new KexiPropertyEditorItem(this, y));
			break;
		}
		case QVariant::Rect:
		{
			m_childprop = new QPtrList<KexiProperty>();
			QRect r = m_value.toRect();
			KexiProperty *x = new KexiProperty(i18n("x"), r.x() );
			m_childprop->append(x);
			KexiProperty *y = new KexiProperty(i18n("y"), r.y() );
			m_childprop->append(y);
			KexiProperty *wid = new KexiProperty(i18n("width"), r.width() );
			m_childprop->append(wid);
			KexiProperty *hei = new KexiProperty(i18n("height"), r.height() );
			m_childprop->append(hei);
			
			m_children = new ChildDict();
			m_children->insert("x", new KexiPropertyEditorItem(this, x));
			m_children->insert("y", new KexiPropertyEditorItem(this, y));
			m_children->insert("width", new KexiPropertyEditorItem(this, wid));
			m_children->insert("height", new KexiPropertyEditorItem(this, hei));
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
	
	m_childprop->setAutoDelete(true);
}

KexiPropertyEditorItem::KexiPropertyEditorItem(KexiPropertyEditorItem *parent, KexiProperty *property)
 : KListViewItem(parent, property->name(), format(property->value() ))
{
	m_value = property->value();
	m_property=property;
	m_childprop = 0;
	m_children = 0;
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
				p->eraseRect(0,0,width,height());
				p->drawPixmap(1, 1, m_property->value().toPixmap());
				break;
			}
			case QVariant::Color:
			{
				p->eraseRect(0,0,width,height());
				QColor ncolor = m_value.toColor();
				p->setBrush(ncolor);
				p->drawRect(2, 2, width - 2, height() - 2);
				QColorGroup nGroup(cg);
				break;
			}
			case QVariant::Bool:
			{
				p->eraseRect(0,0,width,height());
				if(m_value.toBool())
				{
					p->drawPixmap(1, 1, SmallIcon("button_ok"));
					p->drawText(20, height() -3, i18n("True"));
				}
				else
				{
					p->drawPixmap(1, 1, SmallIcon("button_cancel"));
					p->drawText(20, height()-3, i18n("False"));
				}
				break;
			}
			
			default:
			{
				KListViewItem::paintCell(p, cg, column, width, align);
				break;
			}
		}
	}
	else
	{
	KListViewItem::paintCell(p, cg, column, width, align);
	}
	p->setBrush(Qt::lightGray);
	//p->drawLine(0, height(), width, height() );
	//p->drawLine(width, 0, width, height());
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
		case QVariant::Double:
		{
			QString s = KGlobal::locale()->formatNumber(v.toDouble());
			return s;
		}
		default:
		{
			return v.toString();
		}
	}
}

QVariant
KexiPropertyEditorItem::getComposedValue()
{
	switch(m_property->type())
	{
		case QVariant::Size:
		{
			QSize s;
			s.setWidth((*m_children)["width"]->value().toInt());
			s.setHeight((*m_children)["height"]->value().toInt());
			setValue(s);
			return s;
		}
		case QVariant::Point:
		{
			QPoint p;
			p.setX((*m_children)["x"]->value().toInt());
			p.setY((*m_children)["y"]->value().toInt());
			setValue(p);
			return p;
		}
		case QVariant::Rect:
		{
			QRect r;
			r.setX((*m_children)["x"]->value().toInt());
			r.setY((*m_children)["y"]->value().toInt());
			r.setWidth((*m_children)["width"]->value().toInt());
			r.setHeight((*m_children)["height"]->value().toInt());
			setValue(r);
			return r;
		}
		
		default:
		{
			return 0;
		}
	
	}
}

KexiPropertyEditorItem::~KexiPropertyEditorItem()
{
	switch(m_property->type())
	{
		case QVariant::Point:
		case QVariant::Rect:
		case QVariant::Size:
		{
			delete m_childprop;
			delete m_children;
		}
		default:
		{
			return;
		}
	}
}
