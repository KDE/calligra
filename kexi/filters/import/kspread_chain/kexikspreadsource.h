#include "core/filters/kexitableimportsourceiface.h"
#include "kexikspreadsourcebase.h"
#include <dcopref.h>

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
	void showRange(const QString& range);
	void showRange(const QString&,int,int,int,int);
	QRect rangeFromString(const QString& range, QString& tableName);
	bool checkConsistency();
	QString kspreadCellAsString(const QString& table, int x, int y);
	bool kspreadNoContent(const QString& table, QRect area);
protected:
	virtual bool eventFilter ( QObject * watched, QEvent * e );
protected slots:
	void syncHeaderSection();
	void syncDataSection();

private:
	QString m_file;
	int m_dataPos;
	KoDocument *m_kspread;

	QRect m_headerArea;
	QRect m_dataArea;
	QString m_headerTable;
	QString m_dataTable;

	DCOPRef m_documentMap;

	enum DataEndType {RangeEnd=0,ShrinkEmpty=1,ExpandEmpty=2};	

	DataEndType m_dataEnd;
	bool m_recordIsRow;
};

#endif
