#include <kdebug.h>

#include <kexidb/queryschema.h>
#include <kexidb/connection.h>

#include "kexiquerydocument.h"

KexiQueryDocument::KexiQueryDocument(int id, KexiDB::Connection *c, KexiDB::QuerySchema *s)
{
	m_id = id;
	m_schema = s;
	m_connection = c;
	m_history = new History();
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
KexiQueryDocument::addHistoryItem(const QString &query, const QString &error)
{
	HistoryEntry *he=m_history->last();
	if (he)
	{
		if (he->statement() == query)
		{
			he->updateTime(QTime::currentTime());
			return;
		}
	}

	HistoryEntry *e = new HistoryEntry(error.isEmpty(), QTime::currentTime(), query, 0, error);
	m_history->append(e);
}

