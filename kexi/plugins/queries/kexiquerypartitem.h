#ifndef KEXIQUERYPARTITEM_H
#define KEXIQUERYPARTITEM_H

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
		QueryEntry(){;}
		QueryEntry(const QString& source_, const QString& field_, 
			bool show_, const QString& orC_, const QString& andC_): source(source_),
				field(field_),show(show_),orC(orC_),andC(andC_){;}
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

	void store(KoStore*);
	void load(KoStore*);

protected:
	friend class KexiQueryDesigner;
	friend class KexiQueryDesigerGuiEditor;

private:
	QueryEntryList m_queryEntryList;
};

#endif
