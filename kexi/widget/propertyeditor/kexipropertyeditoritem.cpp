/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
#include <qcursor.h>

#include <qpoint.h>
#include <kdebug.h>
#include <kiconloader.h>

#include <klocale.h>

#include "kexipropertyeditoritem.h"

// Helper class for QSizePolicy Editor

class KEXIPROPERTYEDITOR_EXPORT spHelper
{
	public:
	spHelper() {;}
	~spHelper() {;}
	
	static QStringList	list();
	static QString		valueToKey(int key);
	static QSizePolicy::SizeType		keyToValue(const QString &key);
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

QSizePolicy::SizeType
spHelper::keyToValue(const QString &key)
{
	if(key == "Fixed") return QSizePolicy::Fixed;
	if(key == "Minimum") return QSizePolicy::Minimum;
	if(key == "Maximum") return QSizePolicy::Maximum;
	if(key == "Preferred") return QSizePolicy::Preferred;
	if(key == "MinimumExpanding") return QSizePolicy::MinimumExpanding;
	if(key == "Expanding") return QSizePolicy::Expanding;
	if(key == "Ignored") return QSizePolicy::Ignored;
	
	return QSizePolicy::Expanding;
}

// Helper method for cursor
QString
valueToCursorName(int shape)
{
	switch(shape)
	{
		case Qt::ArrowCursor: return i18n("Arrow");
		case Qt::UpArrowCursor: return i18n("Up Arrow");
		case Qt::CrossCursor: return i18n("Cross");
		case Qt::WaitCursor: return i18n("Waiting");
		case Qt::IbeamCursor: return i18n("iBeam");
		case Qt::SizeVerCursor: return i18n("Size Vertical");
		case Qt::SizeHorCursor: return i18n("Size Horizontal");
		case Qt::SizeFDiagCursor: return i18n("Size Slash");
		case Qt::SizeBDiagCursor: return i18n("Size Backslash");
		case Qt::SizeAllCursor: return i18n("Size All");
		case Qt::BlankCursor: return i18n("Blank");
		case Qt::SplitVCursor: return i18n("Split Vertical");
		case Qt::SplitHCursor: return i18n("Split Horizontal");
		case Qt::PointingHandCursor: return i18n("Pointing Hand");
		case Qt::ForbiddenCursor: return i18n("Forbidden");
		case Qt::WhatsThisCursor: return i18n("Whats This");
		default: return QString();
	}
}

// KexiPropertyEditorItem

KexiPropertyEditorItem::KexiPropertyEditorItem(KexiPropertyEditorItem *parent, KexiProperty *property)
 : KListViewItem(parent, property->desc(), format(property->value()))
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
		case QVariant::SizePolicy:
		{
			m_childprop = new QPtrList<KexiProperty>();
			QSizePolicy p = m_value.toSizePolicy();

			KexiProperty *hSize = new KexiProperty(i18n("horSizeType"), 
			   spHelper::valueToKey(p.horData()), spHelper::list());
			m_childprop->append(hSize);
			KexiProperty *vSize = new KexiProperty(i18n("verSizeType"), 
			   spHelper::valueToKey(p.verData()), spHelper::list());
			m_childprop->append(vSize);
			KexiProperty *hStr = new KexiProperty(i18n("hStretch"), (int)p.horStretch() );
			m_childprop->append(hStr);
			KexiProperty *vStr = new KexiProperty(i18n("vStretch"), (int)p.verStretch() );
			m_childprop->append(vStr);
			
			m_children = new ChildDict();
			m_children->insert("hSize", new KexiPropertyEditorItem(this, hSize));
			m_children->insert("vSize", new KexiPropertyEditorItem(this, vSize));
			m_children->insert("hStretch", new KexiPropertyEditorItem(this, hStr));
			m_children->insert("vStretch", new KexiPropertyEditorItem(this, vStr));
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

KexiPropertyEditorItem::KexiPropertyEditorItem(KListView *parent, const QString &text)
 : KListViewItem(parent, text, "")
{
	m_value = "";
	m_property= new KexiProperty();
	m_oldvalue=m_value;
	m_childprop = 0;
	m_children = 0;
	setSelectable(false);
	setOpen(true);
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
	int margin = listView()->itemMargin();
	
	if(column == 1)
	{
		switch(m_value.type())
		{
			case QVariant::Pixmap:
			{
				p->fillRect(0,0,width,height(),QBrush(backgroundColor()));
				p->drawPixmap(margin, margin, m_property->value().toPixmap());
				break;
			}
			case QVariant::Color:
			{
				p->fillRect(0,0,width,height(), QBrush(backgroundColor()));
				QColor ncolor = m_value.toColor();
				p->setBrush(ncolor);
				p->drawRect(margin, margin, width - 2*margin, height() - 2*margin);
				QColorGroup nGroup(cg);
				break;
			}
			case QVariant::Bool:
			{
				p->fillRect(0,0,width,height(), QBrush(backgroundColor()));
				if(m_value.toBool())
				{
					p->drawPixmap(margin, height()/2 -8, SmallIcon("button_ok"));
					p->drawText(QRect(margin+20,0,width,height()-1), Qt::AlignVCenter, i18n("Yes"));
				}
				else
				{
					p->drawPixmap(margin, height()/2 -8, SmallIcon("button_cancel"));
					p->drawText(QRect(margin+20,0,width,height()-1), Qt::AlignVCenter, i18n("No"));
				}
				break;
			}
			
			default:
			{
				if(depth()==0)
					return;
				KListViewItem::paintCell(p, cg, column, width, align);
				break;
			}
		}
	}
	else
	{
		if(depth()==0)
			return;

		p->fillRect(0,0,width, height(), QBrush(backgroundColor()));

		if(isSelected())
		{
			p->fillRect(0,0,width, height(), QBrush(cg.highlight()));
			p->setPen(cg.highlightedText());
		}
		
		QFont f = listView()->font();
		p->save();
//		if(modified())
		if (m_property->changed())
			f.setBold(true);
		p->setFont(f);
		p->drawText(QRect(margin,0,width, height()-1), Qt::AlignVCenter, text(0));
		p->restore();
		
		p->setPen( QColor(200,200,200) ); //like in t.v.
		p->drawLine(width-1, 0, width-1, height()-1);
	}
	
	p->setPen( QColor(200,200,200) ); //like in t.v.
	p->drawLine(-50, height()-1, width, height()-1 );
}

void
KexiPropertyEditorItem::paintBranches(QPainter *p, const QColorGroup &cg, int w, int y, int h)
{
	p->eraseRect(0,0,w,h);
	KListViewItem *item = (KListViewItem*)firstChild();
	if(!item)
		return;
	
	p->save();
	p->translate(0,y);
	while(item)
	{
		p->fillRect(0,0,w, item->height(), QBrush(item->backgroundColor()));
		p->fillRect(-50,0,50, item->height(), QBrush(item->backgroundColor()));
		p->save();
		p->setPen( QColor(200,200,200) ); //like in t.v.
		p->drawLine(-50, item->height()-1, w, item->height()-1 );
		p->restore();
		
		if(item->isSelected())
		{
			p->fillRect(0,0,w, item->height(), QBrush(cg.highlight()));
			p->fillRect(-50,0,50, item->height(), QBrush(cg.highlight()));
		}

		if(item->firstChild())
		{
			p->drawRect(2, item->height()/2 -4, 9, 9);
			p->drawLine(4, item->height()/2, 8, item->height()/2);
			if(!item->isOpen())
				p->drawLine(6, item->height()/2 - 2, 6, item->height()/2 +2);
		}
		
		p->translate(0, item->totalHeight());
		item = (KListViewItem*)item->nextSibling();
	}
	p->restore();
}

void
KexiPropertyEditorItem::setup()
{
	KListViewItem::setup();
	if(depth()==0)
		setHeight(0);
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
			return QString(KGlobal::locale()->formatNumber(v.toDouble()));
		}
		case QVariant::StringList:
		{
			return v.toStringList().join("|");
		}
		case QVariant::SizePolicy:
		{
			QSizePolicy p = v.toSizePolicy(); 
			return QString(spHelper::valueToKey(p.horData()) + "/" + spHelper::valueToKey(p.verData()));
		}
		case QVariant::Cursor:
		{
			QCursor c = v.toCursor();
			return valueToCursorName(c.shape());
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
			QVariant v;
			v = (*m_children)["width"]->value();
			s.setWidth(v.toInt());
			(*m_children)["width"]->property()->setValue(v.toInt());
			
			v = (*m_children)["height"]->value();
			s.setHeight(v.toInt());
			(*m_children)["height"]->property()->setValue(v.toInt());
			
			setValue(s);
			return s;
		}
		case QVariant::Point:
		{
			QPoint p;
			QVariant v;
			v = (*m_children)["x"]->value();
			p.setX(v.toInt());
			(*m_children)["x"]->property()->setValue(v.toInt());
			
			v = (*m_children)["y"]->value();
			p.setY(v.toInt());
			(*m_children)["y"]->property()->setValue(v.toInt());
			
			setValue(p);
			return p;
		}
		case QVariant::Rect:
		{
			QRect r;
			QVariant v;
			v = (*m_children)["x"]->value();
			r.setX(v.toInt());
			(*m_children)["x"]->property()->setValue(v.toInt());
			
			v = (*m_children)["y"]->value();
			r.setY(v.toInt());
			(*m_children)["y"]->property()->setValue(v.toInt());
			
			v = (*m_children)["width"]->value();
			r.setWidth(v.toInt());
			(*m_children)["width"]->property()->setValue(v.toInt());
			
			v = (*m_children)["height"]->value();
			r.setHeight(v.toInt());
			(*m_children)["height"]->property()->setValue(v.toInt());
			
			setValue(r);
			return r;
		}
		case QVariant::SizePolicy:
		{
			QSizePolicy p;
			QVariant v;
			v = (*m_children)["hSize"]->value();
			p.setHorData(spHelper::keyToValue(v.toString()));
			(*m_children)["hSize"]->property()->setValue(v.toString());
			
			v = (*m_children)["vSize"]->value();
			p.setVerData(spHelper::keyToValue(v.toString()));
			(*m_children)["vSize"]->property()->setValue(v.toString());
			
			v = (*m_children)["hStretch"]->value();
			p.setHorStretch(v.toInt());
			(*m_children)["hStretch"]->property()->setValue(v.toInt());
			
			v = (*m_children)["vStretch"]->value();
			p.setVerStretch(v.toInt());
			(*m_children)["vStretch"]->property()->setValue(v.toInt());
			
			setValue(p);
			return p;
		}

		default:
		{
			return 0;
		}
	
	}
}

void
KexiPropertyEditorItem::updateChildValue()
{
	if(m_property->type() == QVariant::Rect)
	{
		QRect r = m_value.toRect();
		(*m_children)["x"]->property()->setValue(r.x());
		(*m_children)["x"]->setValue(r.x());
		(*m_children)["y"]->property()->setValue(r.y());
		(*m_children)["y"]->setValue(r.y());
		(*m_children)["width"]->property()->setValue(r.width());
		(*m_children)["width"]->setValue(r.width());
		(*m_children)["height"]->property()->setValue(r.height());
		(*m_children)["height"]->setValue(r.height());
	}
}

KexiPropertyEditorItem::~KexiPropertyEditorItem()
{
	switch(m_property->type())
	{
		case QVariant::Point:
		case QVariant::Rect:
		case QVariant::Size:
		case QVariant::SizePolicy:
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
