#ifndef _KEXI_DB_TABLE
#define _KEXI_DB_TABLE
#include <qvaluelist.h>
#include "kexidbfield.h"
#include <qstring.h>

class KexiDBTable: public QValueList<KexiDBField> {
public:
	KexiDBTable(const QString & name);
	~KexiDBTable();
	void addField(KexiDBField);
	const QString& tableName() const;

private:
	QString m_tableName;
};

#endif
