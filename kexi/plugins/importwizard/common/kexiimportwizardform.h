
#ifndef _KEXI_TABLE_IMPORT_FORM_H_
#define _KEXI_TABLE_IMPORT_FORM_H_

#include "kexiimportwizardformbase.h"
#include <kexiDB/kexidbtable.h>

class KexiTableImportSourceIface;
class KexiFilterManager;
class KexiFilter;
class KexiImportWizard;

class KexiTableImportForm: public KexiImportWizardFormBase {
	Q_OBJECT
public:
	KexiTableImportForm(KexiFilterManager *filterManager,KexiImportWizard *wiz,const QString& 
		dialogfilter,QMap<QString,QString> mimePluginMapping);
	virtual ~KexiTableImportForm();
private:
	QWidget *m_previousPage;
        bool m_idColumn;
	KexiDBTable m_sourceTable;
	KexiDBTable m_destinationTable;
	KexiFilterManager *m_filterManager;
	QString m_dialogFilter;
	QMap<QString,QString> m_mimePluginMapping;
	KexiFilter *m_filter;
	bool m_recursiveOpen;
	KexiImportWizard *m_wiz;
	QWidget *m_openWidget;
	bool m_initializing;

	void initDestChoice();
	void buildNewTablePage();
	void buildExistingTablePage();
	bool createTable();
	bool importValues(const QString& tableName, int mapLen, int mapping[]);
	void initFileDialog();
protected:
	virtual void accept();
protected slots:
	void pageSelected(const QString&);
	void changeDestinationType(bool);
	void loadPlugin();
public:
	void setMode(unsigned long);
public slots:
	void filterHasBeenLoaded(KexiFilter*,const KURL &);
};


#endif
