#include "kexiquerydocument.h"

KexiQueryDocument::KexiQueryDocument(KexiDB::QuerySchema *s)
{
	m_schema = s;
}

KexiQueryDocument::~KexiQueryDocument()
{
}

void
KexiQueryDocument::addHistoryItem(const QString &time, const QString &query, const QString &error)
{
}

