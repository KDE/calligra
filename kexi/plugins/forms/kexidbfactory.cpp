/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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
#include <qcursor.h>

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
#include <kexipart.h>

#include "kexidbform.h"
#include "kexiformview.h"

#include "kexidbfactory.h"
#include "formmanager.h"

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
	KexiFormView *view = static_cast<KexiFormView*>(w);

	if (!view || !view->parentDialog() || !view->parentDialog()->mainWin() 
		|| !view->parentDialog()->mainWin()->project()->dbConnection())
		return;

	KexiDB::Connection *conn = view->parentDialog()->mainWin()->project()->dbConnection();

	// we check if there is a form with this name
	int id = KexiDB::idForObjectName(*conn, name, KexiPart::FormObjectType);
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
	bool ok = conn->loadDataBlock(id, data , QString::null);
	if(!ok)
		return;

	KFormDesigner::FormIO::loadFormFromString(m_form, m_widget, data);
	m_form->setDesignMode(false);

	m_formName = name;
}

//////////////////////////////////////////

KexiDBLineEdit::KexiDBLineEdit(QWidget *parent, const char *name)
 : KLineEdit(parent, name)
 , KexiDataItemInterface()
{
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(slotTextChanged(const QString&)));
}

KexiDBLineEdit::~KexiDBLineEdit()
{
}

void KexiDBLineEdit::setValueInternal(const QVariant& value)
{
	setText( value.toString() );
}

QVariant KexiDBLineEdit::value()
{
	return text();
}

void KexiDBLineEdit::slotTextChanged(const QString&)
{
	valueChanged();
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

/* @todo allow to inherit from stdwidgets' KLineEdit */
	KFormDesigner::WidgetInfo *wLineEdit = new KFormDesigner::WidgetInfo(this);
	wLineEdit->setPixmap("lineedit");
	wLineEdit->setClassName("KexiDBLineEdit");
	wLineEdit->addAlternateClassName("QLineEdit", true/*override*/);
	wLineEdit->addAlternateClassName("KLineEdit", true/*override*/);
	wLineEdit->setIncludeFileName("klineedit.h");
	wLineEdit->setName(i18n("Line Edit"));
	wLineEdit->setNamePrefix(i18n("Widget name (see above)", "LineEdit"));
	wLineEdit->setDescription(i18n("A widget to input text"));
	m_classes.append(wLineEdit);

	m_propDesc["dataSource"] = i18n("Data source");
	m_propDesc["formName"] = i18n("Form name");
}

KexiDBFactory::~KexiDBFactory()
{
}

/*QString
KexiDBFactory::name()
{
	return("kexidbwidgets");
}*/

QWidget*
KexiDBFactory::create(const QCString &c, QWidget *p, const char *n, KFormDesigner::Container *container)
{
	kexipluginsdbg << "KexiDBFactory::create() " << this << endl;

	QWidget *w=0;

	if(c == "KexiSubForm")
	{
		w = new KexiSubForm(container->form()->manager(), p, n);
	}
	else if(c == "KexiDBLineEdit")
	{
		w = new KexiDBLineEdit(p, n);
		w->setCursor(QCursor(Qt::ArrowCursor));
	}

	return w;
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
KexiDBFactory::startEditing(const QString &classname, QWidget *w, KFormDesigner::Container *container)
{
	m_container = container;
	if(classname == "KexiDBLineEdit")
	{
//! @todo this code should not be copied here but
//! just inherited StdWidgetFactory::clearWidgetContent() should be called
		KLineEdit *lineedit = static_cast<KLineEdit*>(w);
		createEditor(lineedit->text(), lineedit, container, lineedit->geometry(), lineedit->alignment(), true);
	}
}

void
KexiDBFactory::clearWidgetContent(const QString &classname, QWidget *w)
{
//! @todo this code should not be copied here but
//! just inherited StdWidgetFactory::clearWidgetContent() should be called
	if(classname == "KexiDBLineEdit")
		static_cast<KLineEdit*>(w)->clear();
}

QStringList
KexiDBFactory::autoSaveProperties(const QString &classname)
{
	if(classname == "KexiSubForm")
		return QStringList("formName");
//	if(classname == "KexiDBLineEdit")
//		return QStringList("dataSource");
	return QStringList();
}

K_EXPORT_COMPONENT_FACTORY(kexidbwidgets, KGenericFactory<KexiDBFactory>("kexidbwidgets"))

#include "kexidbfactory.moc"
