/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include <qwidget.h>
#include <qobjectlist.h>
#include <qtabwidget.h>
#include <qpainter.h>
#include <qlabel.h>

#include <kdebug.h>
#include <klocale.h>
#include <kcommand.h>
#include <kaction.h>
#include <kmessagebox.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>
#include <kpixmapio.h>
#include <kimageeffect.h>

#include "container.h"
#include "objecttree.h"
#include "objpropbuffer.h"
#include "formIO.h"
#include "formmanager.h"
#include "widgetlibrary.h"
#include "spacer.h"
#include "pixmapcollection.h"
#include "events.h"

#include "form.h"

using namespace KFormDesigner;

Form::Form(FormManager *manager, const char *name)
  : QObject(manager, name)
{
	m_toplevel = 0;
	m_topTree = 0;
	m_collection = 0;
	m_cursors = 0;
	m_manager = manager;
	m_resizeHandles.setAutoDelete(true);
	m_inter = true;
	m_design = true;
	m_autoTabstops = false;
	m_collection = new KActionCollection(this);
	m_history = new KCommandHistory(m_collection, true);
	m_tabstops.setAutoDelete(false);
	m_connBuffer = new ConnectionBuffer();
}


//////////////// Container -related functions ///////////////////////

void
Form::createToplevel(QWidget *container, FormWidget *formWidget, const QString &classname)
{
	kdDebug() << "Form::createToplevel() container= "<< (container ? container->name() : "<NULL>")
		<< " formWidget=" << formWidget << "className=" << name() << endl;

	m_formWidget = formWidget;
	m_toplevel = new Container(0, container, this, name());
	m_topTree = new ObjectTree(i18n("Form"), container->name(), container, m_toplevel);
	m_toplevel->setObjectTree(m_topTree);
	m_toplevel->setForm(this);
	m_pixcollection = new PixmapCollection(container->name(), this);

	m_topTree->setWidget(container);
	m_topTree->addModProperty("caption", name());
	//m_topTree->addModProperty("icon");

	connect(container, SIGNAL(destroyed()), this, SLOT(formDeleted()));

	kdDebug() << "Form::createToplevel(): m_toplevel=" << m_toplevel << endl;
}


Container*
Form::activeContainer()
{
	ObjectTreeItem *it = m_topTree->lookup(m_selected.last()->name());
	if (!it)
		return 0;
	if(it->container())
		return it->container();
	else
		return it->parent()->container();
}

Container*
Form::parentContainer(QWidget *w)
{
	ObjectTreeItem *it;
	if(!w)
		it = m_topTree->lookup(m_selected.last()->name());
	else
		it = m_topTree->lookup(w->name());

	if(it->parent()->container())
		return it->parent()->container();
	else
		return it->parent()->parent()->container();
}



void
Form::setDesignMode(bool design)
{
	m_design = design;
	if(!design)
	{
		TreeDict dict = *(m_topTree->dict());
		TreeDictIterator it(dict);
		for(; it.current(); ++it)
			m_manager->lib()->previewWidget(it.current()->widget()->className(), it.current()->widget(), m_toplevel);

		delete m_topTree;
		m_topTree = 0;
		delete m_toplevel;
		m_toplevel = 0;
	}
}


///////////////////////////// Selection stuff ///////////////////////

void
Form::setSelectedWidget(QWidget *w, bool add)
{
	if((m_selected.isEmpty()) || (w == m_topTree->widget()) || (m_selected.first() == m_topTree->widget()))
		add = false;

	if(!w)
	{
		setSelectedWidget(m_topTree->widget());
		return;
	}

	//raise selected widget and all possible parents
	QWidget *wtmp = w;
	//wtmp->raise();
	//while (wtmp && wtmp->parentWidget() && static_cast<QObject*>(wtmp)!=m_form && !wtmp->inherits("KFormDesigner::FormWidget")
	//	&& static_cast<QObject*>(wtmp->parentWidget())!=this && static_cast<QObject*>(wtmp->parentWidget())!=m_form)
	while(wtmp && wtmp->parentWidget() && (wtmp != m_topTree->widget()))
	{
		wtmp->raise();
		wtmp = wtmp->parentWidget();
	}

	if (wtmp)
		wtmp->setFocus();

	if(!add)
	{
		m_selected.clear();
		m_resizeHandles.clear();
	}
	m_selected.append(w);
	emit selectionChanged(w, add);

	// WidgetStack and TabWidget pages widgets shouldn't have resize handles, but their parent
	if(!m_manager->isTopLevel(w) && w->parentWidget() && w->parentWidget()->isA("QWidgetStack"))
	{
		w = w->parentWidget();
		if(w->parentWidget() && w->parentWidget()->inherits("QTabWidget"))
			w = w->parentWidget();
	}

	if(m_toplevel && w != m_toplevel->widget() && w)
		m_resizeHandles.insert(w->name(), new ResizeHandleSet(w));
}

void
Form::unSelectWidget(QWidget *w)
{
	m_selected.remove(w);
	m_resizeHandles.remove(w->name());
}

void
Form::resetSelection()
{
	m_selected.clear();
	m_resizeHandles.clear();
	emit selectionChanged(0, false);
}

///////////////////////////  Various slots and signals /////////////////////
void
Form::formDeleted()
{
	m_manager->deleteForm(this);
	//delete this;
	deleteLater();
}

void
Form::changeName(const QString &oldname, const QString &newname)
{
	if(oldname == newname)
		return;
	if(!m_topTree->rename(oldname, newname))
	{
		KMessageBox::sorry(m_toplevel->widget()->topLevelWidget(), 
		i18n("A widget with this name already exists. "
			"Please choose another name or rename existing widget."));
		kdDebug() << "Form::changeName() : ERROR : A widget named " << newname << " already exists" << endl;
		(*(m_manager->buffer()))["name"]->setValue(oldname);
	}
	else
		m_connBuffer->fixName(oldname, newname);

}

void
Form::emitChildAdded(ObjectTreeItem *item)
{
	emit childAdded(item);
}

void
Form::emitChildRemoved(ObjectTreeItem *item)
{
	m_tabstops.remove(item);
	if(m_connBuffer)
		m_connBuffer->removeAllConnectionsForWidget(item->name());
	emit childRemoved(item);
}

void
Form::addCommand(KCommand *command, bool execute)
{
	emit m_manager->dirty(this);
	m_history->addCommand(command, execute);
}

///////////////////////////  Tab stops ////////////////////////

void
Form::addWidgetToTabStops(ObjectTreeItem *c)
{
	QWidget *w = c->widget();
	if(!w)
		return;
	if(w->focusPolicy() == QWidget::NoFocus)
	{
		if(!w->children())
		return;

		// For composed widgets, we check if one of the child can have focus
		QObjectList list = *(w->children());
		for(QObject *obj = list.first(); obj; obj = list.next())
		{
			if(obj->isWidgetType() && (((QWidget*)obj)->focusPolicy() != QWidget::NoFocus))
			{
				if(m_tabstops.findRef(c) == -1)
				{
					m_tabstops.append(c);
					return;
				}
			}
		}
	}

	if(m_tabstops.findRef(c) == -1) // not yet in the list
		m_tabstops.append(c);
}

void
Form::autoAssignTabStops()
{
	VerWidgetList list;
	HorWidgetList hlist;

	for(ObjectTreeItem *tree = m_tabstops.first(); tree; tree = m_tabstops.next())
	{
		if(tree->widget())
			list.append(tree->widget());
	}

	list.sort();
	m_tabstops.clear();

	/// We automatically sort widget from the top-left to bottom-right corner
	//! \todo Handle RTL layout (ie form top-right to bottom-left)
	for(QWidget *w = list.first(); w; w = list.next())
	{
		hlist.append(w);

		QWidget *nextw = list.next();
		while(nextw && (nextw->y() < (w->y() + 20)))
		{
			hlist.append(nextw);
			nextw = list.next();
		}
		hlist.sort();

		for(QWidget *widg = hlist.first(); widg; widg = hlist.next())
		{
			ObjectTreeItem *tree = m_topTree->lookup(widg->name());
			if(tree)
				m_tabstops.append(tree);
		}

		nextw = list.prev();
		hlist.clear();
	}
}


///////////// Functions to paste widgets ///////////////////

void
Form::pasteWidget(QDomElement &widg, Container *cont, QPoint pos)
{
	Container *container = cont ? cont : activeContainer();
	if (!container)
		return;

	fixNames(widg); // to avoid nale clash
	if(pos.isNull())
		widg = fixPos(widg); // to avoid widget being at the same location
	else
		widg = fixPos(widg, pos);
	m_inter = false;
	FormIO::loadWidget(container, m_manager->lib(), widg);
	m_inter = true;
}

void
Form::fixNames(QDomElement el)
{
	QString wname;
	for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "name"))
		{
			wname = n.toElement().text();
			while(m_topTree->lookup(wname)) // name already exists
			{
				bool ok;
				int num = wname.right(1).toInt(&ok, 10);
				if(ok)
					wname = wname.left(wname.length()-1) + QString::number(num+1);
				else
					wname += "2";
			}
			if(wname != n.toElement().text()) // we change the name, so we recreate the element
			{
				n.removeChild(n.firstChild());
				QDomElement type = el.ownerDocument().createElement("string");
				QDomText valueE = el.ownerDocument().createTextNode(wname);
				type.appendChild(valueE);
				n.toElement().appendChild(type);
			}

		}
		if(n.toElement().tagName() == "widget") // fix child widgets names
		{
			fixNames(n.toElement());
		}
	}

}

QDomElement
Form::fixPos(QDomElement widg, QPoint newpos)
{
	QDomElement el = widg.cloneNode(true).toElement();
	QDomElement rect;
	// Find the widget geometry if there is one
	for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "geometry"))
		{
			rect = n.firstChild().toElement();
		}
	}

	QDomElement x = rect.namedItem("x").toElement();
	x.removeChild(x.firstChild());
	QDomText valueX = el.ownerDocument().createTextNode(QString::number(newpos.x()));
	x.appendChild(valueX);

	QDomElement y = rect.namedItem("y").toElement();
	y.removeChild(y.firstChild());
	QDomText valueY = el.ownerDocument().createTextNode(QString::number(newpos.y()));
	y.appendChild(valueY);

	return el;
}

QDomElement
Form::fixPos(QDomElement el)
{
	QDomElement rect;
	for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "geometry"))
		{
			rect = n.firstChild().toElement();
		}
	}

	QDomElement x = rect.namedItem("x").toElement();
	QDomElement y = rect.namedItem("y").toElement();
	QDomElement w = rect.namedItem("width").toElement();
	QDomElement h = rect.namedItem("height").toElement();

	int rx = x.text().toInt();
	int ry = y.text().toInt();
	int rw = w.text().toInt();
	int rh = h.text().toInt();
	QRect r(rx, ry, rw, rh);

	QWidget *widg = m_toplevel->widget()->childAt(r.x()+6, r.y()+6, false);
	if(!widg)
		return el;

	while(widg->geometry() == r) // there is already a widget there, with the same size
	{
		widg = m_toplevel->widget()->childAt(widg->x() + 16, widg->y() + 16, false);
		r.moveBy(10,10);
	}
	if(r == QRect(rx, ry, rw, rh))
		return el;
	else
		return fixPos(el, QPoint(r.x(), r.y()));

}

Form::~Form()
{
	delete m_history;
	delete m_topTree;
	delete m_connBuffer;
	m_connBuffer = 0;
	m_resizeHandles.setAutoDelete(false); // otherwise, it tries to delete widgets which doesn't exist anymore
}

//////  FormWidgetBase : helper widget to draw rects on top of widgets

//repaint all children widgets
static void repaintAll(QWidget *w)
{
	QObjectList *list = w->queryList("QWidget");
	QObjectListIt it(*list);
	for (QObject *obj; (obj=it.current()); ++it ) {
		static_cast<QWidget*>(obj)->repaint();
	}
	delete list;
}

void
FormWidgetBase::drawRect(const QRect& r, int type)
{
	QPainter p;
	p.begin(this, true);
	bool unclipped = testWFlags( WPaintUnclipped );
	setWFlags( WPaintUnclipped );

	if (prev_rect.isValid()) {
		//redraw prev. selection's rectangle
		p.drawPixmap( QPoint(prev_rect.x()-2, prev_rect.y()-2), buffer, QRect(prev_rect.x()-2, prev_rect.y()-2, prev_rect.width()+4, prev_rect.height()+4));
	}
	p.setBrush(QBrush::NoBrush);
	if(type == 1) // selection rect
		p.setPen(QPen(white, 1, Qt::DotLine));
	else if(type == 2) // insert rect
		p.setPen(QPen(white, 2));
	p.setRasterOp(XorROP);
	p.drawRect(r);
	prev_rect = r;

	if (!unclipped)
		clearWFlags( WPaintUnclipped );
	p.end();
}

void
FormWidgetBase::initRect()
{
	repaintAll(this);
	buffer.resize( width(), height() );
	buffer = QPixmap::grabWindow( winId() );
	prev_rect = QRect();
}

void
FormWidgetBase::clearRect()
{
	QPainter p;
	p.begin(this, true);
	bool unclipped = testWFlags( WPaintUnclipped );
	setWFlags( WPaintUnclipped );

	//redraw entire form surface
	p.drawPixmap( QPoint(0,0), buffer, QRect(0,0,buffer.width(), buffer.height()) );

	if (!unclipped)
		clearWFlags( WPaintUnclipped );
	p.end();

	repaintAll(this);
}

void
FormWidgetBase::highlightWidgets(QWidget *from, QWidget *to)//, const QPoint &point)
{
	QPoint fromPoint, toPoint;
	if(from->parentWidget())
		fromPoint = from->parentWidget()->mapTo(this, from->pos());
	if(to && to->parentWidget() && (to != this))
		toPoint = to->parentWidget()->mapTo(this, to->pos());

	QPainter p;
	p.begin(this, true);
	bool unclipped = testWFlags( WPaintUnclipped );
	setWFlags( WPaintUnclipped );

	if (prev_rect.isValid()) {
		//redraw prev. selection's rectangle
		p.drawPixmap( QPoint(prev_rect.x(), prev_rect.y()), buffer, QRect(prev_rect.x(), prev_rect.y(), prev_rect.width(), prev_rect.height()));
	}

	p.setPen( QPen(Qt::red, 2) );

	if(to)
	{
		QPixmap pix1 = QPixmap::grabWidget(from);
		QPixmap pix2 = QPixmap::grabWidget(to);

		/*if(from == this)
			p.drawLine( point, mapFrom(to->parentWidget(), to->geometry().center()) );
		else if(to == this)
			p.drawLine( mapFrom(from->parentWidget(), from->geometry().center()), point);
		else*/
		if((from != this) && (to != this))
			p.drawLine( from->parentWidget()->mapTo(this, from->geometry().center()), to->parentWidget()->mapTo(this, to->geometry().center()) );

		p.drawPixmap(fromPoint.x(), fromPoint.y(), pix1);
		p.drawPixmap(toPoint.x(), toPoint.y(), pix2);

		if(to == this)
			p.drawRoundRect(2, 2, width()-4, height()-4, 4, 4);
		else
			p.drawRoundRect(toPoint.x(), toPoint.y(), to->width(), to->height(), 5, 5);
	}

	if(from == this)
		p.drawRoundRect(2, 2, width()-4, height()-4, 4, 4);
	else
		p.drawRoundRect(fromPoint.x(),  fromPoint.y(), from->width(), from->height(), 5, 5);

	if((to == this) || (from == this))
		prev_rect = QRect(0, 0, buffer.width(), buffer.height());
	else if(to)
	{
		prev_rect.setX( (fromPoint.x() < toPoint.x()) ? (fromPoint.x() - 5) : (toPoint.x() - 5) );
		prev_rect.setY( (fromPoint.y() < toPoint.y()) ? (fromPoint.y() - 5) : (toPoint.y() - 5) );
		prev_rect.setRight( (fromPoint.x() < toPoint.x()) ? (toPoint.x() + to->width() + 10) : (fromPoint.x() + from->width() + 10) );
		prev_rect.setBottom( (fromPoint.y() < toPoint.y()) ? (toPoint.y() + to->height() + 10) : (fromPoint.y() + from->height() + 10) ) ;
	}
	else
		prev_rect = QRect(fromPoint.x()- 5,  fromPoint.y() -5, from->width() + 10, from->height() + 10);

	if (!unclipped)
		clearWFlags( WPaintUnclipped );
	p.end();
}

#include "form.moc"

