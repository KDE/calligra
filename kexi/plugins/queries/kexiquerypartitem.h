#ifndef _KEXI_QUERY_PART_ITEM_H
#define _KEXI_QUERY_PART_ITEM_H

#include <kexiprojecthandleritem.h>
#include <qvaluelist.h>

class QString;
class KexiProjectHandler;

class KexiQueryPartItem: public KexiProjectHandlerItem 
{
	Q_OBJECT
public:

	class QueryEntry
	{
	public:
		QString source;
		QString field;
		bool show;
		QString orC;
		QString andC;
	};

	typedef QValueList<QueryEntry> QueryEntryList;


	KexiQueryPartItem(KexiProjectHandler *parent, const QString& name, const QString& mime, 
			const QString& identifier);
	virtual ~KexiQueryPartItem();
	const QueryEntryList &getQueryData();
	void setQueryData(const QueryEntryList&);

protected:
	friend class KexiQueryDesigner;
	friend class KexiQueryDesigerGuiEditor;

private:
	QueryEntryList m_queryEntryList;
};

#endif
