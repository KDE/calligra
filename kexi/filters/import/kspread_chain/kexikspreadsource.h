#include "core/filters/kexitableimportsourceiface.h"
#include "kexikspreadsourcebase.h"


#ifndef _KEXI_KSpread_SOURCE
#define _KEXI_KSpread_SOURCE

class KoDocument;

class KexiKSpreadSource: public KexiKSpreadSourceBase, public KexiTableImportSourceIface {
	Q_OBJECT
public:

	KexiKSpreadSource(QWidget *parent);
	virtual ~KexiKSpreadSource();
	bool setFile(const QString &fileName);
        virtual int fieldCount();
        virtual bool firstTableRow();
        virtual bool nextTableRow();
        virtual QVariant tableValue(int field);
	virtual KexiDBTable tableStructure();
	virtual QPtrList<QWidget> tableSourceWidgets(QWidget*){}
	QString getRange();
protected slots:
	void syncHeaderSection();
	void syncDataSection();

private:
	QString m_file;
	int m_dataPos;
	KoDocument *m_kspread;
	QRect m_headerArea;
	QRect m_dataArea;
};

#endif
