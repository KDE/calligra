/* This file is part of the KDE project
   Copyright (C) 2003   Joseph Wenninger <jowenn@kde.org>

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

#include "kudesigner_kexiplugin.h"
#include "kudesigner_kexiplugin.moc"
#include <propertywidgets.h>
#include <propertyeditor.h>
#include <kudesigner_doc.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <canvdefs.h>
#include <canvbox.h>
#include <creportitem.h>
#include <kexidialogbase.h>
#include <kexiproject.h>
#include <kexidataprovider.h>
#include <kexiprojecthandler.h>
#include <klocale.h>
#include <qlabel.h>


//BEGIN KuKexiDataSourceComboBox

KuKexiDataSourceComboBox:: KuKexiDataSourceComboBox ( const PropertyEditor *editor,
	 QString pname, QString value, KexiDataSourceComboBox::ItemList data, int level, QWidget * parent,
		const char * name) :
	 	KexiDataSourceComboBox(parent,name,data) {
	setPName(pname);
	setValue(value);
	m_level=level;
  	connect(this, SIGNAL(activated(int)), this, SLOT(updateProperty(int)));
	connect(this, SIGNAL(propertyChanged(QString, QString)),
		editor, SLOT(emitPropertyChange(QString, QString)));
}

KuKexiDataSourceComboBox::~KuKexiDataSourceComboBox(){}

void KuKexiDataSourceComboBox::setValue(const QString value, bool emitChange)
{
    if (!value.isNull())
    {
        selectGlobalIdentifier(value);
        if (emitChange) {
            emit propertyChanged(pname(), value);
	}
    }
}

void KuKexiDataSourceComboBox::updateProperty(int val)
{
    emit propertyChanged(pname(), globalIdentifier());
    emit propertyChanged(m_level,val);
}

QString KuKexiDataSourceComboBox::value() const {
	return globalIdentifier();
}


//END KuKexiDataSourceComboBox

KuKexi::KuKexi(QObject *parent, const char* name, const QStringList& args):KuDesignerPlugin(parent,name,args) {
	m_kugar=(KudesignerDoc*)parent;
	m_dialog=((KexiDialogBase*)parent->parent());
	m_kexi=(KexiProject*)m_dialog->kexiProject();

	updateSourceList();
}

KuKexi::~KuKexi(){}
void KuKexi::createPluggedInEditor(QWidget *& retVal,PropertyEditor *editor,
                Property *p, CanvasBox *cb) {
	if (!cb) return;
	if (cb->rtti()==KuDesignerRttiDetail) {
		if (p->type()==1024){
			//retVal = new PComboBox(editor, p->name(), p->value(), &sourceMapping, false, 0, 0);
			retVal = new KuKexiDataSourceComboBox(editor,p->name(), p->value(),m_sourceMapping,
				cb->props["Level"]->value().toInt());
		        connect(retVal, SIGNAL(propertyChanged(int,int)), this, SLOT(slotDataSourceSelected(int,int)));
		}
	} else
		if ((cb->rtti()==KuDesignerRttiCanvasField) && (p->type()==FieldName)) {
			if (((CanvasReportItem*)cb)->section()->rtti()==KuDesignerRttiDetail)
//				PKexiStringListCombo(
				retVal = new PSpinBox(editor, p->name(), p->value(), 0, 10000, 1, 0);
			else
				retVal = new QLabel(i18n("Unsupported"),editor);
		}
}

void KuKexi::newCanvasBox(int type, CanvasBox *cb) {
	switch (type) {
		case KuDesignerRttiDetail:
			cb->props["Datasource"] = *(new PropPtr(new Property(1024, "Datasource", i18n("Datasource"), "0")));
			break;
		default:
			break;
	}
}

void KuKexi::slotDataSourceSelected(int level, int value)
{
	if (!value) return;
	if (m_fieldMap.contains(value)) return;

	kdDebug()<<"Need to insert new field list into map"<<endl;	
	KexiDataSourceComboBox::Item item=*m_sourceMapping.at(value);
	KexiProjectHandler *h=m_kexi->handlerForMime(item.mime);
	KexiDataProvider *dp=(KexiDataProvider*)h;
	m_fieldMap[value]=dp->fields(item.identifier);	
	m_sectionMap[level]=value;
}

void KuKexi::updateSourceList() {
	KexiDataSourceComboBox::fillList(m_kexi,m_sourceMapping);
}

K_EXPORT_COMPONENT_FACTORY( kudesigner_kexiplugin, KGenericFactory<KuKexi> )
