/*  This file is part of the KDE project
    Copyright (C) 2002,2003 Joseph Wenninger <jowenn@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Library General Public License version 2 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <kexikugarhandleritem.h>
#include <kexikugarhandler.h>
#include <kexikugarhandleritem.moc>
#include <kexidataprovider.h>
#include <ksimpleconfig.h>
#include <kparts/componentfactory.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <qfile.h>
#include <kdebug.h>
#include <koStore.h>
#include <qdom.h>
#include <kexiDB/kexidbrecordset.h>
#include <qtextstream.h>
#include <qfile.h>
#include <kexikugarwrapper.h>

KexiKugarHandlerItem::KexiKugarHandlerItem(KexiProjectHandler *handler, const QString& ident, 
	const QString& mime, const QString& title)
 : KexiProjectHandlerItem(handler, ident, mime, title)
	, m_designer(0)
	, m_viewer(0)
{
	m_storedDataSets.resize(51);
	m_storedDataSets.setAutoDelete(true);
	QString tmpPath=dynamic_cast<KexiKugarHandler*>(handler)->tempPath();
	if (!tmpPath.isEmpty()) {
		m_tempPath=tmpPath+fullIdentifier();
		kdDebug()<<"KexiKugarHandlerItem::creating directory: "<<m_tempPath<<endl;
		if (mkdir(QFile::encodeName(m_tempPath),0700)!=0) {
			kdDebug()<<"FAILED"<<endl;
			m_tempPath="";
		}
		else
		{
			m_tempPath+="/";
				if (mkdir(QFile::encodeName(m_tempPath+"data"),0700)!=0) {
				kdDebug()<<"FAILED"<<endl;
			}
			if (mkdir(QFile::encodeName(m_tempPath+"tmpData"),0700)!=0) {
				kdDebug()<<"FAILED"<<endl;
			}

			handler->kexiProject()->addFileReference(FileReference("reports",
				"/"+fullIdentifier()+"/template.kut", "/reports/"+fullIdentifier()+"/template.kut"));
			handler->kexiProject()->addFileReference(FileReference("reports",
				"/"+fullIdentifier()+"/template.kukexi", "/reports/"+fullIdentifier()+"/template.kukexi"));
		}
	}
}

KexiKugarHandlerItem::~KexiKugarHandlerItem() {
	if (!handler()) return;
	if (!(handler()->kexiProject())) return;
	handler()->kexiProject()->removeFileReference(FileReference("reports",
		"/"+identifier()+"/template.kut", "/reports/"+identifier()+"/template.kut"));
	handler()->kexiProject()->removeFileReference(FileReference("reports",
		"/"+identifier()+"/template.kukexi", "/reports/"+identifier()+"/template.kukexi"));
}

const QString &KexiKugarHandlerItem::reportTemplate() const {
	return m_reportTemplate;
}

KoDocument *KexiKugarHandlerItem::designer(bool newReport) {
	if (m_designer) return m_designer;

    QStringList config;
    config<<"plugin=kudesigner_kexiplugin";
    config<<"forcePropertyEditorPosition=left";
    m_designer=KParts::ComponentFactory::createPartInstanceFromLibrary<KoDocument>(QFile::encodeName("libkudesignerpart"),
                                                                                   0,0,this,0,config);
    if (newReport) {
        if (!m_designer->initDoc() ) {
			delete m_designer;
			m_designer=0;
        }
    } else {
		if (!m_designer->openURL(m_tempPath+"template.kut")) {
			delete m_designer;
			m_designer=0;
		}
	}
	return m_designer;
}

void KexiKugarHandlerItem::store (KoStore *ks) {
	if (m_tempPath.isEmpty()) return;
	if (m_designer!=0) m_designer->saveAs(m_tempPath+"template.kut");
	if (!ks->addLocalFile(m_tempPath+"template.kut","/reports/"+identifier()+"/template.kut") )
		kdDebug()<<"Error storing "<<"/reports/"+identifier()+"/template.kut"<<endl;
	if (!ks->addLocalFile(m_tempPath+"template.kukexi","/reports/"+identifier()+"/template.kukexi") )
		kdDebug()<<"Error storing "<<"/reports/"+identifier()+"/template.kukexi"<<endl;
}

void KexiKugarHandlerItem::pluginStorageFile(QString &path) {
	if (m_tempPath.isEmpty()) {
		path=QString::null;
		return;
	}
	path=m_tempPath+"/template.kukexi";
}

void KexiKugarHandlerItem::setReportTemplate (const QString &reportTemplate) {
	m_reportTemplate=reportTemplate;
}

const QStringList &KexiKugarHandlerItem::storedDatasets() const {
	return m_storedDataSetNames;
}

void KexiKugarHandlerItem::setStoredDataset(const QString &datasetName, const QString &data) {
	m_storedDataSets.replace(datasetName,new QString(data));

	if (!m_storedDataSetNames.contains(datasetName))
		m_storedDataSetNames.append(datasetName);

}

void KexiKugarHandlerItem::removeStoredDataset(const QString &datasetName) {
	m_storedDataSets.remove(datasetName);
	m_storedDataSetNames.remove(datasetName);
}

const QString &KexiKugarHandlerItem::storedDataset(const QString &datasetName) const {
	QString *tmp=m_storedDataSets.find(datasetName);
	return tmp?*tmp:QString::null;
}


void KexiKugarHandlerItem::view(KexiView *view) {
	if (m_tempPath.isEmpty()) {
		kdDebug()<<"Put some error messages here"<<endl;
		return;
	}
	if (m_designer) {
		if (m_designer->isModified()) {
			kdDebug()<<"There is a modified designer document. Ask the user if he wants to save and use them"<<endl;
		}
	}
	QFile tmp(m_tempPath+"/template.kut");
	if (!tmp.exists()) {
		kdDebug()<<"show an error message"<<endl;
	}
	QString filename=generateDataFile();
	if (!filename.isEmpty()) {
		new KexiKugarWrapper(view, this ,filename);
	}
}

QString KexiKugarHandlerItem::generateDataFile() {
	KSimpleConfig templateInfo(m_tempPath+"template.kukexi",true);
	QDomDocument doc("KugarData");
	doc.appendChild(doc.createElement("KugarData"));
	doc.documentElement().setAttribute("Template","../template.kut");
	doc.insertBefore(doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\""),doc.firstChild());
	QDomElement parent=doc.documentElement();

	QString queryLine;
	kdDebug()<<"Trying to build data file"<<endl;
	kdDebug()<<templateInfo.readEntry(QString("DETAIL_0"),"NOTHING FOUND")<<endl;
	QDomNode nodeBefore=QDomNode();
	for(int level=0;!(queryLine=templateInfo.readEntry(QString("DETAIL_%1").arg(level),QString())).isEmpty();level++) {
		kdDebug()<<"Found a none empty query line for level "<<level<<endl;

		QString providerName=KexiProjectHandler::handlerNameFromGlobalIdentifier(queryLine);
		if (providerName.isEmpty()) break;
		kdDebug()<<"provider plugin name has been decoded: "<<providerName<<endl;

		KexiDataProvider *prov=KEXIDATAPROVIDER((handler()->kexiProject()->handlerForMime(providerName)));
		if (!prov) break;;
		kdDebug()<<"Provider found"<<endl;

		QString levelStr=QString("%1").arg(level);
		QString levelM1Str=QString("%1").arg(level-1);
		QString shortLevelStr=QString("D%1_").arg(level);
		do {
			if (level) {
				for (;!nodeBefore.isNull();nodeBefore=nodeBefore.nextSibling()) {
					if (nodeBefore.toElement().attribute("level")==levelM1Str) break;
				}
				if (nodeBefore.isNull()) break;
			}


			KexiDBRecordSet  *recs=prov->records(0,KexiProjectHandler::localIdentifier(queryLine),KexiDataProvider::Parameters());
			if (recs) {
				kdDebug()<<"There are records"<<endl;

				while (recs->next()) {
					QDomElement rowitem=doc.createElement("Row");
					rowitem.setAttribute("level",levelStr);
					for (uint i=0;i<recs->fieldCount();i++) {
						rowitem.setAttribute(shortLevelStr+recs->fieldName(i),recs->value(i).toString());
					}
					nodeBefore=parent.insertAfter(rowitem,nodeBefore);
				}
				delete recs;
			}
			nodeBefore=nodeBefore.nextSibling();
		} while (!nodeBefore.isNull());
		nodeBefore=parent.firstChild();
	}

	QFile f(m_tempPath+"tmpData/data1.kud");
	f.open(IO_WriteOnly);
	QTextStream st(&f);
	st.setEncoding(QTextStream::UnicodeUTF8);
	st<<doc.toString();
	f.close();
	return m_tempPath+"tmpData/data1.kud";
}
