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

#include <qwidget.h>

#include <kdebug.h>

#include "form.h"
#include "container.h"

namespace KFormDesigner {

Form::Form(QObject *parent, const char *name, WidgetLibrary *lib)
:QObject(parent, name)
{
	if(!lib)
		m_widgetLib = new WidgetLibrary(this);
	else
		m_widgetLib = lib;

	m_toplevel = 0;
	m_topTree = 0;
}

void
Form::createToplevel(QWidget *container)
{
	kdDebug() << "Form::createToplevel()" << endl;

	m_toplevel = new Container(0, container, this, "form1");

	connect(m_widgetLib, SIGNAL(prepareInsert(const QString&)), this,
	 SLOT(insertWidget(const QString&)));
//	connect(m_widgetLib, SIGNAL(insertWidget(WidgetLibrary *, const QString &)), m_toplevel,
//	 SLOT(slotInsertWidget(WidgetLibrary *, const QString &)));

	kdDebug() << "Form::createToplevel(): m_toplevel=" << m_toplevel << endl;

//	insertWidget("QButtonGroup");
}

void
Form::insertWidget(const QString &c)
{
	kdDebug() << "Form::insertWidget()" << endl;
	if(m_toplevel)
		m_toplevel->emitPrepareInsert(m_widgetLib, c);
}

QWidget*
Form::createInstance(QWidget *parent, bool e)
{
    return NULL;//TMP!
}

QWidget*
Form::createEmptyInstance(const QString &c, QWidget *parent)
{
	kdDebug() << "Form::createEmptyInstance()" << endl;

	QWidget *m = m_widgetLib->createWidget(c, parent, "form1", 0);
	if(!m)
		return 0;

	kdDebug() << "Form::createEmptyInstance() m=" << m << endl;
	createToplevel(m);

	m_topTree = new ObjectTree(c, m->name());
	m_toplevel->setObjectTree(m_topTree);
	return m;
}

Actions
Form::createActions(KActionCollection *parent)
{
	return m_widgetLib->createActions(parent, this, SLOT(insertWidget(const QString &)));
}


Form::~Form()
{
}

}

#include "form.moc"
