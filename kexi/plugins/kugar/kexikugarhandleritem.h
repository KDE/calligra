#ifndef _KEXI_KUGAR_HANDLER_ITEM_H_
#define _KEXI_KUGAR_HANDLER_ITEM_H_

#include <kexiprojecthandleritem.h>
#include <qstring.h>
#include <qmap.h>
#include <qstringlist.h>

class KexiKugarHandler;

class KexiKugarHandlerItem : public KexiProjectHandlerItem
{
	Q_OBJECT
public:
	KexiKugarHandlerItem(KexiKugarHandler *parent, const QString& name, const QString& mime,
                                const QString& identifier);
	~KexiKugarHandlerItem();
	const QString &reportTemplate() const;
	void setReportTemplate (const QString &reportTemplate);
	const QStringList &storedDatasets() const;
        void setStoredDataset(const QString &datasetName, const QString &data);
	void removeStoredDataset(const QString &datasetName);
	const QString &storedDataset(const QString &datasetName) const;
	KoDocument *designer(bool newReport=false);
	void store (KoStore *ks);

private:
	QStringList m_storedDataSetNames;
	QDict<QString> m_storedDataSets;
	QString m_reportTemplate;
	QString m_sourceIdentifier;
	QString m_tempPath;
	KoDocument *m_designer;
	KoDocument *m_viewer;
};

#endif
