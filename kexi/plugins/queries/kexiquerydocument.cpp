#include <kdebug.h>

#include <kexidb/queryschema.h>
#include <kexidb/connection.h>

#include "kexiquerydocument.h"

KexiQueryDocument::KexiQueryDocument(int id, KexiDB::Connection *c, KexiDB::QuerySchema *s)
{
	m_id = id;
	m_schema = s;
	m_connection = c;
}

KexiQueryDocument::~KexiQueryDocument()
{
}

void
KexiQueryDocument::setSchema(KexiDB::QuerySchema *s)
{
	m_schema = s;
	if(!s)
		return;

	m_connection->executeQuery(QString("DELETE FROM kexi__querydata WHERE q_id = %1").arg(m_id));
	m_connection->executeQuery(QString("INSERT INTO kexi__querydata VALUES(%1, '%2', 0)").arg(m_id).arg(m_connection->selectStatement(*s)));
}

void
KexiQueryDocument::addHistoryItem(const QString &time, const QString &query, const QString &error)
{
}

