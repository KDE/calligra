/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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
#include "kexifiltermanager.h"
#include "kexifilter.h"
#include "../kexiproject.h"
#include "kexifilterwizardbase.h"

#include <kmimetype.h>
#include <kdebug.h>
#include <ktrader.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kparts/componentfactory.h>
#include <koFilterManager.h>

KexiFilterManager::KexiFilterManager(KexiProject *project):QObject(project),
	m_project(project)
{
	m_typeNames<<"kexi/table"<<"kexi/query"<<"kexi/form"<<"kexi/script";
}


bool KexiFilterManager::import(ImportSourceType sourceType,unsigned long importType)
{
	if (sourceType==File) return importFile(importType);
	else return false;
}

bool KexiFilterManager::importFile(unsigned long importType) 
{
	QString constraints;
	unsigned long calcImportType=importType;
	if (calcImportType==0) return false;
	
	constraints = "('kexi/*' in [X-KEXI-ImportTypes])";
	for (unsigned int bitcnt=0;((calcImportType!=0) && (bitcnt<m_typeNames.count()));bitcnt++,calcImportType=(calcImportType>>1)){
		if (!(calcImportType & 1)) continue;

		if (!constraints.isEmpty()) constraints+=" or ";
		constraints += "('"+m_typeNames[bitcnt]+"' in [X-KEXI-ImportTypes])";
	}


	kdDebug()<<"KexiFilterManager::import :constraints="<<constraints<<endl;

	KTrader::OfferList filters=KTrader::self()->query("Kexi/FileImportFilter",constraints);
	KTrader::OfferList::Iterator it(filters.begin());

	if (it==filters.end()) {
		kdDebug()<<"KexiFilterManager::import: NOTING FOUND !!!!!"<<endl;
#ifdef __GNUC_
#warning tell the user that there are no filters
#endif
		return false;
	}

	QString dialogfilter;
	QString allNativeSupported;
	QString allSupported;
	QMap<QString,QString> mapping;
	QMap<QString,QString> chainMapping;
	
	for (;it!=filters.end(); ++it) {
		if ((*it)->property("X-KEXI-OfficeChainImport").toBool()) {
			QStringList mtnat=(*it)->property("X-KEXI-Import").toStringList();
			for (unsigned int i2=0;i2<mtnat.count();i2++) {
				QStringList mts=KoFilterManager::mimeFilter( mtnat[i2].utf8(), KoFilterManager::Import );
				for (int i=0;i<(int)mts.count();i++) {
					if (mts[i]!=mtnat[i2])
						chainMapping.insert(mts[i],(*it)->library());
					else 
						mapping.insert(mts[i],(*it)->library());
					
				}
			}

		} else {
			QStringList mts=(*it)->property("X-KEXI-Import").toStringList();
			for (int i=0;i<(int)mts.count();i++) {
				mapping.insert(mts[i],(*it)->library());
			}
		}

	}

	for (QMap<QString,QString>::const_iterator it=mapping.begin();
		it!=mapping.end();++it) {
	
		KMimeType::Ptr mt = KMimeType::mimeType( it.key() );
		dialogfilter+=mt->patterns().join(" ")+"|"+mt->comment()+"\n";
		allSupported+=mt->patterns().join(" ")+" ";
		allNativeSupported+=mt->patterns().join(" ")+" ";

	}
	
	dialogfilter+="!!!!KEXI_NATIVE_SEPERATOR!!!!|-------\n";

	for (QMap<QString,QString>::const_iterator it=chainMapping.begin();
		it!=chainMapping.end();++it) {
		if (!mapping.contains(it.key())) {
			mapping.insert(it.key(),it.data());
		
			KMimeType::Ptr mt = KMimeType::mimeType( it.key() );
			dialogfilter+=mt->patterns().join(" ")+"|"+mt->comment()+"\n";
			allSupported+=mt->patterns().join(" ")+" ";

		}
	}


	kdDebug()<<dialogfilter<<endl;

	if (dialogfilter.isEmpty()) return false; //perhaps show a message
	
	dialogfilter=allNativeSupported+"|"+i18n("All Natively Supported File Types")+"\n"+
		allSupported+"|"+i18n("All Supported File Types")+"\n"+"!!!!KEXI_NATIVE_SEPERATOR!!!!|-------\n"
		+dialogfilter;

	int err=0;
	KexiFilterWizardBase *f=KParts::ComponentFactory::createInstanceFromLibrary<KexiFilterWizardBase>(
		"kexifileimport",this,"kexi import",QStringList(),&err);
	if (err!=0) {
#ifdef __GNUC__
#warning display some error dialog here
#endif
		kdDebug()<<"Import filter instance couldn't be created:"<<err<<endl;
		return false;
	}
	
	f->exec(dialogfilter,mapping,importType,false);
	// f has to delete itself
	return true;
}



KexiFilter* KexiFilterManager::loadFilter(const QString &libname,QObject *parent) {
	int err=0;
	KexiFilter *f=KParts::ComponentFactory::createInstanceFromLibrary<KexiFilter>(
		libname.latin1(),parent,libname.latin1(),QStringList(),&err);
	if (err!=0) {
#ifdef __GNUC__
#warning display some error dialog here
#endif
		kdDebug()<<"Import filter instance couldn't be created:"<<err<<endl;
		return 0;
	}
	return f;
}

KexiProject *KexiFilterManager::project()
{
	return m_project;
}

KexiFilterManager::~KexiFilterManager()
{
}

#include "kexifiltermanager.moc"
