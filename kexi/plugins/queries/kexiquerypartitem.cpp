#include <kexiquerypartitem.h>
#include <qstring.h>
#include <kexiproject.h>
#include <qdom.h>
#include <kdebug.h>
#include <koStore.h>

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

void KexiQueryPartItem::store(KoStore* store) {
        kdDebug() << "KexiQueryPartItem::store(KoStore*)" << endl;

	QDomDocument domDoc("Query");
	domDoc.appendChild(domDoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));

        QDomElement nameElement = domDoc.createElement("Query");
        QDomText attrName = domDoc.createTextNode(identifier());
        nameElement.appendChild(attrName);
        domDoc.appendChild(nameElement);

        QDomElement itemsElement = domDoc.createElement("Items");
        domDoc.appendChild(itemsElement);

	for (QueryEntryList::const_iterator it=m_queryEntryList.begin();it!=m_queryEntryList.end();++it) {
		QDomElement item = domDoc.createElement("item");
		item.setAttribute("source",(*it).source);
		item.setAttribute("field",(*it).field);
		item.setAttribute("shown",(*it).show?"true":"false");
		item.setAttribute("orC",(*it).orC);
		item.setAttribute("andC",(*it).andC);
                itemsElement.appendChild(item);
	}
/*
                QDomElement preparsed = domDoc.createElement("preparsed");
                domDoc.appendChild(preparsed);
                QDomText tPreparsed = domDoc.createTextNode(m_editor->getQuery());
                preparsed.appendChild(tPreparsed); */

        QByteArray data = domDoc.toCString();
        data.resize(data.size()-1);

	if(store) {
        	store->open("/query/" + identifier() + ".query");
                store->write(data);
                store->close();
        }

}

void KexiQueryPartItem::load(KoStore*) {
}

