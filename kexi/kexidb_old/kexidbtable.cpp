#include <kexidbtable.h>

KexiDBTable::KexiDBTable(const QString & name):QValueList<KexiDBField>(),m_tableName(name) {}

KexiDBTable::~KexiDBTable() {}

void KexiDBTable::addField(KexiDBField field) {
	field.setTable(m_tableName);
	append(field);
}

const QString& KexiDBTable::tableName() const {
	return m_tableName;
}
