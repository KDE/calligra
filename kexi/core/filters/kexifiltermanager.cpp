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

#include <kmimetype.h>
#include <kdebug.h>
#include <ktrader.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kparts/componentfactory.h>

KexiFilterManager::KexiFilterManager(KexiProject *project):QObject(project),
	m_project(project)
{
	m_typeNames<<"kexi/table"<<"kexi/query"<<"kexi/form"<<"kexi/script";
}

bool KexiFilterManager::import(unsigned long importType)
{
	QString constraints;
	unsigned long calcImportType=importType;
	if (calcImportType==0) return false;
	for (unsigned int bitcnt=0;((calcImportType!=0) && (bitcnt<m_typeNames.count()));bitcnt++,calcImportType=(calcImportType>>1)){
		if (!(calcImportType & 1)) continue;

		if (!constraints.isEmpty()) constraints+=" or ";
		constraints += "('"+m_typeNames[bitcnt]+"' in [X-KEXI-ImportTypes])";
	}

	kdDebug()<<"KexiFilterManager::import :constraints="<<constraints<<endl;

	KTrader::OfferList filters=KTrader::self()->query("Kexi/ImportFilter",constraints);
	KTrader::OfferList::Iterator it(filters.begin());

	if (it==filters.end()) {
		kdDebug()<<"KexiFilterManager::import: NOTING FOUND !!!!!"<<endl;
#ifdef __GNUC_
#warning tell the user that there are no filters
#endif
		return false;
	}

	QString dialogfilter;
	QString allSupported;
	QMap<QString,QString> mapping;

	for (;it!=filters.end(); ++it) {
		QStringList mts=(*it)->property("X-KEXI-Import").toStringList();
		for (int i=0;i<(int)mts.count();i++) {
			mapping.insert(mts[i],(*it)->library());

			KMimeType::Ptr mt = KMimeType::mimeType( mts[i] );
			dialogfilter+=mt->patterns().join(" ")+"|"+mt->comment()+"\n";
			allSupported+=mt->patterns().join(" ")+" ";
		}

	}
	kdDebug()<<dialogfilter<<endl;

	if (dialogfilter.isEmpty()) return false; //perhaps show a message
	dialogfilter=allSupported+"|"+i18n("All Supported File Types")+"\n"+dialogfilter;

	KFileDialog *dialog=new KFileDialog(QString::null, QString::null, 0L, "file dialog", true);
	dialog->setFilter( dialogfilter);
	if (dialog->exec()!=QDialog::Accepted) {
		delete dialog;
		return true;
	}

	KURL url(dialog->selectedURL());
	delete dialog;

	if ((!url.isValid()) || (url.isEmpty())) {
	//error
		return false;
	}
	KMimeType::Ptr im=KMimeType::findByURL(url);
	if (!mapping.contains(im->name()))  {
		kdDebug()<<"Unsupported mimetype"<<endl;
#ifdef __GNUC__
#warning display an error
#endif
		return false;
	}

	return doImport(mapping[im->name()],url,importType);

}

bool KexiFilterManager::doImport(const QString& lib, const KURL &url, unsigned long allowedTypes)
{
	int err=0;
	QStringList l;
	l<<"IMPORT";;
	KexiFilter *f=KParts::ComponentFactory::createInstanceFromLibrary<KexiFilter> (lib.utf8(),this,"import filter",l,&err);
	if (err!=0) {
#ifdef __GNUC__
#warning display some error dialog here
#endif
		kdDebug()<<"Import filter instance couldn't be created:"<<err<<endl;
		return false;
	}

	bool success=f->import(url,allowedTypes);
	delete f;
	return success;
}

KexiProject *KexiFilterManager::project()
{
	return m_project;
}

KexiFilterManager::~KexiFilterManager()
{
}

#include "kexifiltermanager.moc"
