/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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

#include "kexiformpart.h"


#include <kdebug.h>
#include <kgenericfactory.h>

#include "kexiviewbase.h"
#include "keximainwindow.h"
#include "kexiproject.h"
#include "kexipartinfo.h"

#include <kexidb/connection.h>
#include <kexidb/tableschema.h>
#include <kexidb/cursor.h>

#include <kexidb/fieldlist.h>
#include <kexidb/field.h>

#include <kexidialogbase.h>
#include <kexidatasourcewizard.h>
//#include <kexidatasourcecombo.h>

#include <form.h>
#include <formIO.h>
#include <formmanager.h>
#include <objecttreeview.h>

#include "kexidbform.h"
//#include <kexipropertyeditor.h>

#define NO_DSWIZARD

KexiFormPart::KexiFormPart(QObject *parent, const char *name, const QStringList &l)
 : KexiPart::Part(parent, name, l)
{
	kdDebug() << "KexiFormPart::KexiFormPart()" << endl;
	m_names["instance"] = i18n("Form");
	m_supportedViewModes = Kexi::DataViewMode | Kexi::DesignViewMode;

	m_manager = new KFormDesigner::FormManager(0, 0, "manager");
}

KexiFormPart::~KexiFormPart()
{
}

#if 0
void KexiFormPart::initPartActions(KActionCollection *collection)
{
//this is automatic? -no
//create child guicilent: guiClient()->setXMLFile("kexidatatableui.rc");

	kdDebug()<<"FormPart INIT ACTIONS***********************************************************************"<<endl;
	//TODO

	//guiClient()->setXMLFile("kexiformui.rc");
//js	m_manager->createActions(collection, 0);
}

void KexiFormPart::initInstanceActions( int mode, KActionCollection *col )
{
	if (mode==Kexi::DesignViewMode) {
		m_manager->createActions(col, 0);
		new KAction(i18n("Edit Tab Order"), "tab_order", KShortcut(0), m_manager, SLOT(editTabOrder()), col, "taborder");
		new KAction(i18n("Adjust Size"), "viewmagfit", KShortcut(0), m_manager, SLOT(ajustWidgetSize()), col, "adjust");
	}
	//TODO
}
#endif

void KexiFormPart::initActions()
{
//	new KAction(i18n("Check query"), "test_it", 0, this, SLOT(slotCheckQuery()), 
//		m_instanceGuiClients[Kexi::DesignViewMode]->actionCollection(), "querypart_check_query");

	m_manager->createActions(actionCollectionForMode(Kexi::DesignViewMode), 0);
	createSharedAction(Kexi::DesignViewMode, i18n("Edit Tab Order"), "tab_order", 0, "formpart_taborder");
	createSharedAction(Kexi::DesignViewMode, i18n("Adjust Size"), "viewmagfit", 0, "formpart_adjust_size");
}

KexiViewBase* KexiFormPart::createView(QWidget *parent, KexiDialogBase* dialog,
	KexiPart::Item &item, int viewMode)
{
	kdDebug() << "KexiFormPart::createView()" << endl;
	KexiMainWindow *win = dialog->mainWin();
	if (!win || !win->project() || !win->project()->dbConnection())
		return 0;

	KexiFormPartItem it = m_forms[item.identifier()];
	KFormDesigner::Form *form = 0;
	KexiDB::FieldList *fields = 0;
	if(it.form())
	{
		// The form has been deleted, we need to create a new one
		form = new KFormDesigner::Form(m_manager);
		it.setForm(form);
		kdDebug() << "KexiFormPart::createView(): using existing form: [" << item.identifier() << "] " << it.form() << endl;
	}
	else
	{
		if(item.identifier() < 0)
		{
#ifndef NO_DSWIZARD
			kdDebug() << "KexiFormPart::createView(): booting wizard..." << endl;
			KexiDataSourceWizard *w = new KexiDataSourceWizard(win, win);
			if(!w->exec())
			{
				delete w;
				return 0;
			}
			else
			{
				fields = w->fields();

				delete w;
			}
			kdDebug() << "KexiFormPart::createView(): wizard done!" << endl;
#endif
		}
		else
		{
			fields = 0;
		}

		form = new KFormDesigner::Form(m_manager);
		KexiFormPartItem n(item, form);
		m_forms.insert(item.identifier(), n);
		it = n;
		kdDebug() << "KexiFormPart::createView(): creating form: [" << item.identifier() << "] " << it.form() << endl;
	}

	KexiDBForm *view = 0;
	KFormDesigner::ObjectTreeView *tree = new KFormDesigner::ObjectTreeView(0, "tree");
	view = new KexiDBForm(this, it, win, parent, item.name().latin1(), win->project()->dbConnection(), (viewMode == Kexi::DataViewMode));
	m_manager->setEditors(0, tree);
	view->initForm();

	if(fields)
	{
		QDomDocument dom;
		generateForm(fields, dom);
		KFormDesigner::FormIO::loadFormFromDom(form, view, dom);
	}

	return view;
}

bool
KexiFormPart::remove(KexiMainWindow *, KexiPart::Item &)
{
	return false;
}

void
KexiFormPart::generateForm(KexiDB::FieldList *list, QDomDocument &domDoc)
{
	//this form generates a .ui from FieldList list
	//basically that is a Label and a LineEdit for each field
	domDoc = QDomDocument("UI");
        QDomElement uiElement = domDoc.createElement("UI");
	domDoc.appendChild(uiElement);
	uiElement.setAttribute("version", "3.1");
	uiElement.setAttribute("stdsetdef", 1);

	QDomElement baseClass = domDoc.createElement("class");
	uiElement.appendChild(baseClass);
	QDomText baseClassV = domDoc.createTextNode("QWidget");
	baseClass.appendChild(baseClassV);
	QDomElement baseWidget = domDoc.createElement("widget");
	baseWidget.setAttribute("class", "QWidget");

	int y=0;

	for(unsigned int i=0; i < list->fieldCount(); i++)
	{
		QDomElement lclass = domDoc.createElement("widget");
		baseWidget.appendChild(lclass);
		lclass.setAttribute("class", "QLabel");
		QDomElement lNameProperty = domDoc.createElement("property");
		lNameProperty.setAttribute("name", "name");
		QDomElement lType = domDoc.createElement("cstring");
		QDomText lClassN = domDoc.createTextNode(QString("l%1").arg(list->field(i)->name()));
		lType.appendChild(lClassN);
		lNameProperty.appendChild(lType);
		lclass.appendChild(lNameProperty);

		QDomElement gNameProperty = domDoc.createElement("property");
		gNameProperty.setAttribute("name", "geometry");
		QDomElement lGType = domDoc.createElement("rect");

		QDomElement lx = domDoc.createElement("x");
		QDomText lxV = domDoc.createTextNode("10");
		lx.appendChild(lxV);
		QDomElement ly = domDoc.createElement("y");
		QDomText lyV = domDoc.createTextNode(QString::number(y + 10));
		ly.appendChild(lyV);
		QDomElement lWidth = domDoc.createElement("width");
		QDomText lWidthV = domDoc.createTextNode("100");
		lWidth.appendChild(lWidthV);
		QDomElement lHeight = domDoc.createElement("height");
		QDomText lHeightV = domDoc.createTextNode("20");
		lHeight.appendChild(lHeightV);

		lGType.appendChild(lx);
		lGType.appendChild(ly);
		lGType.appendChild(lWidth);
		lGType.appendChild(lHeight);

		gNameProperty.appendChild(lGType);
		lclass.appendChild(gNameProperty);

		QDomElement tNameProperty = domDoc.createElement("property");
		tNameProperty.setAttribute("name", "text");
		QDomElement lTType = domDoc.createElement("string");
		QDomText lTextV = domDoc.createTextNode(list->field(i)->name());
		lTType.appendChild(lTextV);
		tNameProperty.appendChild(lTType);
		lclass.appendChild(tNameProperty);


		///line edit!


		QDomElement vclass = domDoc.createElement("widget");
		baseWidget.appendChild(vclass);
		vclass.setAttribute("class", "KLineEdit");
		QDomElement vNameProperty = domDoc.createElement("property");
		vNameProperty.setAttribute("name", "name");
		QDomElement vType = domDoc.createElement("cstring");
		QDomText vClassN = domDoc.createTextNode(list->field(i)->name());
		vType.appendChild(vClassN);
		vNameProperty.appendChild(vType);
		vclass.appendChild(vNameProperty);

		QDomElement vgNameProperty = domDoc.createElement("property");
		vgNameProperty.setAttribute("name", "geometry");
		QDomElement vGType = domDoc.createElement("rect");

		QDomElement vx = domDoc.createElement("x");
		QDomText vxV = domDoc.createTextNode("110");
		vx.appendChild(vxV);
		QDomElement vy = domDoc.createElement("y");
		QDomText vyV = domDoc.createTextNode(QString::number(y + 10));
		vy.appendChild(vyV);
		QDomElement vWidth = domDoc.createElement("width");
		QDomText vWidthV = domDoc.createTextNode("200");
		vWidth.appendChild(vWidthV);
		QDomElement vHeight = domDoc.createElement("height");
		QDomText vHeightV = domDoc.createTextNode("20");
		vHeight.appendChild(vHeightV);

		vGType.appendChild(vx);
		vGType.appendChild(vy);
		vGType.appendChild(vWidth);
		vGType.appendChild(vHeight);

		vgNameProperty.appendChild(vGType);
		vclass.appendChild(vgNameProperty);

		y += 20;
	}

	QDomElement lNameProperty = domDoc.createElement("property");
	lNameProperty.setAttribute("name", "name");
	QDomElement lType = domDoc.createElement("cstring");
	QDomText lClassN = domDoc.createTextNode("DBForm");
	lType.appendChild(lClassN);
	lNameProperty.appendChild(lType);
	baseWidget.appendChild(lNameProperty);



	QDomElement wNameProperty = domDoc.createElement("property");
	wNameProperty.setAttribute("name", "geometry");
	QDomElement wGType = domDoc.createElement("rect");

	QDomElement wx = domDoc.createElement("x");
	QDomText wxV = domDoc.createTextNode("0");
	wx.appendChild(wxV);
	QDomElement wy = domDoc.createElement("y");
	QDomText wyV = domDoc.createTextNode("0");
	wy.appendChild(wyV);
	QDomElement wWidth = domDoc.createElement("width");
	QDomText wWidthV = domDoc.createTextNode("340");
	wWidth.appendChild(wWidthV);
	QDomElement wHeight = domDoc.createElement("height");
	QDomText wHeightV = domDoc.createTextNode(QString::number(y + 30));
	wHeight.appendChild(wHeightV);

	wGType.appendChild(wx);
	wGType.appendChild(wy);
	wGType.appendChild(wWidth);
	wGType.appendChild(wHeight);

	wNameProperty.appendChild(wGType);
	baseWidget.appendChild(wNameProperty);

	uiElement.appendChild(baseWidget);
}

KexiFormPartItem::KexiFormPartItem()
{
	m_form = 0;
}

KexiFormPartItem::KexiFormPartItem(KexiPart::Item &it, KFormDesigner::Form *f)
{
	m_item = it;
	m_form = f;
}

KexiFormPartItem::~KexiFormPartItem()
{
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_form, KGenericFactory<KexiFormPart> )

#include "kexiformpart.moc"

