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

//BEGIN KuKexiFieldComboBox

KuKexiFieldComboBox::KuKexiFieldComboBox ( const PropertyEditor *editor,
	 QString pname, QString value, KuKexi *kukexi, int level, QWidget * parent,
		const char * name) :
	 	KComboBox(parent,name),
		m_level(level),
		m_kukexi(kukexi) {
	setPName(pname);
	fill();
	setValue(value);
  	connect(this, SIGNAL(activated(int)), this, SLOT(updateProperty(int)));
	connect(this, SIGNAL(propertyChanged(QString, QString)),
		editor, SLOT(emitPropertyChange(QString, QString)));
}

KuKexiFieldComboBox::~KuKexiFieldComboBox(){}

void KuKexiFieldComboBox::fill() {
	kdDebug()<<"KuKexiFieldComboBox::fill"<<endl;
	insertItem(i18n("<NONE>"));
	for (int i=m_level;i>=0;i--) {
		KuKexi::SectionMap::const_iterator sit=m_kukexi->m_sectionMap.find(i);
		if (sit!=m_kukexi->m_sectionMap.end()) {
			KuKexi::FieldMap::const_iterator fit=m_kukexi->m_fieldMap.find(sit.data());
			if (fit!=m_kukexi->m_fieldMap.end()) {
				QString prefix=i18n("Please make sure that the translated string looks like Detail %1. (%1 preceded by a space and directly followed by a . and no character after that)","Detail %1.").arg(i);
				for (QStringList::const_iterator it=fit.data().begin();it!=fit.data().end();++it) {
					insertItem(prefix+(*it));
				}
			}
		}
	}
}


void KuKexiFieldComboBox::setValue(const QString value, bool emitChange)
{
	if (!value.isNull())
	{
				kdDebug()<<"Trying to lookup: "<<value;
				int cnt=count();
				for (int i=0;i<cnt;++i) {
					if (text(i)== value) {	
						setCurrentItem(i);
						return;
					}
				}

	}
	setCurrentItem(0);
	emit propertyChanged(pname(),"");
}

QString KuKexiFieldComboBox::convertFromStorageName(const QString& stn) {
	if (stn.startsWith("D")) {
		QString tmp=stn.right(stn.length()-1);
		int dpos=tmp.find('_');
		if (dpos!=-1) {
			bool ok;
			int number=tmp.left(dpos).toInt(&ok);
			if (ok) {
				QString entryName=i18n("Please make sure that the translated string looks like Detail %1. (%1 preceded by a space and directly followed by a . and no character after that)","Detail %1.").arg(number);
				entryName=entryName+tmp.right(tmp.length()-1-dpos);
				return entryName;
			}
		}
	}
	return "";
}

QString KuKexiFieldComboBox::convertToStorageName(const QString& displayname) {
	QString translatedPrefix=i18n("Please make sure that the translated string looks like Detail %1. (%1 preceded by a space and directly followed by a . and no character after that)","Detail %1.");
	int dataBegin=translatedPrefix.find(" %1.");
	QString tmp=displayname;
	tmp=tmp.right(tmp.length()-1-dataBegin);
	QString numberStr=tmp.left(tmp.find('.'));
	tmp="D"+numberStr+"_"+tmp.right(tmp.length()-1-tmp.find('.'));
	return tmp;
}

void KuKexiFieldComboBox::updateProperty(int val)
{
	if (!val)
		emit propertyChanged(pname(),"");
	else
		emit propertyChanged(pname(),currentText());
//    emit propertyChanged(pname(), globalIdentifier());
//    emit propertyChanged(m_level,val);
}

QString KuKexiFieldComboBox::value() const {
	return "";
//fixme	return globalIdentifier();
}

//END KuKexiFieldComboBox



KuKexi::KuKexi(QObject *parent, const char* name, const QStringList& args):KuDesignerPlugin(parent,name,args) {
	m_kugar=(KudesignerDoc*)parent;
	m_dialog=((KexiDialogBase*)parent->parent());
	m_kexi=(KexiProject*)m_dialog->kexiProject();

	updateSourceList();
}

KuKexi::~KuKexi(){}
void KuKexi::createPluggedInEditor(QWidget *& retVal,PropertyEditor *editor,
                Property *p, CanvasBox *cb) {
	if (!cb) {
		kdDebug()<<"KuKexi::createPluggedInEditor cb==0 => no custom property editor"<<endl;
		 return;
	}
	else
		kdDebug()<<"KuKexi::createPluggedInEditor cb!=0 => creating custom property editor"<<endl;

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
//				retVal = new PSpinBox(editor, p->name(), p->value(), 0, 10000, 1, 0);
				retVal = new KuKexiFieldComboBox(editor,p->name(),p->value(),
					this,cb->props["Level"]->value().toInt());
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
	item.pixmap.detach();
	KexiProjectHandler *h=m_kexi->handlerForMime(item.mime);
	KexiDataProvider *dp=static_cast<KexiDataProvider*>(h->qt_cast("KexiDataProvider"));
	QStringList l=dp->fields(item.identifier);
	for (QStringList::const_iterator strit=l.begin();strit!=l.end();++strit)
		kdDebug()<<"Field "<<(*strit)<<" added to cache"<<endl;
	m_fieldMap[value]=l;
	m_sectionMap[level]=value;
}

void KuKexi::updateSourceList() {
	KexiDataSourceComboBox::fillList(m_kexi,m_sourceMapping);
}

K_EXPORT_COMPONENT_FACTORY( kudesigner_kexiplugin, KGenericFactory<KuKexi> )
