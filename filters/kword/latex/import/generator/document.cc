#include <qdom.h>
#include <qtextstream.h>

#include "kdebug.h"
#include <koStore.h>
#include "document.h"


bool Document::analyse(QPtrList<Element>* root)
{
	return true;
}

bool Document::generate(KoStore* store)
{
	QDomDocument doc("KWORD");
	doc.appendChild(doc.createProcessingInstruction("xml", 
				"version=\"1.0\" encoding=\"UTF-8\""));
  QDomElement root = doc.createElement( "DOC" );
	root.setAttribute("editor", "LaTex Import Filter");
	root.setAttribute("mime", "application/x-kword");
	root.setAttribute("syntaxVersion", "1");
  doc.appendChild(root);
	kdDebug() << "serializer" << endl;
	serialize(store, doc);
	return true;
}

void Document::serialize(KoStore* store, QDomDocument doc)
{
	QCString str = doc.toCString();
	qWarning(str);
	if(store->open("root"))
	{
		store->write((const char *)str, str.length());
		store->close();
	}
}
