#include <kexikugarhandleritem.h>
#include <kexikugarhandler.h>
#include <kexikugarhandleritem.moc>
#include <sys/stat.h>
#include <sys/types.h>
#include <qfile.h>
#include <kdebug.h>

KexiKugarHandlerItem::KexiKugarHandlerItem(KexiKugarHandler *parent, const QString& name, const QString& mime,
                                const QString& identifier):KexiProjectHandlerItem(parent,name,mime,identifier) {
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
		else m_tempPath+="/";
	}
}

KexiKugarHandlerItem::~KexiKugarHandlerItem() {}
const QString &KexiKugarHandlerItem::reportTemplate() const {
	return m_reportTemplate;
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

