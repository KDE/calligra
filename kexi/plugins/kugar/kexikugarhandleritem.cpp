#include <kexikugarhandleritem.h>
#include <kexikugarhandler.h>
#include <kexikugarhandleritem.moc>
#include <kparts/componentfactory.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <qfile.h>
#include <kdebug.h>
#include <koStore.h>

KexiKugarHandlerItem::KexiKugarHandlerItem(KexiKugarHandler *parent, const QString& name, const QString& mime,
                                           const QString& identifier)
    : KexiProjectHandlerItem(parent,name,mime,identifier),
      m_designer(0),m_viewer(0)
{
	m_storedDataSets.resize(51);
	m_storedDataSets.setAutoDelete(true);
	QString tmpPath=parent->tempPath();
	if (!tmpPath.isEmpty()) {
		m_tempPath=tmpPath+identifier;
		kdDebug()<<"KexiKugarHandlerItem::creating directory: "<<m_tempPath<<endl;
		if (mkdir(QFile::encodeName(m_tempPath),0700)!=0) {
			kdDebug()<<"FAILED"<<endl;
			m_tempPath="";
		}
		else
		{
			m_tempPath+="/";
		        parent->kexiProject()->addFileReference(FileReference("reports",
				"/"+identifier+"/template.kut", "/reports/"+identifier+"/template.kut"));
		        parent->kexiProject()->addFileReference(FileReference("reports",
				"/"+identifier+"/template.kukexi", "/reports/"+identifier+"/template.kukexi"));
		}
	}
}

KexiKugarHandlerItem::~KexiKugarHandlerItem() {
	if (!projectPart()) return;
	if (!(projectPart()->kexiProject())) return;
	projectPart()->kexiProject()->removeFileReference(FileReference("reports",
		"/"+shortIdentifier()+"/template.kut", "/reports/"+shortIdentifier()+"/template.kut"));
	projectPart()->kexiProject()->removeFileReference(FileReference("reports",
		"/"+shortIdentifier()+"/template.kukexi", "/reports/"+shortIdentifier()+"/template.kukexi"));
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
	if (!ks->addLocalFile(m_tempPath+"template.kut","/reports/"+shortIdentifier()+"/template.kut") )
		kdDebug()<<"Error storing "<<"/reports/"+shortIdentifier()+"/template.kut"<<endl;
	if (!ks->addLocalFile(m_tempPath+"template.kukexi","/reports/"+shortIdentifier()+"/template.kukexi") )
		kdDebug()<<"Error storing "<<"/reports/"+shortIdentifier()+"/template.kukexi"<<endl;
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

