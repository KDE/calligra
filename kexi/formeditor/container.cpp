/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>

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

#include <qpainter.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qobjectlist.h>

#include <kdebug.h>
#include <klocale.h>
#include <kpopupmenu.h>

#include "container.h"
#include "widgetlibrary.h"
#include "objecttree.h"
#include "form.h"

namespace KFormDesigner {

Container::Container(Container *toplevel, QWidget *container, QObject *parent, const char *name, bool attach)
:QObject(parent, name)
{
	m_container = container;
	m_gridX = 10;
	m_gridY = 10;

	m_prepare = false;
	m_moving = 0;
	m_resizeHandles = 0;
	m_lib = 0;
	m_selected = 0;
	m_tree = 0;
	m_form = 0;
	m_toplevel = toplevel;
//	m_copiedw = 0;
//	m_cut = false;

	container->installEventFilter(this);

	if(toplevel)
	{
		connect(toplevel, SIGNAL(modeChanged(bool)), this, SLOT(setEditingMode(bool)));
		connect(toplevel, SIGNAL(selectionChanged(QWidget*)), this, SLOT(slotSelectionChanged(QWidget *)));
		connect(toplevel, SIGNAL(insertStop()), this, SLOT(stopInsert()));

		connect(toplevel, SIGNAL(prepareInsert( const QString &)), this,
		 SLOT(slotPrepareInsert( const QString &)));
		// connect(toplevel, SIGNAL(preparePaste(QWidget*, bool)), this, SLOT(slotPreparePaste(QWidget*, bool)));
		// connect(this, SIGNAL(preparePaste(QWidget*, bool)), toplevel, SLOT(slotPreparePaste(QWidget*, bool)));
		
		m_lib = form()->widgetLibrary();
		
		Container *pc = static_cast<Container *>(parent);
		
		if(attach)
		{
			ObjectTreeItem *it = new ObjectTreeItem(widget()->className(), widget()->name(), widget());
			setObjectTree(it);
			const QString pn = parent->name();
			ObjectTreeItem *parent = form()->objectTree()->lookup(pn);
			form()->objectTree()->addChild(parent, it);
		}
		else
		{
			if(!pc)
			{
				toplevel->registerChild(this);
			}
			else
			{
				ObjectTreeItem *it = new ObjectTreeItem(widget()->className(), widget()->name(), widget());
				setObjectTree(it);
				form()->objectTree()->addChild(pc->tree(), it);
			}

		}

		connect(container, SIGNAL(destroyed()), this, SLOT(widgetDeleted()));
	}
}

bool
Container::eventFilter(QObject *s, QEvent *e)
{
	switch(e->type())
	{
		case QEvent::MouseButtonPress:
		{
			kdDebug() << "QEvent::MouseButtonPress sender object = " << s->name() << endl;
			kdDebug() << "QEvent::MouseButtonPress this          = " << this->name() << endl;

			m_moving = static_cast<QWidget*>(s);
			if(m_moving->parent()->inherits("QWidgetStack"))
			{
				m_moving = m_moving->parentWidget()->parentWidget();
			}

			if(m_toplevel)
				m_toplevel->setSelectionChanged(m_moving);
			else
			{
				if(m_moving == m_container)
				setSelectionChanged(0);
				else
				setSelectionChanged(m_moving);
			}

			QMouseEvent *mev = static_cast<QMouseEvent*>(e);
			m_grab = QPoint(mev->x(), mev->y());

			if(s == m_container && m_prepare)
			{
				int tmpx,tmpy;
				tmpx = int((float)mev->x()/((float)m_gridX)+0.5);
				tmpx*=m_gridX;
				tmpy = int((float)mev->y()/((float)m_gridY)+0.5);
				tmpy*=m_gridX;

				m_insertBegin = QPoint(tmpx, tmpy);
				return true;
			}

			return true;
		}
		case QEvent::MouseButtonRelease:
		{
			QMouseEvent *mev = static_cast<QMouseEvent*>(e);
			if(m_prepare)
			{
				if(!m_lib)
					return true;

//				const char *name = form()->objectTree()->genName(m_insertClass).latin1();
				QString name = form()->objectTree()->genName(m_insertClass);
				QWidget *w = m_lib->createWidget(m_insertClass, m_container, name.latin1(), this);

				if(m_toplevel)
					m_toplevel->stopInsert();
				else
					stopInsert();

				if(!w)
					return true;

				addWidget(w, m_insertRect);
				
				if (!form()->objectTree()->lookup(name))
				form()->objectTree()->addChild(tree(), new ObjectTreeItem(m_insertClass, name, w));
				kdDebug() << "Container::eventFilter(): widget added " << this << endl;
			}
			else if(mev->button() == RightButton)
			{
				kdDebug() << "Container::eventFilter(): context menu" << endl;
				KPopupMenu *p = new KPopupMenu();
				p->insertItem(i18n("Remove Item"), this, SLOT(deleteItem()));
				p->insertItem(i18n("Copy"), this, SLOT(copyWidget()));
				p->insertItem(i18n("Cut"), this, SLOT(cutWidget()));
				p->insertItem(i18n("Paste"), this, SLOT(pasteWidget()));
				p->insertSeparator();
				
				QWidget *w = (QWidget*)s;
				QString n = w->className();
				p->insertTitle(n);
				
				if(!m_lib) { return true; }
				m_lib->createMenuActions(n,w,p,this);
				
				m_insertBegin = QCursor::pos();
				p->exec(QCursor::pos());
				
				delete p;
			}
			return true; // eat
		}
		case QEvent::MouseMove:
		{
			QMouseEvent *mev = static_cast<QMouseEvent*>(e);
			if(s == m_container && m_prepare)
			{
				int tmpx,tmpy;
				tmpx = int((float)mev->x()/((float)m_gridX)+0.5);
				tmpx*=m_gridX;
				tmpy = int((float)mev->y()/((float)m_gridY)+0.5);
				tmpy*=m_gridX;

				m_insertRect = QRect(m_insertBegin, QPoint(tmpx, tmpy));
				updateBackground();
				return true;
			}
			if(mev->state() & Qt::LeftButton)
			{
				if(!m_toplevel && m_moving == m_container)
					break;
				int tmpx = (((m_moving->x()+mev->x()-m_grab.x())+m_gridX/2)/m_gridX)*m_gridX;
				int tmpy = (((m_moving->y()+mev->y()-m_grab.y())+m_gridY/2)/m_gridY)*m_gridY;
				if((tmpx!=m_moving->x()) ||(tmpy!=m_moving->y()))
					m_moving->move(tmpx,tmpy);
			}
			return true; // eat
		}
		case QEvent::Resize:
			if(s==m_container)
				updateBackground();
			return false;

		case QEvent::MouseButtonDblClick:
		case QEvent::ContextMenu:
		case QEvent::Enter:
		case QEvent::Leave:
		case QEvent::FocusIn:
		case QEvent::FocusOut:
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
			return true; // eat them

		default:
			return false; // let the widget do the rest ...
	}
	return false;
}

void
Container::addWidget(QWidget *w, QRect r)
{
	w->setGeometry(r);
	w->show();
}

Form *
Container::form()
{
	if(m_toplevel)
		return m_toplevel->form();

	return m_form;
}

void
Container::setForm(Form *form)
{
	m_form = form;
	m_lib = form->widgetLibrary(); 
}

void
Container::updateBackground()
{
	QPainter *p = new QPainter();
	m_dotBg = QPixmap(m_container->size());
	p->begin(&m_dotBg, this);

	QColor c = m_container->paletteBackgroundColor();
	p->setPen(QPen(c));
	QBrush bg(c);
	p->setBrush(bg);
	p->drawRect(0, 0, m_container->width(), m_container->height());
	QPen dots(black, 1);
	p->setPen(dots);
	int cols = m_container->width() / m_gridX;
	int rows = m_container->height() / m_gridY;

	for(int rowcursor = 1; rowcursor < rows; ++rowcursor)
	{
		for(int colcursor = 1; colcursor < cols; ++colcursor)
		{
			p->drawPoint(colcursor *m_gridX, rowcursor *m_gridY);
		}
	}

	if(m_prepare)
	{
		p->setBrush(QBrush::NoBrush);
		p->setPen(QPen(black, 2));
		p->drawRect(m_insertRect);
	}

	p->end();
	m_container->setPaletteBackgroundPixmap(m_dotBg);
}

void
Container::setSelectionChanged(QWidget *w)
{
	slotSelectionChanged(w);
	emit selectionChanged(w);
}

void
Container::setEditingMode(bool)
{
}

void
Container::registerChild(Container *t)
{
	ObjectTreeItem *it = new ObjectTreeItem(t->widget()->className(), t->widget()->name(), t->widget());
	t->setObjectTree(it);

	form()->objectTree()->addChild(it);
}

void
Container::slotSelectionChanged(QWidget *w)
{
	kdDebug() << "slotSelectionChanged " << (void *)w << " == " << (void *)m_moving << endl;

	if(m_resizeHandles)
	{
		delete m_resizeHandles;
		m_resizeHandles = 0;
	}
	m_selected = 0;

	if(w && w == m_moving)
	{
		m_resizeHandles = new ResizeHandleSet(w);
		m_selected = w;
		form()->setSelectedWidget(w);
	}
	else
		form()->setSelectedWidget(0);
}

void
Container::emitPrepareInsert( const QString &classname)
{
	slotPrepareInsert( classname);
	emit prepareInsert(classname);
}


void
Container::slotPrepareInsert( const QString &classname)
{
//	emit insertRequested(f, classname);
	kdDebug() << "Container::insertWidget(this=" << m_container->name() << ","<< m_lib << "," << classname << ")" << endl;
	m_container->setCursor(QCursor(CrossCursor));
	m_prepare = true;

	m_insertClass = classname;

}

void
Container::stopInsert()
{
	if(!m_toplevel)
		emit insertStop();

	m_prepare = false;
	updateBackground();
	m_container->setCursor(QCursor(ArrowCursor));
}

Container*
Container::toplevel()
{
	if(m_toplevel)
		return m_toplevel;
	else
		return this;
}

ObjectTreeItem*
Container::tree()
{
	return m_tree;
}

void
Container::deleteItem()
{
	//take it out of da tree
	if(m_selected)
	{
		kdDebug() << "deleting item : " << m_selected->name() << endl;
		form()->objectTree()->removeChild(m_selected->name());
		if(m_selected==form()->copiedWidget())
			form()->preparePaste(0,false);
		delete m_selected;
		delete m_resizeHandles;
		m_selected = 0;
		m_resizeHandles = 0;
	}
}

void
Container::widgetDeleted()
{
	kdDebug() << "Deleting container : " << m_tree->name() << endl;
	delete this;
}

void
Container::copyWidget()
{
	if(m_selected)
	{
	kdDebug() << "Copied Widget : " << m_selected->name() << endl;
	form()->preparePaste(m_selected, false);
	}
	else
		form()->preparePaste(0,false);
}

void
Container::cutWidget()
{
	if(m_selected)
	{
	kdDebug() << "Cut Widget : " << m_selected->name() << endl;
	form()->preparePaste(m_selected, true);
	}
	else
		form()->preparePaste(0,true);
}

void
Container::pasteWidget()
{
	kdDebug() << "....pasting...." << endl;
	QWidget *copiedw = form()->copiedWidget();
	bool cut = form()->isCutting();

	if(copiedw)
	{
	kdDebug() << "Pasted Widget : " << copiedw->name() << endl;
	if(copiedw == m_container)  { return;}
	if(copiedw->child(m_container->name()))  {return ;}  //exit if the we are a child of the copied widget 
		
		if(!cut)
		{
		QString name = form()->objectTree()->genName(copiedw->className());
		const QString classname = copiedw->className();
		
		if(!m_lib)  { return;}
		QWidget *w = m_lib->createWidget(classname, m_container, name.latin1(), this); 
		if(!w)  { return;}
		
		QPoint p = m_container->mapFromGlobal(m_insertBegin);
		QRect rect(p.x(), p.y(), copiedw->width(), copiedw->height());
		addWidget(w, rect);
		
		if (!form()->objectTree()->lookup(name))
			form()->objectTree()->addChild(m_tree, new ObjectTreeItem(copiedw->className(), name, w));
		}
		
		else
		{
		QPoint p = m_container->mapFromGlobal(m_insertBegin);
		copiedw->reparent(m_container, p, true);
		ObjectTreeItem *it = form()->objectTree()->lookup(copiedw->name());
		it->parent()->remChild(it);
		tree()->addChild(it);
		it->setParent(tree());
		
		form()->preparePaste(copiedw, false);
		}
	}
}

Container::~Container()
{
}

}

#include "container.moc"
