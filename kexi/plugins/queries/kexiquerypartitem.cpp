#include <kexiquerypartitem.h>
#include <qstring.h>
#include <kexiproject.h>


KexiQueryPartItem::KexiQueryPartItem(KexiProjectHandler *parent,
		const QString& name, const QString& mime, 
		const QString& identifier)
	:KexiProjectHandlerItem(parent,name,mime,identifier){
}

KexiQueryPartItem::~KexiQueryPartItem() {
}

const KexiQueryPartItem::QueryEntryList &KexiQueryPartItem::getQueryData() {
	return m_queryEntryList;
}


void KexiQueryPartItem::setQueryData(const KexiQueryPartItem::QueryEntryList& newlist) {
	m_queryEntryList=newlist;
}
