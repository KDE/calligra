#include "core/filters/kexitableimportsourceiface.h"
#include "kexicsvsourcebase.h"


#ifndef _KEXI_CSV_SOURCE
#define _KEXI_CSV_SOURCE

class KexiCSVSource: public KexiCSVSourceBase, public KexiTableImportSourceIface {
	Q_OBJECT
public:

	KexiCSVSource(QWidget *parent);
	virtual ~KexiCSVSource();
	bool setFile(const QString &fileName);
        virtual int fieldCount();
        virtual bool firstTableRow();
        virtual bool nextTableRow();
        virtual QVariant tableValue(int field);
	virtual KexiDBTable tableStructure();
private:
	QString m_file;
	int m_dataPos;
protected slots:
	bool parseFile(const QString &file);
	void reparse(const QString &file);
	void reparse();

};

#endif
