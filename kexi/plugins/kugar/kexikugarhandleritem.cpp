#include <kexikugarhandleritem.h>
#include <kexikugarhandleritem.moc>

KexiKugarHandlerItem::KexiKugarHandlerItem(KexiProjectHandler *parent, const QString& name, const QString& mime,
                                const QString& identifier):KexiProjectHandlerItem(parent,name,mime,identifier) {
	m_storedDataSets.resize(51);
	m_storedDataSets.setAutoDelete(true);
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

const QString& KexiKugarHandlerItem::dataSource() const {
	return m_sourceIdentifier;
}

void KexiKugarHandlerItem::setDataSource(const QString &identifier) {
	m_sourceIdentifier=identifier;
}
