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

#include <qpopupmenu.h>
#include <qscrollview.h>

#include <kgenericfactory.h>
#include <klocale.h>
#include <kdebug.h>

#include <container.h>
#include <form.h>
#include <formIO.h>
#include <formmanager.h>
#include <objecttree.h>
#include <kexidb/utils.h>
#include <kexidb/connection.h>

#include "kexidbform.h"
#include "kexiformview.h"

#include "kexidbfactory.h"

KexiSubForm::KexiSubForm(KFormDesigner::FormManager *manager, QWidget *parent, const char *name)
: QScrollView(parent, name), m_manager(manager), m_form(0), m_widget(0)
{
	setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	viewport()->setPaletteBackgroundColor(colorGroup().mid());
}

void
KexiSubForm::setFormName(const QString &name)
{
	if(name.isEmpty())
		return;

	// we need a KexiFormView*
	QWidget *w = parentWidget();
	while(w && !w->isA("KexiFormView"))
		w = w->parentWidget();
	KexiFormView *view = (KexiFormView*)w;
	if(!view)
		return;

	// we check if there is a form with this name
	int id = KexiDB::idForObjectName(*(view->connection()), name, 3);
	if((id == 0) || (id == view->parentDialog()->id())) // == our form
		return; // because of recursion when loading

	// we create the container widget
	delete m_widget;
	m_widget = new QWidget(viewport(), "kexisubform_widget");
	m_widget->show();
	addChild(m_widget);
	m_form = new Form(m_manager, this->name());
	m_form->createToplevel(m_widget);

	// and load the sub form
	QString data;
	bool ok = view->connection()->loadDataBlock(id, data , QString::null);
	if(!ok)
		return;

	KFormDesigner::FormIO::loadFormFromString(m_form, m_widget, data);
	m_form->setDesignMode(false);

	m_formName = name;
}

//////////////////////////////////////////

KexiDBFactory::KexiDBFactory(QObject *parent, const char *name, const QStringList &)
 : KFormDesigner::WidgetFactory(parent, name)
{
	KFormDesigner::WidgetInfo *wView = new KFormDesigner::WidgetInfo(this);
	wView->setPixmap("form");
	wView->setClassName("KexiDBForm");
	wView->setName(i18n("Database Form"));
	wView->setNamePrefix(i18n("DBForm"));
	wView->setDescription(i18n("A db-aware form widget"));
	m_classes.append(wView);

	KFormDesigner::WidgetInfo *wSubForm = new KFormDesigner::WidgetInfo(this);
	wSubForm->setPixmap("form");
	wSubForm->setClassName("KexiSubForm");
	wSubForm->setName(i18n("Sub Form"));
	wSubForm->setNamePrefix(i18n("SubForm"));
	wSubForm->setDescription(i18n("A form widget included in another Form"));
	m_classes.append(wSubForm);
}

QString
KexiDBFactory::name()
{
	return("kexidbwidgets");
}

KFormDesigner::WidgetInfo::List
KexiDBFactory::classes()
{
	return m_classes;
}

QWidget*
KexiDBFactory::create(const QString &c, QWidget *p, const char *n, KFormDesigner::Container *container)
{
	kdDebug() << "KexiDBFactory::create() " << this << endl;

	if(c == "KexiSubForm")
	{
		KexiSubForm *subform = new KexiSubForm(container->form()->manager(), p, n);
		return subform;
	}

	return 0;
}

bool
KexiDBFactory::createMenuActions(const QString &, QWidget *w, QPopupMenu *,
		   KFormDesigner::Container *container, QValueVector<int> *)
{
	m_widget = w;
	m_container = container;

	return false;
}

void
KexiDBFactory::startEditing(const QString &, QWidget *, KFormDesigner::Container *container)
{
	m_container = container;
	return;
}

QStringList
KexiDBFactory::autoSaveProperties(const QString &classname)
{
	if(classname == "KexiSubForm")
		return QStringList("formName");
	return QStringList();
}

KexiDBFactory::~KexiDBFactory()
{
}

K_EXPORT_COMPONENT_FACTORY(kexidbwidgets, KGenericFactory<KexiDBFactory>)

#include "kexidbfactory.moc"
